/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017 Hibbard M. Engler

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License   
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*
sr_sample_maker is derived from sr_player version 1.27.
This pulls in some server and receiver stuff to make a 10 second or so sample of a song.
In doing so, it will 
1. use music_broadcast_stream to stream the information (alternate global area)
2. do the sr_player stuff (such as lyrics)
3. call silentradiance_frame to grab the frame and make it into a movie - with an image shown first.
4. package it all into a webm,mp4,and ogg using ffmpeg


$Revision: 1.2 $

This tries to get lrc timing, and since timing for lrc is in hundreths of a second, we will do it in 2 packets of 
240 to equal 1 200th *2
......... - nothing
Z - tempo onset
o - onset of beat
tempo onset is estimated after a while - 4 four in a descent time.
*/

#include "aubio_utils.h"

#include "srx_format.h"
#define PROG_HAS_ONSET 0
#define PROG_HAS_OUTPUT 0
#define PROG_HAS_JACK 0
#define PROG_HAS_TEMPO 0

/* save 0.25 seconds or so */
#define SOUNDRING_COUNT 4000



char ourname[2000]; /* ourname is the process number we will use for our temp dir etc. */


/* note - these structures also exist in packet_summary (client) and srplayer (server)
with slightly different names.
packet_summary reads what we are writing,  both can print out text 
but from packet summary, we can have the blue bounding ball!
*/

/* size of text */
//#define SR_MEASURE_MAX_SIZE 1600
#define SR_MEASURE_MAX_SIZE 800
// silent radiance 1.2 is fixed at 800, but we want to expand to 1600
//#define SR_MEASURE_NUM_LINES 9
#define SR_MEASURE_NUM_LINES 4

typedef struct sr_measure_chunk {
  long beat_buffer_current_frame;
  char beat_buffer[SR_MEASURE_MAX_SIZE];
  long beat_buffer_length; // length in 1/100ths of a second
  int number_line_buffers;
  char line_buffer[SR_MEASURE_NUM_LINES][SR_MEASURE_MAX_SIZE];
  int line_buffer_len[SR_MEASURE_NUM_LINES];
  int sent;
  } sr_measure_chunk;


char next_beat_buffer[SR_MEASURE_MAX_SIZE];


#define NUMBER_SEND_LINES 400
/* sending command queue up */
char *send_line[NUMBER_SEND_LINES];
int head_send_line=0;
int tail_send_line=0;






#define MEASURE_CHUNK_COUNT 40
sr_measure_chunk measure_chunks[MEASURE_CHUNK_COUNT];
volatile int measurechunkhead=0;
volatile int measurechunktail=0;


int transform_mode = 0;




/* shared memory header stuff */
void setup_shared_memory_named(ourname);
void add_command_line_to_shared_memory(sr_measure_chunk *c);
/* if command line is 0 - we reset frame number  to 0 */
void add_line_to_shared_memory(sr_measure_chunk *c,int lineno);
/* might be multiple commands */

void set_frame_number_in_shared_memory(long frame_number);
/* send periodically to keep things in check - especially if they change channels */

void finish_shared_memory();

struct silent_radiance *sr=NULL;
struct silent_radiance_client *sr_client=NULL;


















extern int init_receiver_sound(int block);
extern int playOneSoundBuffer();
extern void finish_play_sound();


volatile int soundringhead=0; /* start of the soundringqueue. normally incremented by sound.  But could overflow */
volatile int soundringfirst=0; /* also incrmented by processor.c - probably the first viable sound in the ring */
volatile int soundringtail=0; /* the end on the sound ring. this is incremented by processor.c whenever a packet is added */
volatile int soundringcurrent=-1; /* we are playing this RIGHT NOW: -1 if we are not playing.  This is updated by the sound subsystem */
volatile short soundring[SOUNDRING_COUNT][960];  /* points to soundring_area */

volatile long soundframe[SOUNDRING_COUNT];


#include "shm_communication.h"
/* yup - we communicate to the server backend */


#include "aubio_parse_args.h"

aubio_wavetable_t *wavetable;
aubio_tempo_t *tempo;
fvec_t * tempo_out;
FILE *srxfile=NULL;



