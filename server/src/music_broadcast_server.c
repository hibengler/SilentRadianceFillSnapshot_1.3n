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
$Revision: 1.19 $
*/

/* note - aubiotools 0.4.0.1 has a bug that crashes the transmitter sometimes
basically upgrading to ubuntu 16.04 will probably fix this.
*/

#include "music_broadcast_server.h"
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <alsa/asoundlib.h>
#include <sys/time.h>

#include "aubio_utils.h"
#define PROG_HAS_ONSET 1
#define PROG_HAS_TEMPO 1

#define PROG_HAS_OUTPUT 0
#define PROG_HAS_JACK 1

#include "crypt.h"
#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"
#include "shm_communication.h"
#include "transmitter.h"


#include "aubio_parse_args.h"

//define DECODETEST
/* ^^^ used to decode and test the ddecong here in line*/
extern FILE *write_file_spec;



int aubio_jack=0; /* 1 is standard aubio, 0 uses alsa instead of jack */
aubio_onset_t *o;
aubio_tempo_t *tempo;
aubio_pitch_t *pitchrec;
fvec_t *pitch;
fvec_t *onset;
smpl_t is_onset;
fvec_t * tempo_out;

/* for alsa connect */
snd_pcm_t *handle;
snd_pcm_hw_params_t *params;
  
struct silent_radiance *sr;
/* this is a shared memory segment, that allows for external commands to be sent to the server, and the server can also communicate with 
clients on the same computer. 
Commands are used to do group color functions.
*/


OpusEncoder *opusencoder=NULL;
#ifdef DECODETEST
OpusDecoder *opusdecoder = NULL;
#endif

unsigned char *compressed_buffers[MAXFRAMES];
int compressed_lengths[MAXFRAMES];
int frame_numbers[MAXFRAMES];
unsigned int frame_number=0;
unsigned int flags=0;



aubio_tempo_t * tempo;
fvec_t * tempo_out;
smpl_t is_beat = 0;
uint_t is_silence = 0.;


void process_print (void)
{
  if ( is_onset ) {
    outmsg ("%f\n", aubio_onset_get_last_s (o) );
  }
}


void redo_onset_if_stuck(int);


static int should_we_send=0;

int sample=0;

