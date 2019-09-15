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
#ifndef DASOUND24_GLUE_H
#define DASOUND24_GLUE_H
#include "main.h"
#include "preferences.h"

#define MAXFRAMES 36
#define MAXFRAMES_CHECKSUM 12
extern struct preferences *main_preferences;
#define PER_FRAME_OVERHEAD 16
#include <stdio.h>

#define logit(...) {fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\n");}

extern jobject database;
extern JNIEnv *databaseEnvironment;

extern int ip4_last16;
extern int ip6_last16;
extern char ip4_address[50];
extern char ip6_address[50];

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

typedef jobject sqlite3;
extern int sqlite3_exec(sqlite3 db,
char *statement,int *callback(void *NotUsed, int argc, char **argv, char **azColName),
  int flag,char *ErrorMessage) ;

extern void fake_init_receiver_internet(int block);
extern int fake_get_internet_receiver_packet();
extern void fake_shutdown_receiver_internet();

#endif //DASOUND24_GLUE_H


