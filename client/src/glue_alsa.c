/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler

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

/*$Revision: 1.37 $*/

#include <stdlib.h>
#include <string.h>
#include <math.h>

// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
// #include <android/log.h>


// for native asset manager
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"

#ifndef SR_NOSOUND
#define SR_SOUND
#endif

#include "glue.h"
#include "network_receiver.h"

#ifdef SR_READ_FILE    
#include "file_receiver.h"
#endif
#include "curl_web_stream.h"

#include "processor.h"

#ifdef SR_SOUND
#include "sound_alsa.h"
#endif

#include "packet_summary.h"

int watchdog_point=0; // Used for debugging when we freeze up on the receiver thread.

float xdpi = 24; /* set from ios/android prior to initializing the c stuff. This helps us get the aspect ratio of items perfect */
float ydpi = 24; /* set from ios/android prior to initializing the c stuff. This helps us get the aspect ratio of items perfect */

#include <pthread.h>

int ip4_last16;
char ip4_address[50];
int ip6_last16;
char ip6_address[50];

volatile int soundPlayCool=0;

int i_am_a_tv=0;  // flag used in android only right now.
// If set to 1, the diamonds that bring up the store are unavailable
// on the main menu, because some tv do not ahve a browser.


int got_new_source = 0; // set to 1 when we got a new source to do
char *new_source = NULL; // set to a new source to apply when got a new source
int new_block =0;  // set to the next block value
int demo_screen_width=480;  // used for demo screen captures
int demo_screen_height=480;
int demo_taper_volume_flag=0; // used to taper the audio with -t
int make_demo_mode=0;   // set to 1 for demo, and 2 for 

volatile struct timeval demo_time_of_day = (struct timeval) {-1,0};
// this is used
 // as a surrogate time by sr_gettimeofday, and then 
 // glue_alsa adjusts this by 5000 microseconds for each 1/200th of a second.


struct timeval demo_frame_time;
struct timeval demo_start_time = (struct timeval){-1,0}; 
struct timeval demo_end_time = (struct timeval){-1,0}; 

struct timeval demo_start_video_frame_time_second = (struct timeval){-1,0}; 
double demo_start_sub_millisecond_video_time_second = 0.;
// the time of second 0 for the first second ( and sub-second)
// so this could go over - and thats how we tell if we are in a new second
// and that is how we tell to use the fudged end value - is the correct one
// if it is nore than one second greater than the start one.

struct timeval demo_start_video_frame_time = (struct timeval){-1,0}; // this is computed in glue_alsa 
                          // and then set into demo_time_if_day
double demo_start_sub_millisecond_video_time = 0.;

struct timeval demo_end_video_frame_time = (struct timeval){-1,0}; // this is computed in glue_alsa 
double demo_end_sub_millisecond_video_time = 0.;
struct timeval demo_end_video_frame_time_second = (struct timeval){-1,0}; 
double demo_end_sub_millisecond_video_time_second = 0.;

struct timeval  demo_next_end_video_frame_time = (struct timeval){-1,0}; // this is used to compute the next video time frame
   // before we do the video processing.  And then we set this - and the second number info 
   // after processing - so the start and end range is used by our
   // demo clock that goes every 5000 microseconds per audio frame
double demo_next_end_sub_millisecond_video_time; // similar - but for
// the sub millisecond position of frames

	
// computed to handle weirdo frame rates
static double frametime_in_seconds_d; // 1.0 / demo_fps
static double frametime_in_microseconds_d; // 1000000.0 / demo_fps
static unsigned long frametime_in_microseconds; // 1000000.0 / demo_fps

static unsigned long fudge_in_microseconds; // 1000000. / (frametime_in_microseconds *demo_fps )
//                                           (frametime_in_microseconds * demo_fps);
// This should handle decimal fps and 30 fps well.
static unsigned long fudge_in_microseconds; // a fudge to add - int
static double fudge_in_microseconds_d;  // 1 million minus 
//        (one over demofps * demo_fps); // could have some decimal adjustment
static double sub_fudge_in_microseconds_d; // the fudge beyond the microsecond level
//      
double one_over_demo_fps; // computed

