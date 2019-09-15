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
#define MAXFRAMES 12

#define SR_READ_FILE
/* ^^^ allows reading a file from the command line or - for standard input */


// SHOW_FRAMES is not defined
extern int watchdog_point; /* used to identify where we freeze up in the receiver thread */

extern void NativeInit(int argc,char *argv[]);
extern void NativeFinish();
extern int NativeWork();
extern void NativeWorkx();

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

#endif //DASOUND24_GLUE_H