/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <poll.h>
#include <errno.h>




  int start_chunk_number=0; // line number to start playing at.  Starts at line 1 - will only play one line then
  int slow_down_speed=1;
  
  int current_chunk_number=1;
  int file_chunk_number=0;
  long file_frame=0;
  int number_chunks=1;


void play_one(char *out1);



void gather_srx_handle2(long the_frame);
void gather_srx_handle(long the_frame,long delayme,long delayserver);

int delay_frames=0;
int delay_frames_server=0;
long last_srx_handle_frame=-1l;


void process_block(fmat_t *imat, fvec_t *obuf)
{
if (sr) {
  delay_frames_server=sr->server.vol.delay_frames;
  }
  
gather_srx_handle(file_frame,delay_frames,delay_frames_server);
short *out1;
fvec_t *ibuf;
{ /* make ibuf form stereo matrix */
  out1 = (short *)(soundring[soundringtail]);
  
  fvec_t ibufx;
  ibuf = &ibufx;
  ibufx.length = imat->length;
  smpl_t combdata[8000];
  ibufx.data = combdata;
  int i;
  int j=0;
  if (transform_mode==2) {
    for (i=0;i<imat->length;i++) { 
      float common = (imat->data[1][i] - imat->data[0][i])*0.5;
      imat->data[0][i] = common;
      imat->data[1][i] = common;
      }
    }
  else if (transform_mode==1) {
    for (i=0;i<imat->length;i++) { 
      float common = (imat->data[1][i] - imat->data[0][i])*0.5;
      imat->data[0][i] = imat->data[0][i] + common;
      imat->data[1][i] = imat->data[1][i] - common;
      }
    }
  for (i=0;i<imat->length;i++) { 
    int k;
    for (k=0;k<slow_down_speed;k++) {
      out1[j++] = (imat->data[0][i]*32767.);
      out1[j++] = (imat->data[1][i]*32767.);
      if (j>=480) {
        j=0;
	soundframe[soundringtail] = file_frame;
	soundringtail = (soundringtail+1) % SOUNDRING_COUNT;
	if (soundringtail==soundringhead) {
	  soundringhead = (soundringhead+1) % SOUNDRING_COUNT;
	  }
//        playOneSoundBuffer();	  
        if ((start_chunk_number)&&(current_chunk_number<start_chunk_number)) {}
        else if ((start_chunk_number)&&(current_chunk_number>start_chunk_number
	   +number_chunks-1)) {
	  exit(0);
	  }
	else {
	  play_one((char *)out1);
	  }
        } /* if we overflowed j */
      } /* for each k to slow it down */
    } /* for each input packet */
  file_frame++;
  }
}



void send_next_bunch(long current_frame); /* current_frame is the_frame - both delays */

static long old_frame=-1;
static long old_frame_server=-1;
void gather_srx_handle(long the_frame,long delayme,long delayserver) {
//fprintf(stderr,"h %ld,%ld,%ld\n",the_frame,delayme,delayserver);
if (the_frame<0) the_frame=0;
while(the_frame-(delayme+delayserver) >old_frame_server) {
  old_frame_server++;
  send_next_bunch(old_frame_server);
  }
while(the_frame-delayme >old_frame) {
  old_frame++;
  gather_srx_handle2(old_frame);
  }
}


int process_state=0;


void process_print() {}

#define BUNDLE_TO_GATHER 5



void done_measure_chunk() {
measurechunktail = (measurechunktail+1) % MEASURE_CHUNK_COUNT;
if (measurechunktail==measurechunkhead) {
  measurechunkhead = (measurechunkhead+1) % MEASURE_CHUNK_COUNT;
  }  
}




static int is_srx_input_100_flag=0;

static long block_next_current_frame = 0;