double demo_fps=30.;
double demo_start_time_seconds=15.; // =15.f; , or -1 for mtv demo
double demo_length_seconds=20.;   // 20.f; , or -1 for mtv demo


struct timeval demo_turn_on_disco_info_time_start = (struct timeval){-1,0};
struct timeval demo_turn_off_disco_info_time_start = (struct timeval){-1,0};
struct timeval demo_turn_on_disco_info_time_end = (struct timeval){-1,0};
struct timeval demo_turn_off_disco_info_time_end = (struct timeval){-1,0};

void *SoundPlay(void *arg)
{
#ifdef SR_SOUND
    init_receiver_sound(1);
    init_packet_summary();
while (soundPlayCool) {
  playOneSoundBuffer();
//  compute_packet_summary();
//  fprintf(stderr,"%d warn %f start %d  nogap %d cur %d end %d q %f \n",packet_summary.state,packet_summary.warming_quality,
//     packet_summary.start_frame,packet_summary.no_gap_frame,packet_summary.current_frame,
//     packet_summary.end_frame,packet_summary.quality);
  }
#endif

return NULL;
}

extern void display_set_stream_is_closing();// just resets things for a new stream
extern void finish_stream_sound();

static int did_init_receiver_internet=0;

int init_packet_source_block_flag=0;
// probably needs to be in all the glues. probably is the same
int init_packet_source(char *source,int block) {
init_packet_source_block_flag=block;
#ifdef SR_READ_FILE
#ifdef SR_SOUND
finish_stream_sound();
#endif
#ifndef NO_GRAPHICS
display_set_stream_is_closing(); // reset backward worm
#endif
if (we_are_reading_file) {
  shutdown_file_reader();
  we_are_reading_file = 0;
  }
if (we_are_streaming_web) {
  shutdown_web_stream();
  we_are_streaming_web = 0;
  }

if (source) {
  if (strchr(source,':')) {
    init_web_stream(source);
    }
  else {
    init_receiver_file(source);
    }
  }
else {
#endif
  if (!did_init_receiver_internet)  {
    init_receiver_internet(block);
    did_init_receiver_internet=1;
    }
#ifdef SR_READ_FILE     
  }
#endif
}





int switch_packet_source(char *source,int block) {
#ifdef SR_READ_FILE
//if (source) 
//  fprintf(stderr,"switch packet source!!!!!!!!!!!! %s   , %d\n",source,block);
//else {
//  fprintf(stderr,"switch packet source!!!!!!!!!!!!  wifi , %d\n",block);
//  }

got_new_source = 1;
new_source = source;
new_block = block;

#endif
return 0;
}









#ifndef NO_GRAPHICS
extern char * top_menu_url(); // from top_menu.c, gets the url from preferences
#endif


void *NetworkStuff(void *arg)
{
init_packet_summary();

      char *url=NULL; 
#ifndef NO_GRAPHICS
      url=top_menu_url(); // from top menu preferences
#endif      
      init_packet_source(url,0);

while (1) {
  NativeWork();
//  fprintf(stderr,".");
  }
return NULL;
}


#ifdef SR_READ_FILE    
void *ReadFileStuff(void *arg)
{
init_packet_summary();
char *filename = (char *)arg;
init_packet_source(filename,0);
while (1) {
  NativeWork();
//  fprintf(stderr,".");
  }
return NULL;
}






void *WebStreamStuff(void *arg)
{
init_packet_summary();
char *filename = (char *)arg;
init_packet_source(filename,0);
while (1) {
  NativeWork();
//  fprintf(stderr,".");
  }
return NULL;
}
#endif





void NativeInit(int argc, char *argv[]) {
//    write_file_spec =fopen("recieve.srd","w");



    init_processor();
    timer_on_polling_flag=1;
#ifdef SR_READ_FILE    
    if (argc==2) {
      if (argv[1]) {
        init_packet_source(argv[1],0);
	}
      }
    else {
#endif
      char *url=NULL; 
#ifndef NO_GRAPHICS
      url=top_menu_url(); // from top menu preferences
#endif      
      init_packet_source(url,0);
#ifdef SR_READ_FILE    
    }
#endif
#ifdef SR_SOUND      
    {
      pthread_t tid[1];
      soundPlayCool=1;
      int err;
      err = pthread_create(tid,NULL,&SoundPlay,NULL);
      if (err != 0) 
         logit("\ncan't create thread :[%s]", strerror(err));
      }
#else
   init_packet_summary();   
#endif

}


