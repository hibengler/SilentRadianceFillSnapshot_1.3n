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
if (argc<=7) {
  fprintf(stderr,"Usage: sr_send_bgp_binary <command_char> <image_id> <image_name> <upc> <original_width> <original_height> <ntimes>\n");
  fprintf(stderr,"These are not usually sent throug broadcast, but streaming is ok\n");
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

char *image_name = argv[3];
if (strlen(image_name)>128) {
  fprintf(stderr,"image_name is too big\n");
  exit(1);
  }


char *upc = argv[4];
if (strlen(upc)>40) {
  fprintf(stderr,"upc is too big\n");
  exit(1);
  }  

int original_width = atoi(argv[5]);
if (original_width<0) original_width=64;

int original_height = atoi(argv[6]);
if (original_height<0) original_height=64;

int ntimes = atoi(argv[7]);
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
 
FILE *bf = fopen(image_name,"r");
if (!bf) {
  fprintf(stderr,"cannot open %s\n",image_name);
  detatch_slient_radiance_shared_memory(sr);
  exit(1);
  }


struct stat st;
fstat(fileno(bf), &st);
unsigned int file_size = st.st_size;



fprintf(stderr,"doing command %c\n",command);


int i=0;
buf[i++]=command;                    /* 0 - command */
buf[i++]=' ';                        /* subcommand reserved */
buf[i++]=image_id&0xff;             /* 1 -2 -id numbers */
buf[i++]= (image_id>>8) & 0xff;
buf[i++]=0xff;                       /* chunk number */
buf[i++]=0xff;
buf[i++]=0xff;


buf[i++]=file_size&0xff;             /* 8 file size */
buf[i++]=(file_size>>8)&0xff;
buf[i++]=(file_size>>16)&0xff;
buf[i++]=(file_size>>24)&0xff;

buf[i++]=1;                           /* number of frames - subsequent id numbers are used */
buf[i++]=0;
buf[i++]=original_width&0xff;             // width and height
buf[i++]=(original_width>>8)&0xff;
buf[i++]=(original_height)&0xff;
buf[i++]=(original_height>>8)&0xff;
buf[i++]=0;                           /* reserved - 2 */
buf[i++]=0;
strcpy(buf+i,image_name);
i += strlen(buf+i)+1;
strcpy(buf+i,upc);
i += strlen(buf+i)+1;
buf[i++]=0;   /* 8 more reser4ved */
buf[i++]=0;
buf[i++]=0;
buf[i++]=0;
buf[i++]=0;
buf[i++]=0;
buf[i++]=0;
buf[i++]=0;


int mm;
for (mm=0;mm<3;mm++) {
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

  silent_radiance_add_client_command_queue(sr_client,buf,i);
  }
 
unsigned long pos = 0;
unsigned long l;
unsigned long left=file_size;
unsigned int index=0;
while (left) {
  unsigned long chunk=left;
  if (chunk>255-7) chunk=255-7;
  unsigned long l=fread(buf+7,1,chunk,bf);
  if (l==0) {
    fclose(bf);
    fprintf(stderr,"error reading file\n");
    detatch_slient_radiance_shared_memory(sr);
    exit(-2);
    }
  else if (l<chunk) {
    if (chunk!= left) {
      fclose(bf);
      fprintf(stderr,"error short read of file\n");
      detatch_slient_radiance_shared_memory(sr);
      exit(-2);
      }
    }
  /* here l is full */
  buf[4] = index&0xff;
  buf[5] = (index>>8)&0xff;
  buf[6] = (index>>16)&0xff;
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
  silent_radiance_add_client_command_queue(sr_client,buf,7+l);

  left -=l;
  index++;
  }
fclose(bf);
}

/* add the turn me on image command */

buf[0]=buf[0]+32; /* lowercase */

for (iteration=0;iteration<ntimes;iteration++) {
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


