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


#include <emscripten.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>


/* for testing - DO_NOTING tries to make an app that does absolutly nothing - 
trying to lock down performance */
//#define DO_NOTHING 1
#define DO_NO_WORK 1

/* for testing - use da file or read from web snakey7*/
//#define USE_FILE

// for native asset manager
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"
#include "curl_web_stream.h"

#ifndef SR_NOSOUND
#define SR_SOUND
#endif

#include "glue.h"
#include "network_receiver.h"

#define SR_READ_FILE

#ifdef SR_READ_FILE    

#include "file_receiver.h"
#endif

#include "processor.h"

#ifdef SR_SOUND
#include "sound_sdl.h"
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



void *SoundPlay(void *arg)
{return NULL;}


void *NetworkStuff(void *arg)
{
init_packet_summary();
init_receiver_internet(1);
while (1) {
  NativeWork();
  }
return NULL;
}


#ifdef SR_READ_FILE    
void *ReadFileStuff(void *arg)
{
init_packet_summary();
char *filename = (char *)arg;
init_receiver_file(filename);
fprintf(stderr,"ok file open\n");
while (1) {
  fprintf(stderr,".");
  NativeWork();
  }
return NULL;
}





void *WebStreamStuff(void *arg)
{
init_packet_summary();
char *filename = (char *)arg;
init_web_stream(filename);
while (1) {
  fprintf(stderr,"ok file opend\n");
  NativeWork();
  fprintf(stderr,".");
  }
return NULL;
}
#endif


void NativeInit(int argc, char *argv[]) {
#ifdef DO_NOTHING
exit(0);
#endif

//#define WORLD_download7_dot_cgi "download7.cgi"
char *dd=WORLD_download7_dot_cgi;
fprintf(stderr,"going to read %s\n",dd);

    init_processor();
    timer_on_polling_flag=1; // ???
    
fprintf(stderr,"opening url\n");
init_web_stream(dd);

init_receiver_sound(1);
init_packet_summary();   
fprintf(stderr,"ok\n");
}



void NativeInitThreadAll(int argc,char *argv[]) {
//    write_file_spec =fopen("recieve.srd","w");

    init_processor();
#ifdef SR_SOUND      
    {
      pthread_t tid[1];
      soundPlayCool=1;
      int err;
    init_receiver_sound(1);
    init_packet_summary();
      }
#endif
#ifdef SR_READ_FILE    
    if (argc==2) {
      if (argv[1]) {

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

int audio_len=0;
int audio_done=0;
int read_files=0;
int process_packets=0;
int fff=0;
void NativeWorkx() {
#ifdef DO_NOTHING
return;
#endif
#ifdef DO_NO_WORK
return;
#endif
NativeWork();
fff++;
if (fff==100) {
          int queue_size = soundringtail-soundringhead;
        if (queue_size<0) queue_size += SOUNDRING_COUNT;
  fprintf(stderr,"still going read %d   queue size %d\n",we_are_reading_file,queue_size);
  fprintf(stderr,"read files %d processed packets %d   audio done %d len %d\n",read_files,process_packets,audio_done,audio_len);

  fff = 0;
  read_files=0;
  process_packets=0;
  audio_done=0;
  audio_len=0;
  }
}

int NativeWork() {
    /* loop up to 3 times to get receiver packets - in case they are queued up */


//fprintf(stderr,"ok work %d\n",we_are_reading_file);
int i=0;
{
  if (soundPlayCool) {
    //playOneSoundBuffer();
    }
#ifdef SR_READ_FILE    
    if (we_are_reading_file) {
fprintf(stderr,"aeeeee\n");
      if (we_are_reading_file==-1) {
         int queue_size = soundringtail-soundringhead;
        if (queue_size<0) queue_size += SOUNDRING_COUNT;
        if (queue_size==0) {    fprintf(stderr,"all done\n");   exit(0);}
	}
      int max=5;
      while (get_packet_from_file()) {	
        read_files++;
        process_packet_ignore_framestart(); process_packets++;
	
        i++;
	max--;
	if (!max) break;
        }
      }
    else if (we_are_streaming_web) {
      if (we_are_reading_file==-1) {
        int queue_size = soundringtail-soundringhead;
        if (queue_size<0) queue_size += SOUNDRING_COUNT;
        if (queue_size==0) {       exit(0);}
	}
	
      int max=5;
      while (get_packet_from_web_stream()) {	
        read_files++;
	fprintf(stderr,"did good\n");
        process_packet_ignore_framestart(); process_packets++;
        i++;
	max--;
	if (!max) break;	
        }
      }
    else {
#endif        
    int max=20;
    while (get_internet_receiver_packet()) {
        process_packets++;
        process_packet();
	max--;
	if (!max) break;
        i++;
        }
     	
     compute_packet_summary();

#ifdef SR_READ_FILE    
}
#endif
    }
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
//#define WORLD_glue_js_ ""
strcpy(new_path,WORLD_glue_js_);
        logit("file from %s   to %s",relative_path,new_path);
//        return get_file_data(new_path);
return(empty);
}

