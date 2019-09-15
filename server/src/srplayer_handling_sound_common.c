/*

$Revision: 1.1 $
 from version 1.28 of sr_player.c
 
This is the sound output part of srplayer
*/



#include "srplayer_handling_sound_common.h"


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




int slow_down_speed=1;
  













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




void clear_out_sound_buffer() {
}




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







												
	  

		  


	  




		  
/* 
  $Log: srplayer_handling_sound_common.c,v $
  Revision 1.1  2018/12/21 23:12:00  hib
  pulled the sound code out of srplayer, preparing to make
  srplayer---broadcast combo like thing

  
  from Revision 1.28 of srplayer.c
  
*/