void gather_srx_next_bunch() {
int a;
if (!srxfile) {return;}
lewp:

a=measurechunktail-measurechunkhead;
if (a<0) a += MEASURE_CHUNK_COUNT;
if (a>=BUNDLE_TO_GATHER) {
   return;
   }
 else {
//   fprintf(stderr,"gatheres %d s %d\n",a,process_state);
   }
struct sr_measure_chunk *mc = measure_chunks+measurechunktail;

if (process_state==0) {
  is_srx_input_100_flag=0;
  /* see how many we have taken in */
  leawp:
  if (!fgets(next_beat_buffer,SR_MEASURE_MAX_SIZE,srxfile)) {
    fclose(srxfile);
    srxfile=NULL;
    return;
    }
  if (srx_is_comment(next_beat_buffer)) {
    is_srx_input_100_flag=1;
    goto leawp;
    }
  if (is_srx_input_100_flag) {
    srx_100_to_srx_200(mc->beat_buffer,next_beat_buffer,SR_MEASURE_MAX_SIZE);
    }
  else {  
    srx_expand(mc->beat_buffer,next_beat_buffer,SR_MEASURE_MAX_SIZE);
    }
//  fprintf(stderr,"0old	%s\n0new %s\n",next_beat_buffer,mc->beat_buffer);
  file_chunk_number++;
  mc->beat_buffer_length = strlen(mc->beat_buffer)-1;
  mc->beat_buffer_current_frame = file_frame;
  mc->sent=0;
  block_next_current_frame = mc->beat_buffer_current_frame + mc->beat_buffer_length;
  process_state=1;
  mc->number_line_buffers=0;
  goto lewp;
  }
else if (process_state==1) {
  while (srxfile) {
    char read_beat_buffer[SR_MEASURE_MAX_SIZE];
    if (!fgets(read_beat_buffer,SR_MEASURE_MAX_SIZE,srxfile)) {
      fclose(srxfile);
      srxfile=NULL;
      continue;
      }
    if (srx_is_comment(read_beat_buffer)) {
      continue;
      }
    if (is_srx_input_100_flag) {
      srx_100_to_srx_200(next_beat_buffer,read_beat_buffer,SR_MEASURE_MAX_SIZE);
      }
    else {  
      srx_expand(next_beat_buffer,read_beat_buffer,SR_MEASURE_MAX_SIZE);
      }
//    fprintf(stderr,"old	%s\nnew %s\n",read_beat_buffer,next_beat_buffer);
      
    int is_control = 1;
    char *p = next_beat_buffer;
    while(*p) {
      if ((*p < '0')||(*p == '|')||(*p == '+')||(*p == '.')||(*p == '-'))
        {
	p++;
	}
      else {
        is_control=0;
	break;
	}
      }
    
    if (is_control) {
      process_state = 2;
      done_measure_chunk();
      goto lewp;
      }
    memset(mc->line_buffer[mc->number_line_buffers],0,SR_MEASURE_MAX_SIZE);
    strcpy(mc->line_buffer[mc->number_line_buffers],next_beat_buffer);
    mc->line_buffer_len[mc->number_line_buffers] = strlen(mc->line_buffer[mc->number_line_buffers]);
    if (mc->number_line_buffers<SR_MEASURE_NUM_LINES)     mc->number_line_buffers++;    
    }
  process_state = 3;
  done_measure_chunk();    
  }
else if (process_state==2) {
  file_chunk_number++;
  strcpy(mc->beat_buffer,next_beat_buffer);
  mc->beat_buffer_length = strlen(mc->beat_buffer)-1;
  mc->beat_buffer_current_frame = block_next_current_frame;
  mc->sent=0;
  block_next_current_frame = mc->beat_buffer_current_frame + mc->beat_buffer_length;
  process_state=1;
  mc->number_line_buffers=0;
  goto lewp;
  } 
else if (process_state==3) {
 return;
 }
}  






void send_next_bunch(long current_frame) { /* current_frame is the_fraem - both delays */
int m;
m = measurechunkhead;
while (m != measurechunktail) {
  struct sr_measure_chunk *mc = measure_chunks+m;
  if (current_frame > (mc->beat_buffer_current_frame+mc->beat_buffer_length)) {
    measurechunkhead = (measurechunkhead+1) % MEASURE_CHUNK_COUNT;
    current_chunk_number++;
    gather_srx_next_bunch();
    }
  else break;
  m = (m+1) % MEASURE_CHUNK_COUNT;    
  }
while (m != measurechunktail) {
  struct sr_measure_chunk *mc = measure_chunks+m;
//  - turned this off to get multiple lines dumped ahead of time
// - now we dont
//  else if (current_frame < mc->beat_buffer_current_frame) {
//    break;
//    }
  if (current_frame <= mc->beat_buffer_current_frame+mc->beat_buffer_length) {
    if (!mc->sent) {
      add_command_line_to_shared_memory(mc);
      int i;
      for (i=0;i<mc->number_line_buffers;i++) {
        add_line_to_shared_memory(mc,i);
        }
      mc->sent = 1;
      break; // dont overload the memory
      }
    } /* if it is time */
  m = (m+1) % MEASURE_CHUNK_COUNT;  
  }
if ((current_frame&255)==0) {
  set_frame_number_in_shared_memory(current_frame);
  }
}  