int process_block(fmat_t * imat, fvec_t *obuf) {

  
flags = flags & 0xffffff00;
fvec_t *ibuf;
smpl_t combdata[8000];
{ /* make ibuf form stereo matrix */
  fvec_t ibufx;
  ibuf = &ibufx;
  ibufx.length = imat->length;
  
  
  
  ibufx.data = combdata;
  int i;
  for (i=0;i<imat->length;i++) { 
    combdata[i] =(imat->data[0][i]+imat->data[1][i])*0.5;
    }
  } /* block to make a ibuf out of a imat */
    
  aubio_onset_do (o, ibuf, onset);
  is_onset = fvec_get_sample(onset, 0);
  
  redo_onset_if_stuck(is_onset);
  
  /* note - aubio 0.4.3 fixes a bug with this - so you are warned */
  aubio_tempo_do (tempo, ibuf, tempo_out);
  is_beat = fvec_get_sample (tempo_out, 0);
  if (silence_threshold != -90.)
    is_silence = aubio_silence_detection(ibuf, silence_threshold);

  aubio_pitch_do (pitchrec, ibuf, pitch);
  float the_pitch;
  the_pitch = fvec_get_sample(pitch, 0);


  if ( !usejack && ! sink_uri ) return(-1);
  if (obuf) fvec_zeros (obuf); /* dont need for alsa */
  if ( is_beat &&(!is_silence) ) {
//     fprintf(stderr,"B");
    flags |= 2;
  }
  if ( is_onset ) {
//     fprintf(stderr,"o");
     flags |= 1;
     }
if (imat->length != 240) {
  fprintf(stderr,"qqq %d\n",imat->length);  
  }

  float db_level=aubio_db_spl(ibuf);

{ /* generate opus */
  float interleave[10000];
  /* so opus does only 240 bytes - so we have 16*4 additional samples we need to add to the packet */
  unsigned char *t = compressed_buffers[MAXFRAMES-1];
  
  
  int i; /* save a little time */
  if (write_file_spec) {
    frame_numbers[0]=frame_number;
    } 
  else {
    for (i=MAXFRAMES-1;i>0;i--) {
      compressed_lengths[i]=compressed_lengths[i-1];
      compressed_buffers[i]=compressed_buffers[i-1];
      frame_numbers[i]=frame_numbers[i-1];
      }
    compressed_buffers[0] = t;
    compressed_lengths[0] = 0;
    frame_numbers[0]=frame_number;

    } 
  
  for (i=0;i<imat->length;i++) {
    interleave[i+i] =imat->data[0][i];
    interleave[i+i+1]=imat->data[1][i];
    }
    


  int packet_size=  opus_encode_float(opusencoder,interleave,240,compressed_buffers[0],4000);
  compressed_lengths[0] = packet_size;
//fprintf(stderr,"frame %d packet_size %d key %d\n",
//frame_number,packet_size,28111*packet_size);
  } /* opus stuff */
#define COMMAND_PACKET_MAX_SIZE 780
unsigned char build_server_packet_buf[2000];
int packet_buf_len = 0;


{ /* 16 extra bytes */
  int microseconds;
  struct timeval tv;
  gettimeofday(&tv,NULL);
  microseconds = tv.tv_sec & 1024; /* 277 hours*/
  microseconds = microseconds*1000000 + (tv.tv_usec);
  
  int l = 0;
  unsigned char *statistics=compressed_buffers[0]+compressed_lengths[0];
  float dummy_pitch=the_pitch;
  unsigned char *pt = (unsigned char *)&the_pitch;
  statistics[l++]=pt[0];                                                        /* pitch */
  statistics[l++]=pt[1];
  statistics[l++]=pt[2];
  statistics[l++]=pt[3];
  statistics[l++]=microseconds&255;                                     /* microseconds - is modulo 1024 seconds */
  statistics[l++]= (microseconds>>8)&255;
  statistics[l++]= (microseconds>>16)&255;
  statistics[l++]= (microseconds>>24)&255;
  pt = (unsigned char *) &db_level;                                    /* db level */
  statistics[l++]=pt[0];
  statistics[l++]=pt[1];
  statistics[l++]=pt[2];
  statistics[l++]=pt[3];
  statistics[l++]=flags&255;                                           /* flags */
  statistics[l++]= (flags>>8)&255;
  statistics[l++]= (flags>>16)&255;
  statistics[l++]= (flags>>24)&255;
  }
   
int send_it=0;
if (poor_mode) {
  should_we_send = (should_we_send + 1) % 5;
  if ((should_we_send==0)||(should_we_send==2)) 
    send_it=1;
  }
else {
  send_it=1;
  }
  
int rc=0;
if (send_it) {
  silent_radiance_build_server_packet(sr,COMMAND_PACKET_MAX_SIZE,build_server_packet_buf,&packet_buf_len);
  if (write_file_spec) {
    rc=stroop(compressed_lengths,compressed_buffers,frame_numbers,build_server_packet_buf,packet_buf_len,poor_mode);
    }
  else {
    rc=boop(compressed_lengths,compressed_buffers,frame_numbers,build_server_packet_buf,packet_buf_len,poor_mode);
    } /* could do both but for now, either or */  
  }

flags = flags <<8;
frame_number++;
if (send_it) {
  sr->server.vol.frame_number = frame_number;
  }
if (frame_number<0) frame_number=0;
//if (frame_number % 100 == 0)fprintf(stderr,"size  %d      \r",compressed_length1);  

    
return rc;    
}


int init_sound() {
int rc;
  /* Open PCM device for recording (capture). */
  rc = snd_pcm_open(&handle, "default",
                    SND_PCM_STREAM_CAPTURE, 0);
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

  unsigned int val;
  int dir;
  int size;
  /* 44100 bits/second sampling rate (CD quality) */
  val = 48000;
  snd_pcm_hw_params_set_rate_near(handle, params,
                                  &val, &dir);

  /* Set period size to 32 frames. */
  snd_pcm_uframes_t frames;
  frames = 240;
  snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    exit(1);
  }
  
  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params,
                                      &frames, &dir);
  size = frames * 4; /* 2 bytes/sample, 2 channels */
  fprintf(stderr,"frames is %d\n",(int)frames);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params,
                                         &val, &dir);