extern char new_display_title_text[];
extern int change_display_title_text;


static int demo_processing_flag=1;





void compute_next_demo_end_video_time() {
  // block to compute demo_end_video_time based on frames per second
  // note - we handle fractional fps that are not 1/2, 1/4, etc
  // this requires fudging the milliseconds whenever we go into the next second
  // and it also requires adding a sub millisecond fudge that is used
  // to keep the fudge in line - so something like 29.92828297 or 30.000175
  // will still work 
  demo_next_end_video_frame_time = add_milliseconds_to_time(
     demo_start_video_frame_time,frametime_in_microseconds);
  demo_next_end_sub_millisecond_video_time = demo_start_sub_millisecond_video_time;
  logit("demo_start_video_time_frame  %ld.%ld (%ld)\n",demo_start_video_frame_time.tv_sec,
      demo_start_video_frame_time.tv_usec,fudge_in_microseconds);
      
  
  // thats the usual for demo_next_end, but sometimes we cross a second barrier
  // now second barriers increment by seconds - exactly
  // but our frame might not always hit the millisecond mark, or sub millisecond.
  // its frame could be way different. like 1 1/2 frames per second
  // video frame 0 starts at 0,0 then goes to 0,750000
  // video frame 1 starts at 0.750000, then ends on 1,500000
  // In that case, the second will increment, but we dont start the next
  // frame at 000000, we start at 500000
  // the next frame should be 2,250000
  // and there can be a fudge in milliseconds, say 30 fps, thats 333333 milliseconds
  // but there is a 1 millisecond fudge at the end
  // sometime the fudge is greater than 1 millisecond or less, say 1.2 milliseconds
  // so we keep track of the 0.2 additional push, which might not be viable for , in theis case 5 seconds to get one additional milliseconds.
  // this SHOULD work for up to 1 million frames per second, and down to 1 frame every 1000000 seconds
  
  
  struct timeval test_alternate_end;
  unsigned long test_fudge = fudge_in_microseconds;
  double test_subfudge = demo_start_sub_millisecond_video_time + 
       sub_fudge_in_microseconds_d;
  unsigned long sub_test_fudge_adjustment = test_subfudge;
  test_fudge += sub_test_fudge_adjustment; 
           /* just in case we have to deal with 
         some weird framerate like 19.3 . The sub_test_fudge_adjustment should be 0 or 1
	 because the fudge values are 0 or 1 */
  double  sub_test_fudge_adjustment_d = (sub_test_fudge_adjustment);
  test_subfudge = test_subfudge - sub_test_fudge_adjustment_d; // usually -1 but sometimes more
  test_alternate_end = add_milliseconds_to_time
     (demo_next_end_video_frame_time,test_fudge);
  
  unsigned long difference_in_milliseconds_alt = time_difference_in_milliseconds(
    demo_end_video_frame_time_second,test_alternate_end);

  if (difference_in_milliseconds_alt <=0) {
    demo_next_end_video_frame_time = test_alternate_end;
    demo_next_end_sub_millisecond_video_time= test_subfudge;
    }
 
}    

void assign_next_demo_video_times() {    
  unsigned long difference_in_milliseconds_alt = time_difference_in_milliseconds(
    demo_end_video_frame_time_second,demo_end_video_frame_time);
  if (difference_in_milliseconds_alt <=0) {
    fprintf(stderr,"difference in milliseonsa alt is %ld  (should be 0 or -1) \n",
       difference_in_milliseconds_alt);
    demo_start_video_frame_time_second = demo_end_video_frame_time_second;
    demo_start_sub_millisecond_video_time_second =  demo_end_sub_millisecond_video_time_second;
    
    demo_end_video_frame_time_second = add_milliseconds_to_time(
        demo_end_video_frame_time_second,(long)1000000);
    demo_end_sub_millisecond_video_time_second = demo_start_sub_millisecond_video_time_second;

    demo_start_video_frame_time = demo_end_video_frame_time;
    demo_start_sub_millisecond_video_time = demo_end_sub_millisecond_video_time;
    }
  else {
    demo_start_video_frame_time = demo_end_video_frame_time;
    demo_start_sub_millisecond_video_time = demo_end_sub_millisecond_video_time;
    }
  demo_end_video_frame_time = demo_next_end_video_frame_time;
  demo_end_sub_millisecond_video_time = demo_next_end_sub_millisecond_video_time;
      
}

