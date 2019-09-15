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

/* Shared memory Version 1 */
#ifndef SHM_COMMUNICATION_H
#define SHM_COMMUNICATION_H


#define SILENT_RADIANCE_MAX_CLIENTS 100
#define SILENT_RADIANCE_MAX_CLIENT_COMMANDS 10
#define SILENT_RADIANCE_MAX_COMMAND_SIZE 256
#define SILENT_RADIANCE_MAX_COMMAND_SIZE_P1 257

/* this will have other things, such as x,y positions for various phones */
struct silent_radiance_server {
int structure_version; /* always set to 1 for now.  useful to have out of date programs fail */
int frame_number;
int delay_frames; /* delay frames when converting to broadcast */
};


/* union is to handle volatile/non volatile crap */
union silent_radiance_server_union {
  volatile struct silent_radiance_server vol;
  struct silent_radiance_server me;
  };


/* client has a list of commands */
struct silent_radiance_client {
int command_head;
int command_tail;
char commands[SILENT_RADIANCE_MAX_CLIENT_COMMANDS][SILENT_RADIANCE_MAX_COMMAND_SIZE_P1];
int commands_sent_count[SILENT_RADIANCE_MAX_CLIENT_COMMANDS];
int commands_length[SILENT_RADIANCE_MAX_CLIENT_COMMANDS];
};

/* union again for the non volatile crap */
union silent_radiance_client_union {
  volatile struct silent_radiance_client vol;
  struct silent_radiance_client me;
  };



/* structure of all shared memory */
struct silent_radiance {
union silent_radiance_server_union server;
volatile int lock; /* used to allocate clients or knock them out */
volatile int client_enabled[SILENT_RADIANCE_MAX_CLIENTS];
union silent_radiance_client_union clients[SILENT_RADIANCE_MAX_CLIENTS];
};






/* the first function to call, client or server. This sets up the shared memory area. */
extern struct silent_radiance * get_silent_radiance_shared_memory (char *name);
		     



/* called at the end.  the client usually calls silent_radiance_finish_up_client_after_all_commands_sent instead */
extern void detatch_silent_radiance_shared_memory(struct silent_radiance *sr);





/* called to get a client allocated to this program. */
extern struct silent_radiance_client *silent_radiance_get_client(struct silent_radiance *sr);



extern void silent_radiance_release_client(struct silent_radiance *sr,struct silent_radiance_client *sr_client);


/* used to wipe out all commands wheter they have been sent or not - makes it good as an interruption */
extern void silent_radiance_clear_client_command_queue(struct silent_radiance_client *sr_client);



/* add a command to the queue. The commands are arbitrary, but will usually have an intro code at the start */
extern void silent_radiance_add_client_command_queue(struct silent_radiance_client *sr_client,char *command,int command_length);



/* This should be run after a cycle of time, or after a new event happens if event driven 
This cleans up the queue for more things.  
*/
extern void silent_radiance_clean_client_command_queue(struct silent_radiance_client *sr_client,int ntimes);

/* this waits till the server does the work. will return -1 if the server is non responsive, 0 if ok */
extern int silent_radiance_wait_until_all_commands_sent(struct silent_radiance *sr,struct silent_radiance_client *sr_client,int repeat_count);







/* this is a nice way for the client to finish up, especially if it is a quick one off command. Return -1 if no server active */
extern int silent_radiance_finish_up_client_after_all_commands_sent(struct silent_radiance *sr,struct silent_radiance_client *sr_client,int ntimes);



/* called by the server every time we are about to build a packet to send out */
extern void silent_radiance_build_server_packet(struct silent_radiance *sr,int max_size,unsigned char *full,int *pfinal_length);


/* Easy one call that will send one command and cleanly get out */
extern int silent_radiance_simple_command(char *name,char *command,int command_length);


extern void detatch_slient_radiance_shared_memory (struct silent_radiance *sr);

#endif