int output_info=0; // set to 1 to print out the lyrics

void gather_srx_handle2(long the_frame) { /* the_frame is - the me delay, not the server delay */
gather_srx_next_bunch();
int m=measurechunkhead;
while  (measurechunktail!=m) {
  struct sr_measure_chunk *mc = measure_chunks+m;
  if (the_frame > (mc->beat_buffer_current_frame+mc->beat_buffer_length)) { // save it for 
    }
  else if (the_frame < mc->beat_buffer_current_frame) {
//    fprintf(stderr,"frame %ld < %ld\n",the_frame,mc->beat_buffer_current_frame);
    return;
    }
  else {
    long indent = (the_frame-mc->beat_buffer_current_frame);
    int i;
//    fprintf(stderr,"f %ld pos %ld\n",the_frame,indent);
    for (i=0;i<mc->number_line_buffers;i++) {
      char *buf = mc->line_buffer[i];
      if ( (buf[indent])&&(buf[indent] != ' ')&&((indent==0)||(buf[indent-1] == ' ')) ) { /* if we found a new word */
        int indent2=indent;
        if (buf[indent2]== '_') { indent2++;}
        else if (!indent2) {}
        else if (output_info) fputc(' ',stderr);
        char ch;
        while ((ch=buf[indent2])&&(ch != ' ')) { if (ch != '_')  if (output_info) putc(ch,stderr); indent2++; }
        }
      }    
    if (mc->number_line_buffers==0) {
//      if (output_info) fputc(mc->beat_buffer[indent],stderr);
      }
    return;
    }
  m = (m+1) % MEASURE_CHUNK_COUNT;
  }
return;
}





void clear_out_sound_buffer() {
}

char *argvf[] = {"srplayer","-r","48000","","","","","","","","","","","","","","","","","","","","",""};




void exit_handler1(void);

      

int main(int argc, char **argv) {
  atexit(exit_handler1);
  init_receiver_sound(0);
  start_chunk_number = 0;
  slow_down_speed = 1;
  transform_mode = 0;
  number_chunks = 1;
  
  int i;
  int ag=1;
  i=1;
  while (ag<argc) {
    if (strcmp(argv[ag],"-l")==0) {
      start_chunk_number = atoi(argv[ag+1]);
      ag+=2;
      }
    else if (strcmp(argv[ag],"-c")==0) {
      number_chunks = atoi(argv[ag+1]);
      ag+=2;
      }
    else if (strcmp(argv[ag],"-x")==0) {
      slow_down_speed = atoi(argv[ag+1]);
      ag+=2;
      }
    else if (strcmp(argv[ag],"-a")==0) {
      transform_mode = atoi(argv[ag+1]);
      ag+=2;
      }
    else if (strcmp(argv[ag],"-t")==0) {
      output_info = 1;
      ag+=1;
      }
    else {
      argvf[i+2]=argv[ag];  
      i++;  
      ag++;  
      }
    }
  argc=i;  
    
//  for (i=0;i<argc+2;i++) {fprintf(stderr," %s\n",argvf[i]  );}
  if (argc<=1) {
    fprintf(stderr,"usage: srplayer [-t ] show the lyrics [-l l] [-x factor] [-a mode] audio.ext\nWhere\n-l - indicates line number, -x is speed factor\n"
    "-a 1 removes music and plays lyrics part\n"
    "-a 2 plays lyrics and removes music\n");
    }
    
  examples_common_init(argc+2,argvf,"hi",0);
  verbmsg ("using source: %s at %dHz\n", source_uri, samplerate);
  verbmsg ("buffer_size: %d, ", buffer_size);
  verbmsg ("hop_size: %d, ", hop_size);
  
  
  /* get our directory */
  pid_t  a = getpid();
  sprintf(ourname,"srm_%d\n",(int)a);
  
  setup_shared_memory_named(ourname);
  if (sr) {
//    fprintf(stderr,"linked to shared memory broadcaster\n");
    }
      
  current_chunk_number = 1;
  file_chunk_number = 0;
  
  
  
  fprintf(stderr,"look for voice %s\n",argvf[argc+2-1]);
  char filename2[2000];
  strcpy(filename2,argvf[argc+2-1]);
  char *px=filename2+strlen(filename2)-1;
  while (px >filename2) {
    if ((*px) == '.') break;
    px--;
    }
  strcpy(px,".srx");
  srxfile = fopen(filename2,"r");
  if (srxfile) fprintf(stderr,"found\n");
  
  
  
  gather_srx_next_bunch();
  examples_common_process((aubio_process_func_t)process_block, process_print);
  clear_out_sound_buffer();
//  finish_play_sound();
//  examples_common_del();
exit(0);
}





