void NativeInitDemo(int argc, char *argv[],int the_make_demo_mode) {
    make_demo_mode = the_make_demo_mode;
if (make_demo_mode==0) {
  demo_frame_time = (struct timeval){-1,0}; // unused
  demo_time_of_day = (struct timeval){-1,0}; //unused
  demo_start_time = (struct timeval){-1,0};
  demo_end_time =(struct timeval){-1,0};
  demo_fps=1.; // unused. fps tries for 60 but is variable because display varies
  demo_start_time_seconds = -1.;
  demo_length_seconds = -1.;
  demo_turn_on_disco_info_time_start =  (struct timeval){-1,0};
  demo_turn_off_disco_info_time_start =  (struct timeval){-1,0};
  demo_turn_on_disco_info_time_end =  (struct timeval){-1,0};
  demo_turn_off_disco_info_time_end =  (struct timeval){-1,0};
  }
else if (make_demo_mode==1) {
  demo_fps=30.;
  demo_start_time_seconds = 15.;
  demo_length_seconds = 20.;
  demo_start_time = (struct timeval){
     ((unsigned long)demo_start_time_seconds)
     ,0};
  demo_end_time =(struct timeval){
          ((unsigned long)demo_start_time_seconds+demo_length_seconds)
      ,0};
	  
  demo_turn_on_disco_info_time_start =  (struct timeval){-1,0};
  demo_turn_off_disco_info_time_start =  (struct timeval){-1,0};
  demo_turn_on_disco_info_time_end =  (struct timeval){-1,0};
  demo_turn_off_disco_info_time_end =  (struct timeval){-1,0};
  }
else if (make_demo_mode==2) {
  demo_fps=30.;
  demo_start_time_seconds = 0.;
  demo_length_seconds = 100000000.;
  demo_start_time = (struct timeval){
     ((unsigned long)demo_start_time_seconds)
     ,0};
  demo_end_time =(struct timeval){
          ((unsigned long)demo_start_time_seconds+demo_length_seconds)
      ,0};
	  
  
  }

  
    
    
if (argc==5) {
  demo_start_time_seconds = atof(argv[2]);
  demo_length_seconds = atof(argv[3]);
  
  sr_gettimeofday(&demo_start_time,NULL);
  
  { // block to handle microseconds
    unsigned long demo_s = demo_start_time_seconds;
    demo_start_time.tv_sec += demo_s;
    double demo_sd= demo_s;
    double demo_usd = (demo_start_time_seconds - demo_sd) *1000000.;
    unsigned long demo_us = demo_usd;
    demo_start_time.tv_usec += demo_us;
    if (demo_start_time.tv_usec >1000000) {
      demo_start_time.tv_usec -= 1000000;
      demo_start_time.tv_sec++;
      }
    }
    
  demo_end_time = demo_start_time;
  if (demo_length_seconds ==0) demo_length_seconds=10000000.; // might determine the real length here
  // pass in real length + some fudge
  { // block to handle microseconds addition - we have a function for this
    unsigned long demo_s = demo_length_seconds;
    demo_end_time.tv_sec += demo_s;
    double demo_sd= demo_s;
    double demo_usd = (demo_length_seconds - demo_sd) *1000000.;
    unsigned long demo_us = demo_usd;
    demo_end_time.tv_usec += demo_us;
    if (demo_end_time.tv_usec >1000000) {
      demo_end_time.tv_usec -= 1000000;
      demo_end_time.tv_sec++;
      }
    }
    
  
  fprintf(stderr,"song:%s\n	from %lf length %lf resolution %dx%d \n"
"  	Title: %s\n",
     argv[1],demo_start_time_seconds,demo_length_seconds,
     demo_screen_width,demo_screen_height,
     argv[4]);
  strcpy(new_display_title_text,argv[4]);
  strcpy(current_display_title_text,argv[4]);
  change_display_title_text=1;
  }
else {
  fprintf(stderr,"bad usage: should be %s <file_or_url> "
     " <start_time_seconds> <length_seconds> \"<title text>\"\n"
     "Where:\n"
     	"<file_or_url>	 is the file name or url to receive\n"
     	"<start_time_seconds> is the start time in seconds (can have decimals)\n"
	"                      0 for start time will start at beginning\n"
	"<length_time_seconds> is the length tile in seconds (can have decimmals)\n"
	"                      0 for length will produce full\n",argv[0]);
  exit(1);
  }


    
if (make_demo_mode) {
  demo_turn_on_disco_info_time_start =  (struct timeval){-1,0};
  demo_turn_off_disco_info_time_start =  (struct timeval){-1,0};
  demo_turn_on_disco_info_time_end =  (struct timeval){-1,0};
  demo_turn_off_disco_info_time_end =  (struct timeval){-1,0};
  demo_turn_on_disco_info_time_start =  demo_start_time;
  demo_turn_off_disco_info_time_start =  (struct timeval){
     demo_start_time.tv_sec+1000000,0};
  demo_turn_on_disco_info_time_end =  (struct timeval){-1,0};
  demo_turn_off_disco_info_time_end =  (struct timeval){-1,0};
  
  sr_gettimeofday(&demo_start_video_frame_time,NULL);
  demo_start_sub_millisecond_video_time = 0.; // to handle real weird time frames
  demo_frame_time = demo_start_video_frame_time;
  
  demo_start_video_frame_time_second = demo_start_video_frame_time;
  demo_start_sub_millisecond_video_time_second = 0.;
  
  demo_end_video_frame_time_second = add_milliseconds_to_time(demo_start_video_frame_time_second,(long)1000000);
  demo_end_sub_millisecond_video_time_second = demo_start_sub_millisecond_video_time_second;
  
  // demo_time_of_day is implicitly set   
  }
if (demo_fps==0.) demo_fps=0.01;

{ // block to  compute the factors to make sure video frames are accurate
  one_over_demo_fps = 1.0 / demo_fps;
  double temp = 1000000. / demo_fps;
  frametime_in_microseconds = temp;
  do {
    double temp2 = frametime_in_microseconds; /* clipped of decimal part */
    fudge_in_microseconds_d = 1000000. - (temp2 * demo_fps);
    if (fudge_in_microseconds_d<0.) {
      fprintf(stderr,"fudge_in_microseconds negative %lf - error\n",fudge_in_microseconds_d);
      frametime_in_microseconds--;
      if (frametime_in_microseconds<0) {
        fprintf(stderr,"framerate less than a microdecond error\n");
        exit(-1);
	}
      continue;
      }
    do {
      fudge_in_microseconds = fudge_in_microseconds_d;
      double ss = fudge_in_microseconds;
      sub_fudge_in_microseconds_d = fudge_in_microseconds_d-ss;
      if (sub_fudge_in_microseconds_d<0.) {
         fprintf(stderr,"weird - subfudge is less than 0: %lf\n",sub_fudge_in_microseconds_d);
         sub_fudge_in_microseconds_d = sub_fudge_in_microseconds_d + 1.;
         fudge_in_microseconds--;
         if (fudge_in_microseconds<0) {
           fprintf(stderr,"second part fudge_in_microseconds negative %lf - error\n",fudge_in_microseconds_d);
           frametime_in_microseconds--;
           if (frametime_in_microseconds<0) {
             fprintf(stderr,"framerate less than a microdecond error\n");
             exit(-1);
	     }
	   }
	 }
      } while ((sub_fudge_in_microseconds_d<0.)||(fudge_in_microseconds_d<0.));
    } while (fudge_in_microseconds_d<0.);
  // Ok here we have a positive fudge in mucroseconds, and a positive sub fudge in microseoncd
  // guarunteed - makes adding things easier
  } // block to compute the factors for incrementing video time

compute_next_demo_end_video_time();
// normally we assign after we do a video frame with assign_next_demo_video_times,  BUT
// this is the first frame that starts when we start, and so
// we just need (usually) to set the end video frame
// note:sub second frame rates needs adjustement
demo_end_video_frame_time = demo_next_end_video_frame_time;
demo_end_sub_millisecond_video_time = demo_next_end_sub_millisecond_video_time;

fprintf(stderr,"from %ld,%ld(%lf) to %ld,%ld(%lf)\n",
    demo_start_video_frame_time.tv_sec,
    demo_start_video_frame_time.tv_usec,
    demo_start_sub_millisecond_video_time,
    demo_start_video_frame_time.tv_sec,
    demo_start_video_frame_time.tv_usec,
    demo_start_sub_millisecond_video_time);

	
/* ^^^^ argument processing ^^^ */		        

    init_processor();
    timer_on_polling_flag=1;
      
#ifdef SR_READ_FILE    
    if (argc>=2) {
      if ((argv[1])&&(argv[1][0])) {
        init_packet_source(argv[1],0);
	}
      }
    else {
#endif
      char *url=NULL; 
#ifndef NO_GRAPHICS
      url=top_menu_url(); // from top menu preferences
#endif      
      init_packet_source(url,0);
#ifdef SR_READ_FILE    
    }
#endif

    
#ifdef SR_SOUND
    init_receiver_sound(1);
#endif    
    init_packet_summary();
    soundPlayCool=1;
}







