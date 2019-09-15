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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include "shm_communication.h"







/* the first function to call, client or server. This sets up the shared memory area. */
struct silent_radiance * get_silent_radiance_shared_memory (char *name) 
{
char buf[4000];
if (strlen(name)>3000) name[3000]='\0';
sprintf(buf,"touch /run/shm/sr_%s.shm",name);
system(buf);
sprintf(buf,"/run/shm/sr_%s.shm",name);
int shmflg = IPC_CREAT|IPC_EXCL | 0660;
int size=1024*1024*10;
key_t key;
struct silent_radiance *sr;
key = ftok(buf,'A');
if (key==-1) {
  fprintf(stderr,"error - no key\n");
  exit(1);
  }
int shmid = shmget(key,(size_t) size, shmflg);
if (shmid==-1) {
  shmflg = 0660; /* no need to create */
  shmid = shmget( key, (size_t) size, shmflg);
  if (shmid==-1) {
    fprintf(stderr,"error - no shared memory");
    exit(1);
    }
  sr = shmat(shmid, (void *)0, 0);
  if ((void *)sr == ((void *)(-1))) {
        perror("shmat");
        exit(1);
    }
  if (sr->server.vol.structure_version!=1) {
    fprintf(stderr,"Bad version %d for shared memory. Should be 1\n",sr->server.vol.structure_version);
    exit(-1);
    }
  }
else {
  fprintf(stderr,"new shared memory\n");
  sr = shmat(shmid, (void *)0, 0);
  if ((void *)sr == ((void *)(-1))) {
        perror("shmat");
        exit(1);
    }
  sr->server.vol.structure_version=1;  
  memset((void *)((char *)(sr)+sizeof(int)),0,1024*1024*10-sizeof(int));
  sr->server.vol.frame_number = -1;
  sr->server.vol.delay_frames = 0;
  }
return(sr);
}		     




/* called at the end.  the client usually calls silent_radiance_finish_up_client_after_all_commands_sent instead */


void detatch_slient_radiance_shared_memory (struct silent_radiance *sr) {
shmdt(sr);
}




/* called to get a client allocated to this program. */
struct silent_radiance_client *silent_radiance_get_client(struct silent_radiance *sr) {
/* this is bad thread safe, but whatever */
while (sr->lock) {
  struct timespec thislong;
             thislong.tv_sec = 0;
             thislong.tv_nsec = 1000000 * ((rand()%40)+1); /* 1-40 milliseconds */
             nanosleep(&thislong, &thislong);
  }
sr->lock=1;
{
  int i;
  for (i=0;i<SILENT_RADIANCE_MAX_CLIENTS;i++) {
    if (sr->client_enabled[i]==0) break;
    }
  if (i==SILENT_RADIANCE_MAX_CLIENTS) {
    sr->lock = 0;
    fprintf(stderr,"too many clients\n");
    return(NULL);
    }
  struct silent_radiance_client *sr_client = &(sr->clients[i].me);
  sr_client->command_tail= sr_client->command_tail % SILENT_RADIANCE_MAX_CLIENT_COMMANDS;
  sr_client->command_head= sr_client->command_tail;
  sr->client_enabled[i]=1;
  sr->lock=0;
  return(&(sr->clients[i].me));
  }
}



/* called to release a client from the program.
Note - the client usually calls silent_radiance_finish_up_client_after_all_commands_sent instead */

void silent_radiance_release_client(struct silent_radiance *sr,struct silent_radiance_client *sr_client) {
  int i;
  for (i=0;i<SILENT_RADIANCE_MAX_CLIENTS;i++) {
    if (&(sr->clients[i].me)== sr_client) break;
    }
  if (i==SILENT_RADIANCE_MAX_CLIENTS) {
    fprintf(stderr,"warning: sr client release does not exist\n");
    }
  while (sr->lock) {
    struct timespec thislong;
             thislong.tv_sec = 0;
             thislong.tv_nsec = 1000000 * ((rand()%40)+1); /* 1-40 milliseconds */
             nanosleep(&thislong, &thislong);
    }
  sr->lock=1;
  sr->client_enabled[i]=0;
  sr_client->command_head = sr_client->command_tail;
  sr->lock=0;
}


/* used to wipe out all commands wheter they have been sent or not - makes it good as an interruption */
void silent_radiance_clear_client_command_queue(struct silent_radiance_client *sr_client) {
sr_client->command_head = sr_client->command_tail;
}



/* add a command to the queue. The commands are arbitrary, but will usually have an intro code at the start */
void silent_radiance_add_client_command_queue(struct silent_radiance_client *sr_client,char *command,int command_length) {
int i=sr_client->command_tail;
if (command_length>SILENT_RADIANCE_MAX_COMMAND_SIZE) command_length=SILENT_RADIANCE_MAX_COMMAND_SIZE;
sr_client->commands_length[i]=0;
memcpy((void *)sr_client->commands[i],command,command_length);
sr_client->commands_length[i]=command_length;
sr_client->commands_sent_count[i]=-1;
sr_client->command_tail = (i+1)% SILENT_RADIANCE_MAX_CLIENT_COMMANDS;
if (sr_client->command_head == sr_client->command_tail) {
  sr_client->command_head = (sr_client->command_head+1) % SILENT_RADIANCE_MAX_CLIENT_COMMANDS;
  }
}