/* simple_sound_alsa.c varisnt - cause idontknow
*/

/*

This example reads standard from input and writes
to the default PCM device for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <stdio.h> /* for printf() and fprintf() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <errno.h>

/*#define THREADING_ON 1 
^^^ for threadding example - doesnt help much just burns a cpu */




#include <alsa/asoundlib.h>

static volatile int wipe_sound=0;  // quick buffer clear

short zero_buffer[480];




  int init_send_sound=1;
  long loops;
  int rc;
  int size;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  char *buffer;
  struct pollfd *ourpolls;
  int soundpolls_count;
  int socket_poll;
  
  int sound_state = 0;
  
int no_sound = 0;



  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_uframes_t frames;






void play_one(char *out1) {
    int  rc;
lp:
    rc = snd_pcm_writei(handle, out1, 240);
    if (rc== -EPIPE) {
      snd_pcm_prepare(handle);
      goto lp;
      } 
    else if (rc < 0) {
			fprintf(stderr,"ERROR. Can't write to PCM device.%d: %s\n", 
			rc,snd_strerror(rc));
			}  			
    else {
      }
     snd_pcm_sframes_t delay;
     rc = snd_pcm_delay(handle, &delay);
     if (rc != 0) {
       fprintf(stderr,"get delay error %d\n",rc);
       }
     else {
       delay_frames = (delay+239)/240;
//       fprintf(stderr,"delay>%d\n",delay_frames);
       }

}






int init_receiver_sound(int block) {
int rc;
  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle, "default",
                    SND_PCM_STREAM_PLAYBACK,0);
//                    SND_PCM_STREAM_PLAYBACK,SND_PCM_NONBLOCK);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }
  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);
    

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,
                              SND_PCM_FORMAT_S16_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle, params, 2);

  int val = 48000;
  int dir =0;
  snd_pcm_hw_params_set_rate_near(handle, params,
                                  &val, 0);

int err;

  frames = 240;
  rc=snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, &dir);
			      
  snd_pcm_uframes_t f;
  rc = snd_pcm_hw_params_get_period_size(params, &f, &dir);
          if (rc < 0) {
	                  fprintf(stderr,"Unable to get period size for playback: %s\n", snd_strerror(err));
         exit(-1);
         }
  fprintf(stderr,"period size is %d\n",(int)f);




       /* set the buffer time */
	unsigned int time_buffer = 25000;
        err = snd_pcm_hw_params_set_buffer_time(handle, params,time_buffer,dir);
        if (err < 0) {
                fprintf(stderr,"Unable to set buffer size: %s\n",snd_strerror(err));
                exit(-1);
        }

#ifdef dfsjklsfdjdsf
       /* set the buffer time */
	unsigned int time_buffer = 960;
        err = snd_pcm_hw_params_set_buffer_size(handle, params,time_buffer);
        if (err < 0) {
                fprintf(stderr,"Unable to set buffer size: %s\n",snd_strerror(err));
		
        //        exit(-1);
        }
#endif	


snd_pcm_uframes_t acsize;
        err = snd_pcm_hw_params_get_buffer_size(params, &acsize);
        if (err < 0) {
                fprintf(stderr,"Unable to get buffer size for playback: %s\n", snd_strerror(err));
            exit(-1);
        }
  fprintf(stderr,"buffer size is %d\n",(int)acsize);

  
						  