void NativeInitThreadAll(int argc,char *argv[]) {
//    write_file_spec =fopen("recieve.srd","w");

    init_processor();
#ifdef SR_SOUND      
    {
      pthread_t tid[1];
      soundPlayCool=1;
      int err;
      err = pthread_create(tid,NULL,&SoundPlay,NULL);
      if (err != 0) 
         logit("\ncan't create thread :[%s]", strerror(err));
      }
#endif
#ifdef SR_READ_FILE    
    if ((argc==2)&&(argv[1])) {

        pthread_t nid[1];

        int err;
        if (strchr(argv[1],':')) {
          err = pthread_create(nid,NULL,&WebStreamStuff,argv[1]);
          if (err != 0) 
             logit("\ncan't create file reading web stream :[%s]", strerror(err));
	  }
	else {
          err = pthread_create(nid,NULL,&ReadFileStuff,argv[1]);
          if (err != 0) 
             logit("\ncan't create file reading thread :[%s]", strerror(err));
	  }
        }
      else {
#endif

    {
      pthread_t nid[1];

      int err;
      err = pthread_create(nid,NULL,&NetworkStuff,NULL);
      if (err != 0) 
         logit("\ncan't create network thread :[%s]", strerror(err));
      }

#ifdef SR_READ_FILE    
    }
#endif
}

