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
//
// Created by hib on 6/15/16.
//

#ifndef DASOUND24_GLUE_H
#define DASOUND24_GLUE_H


#define LINUX_CLIENT 1
#define SHOW_SPLASH 1
#define MAXFRAMES 12


// sometimes we use this to make apple screen shote, because it is easier to run.
// so we need special resolutions, not fullscreen, etc
//#define LINUX_SIMULATE_APPLE

//#define LINUX_SIMULATED_APPLE_IS_6_POINT_5_VERTICAL
// or
//#define LINUX_SIMULATED_APPLE_IS_6_POINT_5_HORIZONTAL
// or
//#define LINUX_SIMULATED_APPLE_IS_12_POINT_9_GEN_3_VERTICAL
// or
//#define LINUX_SIMULATED_APPLE_IS_12_POINT_9_GEN_3_HORIZONTAL




#include "world_internationalization.h"

#ifdef __cplusplus
extern "C" {
#endif



#define COLLECT_BPG_IMAGES 1
// ^ set here in glue or on make - this makes up collect the bpg images for showing.
#ifdef NO_SOUND
 #undef COLLECT_BPG_IMAGES
#else
#ifdef NO_GRAPHICS
 #undef COLLECT_BPG_IMAGES
#else
#define COLLECT_BPG_IMAGES 1
#endif
#endif




#define SR_READ_FILE
/* ^^^ allows reading a file from the command line or - for standard input */

extern int i_am_a_tv; // ignore in linux. for android

extern int watchdog_point; /* used to identify where we freeze up in the receiver thread */

extern void NativeInit(int argc,char *argv[]);
extern void NativeFinish();
extern int NativeWork();

#define PER_FRAME_OVERHEAD 16
#define MAXFRAMES_CHECKSUM 12


#include <stdio.h>

#define logit(...) {fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\n");}

extern int ip4_last16;
extern int ip6_last16;
extern char ip4_address[50];
extern char ip6_address[50];

extern int do_sql(char *statement);

extern int select_int_sql(char *command);




typedef struct {
        const long data_length;
        const void* data;
        const void* file_handle;
} FileData;

void NativeInitThreadAll(int argc, char *argv[]);



extern float xdpi; /* pulled from the java system, helps us figure out the aspect ratio */
extern float ydpi; /* aspect ratio stuff */
extern int lightning_flag;
extern int originally_landscape;

extern int pick_wifi_network_flag; /* set to 1 if someone presses the big smile face.  Then
        on the front end, we make a call to choose the network */


FileData get_asset_data(const char* relative_path);
void release_asset_data(const FileData* file_data);
extern int do_sql(char *statement);

extern int select_int_sql(char *command);

extern struct preferences *main_preferences;


extern int init_packet_source(char *url_or_null_if_wifi,int block);
extern int init_packet_source_block_flag; // flag when last called 

extern int switch_packet_source(char *url_or_null_if_wifi,int block); // close the source, and then wait then open a new one

void NativeInitDemo(int argc, char *argv[],int demo_processing_flag);
// demo_processing_flag is 0,1, or 2

int NativeWorkDemo();


#ifndef SR_NOSOUND
extern long sound_delay_frames;
#endif


extern void glue_show_url_and_keep_playing(char *url);
extern void glue_show_url_and_stop_playing(char *url);


// there is a different demo_p
/* demo mode has a different time of day which is determined by the output of demo_sound - 
  it increases 5000 with each demo sound
  demo_mode doesn't process things in the background - it is linear
  and with each sound packet outputted, 1/200th of a second is added to demo_time_of_day
  */
extern volatile struct timeval demo_time_of_day;  // set to last time of day called
                                         // or set to a certain time of day because of make_demo_mode

extern int make_demo_mode; // set to 1 for standard 480x480 demo, 1 for mtv like 1920x1080 60fps demo
extern int demo_screen_width;  // in glue defaults to 480 - but changed to 1920 for mtv version
extern int demo_screen_height; // in glue - defaults to 480 - but 1920 for demo type version

extern double demo_fps; //=30.f;  could be different
extern double demo_start_time_seconds; // =15.f; , or -1 for mtv demo
extern double demo_length_seconds;   // 20.f; , or -1 for mtv demo
extern struct timeval demo_start_time;
extern struct timeval demo_end_time;

extern struct timeval demo_start_video_frame_time; // start time, relative to the current gettime when starting
extern struct timeval demo_end_video_frame_time;  // end time, relative ...
extern struct timeval demo_turn_on_disco_info_time_start; // normally demo_start_time anbd demo_end_time fopr type 1
                                   // mtv will be 10 seconds before and 10 seconds after
extern struct timeval demo_turn_off_disco_info_time_start; // normally -1 sec demo_end_time
                                  // mtv will be 10 seconds before demo_end_time to demo_end_time
extern struct timeval demo_turn_on_disco_info_time_end; // normally demo_start_time anbd demo_end_time fopr type 1
                                   // mtv will be 10 seconds before and 10 seconds after
extern struct timeval demo_turn_off_disco_info_time_end; // normally -1 sec demo_end_time
                                  // mtv will be 10 seconds before demo_end_time to demo_end_time

extern struct timeval demo_frame_time;

#ifdef __cplusplus
}
#endif



#endif