if (rc<0) 			      
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames,
                                    &dir);
/*				    
      int buffer_size;
      int period_size;
      int period_cnt;		    
    buffer_size = pcm_buffer_size(params);
    period_size = pcm_period_size(params);
    period_cnt = buffer_size/period_size;

        fprintf (stderr,"period_cnt = %d\n", pcm->period_cnt);
        fprintf (stderr,"period_size = %d\n", pcm->period_size);
        fprintf (stderr,"buffer_size = %d\n", pcm->buffer_size);
*/

  return 0;
}



void finish_play_sound() {
if (handle) {
  snd_pcm_t *h = handle;
  handle=NULL;
  snd_pcm_drain(h);
  snd_pcm_close(h);
  }
return;
}





/*-------------------------- shared memory stuff */





void setup_shared_memory_named(char *name) {
sr = get_silent_radiance_shared_memory(name);
if (!sr) return;
sr_client = silent_radiance_get_client(sr);
if (!sr_client) {
  fprintf(stderr,"NOSHAREDMEM LEFT");
  detatch_slient_radiance_shared_memory(sr);
  sr=NULL;
  return;
  }

}

int flush_send_lines();

void finish_shared_memory() {
if (sr) {
  if (sr_client) {
    flush_send_lines();
    silent_radiance_release_client(sr,sr_client);
    sr_client=NULL;
    }
  //silent_radiance_finish_up_client_after_all_commands_sent(sr,sr_client,0);
  detatch_slient_radiance_shared_memory(sr);
  sr=NULL;
  }
}

//int fixonce=1;

void not_moving_queue_axnay(char *command) {
if (!sr) return;
if (!sr_client) return;
if (!command) goto nevermind;
/*if (fixonce) {
  fprintf(stderr,"fixonce\n");
  silent_radiance_clear_client_command_queue(sr_client);
  fixonce=0;
  }*/
int s = (tail_send_line+1)%NUMBER_SEND_LINES;
if (s==head_send_line) {
nevermind:
  silent_radiance_clear_client_command_queue(sr_client);
  fprintf(stderr,"NEVERMIND, BROADCAST SERVER NOT RESPONDING\n");
  finish_shared_memory();
  }
send_line[tail_send_line] = strdup(command);
tail_send_line = s;
}

int add_command_if_can_now() {
if (!sr) return 0;
if (!sr_client) return 0;
if (head_send_line==tail_send_line) return 1;
if (sr_client->command_head == ((sr_client->command_tail+1)% SILENT_RADIANCE_MAX_CLIENT_COMMANDS)) {
  return 0;
  }

while (head_send_line!=tail_send_line) {
  if (sr_client->command_head == ((sr_client->command_tail+1)% SILENT_RADIANCE_MAX_CLIENT_COMMANDS)) {
    return(1);
    }
  fprintf(stderr,"unhook\n");
  silent_radiance_add_client_command_queue(sr_client,send_line[head_send_line],strlen(send_line[head_send_line]));
  free(send_line[head_send_line]);
  send_line[head_send_line] = NULL;
  head_send_line = (head_send_line+1)%NUMBER_SEND_LINES;
  if (sr_client)  silent_radiance_clean_client_command_queue(sr_client,1);
  }
return 1;
}			
						
int flush_send_lines() {
add_command_if_can_now();
// could waiy, but why?
}
												


void exit_handler1(void) {
finish_shared_memory();
clear_out_sound_buffer();
finish_play_sound();
    
    
examples_common_del();
      
}

		  
/* 
  $Log: sr_sample_maker.c,v $
  Revision 1.2  2018/12/21 17:27:06  hib
  Pulled out the srx stuff for reading/processing a sound file from srplayer into sr_handling_srx_common
  The reason for doing this, is we are going to make an srplayer who does raw - although we might already have that,  we dont have a way to do a non default public program.
  Anyways, this is to get closer to make a demo movie for a song.

  Revision 1.1  2018/12/21 15:19:21  hib
  starting on sr_sample_maker
  from v1.27 of srplayer.c
  
*/