void NativeFinish() {
#ifdef SR_READ_FILE    
    if (we_are_reading_file) {
      shutdown_file_reader();
      }
    else if (we_are_streaming_web) {
      shutdown_web_stream();
      }
    else {
#endif    
    shutdown_receiver_internet();
#ifdef SR_READ_FILE    
}
#endif
    shutdown_processor();
}




int NativeVibratingFlag() {
    int current=soundringcurrent;
    if (current==-1) return(0);  /* cant parse */
    volatile unsigned char *packet = commandring[current];
    int sampleSize=commandlen[current];
    if (sampleSize<36) return(0);
    int flag;
    { /* compute flag */
        int l = sampleSize-8;
        unsigned int i1=packet[l]&0xff;
        unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;
        i1 = i1 | i2 | i3 | i4; /* use all 4 packets so we don't miss a beat */
        flag = i1+(i2<<8)+(i3<<16)+(i4<<24);

    }
    if (!(flag&2)) { /* if we are not on a beat now */
        return(0);
    }
    return(1);

}

int NativeDbLevel() {
    int current=soundringcurrent;
    if (current==-1) return(7734);  /* cant parse */
    volatile unsigned char *packet = commandring[current];
    int sampleSize=commandlen[current];
    if (sampleSize<36) return(7734);
    int db_level_int;
    {
        int l = sampleSize - 12;
        int i1 = packet[l] & 0xff;
        int i2 = packet[l + 1] & 0xff;
        int i3 = packet[l + 2] & 0xff;
        int i4 = packet[l + 3] & 0xff;
        db_level_int = i1 + (i2 << 8) + (i3 << 16) + (i4 << 24);
    }

    return(db_level_int);

}

