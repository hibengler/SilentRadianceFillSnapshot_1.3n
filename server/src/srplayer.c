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

$Revision: 1.31 $

This tries to get lrc timing, and since timing for lrc is in hundreths of a second, we will do it in 2 packets of 
240 to equal 1 200th *2
......... - nothing
Z - tempo onset
o - onset of beat
tempo onset is estimated after a while - 4 four in a descent time.
*/

//#include "aubio_utils.h"
//#include "srx_format.h"
#include "sr_handling_srx_common.h"
#include "srplayer_handling_sound_common.h"

#define PROG_HAS_ONSET 0
#define PROG_HAS_OUTPUT 0
#define PROG_HAS_JACK 0
#define PROG_HAS_TEMPO 0

/* save 0.25 seconds or so */
//#define SOUNDRING_COUNT 4000

/* note - these structures also exist in packet_summary (client) and srplayer (server)
with slightly different names.
packet_summary reads what we are writing,  both can print out text 
but from packet summary, we can have the blue bounding ball!
see sr_handling_srx_common.h
*/



char next_beat_buffer[SR_MEASURE_MAX_SIZE];










int transform_mode = 0;




/* shared memory header stuff */
void setup_shared_memory();
/* if command line is 0 - we reset frame number  to 0 */


void finish_shared_memory();



















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
  
  int current_chunk_number=1;
  int file_chunk_number=0;
  long file_frame=0;
  int number_chunks=1;






int delay_frames=0;
int delay_frames_server=0;
long last_srx_handle_frame=-1l;


void process_block_c(fmat_t *imat, fvec_t *obuf)
{
if (sr_c) {
  delay_frames_server=sr_c->server.vol.delay_frames;
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







void process_print() {}

#define BUNDLE_TO_GATHER 5







static int is_srx_input_100_flag=0;

static long block_next_current_frame = 0;












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
  setup_shared_memory();
  if (sr_c) {
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
  examples_common_process((aubio_process_func_t)process_block_c, process_print);
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
  
  



  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_uframes_t frames;



















												
	  

		  


	  



void exit_handler1(void) {
finish_shared_memory();
clear_out_sound_buffer();
finish_play_sound();
    
    
examples_common_del();
      
}

		  
/* 
  $Log: srplayer.c,v $
  Revision 1.31  2018/12/22 02:53:28  hib
  Got srplayer_to_sirad to work - now need seconds start and seconds len
  So we can sample it
  Then need a special silent radiance th splashes the album cover and
  does the dancinc and records that to a png file - and records the sound as well

  then we can use avconf to make a .ogg, a .mp4, and a .something else so we can play the middle of the song with the excitement.

  Revision 1.30  2018/12/22 00:42:42  hib
  wip for new sr player right to sirad.  Made music_broadcast quit if cant write to stdout (or any file) any more.
  (untested)

  Revision 1.29  2018/12/21 23:12:00  hib
  pulled the sound code out of srplayer, preparing to make
  srplayer---broadcast combo like thing

  Revision 1.28  2018/12/21 17:27:06  hib
  Pulled out the srx stuff for reading/processing a sound file from srplayer into sr_handling_srx_common
  The reason for doing this, is we are going to make an srplayer who does raw - although we might already have that,  we dont have a way to do a non default public program.
  Anyways, this is to get closer to make a demo movie for a song.

  Revision 1.27  2018/10/31 02:48:27  hib
  Set up sr000 for full and incremental logging

  Revision 1.26  2018/09/30 18:51:31  hib
  fucking cool

  Revision 1.25  2018/09/29 22:56:23  hib
  made the player queue things up in case broadcaster was busy.

  Revision 1.24  2018/09/29 22:29:51  hib
  try it out

  Revision 1.23  2018/09/29 22:02:23  hib
  slight bug in compress - try not to use it - see if we can fix the problem just in SR

  Revision 1.22  2018/09/29 21:10:56  hib
  fixed bugs in reading 100 char or 200 char file

  Revision 1.21  2018/09/29 20:56:13  hib
  We can read 100 char/sec files for srx now, and fixed tab handling

  Revision 1.20  2018/09/29 20:13:30  hib
  some fix to srplayer - bu t need to queue when broadcast is busy.
  wrote srx_format - oh yeah

  Revision 1.19  2018/08/26 15:04:57  hib
  cool added -c for count of lines

  Revision 1.18  2018/08/25 10:00:48  hib
  made it give out more lines

  Revision 1.17  2018/08/20 13:51:50  hib
  wip

  Revision 1.16  2018/07/24 20:49:01  hib
  think it got it to relinuqish the sr client so we stop running out on production

  Revision 1.15  2018/07/22 13:35:46  hib
  Got srplayer to work flawlessly - excep the nasty error at the end

  Revision 1.14  2018/07/22 11:50:56  hib
  Made shm_communication version 1. This way we can see the delay frames in the input stream

  Revision 1.13  2018/07/22 11:42:36  hib
  Added a delay for the music broadcast server

  
*/
