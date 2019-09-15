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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#include "shm_communication.h"


int main(int argc,char *argv[]) {
if (argc<=3) {
  fprintf(stderr,"Usage: sr_send_bgp_binary <command_char> <image_id> <ntimes>\n");
  fprintf(stderr,"command_char is i for images\n");
  fprintf(stderr,"image_id is 0-65534, or 65535 (-1) for off\n");
  exit(1);
  }
char buf[200000];
buf[0]='\0';

char command = argv[1][0];
int image_id = atoi(argv[2]);
if ((image_id<0)||(image_id> 65535)) {
  fprintf(stderr,"image_id out of range\n");
  exit(1);
  }

int ntimes = atoi(argv[3]);
if (ntimes<0) ntimes=1;
int repeats=1;



struct silent_radiance *sr;
sr = get_silent_radiance_shared_memory("default");
if (!sr) return(-2);
struct silent_radiance_client *sr_client;
sr_client = silent_radiance_get_client(sr);
if (!sr_client) {
  detatch_slient_radiance_shared_memory(sr);
  return(-2);
  }


int iteration;
for (iteration=0;iteration<ntimes;iteration++) {
 

  int i=0;
  buf[i++]=command;                    /* 0 - command */
  buf[i++]=' ';                        /* subcommand reserved */
  buf[i++]=image_id&0xff;             /* 1 -2 -id numbers */
  buf[i++]= (image_id>>8) & 0xff;

  do {
    silent_radiance_clean_client_command_queue(sr_client,repeats);
    if (sr_client->command_head == ((sr_client->command_tail+1)% SILENT_RADIANCE_MAX_CLIENT_COMMANDS)) {
             struct timespec thislong;
              thislong.tv_sec = 0;
	                   thislong.tv_nsec = 4000000; /* 4 milliseconds */
			                nanosleep(&thislong, &thislong);
					
      }
    else break;
    } while (1);
  silent_radiance_add_client_command_queue(sr_client,buf,4);

  }

  

silent_radiance_finish_up_client_after_all_commands_sent(sr,sr_client,repeats);
exit(0);
}