/* This should be run after a cycle of time, or after a new event happens if event driven 
This cleans up the queue for more things.  
*/
void silent_radiance_clean_client_command_queue(struct silent_radiance_client *sr_client,int repeat_count) {
int i=sr_client->command_head;
while (i!= sr_client->command_tail) {
  if (sr_client->commands_sent_count[i]>=repeat_count) {
    i=(i+1)%SILENT_RADIANCE_MAX_CLIENT_COMMANDS;
    sr_client->command_head=i;
    }
  else {
    break;
    }
  }
}


/* this waits till the server does the work. will return -1 if the server is non responsive, 0 if ok */
int silent_radiance_wait_until_all_commands_sent(struct silent_radiance *sr,struct silent_radiance_client *sr_client,int repeat_count) {
int start_frame = sr->server.vol.frame_number;
int i;
for (i=0;i<500;i++) {
  silent_radiance_clean_client_command_queue(sr_client,repeat_count);
//  fprintf(stderr,"%d,%d   frame %d sent %d\n",sr_client->command_head,sr_client->command_tail,sr->server.vol.frame_number,
//       sr_client->commands_sent_count[sr_client->command_head]);
  if (sr_client->command_head != sr_client->command_tail) {
        struct timespec thislong;
             thislong.tv_sec = 0;
             thislong.tv_nsec = 4000000; /* 4 milliseonds */
             nanosleep(&thislong, &thislong);
    }
  else {
    break;
    }
  }
if ((i==500) &&sr->server.vol.frame_number == start_frame) {
  return(-1); /* no server movement */
  }
return(0);
}







/* this is a nice way for the client to finish up, especially if it is a quick one off command. Return -1 if no server active */
int silent_radiance_finish_up_client_after_all_commands_sent(struct silent_radiance *sr,struct silent_radiance_client *sr_client,int repeat_count) {
int wait_result=silent_radiance_wait_until_all_commands_sent(sr,sr_client,repeat_count);

silent_radiance_release_client(sr,sr_client);
detatch_slient_radiance_shared_memory(sr);
return wait_result;
}



/* called by the server every time we are about to build a packet to send out */
void silent_radiance_build_server_packet(struct silent_radiance *sr,int max_size,unsigned char *full,int *pfinal_length) {
unsigned char *ptr = full;
int length = 0;

volatile struct silent_radiance_client *sr_clientv;
struct silent_radiance_client *sr_client;
int i;
for (i=0;i<SILENT_RADIANCE_MAX_CLIENTS;i++) {
  int flag=0;
  if (sr->client_enabled[i]) {
    sr_client=&(sr->clients[i].me);
    sr_clientv=&(sr->clients[i].vol);
    int j;
    j=sr_clientv->command_head;
    while (j!=sr_clientv->command_tail) {
      if (sr_clientv->commands_sent_count[j]<1) {
        if (sr_clientv->commands_length[j]+1+length > max_size) {
	  flag=1;
	  break;
	  } /* if we dont have room to send */
	*ptr++ = (unsigned char)sr_clientv->commands_length[j];
	memcpy(ptr,(void *)sr_client->commands[j],sr_client->commands_length[j]);
        length += 1 + sr_client->commands_length[j];
	ptr += sr_client->commands_length[j];
	sr_clientv->commands_sent_count[j]=1;
	
	sr_client->commands[j][sr_client->commands_length[j]]='\0';
//	fprintf(stderr,"debug command is %s frame %d\n",sr_client->commands[j],sr->server.me.frame_number);
	
	} /* if we need to send */
      j = j+1;
      if (j>SILENT_RADIANCE_MAX_CLIENT_COMMANDS) j=0;
      } /* while going through all commands */
//    if (flag) break;   /* might do another command - but the queue order per client is preserved. enableing this will make it quit quicker - no beig deal */ 
    } /* if this is an enabled client */
  } /* for each client */
*pfinal_length = length;
}


/* Easy one call that will send one command and cleanly get out */
int silent_radiance_simple_command(char *name,char *command,int command_length) {
struct silent_radiance *sr;
sr = get_silent_radiance_shared_memory(name);
if (!sr) return(-2);
struct silent_radiance_client *sr_client;
sr_client = silent_radiance_get_client(sr);
if (!sr_client) {
  detatch_slient_radiance_shared_memory(sr);
  return(-2);
  }

silent_radiance_add_client_command_queue(sr_client,command,command_length);

return(silent_radiance_finish_up_client_after_all_commands_sent(sr,sr_client,1));
}