fprintf(stderr,"size is %d\n",size);
  return 0;
}


int finish_sound() {
  snd_pcm_drain(handle);
  snd_pcm_close(handle);
}



int no_onset_count=0;

void init_aubio_onset_stuff() {
  o = new_aubio_onset (onset_method, buffer_size, hop_size, samplerate);
  if (onset_threshold != 0.)
    aubio_onset_set_threshold (o, onset_threshold);
  if (silence_threshold != -90.)
    aubio_onset_set_silence (o, silence_threshold);
  onset = new_fvec (1);
  no_onset_count=0;
}




void finish_aubio_onset_stuff() {
if (o) {
  del_aubio_onset (o);
  o = NULL;
  }
if (onset) {
      del_fvec (onset);
   onset = NULL;
   }
}


void redo_onset_if_stuck(int is_onset) {
if (is_onset) {
  no_onset_count=0;
  }
else {
  no_onset_count++;
  }
if (no_onset_count > 4800) {
  fprintf(stderr,"no onset count for 20 seconds.  Restartting onset count\n");
  finish_aubio_onset_stuff(); 
  init_aubio_onset_stuff(); 
  }
}




void init_aubio_stuff(int argc, char *argv[]) {

  argc=1;
  examples_common_init(argc,argv,"transmitter",aubio_jack);


  init_aubio_onset_stuff();


  pitch = new_fvec(1);  
  pitchrec = new_aubio_pitch("default",buffer_size,hop_size,samplerate);
  /*
   if (pitch_tolerance != 0.)
    aubio_pitch_set_tolerance (o, pitch_tolerance);
  if (silence_threshold != -90.)
    aubio_pitch_set_silence (o, silence_threshold);
  aubio_pitch_set_unit(pitchrec,default);
  */


  tempo_out = new_fvec(2);
  tempo = new_aubio_tempo(tempo_method, buffer_size, hop_size, samplerate);
  if (onset_threshold != 0.) aubio_tempo_set_threshold (tempo, onset_threshold);
}

void init_shared_memory_stuff() {
  sr=get_silent_radiance_shared_memory("default");
}



void init_opus_encoder_stuff() {
  
 /* opus */
    int err;
    int application=OPUS_APPLICATION_RESTRICTED_LOWDELAY;

    opusencoder = opus_encoder_create((opus_int32)(48000),2,application,&err); /* it was 44100, but android phones failed if not 48k
       for the superpowered stuff */
    if (err != OPUS_OK)
       {
          fprintf(stderr, "Cannot create encoder: %s\n", opus_strerror(err));
          exit( EXIT_FAILURE);
       }
    if (poor_mode) {
      opus_encoder_ctl(opusencoder, OPUS_SET_BITRATE(110000));
      opus_encoder_ctl(opusencoder, OPUS_SET_VBR(1));
      opus_encoder_ctl(opusencoder, OPUS_SET_VBR_CONSTRAINT(0));
//      opus_encoder_ctl(opusencoder,OPUS_SET_COMPLEXITY(3));
//      opus_encoder_ctl(opusencoder,OPUS_SET_PACKET_LOSS_PERC(60));
      }
    else {
      opus_encoder_ctl(opusencoder, OPUS_SET_BITRATE(192000));
      opus_encoder_ctl(opusencoder, OPUS_SET_VBR(1));
      opus_encoder_ctl(opusencoder, OPUS_SET_VBR_CONSTRAINT(0));
      }
   opus_encoder_ctl(opusencoder,OPUS_SET_SIGNAL(OPUS_APPLICATION_RESTRICTED_LOWDELAY));
#ifdef DECODETEST
{ /* opus decode test */
   int err;
    opusdecoder = opus_decoder_create((opus_int32)(48000),2,&err); 
    if (err != OPUS_OK)
       {
          fprintf(stderr, "Cannot create decoder: %s\n", opus_strerror(err));
          return EXIT_FAILURE;
       }
    }
 
#endif
}

void init_frame_caching_stuff() {    
    {
      int i;
      for (i=0;i<MAXFRAMES;i++) {
        compressed_lengths[i]=0;
        compressed_buffers[i]=malloc(4000);
	}
      }
    
    frame_number=0;
    flags=0;
}