#define MINFREQ 60.0f
#define MAXFREQ 20000.0f

static inline float floatToFrequency(float value) {
    if (value > 0.97f) return MAXFREQ;
    if (value < 0.03f) return MINFREQ;
    value = powf(10.0f, (value + ((0.4f - fabsf(value - 0.4f)) * 0.3f)) * log10f(MAXFREQ - MINFREQ)) + MINFREQ;
    return value < MAXFREQ ? value : MAXFREQ;
}

extern void process_packet();



int NativeWork() {
    /* loop up to 3 times to get receiver packets - in case they are queued up */
#ifdef SR_READ_FILE    
    if (got_new_source) {
//      if (new_source) {
//          fprintf(stderr,"source is down!!!!!!!!!!!!!!!!!!!!!!! change to %s\n",new_source);
//	  }
//      else {
//	  fprintf(stderr,"source is down!!!!!!! change to wifi\n");
//	  }
      got_new_source=0;
      init_packet_source(new_source,new_block);
      }
#endif      

int i=0;
while (1) {
	if (i>10) break;
#ifdef SR_READ_FILE    
    if (we_are_reading_file) {
      if (we_are_reading_file==-1) {
        int queue_size = soundringtail-soundringhead;
        if (queue_size<0) queue_size += SOUNDRING_COUNT;
        if (queue_size==0) {  demo_processing_flag=0;     exit(0);}
	}
      if (get_packet_from_file()) {

	
        process_packet_ignore_framestart();
        i++;
        }
      else {
        break;
	}
      }
    else if (we_are_streaming_web) {
      if(we_are_streaming_web==-1) {break;}
      if (get_packet_from_web_stream()) {
//        fprintf(stderr,"yeah\n");
        process_packet();
        i++;
        }
      else {
        break;
	}
      }
    else {
#endif        
    if (get_internet_receiver_packet()) {
        process_packet();
        i++;
        compute_packet_summary();
        }
    else {
        compute_packet_summary();
        break;
        }
#ifdef SR_READ_FILE    
}
#endif
  } // while looping
    
    
watchdog_point=5;
return(i);
}





#ifndef NO_GRAPHICS
int Renderer_Step_Demo();
int Renderer_Step();
#endif



int NativeWorkDemo() {
#ifdef SR_READ_FILE    
if (got_new_source) {
  got_new_source=0;
  init_packet_source(new_source,new_block);
  }
#endif      

#ifdef SR_READ_FILE    
if (we_are_reading_file) {
  if (we_are_reading_file==-1) {
    int queue_size = soundringtail-soundringcurrent;
    if (queue_size<0) queue_size += SOUNDRING_COUNT;
    if (queue_size==0) {     demo_processing_flag=0;  exit(0);}
    }
  else {
    if (get_packet_from_file()) {	
      process_packet_ignore_framestart();
      }
    else {
      we_are_reading_file=-1;
      }
    }
#ifdef SR_SOUND
  if (playOneSoundBuffer()) {
    increase_demo_time(5000);
    logit("Work:demotime_of_day %ld.%ld\n",demo_time_of_day.tv_sec,
      demo_time_of_day.tv_usec);
    logit("      demo_end_video_frame_time %ld.%ld\n",demo_end_video_frame_time.tv_sec,
      demo_end_video_frame_time.tv_usec);
    if (time_difference_in_milliseconds(demo_end_video_frame_time,
      demo_time_of_day)>0) {
      struct timeval tt = demo_time_of_day;
      demo_time_of_day = demo_frame_time;
      
      logit("Work:compute_next_demo_end_video_time\n");

      compute_next_demo_end_video_time();
#ifndef NO_GRAPHICS

      logit("Work:Renderer_step_demo\n");
      Renderer_Step_Demo();

#endif      
      demo_time_of_day = tt;
      
      assign_next_demo_video_times();

      }
    }  /* while processing sound buffers */
#endif
  }
#endif

    
    
watchdog_point=5;
return(demo_processing_flag); // currently always is 1 - but could turn to 0 for things that dont receive
}



