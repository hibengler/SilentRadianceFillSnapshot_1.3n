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

/*$Revision: 1.14 $*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
// #include <android/log.h>

// this is it

// for native asset manager
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"

#ifdef INCLUDE_VISUALIZER
#include "soundin_portaudio.h"
#endif


#ifndef SR_NOSOUND
#define SR_SOUND
#endif

#include "glue.h"
#ifdef _WIN32
#include "win64_network_receiver.h"
#else
#include "network_receiver.h"
#endif

#ifdef SR_READ_FILE    
#include "file_receiver.h"
#endif
#include "curl_web_stream.h"

#include "processor.h"

#ifdef SR_SOUND
#include "sound_alsa.h"
#endif

#include "packet_summary.h"

#include <SDL2/SDL_thread.h>

int watchdog_point=0; // Used for debugging when we freeze up on the receiver thread.

float xdpi = 24; /* set from ios/android prior to initializing the c stuff. This helps us get the aspect ratio of items perfect */
float ydpi = 24; /* set from ios/android prior to initializing the c stuff. This helps us get the aspect ratio of items perfect */

int got_new_source = 0; // set to 1 when we got a new source to do
char *new_source = NULL; // set to a new source to apply when got a new source
int new_block =0;  // set to the next block value

int audio_len=0;
int audio_done=0;


int ip4_last16;
char ip4_address[50];
int ip6_last16;
char ip6_address[50];

volatile int soundPlayCool=0;



int NetworkStuff(void *arg)
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
return 0;
}



#ifdef SR_READ_FILE    
int ReadFileStuff(void *arg)
{
init_packet_summary();
char *filename = (char *)arg;
init_receiver_file(filename);
while (1) {
  NativeWork();
//  fprintf(stderr,".");
  }
return 0;
}






int WebStreamStuff(void *arg)
{
init_packet_summary();
char *filename = (char *)arg;
init_web_stream(filename);
while (1) {
  NativeWork();
//  fprintf(stderr,".");
  }
return 0;
}
#endif


void NativeInit(int argc, char *argv[]) {
//    write_file_spec =fopen("recieve.srd","w");



    init_processor();
    timer_on_polling_flag=1;
#ifdef SR_READ_FILE    
    if (argc==2) {
      if (argv[1]) {
        if (strchr(argv[1],':')) {
	  init_web_stream(argv[1]);
	  }
	else {
          init_receiver_file(argv[1]);
	  }
	}
      }
    else {
#endif
    init_receiver_internet(1);
#ifdef SR_READ_FILE    
    }
#endif
#ifdef SR_SOUND      
    {
      soundPlayCool=1;
      init_receiver_sound(1);
      init_packet_summary();
      }
#else
   init_packet_summary();   
#endif

}

SDL_Thread *sdl_thread;

void NativeInitThreadAll(int argc,char *argv[]) {
//    write_file_spec =fopen("recieve.srd","w");

    init_processor();
#ifdef SR_SOUND      
    {
      soundPlayCool=1;
    init_receiver_sound(1);
      }
#endif
    init_packet_summary();
    if ((argc==2)&&(argv[1])) {

        int err;
        if (strchr(argv[1],':')) {
          char *filename = (char *)argv[1];
//	  sdl_thread = SDL_CreateThread(&WebStreamStuff,"webstreamstuff",(void *)filename);
          //init_packet_summary();
 //         init_web_stream(filename);
          }	  
	else {
          //init_packet_summary();
          char *filename = (char *)argv[1];
//	  sdl_thread = SDL_CreateThread(&ReadFileStuff,"readfilestuff",(void *)filename);
//          init_receiver_file(filename);
	  }
        }
      else {
        fprintf(stderr,"internet up\n");
//	sdl_thread = SDL_CreateThread(&NetworkStuff,"Networkstuff",(void *)NULL);
  //      init_receiver_internet(1);
        }
}





void NativeFinish() {
    if (we_are_reading_file) {
      shutdown_file_reader();
      }
    else if (we_are_streaming_web) {
      shutdown_web_stream();
      }
    else {
      shutdown_receiver_internet();
      }
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


int c;
int NativeWork() {
    /* loop up to 3 times to get receiver packets - in case they are queued up */
int i=0;
while (1) {
    if (we_are_reading_file) {
      if (we_are_reading_file==-1) {
        int queue_size = soundringtail-soundringhead;
        if (queue_size<0) queue_size += SOUNDRING_COUNT;
        if (queue_size==0) {       exit(0);}
	}
      if (get_packet_from_file()) {
//        process_packet_ignore_framestart();
        i++;
        }
      else {
        break;
	}
      }
    else if (we_are_streaming_web) {
      if (get_packet_from_web_stream()) {
        process_packet();
        i++;
        }
      else {
//        fprintf(stderr,"n");
        break;
	}
      }
    else {
      if (get_internet_receiver_packet()) {
        fprintf(stderr,"!");	
        process_packet();
        i++;
        }
      else {
        break;
        }
      }
    c=c+1;
    if ((c%100)==99) {
      int queue_size = soundringtail-soundringhead;
      if (queue_size<0) queue_size += SOUNDRING_COUNT;
//      fprintf(stderr,"\n%d\n",queue_size);
      }
    if (i>3) break;
    }
//    Sleep(10);
if(i) compute_packet_summary();
watchdog_point=5;
return(i);
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



		
		
extern void release_file_data(const FileData* file_data);
void release_asset_data(const FileData* file_data) {
        assert(file_data != NULL);
//        release_file_data(file_data);
}


FileData empty;
FileData get_asset_data(const char* relative_path) {
        assert(relative_path != NULL);
        char new_path[20000];
        strcpy(new_path,relative_path);
//        get_path_for_resource_file(new_path);
//#define WORLD_glue_win64_ ""
strcpy(new_path,WORLD_glue_win64_);
        logit("file from %s   to %s",relative_path,new_path);
//        return get_file_data(new_path);
return(empty);
}




#ifdef INCLUDE_VISUALIZER

void NativeInitVisualizer(int argc, char *argv[]) {
soundin_init();
timer_on_polling_flag=1;
init_packet_summary();   
packet_summary_ignore_queue_size=1;
}



void NativeWorkVisualizer() {
soundin_process_aubio();
compute_packet_summary();   
}

void glue_up_volume() {
soundin_factor *= 1.166666666666f;
}

void glue_down_volume() {
soundin_factor /= 1.1125f;
if (soundin_factor<1.) {
  soundin_factor=1.;
  }
}

void NativeFinishVisualizer(int argc, char *argv[]) {

packet_summary_ignore_queue_size=0;
}
#else 


void glue_up_volume() {}
void glue_down_volume() {}

#endif




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