void  init_save_session_stuff() {

//write_file_spec =fopen("record.sirad","w");
}


void buffer_to_imat(unsigned char *b,fmat_t *imat) {
      int i;
      float one_over_max = 1.0/32768.;
      for (i=0;i<240;i++) {
        unsigned short u;
	short s;
	u = b[0] + (b[1]<<8);
	s=u;
        imat->data[0][i] = ((float)s)* one_over_max;
	imat->data[0][i] *= 0.5;

	b +=2;
	
	u = b[0] + (b[1]<<8);
	s=u;
	imat->data[1][i] = ((float)s)* one_over_max;
	imat->data[1][i] *= 0.5;
	b +=2;
        }
} /* block to convert to imat format */

int delay_frames = 0;


int music_broadcast_loop() {
fmat_t *imat;
imat = new_fmat(2,240);
unsigned char buffer[4000];  
int rc=0;
while (!rc) {
    rc = snd_pcm_readi(handle, buffer, 240);
//    fprintf(stderr,".");
    if (rc == -EPIPE) {
      /* EPIPE means overrun */
      fprintf(stderr, "overrun occurred\n");
      snd_pcm_prepare(handle);
    } else if (rc < 0) {
      fprintf(stderr,
              "error from read: %s\n",
              snd_strerror(rc));
    } else if (rc != (int)240) {
      fprintf(stderr, "short read, read %d frames\n", rc);
    }
    
     snd_pcm_sframes_t delay;
     rc = snd_pcm_delay(handle, &delay);
     if (rc != 0) {
       fprintf(stderr,"get delay error %d\n",rc);
       }
     else {
       delay_frames = (delay+239)/240;
       sr->server.vol.delay_frames = delay_frames;
//       fprintf(stderr,"delay>%d\n",delay_frames);
       }

    
    buffer_to_imat(buffer,imat);

    rc = process_block(imat,NULL);
    } // while process_block works
}

 

/* reading 48000 raw */
int music_broadcast_stdin_loop() {
fmat_t *imat;
imat = new_fmat(2,240);
unsigned char buffer[4000];  
int rc;
struct timeval before;
before.tv_sec=0;
before.tv_usec = 0;
long long_term_diff=0;

while (rc=fread(buffer,1,960,stdin)==960) {
    sample += 960;
    buffer_to_imat(buffer,imat);

    { /* slow us down */
      struct timeval now;
      int flag=1;
      do {
        gettimeofday(&now,NULL);
        long long diff;
        if (before.tv_usec) {
          diff = now.tv_sec - before.tv_sec;
          if ((diff >5)||(diff<-5)) 
	    {
	    flag=0;
//            fprintf(stderr,"bug diff %d\n",(int)diff);
	    long_term_diff=0;
	    before.tv_sec=0;
	    }
          else {
            diff = diff*1000000 + now.tv_usec - before.tv_usec;
	    long_term_diff += diff;
//            fprintf(stderr,"%d %d\n",(int)diff,(int)(long_term_diff));
  	    if (long_term_diff>=5000) flag=0;
	    }
          if (flag) {
//            fprintf(stderr,"w %d ",(int)(5000-long_term_diff));
               struct timespec thislong;
               thislong.tv_sec = 0;
               thislong.tv_nsec = 1000 * (5000-long_term_diff);
               nanosleep(&thislong, &thislong);
	    }
	  } /* if not the very first one */
        before.tv_sec=now.tv_sec;
        before.tv_usec = now.tv_usec;
	} while (flag);
      long_term_diff =long_term_diff-5000;
      }
      

    rc = process_block(imat,NULL);
    if (rc) return 1;
    } /* while stream */
return 0;
}

 





static fmat_t *buffer_imat;
static struct timeval buffer_before;
static long buffer_long_term_diff;

int music_broadcast_buffer_start() {
buffer_imat = new_fmat(2,240);
buffer_before.tv_sec=0;
buffer_before.tv_usec = 0;
buffer_long_term_diff=0;
return 0;
}

int music_broadcast_buffer_write(char *buffer) {
fmat_t *imat= buffer_imat;
int rc;

sample += 960;
buffer_to_imat(buffer,imat);

buffer_long_term_diff =buffer_long_term_diff+5000;
      

rc = process_block(imat,NULL);
return rc;

}

  
    