void NativeSoundStart() {

}


void NativeSoundStop() {

}





void NativeSetIPAddress(int ipv4) {
/* note the byte order is reverse what it is in android java */
unsigned int ipv4x = ipv4;
int i4=ipv4x&255;
ipv4x= ipv4x>>8;
int i3=ipv4x&255;
ipv4x= ipv4x>>8;
int i2=ipv4x&255;
ipv4x= ipv4x>>8;
int i1=ipv4x&255;
ipv4x= ipv4x>>8;
ip4_last16 = (i2<<8)+i1;
ip6_last16=ip4_last16;
// WORLD_INTERNATIONAL off
sprintf(ip4_address,"%d.%d.%d.%d",i4,i3,i2,i1);
// WORLD_INTERNATIONAL on
ip6_address[0]='\0';

}



int do_sql(char *statement) {
char *error_message;
int rc;
#ifdef ZERO_SQL
logit("%s",statement);
rc= sqlite3_exec(database,statement,do_callback,0,&error_message);
if (rc!=SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", error_message);
      sqlite3_free(error_message);
      return(rc);
    }
#endif
return(0);
}

int select_int_sql(char *command) {
char *error_message;
#ifdef ZERO_SQL
return(sqllite3_one_int(database,command,do_callback,0,error_message));
#else
return(0);
#endif
}

int originally_landscape=1;
struct preferences *main_preferences;



extern FileData get_file_data(const char* path);

FileData get_file_data(const char* path) {
        if(path == NULL) { return   (FileData) {0l, NULL, NULL}; }
	

//#define WORLD_display_default_rb "rb"
        FILE* stream = fopen(path, WORLD_display_default_rb);
	
        if (stream == NULL) return   (FileData) {0l, NULL, NULL};
        fseek(stream, 0, SEEK_END);
        long stream_size = ftell(stream);
        fseek(stream, 0, SEEK_SET);

        void* buffer = malloc(stream_size);
	
        size_t r =fread(buffer, (size_t)stream_size, (size_t)1, stream);
        if (ferror(stream) != 0) {
	  free(buffer);
	   return   (FileData) {0l, NULL, NULL};
	   }
        fclose(stream);

        return (FileData) {stream_size, buffer, NULL};
}



void release_file_data(const FileData* file_data) {
        if (file_data == NULL) return;
        if(file_data->data == NULL) return;

        free((void*)file_data->data);
//	file_data->data = NULL;
	
}

		
		
extern void release_file_data(const FileData* file_data);
void release_asset_data(const FileData* file_data) {
        if (file_data == NULL) return;
//        release_file_data(file_data);
}


FileData empty;
FileData get_asset_data(const char* relative_path) {
        if(relative_path == NULL) {
	     (FileData) {0l, NULL, NULL};
	     }
        char new_path[20000];
        strcpy(new_path,relative_path);
//        get_path_for_resource_file(new_path);
//#define WORLD_glue_alsa_ ""
//strcpy(new_path,WORLD_glue_alsa_);

        logit("file from %s   to %s",relative_path,new_path);
        return get_file_data(new_path);
//return(empty);
}


void glue_show_url_and_keep_playing(char *url) {
char workspace[4000];
//#define WORLD__slash_usr_slash_bin_slash_firefox__dqt__pct_s_dqt___amp__lf_ "/usr/bin/firefox \"%s\" &\n"
sprintf(workspace,WORLD__slash_usr_slash_bin_slash_firefox__dqt__pct_s_dqt___amp__lf_,url);
int x=system(workspace);
}

void glue_show_url_and_stop_playing(char *url) {
glue_show_url_and_keep_playing(url);
sleep(7);
exit(0);
}