int music_broadcast_main(int argc, char **argv) {
  // override general settings from utils.c
  
  init_internet(argc,argv);
  
  init_aubio_stuff(argc,argv);
  
  init_shared_memory_stuff();

  init_opus_encoder_stuff();

  init_frame_caching_stuff();
  
  init_save_session_stuff();

if (aubio_jack) {
  examples_common_process((aubio_process_func_t)process_block, process_print);
  del_aubio_tempo(tempo);
  del_fvec(tempo_out);

  examples_common_del();
  return(0);
  }
else {
  init_sound();  
  return music_broadcast_loop();
  }

}







int music_broadcast_from_stdin(int argc, char **argv) {
  // override general settings from utils.c
  
  init_internet(argc,argv);
  
  init_aubio_stuff(argc,argv);
  
  init_shared_memory_stuff();

  init_opus_encoder_stuff();

  init_frame_caching_stuff();
  
  init_save_session_stuff();

  return music_broadcast_stdin_loop();

}








int music_broadcast_from_buffer(int argc, char **argv) {
  // override general settings from utils.c
  
  init_internet(argc,argv);
  
  init_aubio_stuff(argc,argv);
  
  init_shared_memory_stuff();

  init_opus_encoder_stuff();

  init_frame_caching_stuff();
  
  init_save_session_stuff();

  music_broadcast_buffer_start();
// call music_broadcast_buffer_write
return 0;
}


/*
$Log: music_broadcast_server.c,v $
Revision 1.19  2018/12/22 00:42:42  hib
wip for new sr player right to sirad.  Made music_broadcast quit if cant write to stdout (or any file) any more.
(untested)

Revision 1.18  2018/09/02 09:08:17  hib
wip

Revision 1.17  2018/07/22 11:42:36  hib
Added a delay for the music broadcast server

Revision 1.16  2018/05/07 20:18:34  hib
Put a 20 second reset for onset checking just in case it gets stuck

Revision 1.15  2018/05/02 03:06:39  hib
still messing with flags and offset

Revision 1.14  2018/05/02 02:59:33  hib
why it was no beat?

Revision 1.13  2018/05/02 00:02:35  hib
wip

Revision 1.12  2018/05/01 23:52:50  hib
wip - beat onset works

Revision 1.11  2017/10/20 05:36:09  hib
almost missed this

Revision 1.10  2017/10/17 22:39:09  hib
making to broadcast to web server

Revision 1.9  2017/07/05 13:07:00  hib
copyleft everything

Revision 1.8  2016/11/15 17:51:58  hib
added notes

Revision 1.7  2016/11/02 19:07:48  hib
Consolidated transmitter ipv6 and ipv4 together. Also made SR00 protocol
and poor_mode so we can somewhat effectively transmit on wifi cards limited to 1MB/s.
This version is still kludgy, and there is a bug where the sr_version gets overwritten - so there is more work to find an overbearing leak that changes things too much.
But - it is a major upgrade.

Revision 1.6  2016/10/28 20:28:06  hib
work in progress - got aubio cleaned up from the bug

Revision 1.5  2016/10/14 01:30:15  hib
got the broadcaster to work from the logo program.

Revision 1.4  2016/10/13 12:14:40  hib
work in progress

Revision 1.3  2016/10/13 09:18:57  hib
try again. making the music_broadcast_from_stdin to allow us to transmit the om noise from sound_v2

Revision 1.2  2016/10/13 07:13:04  hib
cool

Revision 1.1  2016/10/13 07:00:06  hib
mde the transmitter build different

Revision 1.10  2016/10/13 06:01:39  hib
cleanup

Revision 1.9  2016/09/10 07:03:42  hib
Added the ability to send commands to the transmitter viw shared memory! wow!

Revision 1.8  2016/07/26 01:48:57  hib
minor

Revision 1.7  2016/07/21 07:18:49  hib
...

Revision 1.6  2016/06/29 23:27:47  hib
We will just do one level of encryption

Revision 1.5  2016/06/29 22:48:24  hib
fix music broadcast for level 2 decrypt

Revision 1.4  2016/06/29 22:23:49  hib
fixed receiver stuff for encryption level 1

Revision 1.3  2016/06/22 10:20:06  hib
made ip address variable

*/
