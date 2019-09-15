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
/*

$Revision: 1.2 $

This tries to get lrc timing, and since timing for lrc is in hundreths of a second, we will do it in 2 packets of 
240 to equal 1 200th *2
......... - nothing
Z - tempo onset
o - onset of beat
tempo onset is estimated after a while - 4 four in a descent time.
*/


#include "srx_format.h"
#include "sr_handling_srx_common.h"



/* note - these structures also exist in packet_summary (client) and srplayer - sr_handling_srx_common.h (server)
with slightly different names.
packet_summary reads what we are writing,  both can print out text 
but from packet summary, we can have the blue bounding ball!
*/


char next_beat_buffer[SR_MEASURE_MAX_SIZE];


/* sending command queue up */
char *send_line[NUMBER_SEND_LINES];
int head_send_line=0;
int tail_send_line=0;


sr_measure_chunk measure_chunks[MEASURE_CHUNK_COUNT];
volatile int measurechunkhead=0;
volatile int measurechunktail=0;




struct silent_radiance *sr_c=NULL;
struct silent_radiance_client *sr_client=NULL;























aubio_wavetable_t *wavetable;
aubio_tempo_t *tempo;
fvec_t * tempo_out;
FILE *srxfile=NULL;


/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <poll.h>
#include <errno.h>




  








  
//gather_srx_handle(file_frame,delay_frames,delay_frames_server);




static long old_frame=-1;
static long old_frame_server=-1;
void gather_srx_handle(long the_frame,long delayme,long delayserver) {
//fprintf(stderr,"h %ld,%ld,%ld\n",the_frame,delayme,delayserver);
if (the_frame<0) the_frame=0;
while(the_frame-(delayme+delayserver) >old_frame_server) {
  old_frame_server++;
  send_next_bunch(old_frame_server);
  }
while(the_frame-delayme >old_frame) {
  old_frame++;
  gather_srx_handle2(old_frame);
  }
}


int process_state=0;






void done_measure_chunk() {
measurechunktail = (measurechunktail+1) % MEASURE_CHUNK_COUNT;
if (measurechunktail==measurechunkhead) {
  measurechunkhead = (measurechunkhead+1) % MEASURE_CHUNK_COUNT;
  }  
}




static int is_srx_input_100_flag=0;

static long block_next_current_frame = 0;

void gather_srx_next_bunch() {
int a;
if (!srxfile) {return;}
lewp:

a=measurechunktail-measurechunkhead;
if (a<0) a += MEASURE_CHUNK_COUNT;
if (a>=BUNDLE_TO_GATHER) {
   return;
   }
 else {
//   fprintf(stderr,"gatheres %d s %d\n",a,process_state);
   }
struct sr_measure_chunk *mc = measure_chunks+measurechunktail;

if (process_state==0) {
  is_srx_input_100_flag=0;
  /* see how many we have taken in */
  leawp:
  if (!fgets(next_beat_buffer,SR_MEASURE_MAX_SIZE,srxfile)) {
    fclose(srxfile);
    srxfile=NULL;
    return;
    }
  if (srx_is_comment(next_beat_buffer)) {
    is_srx_input_100_flag=1;
    goto leawp;
    }
  if (is_srx_input_100_flag) {
    srx_100_to_srx_200(mc->beat_buffer,next_beat_buffer,SR_MEASURE_MAX_SIZE);
    }
  else {  
    srx_expand(mc->beat_buffer,next_beat_buffer,SR_MEASURE_MAX_SIZE);
    }
//  fprintf(stderr,"0old	%s\n0new %s\n",next_beat_buffer,mc->beat_buffer);
  file_chunk_number++;
  mc->beat_buffer_length = strlen(mc->beat_buffer)-1;
  mc->beat_buffer_current_frame = file_frame;
  mc->sent=0;
  block_next_current_frame = mc->beat_buffer_current_frame + mc->beat_buffer_length;
  process_state=1;
  mc->number_line_buffers=0;
  goto lewp;
  }
else if (process_state==1) {
  while (srxfile) {
    char read_beat_buffer[SR_MEASURE_MAX_SIZE];
    if (!fgets(read_beat_buffer,SR_MEASURE_MAX_SIZE,srxfile)) {
      fclose(srxfile);
      srxfile=NULL;
      continue;
      }
    if (srx_is_comment(read_beat_buffer)) {
      continue;
      }
    if (is_srx_input_100_flag) {
      srx_100_to_srx_200(next_beat_buffer,read_beat_buffer,SR_MEASURE_MAX_SIZE);
      }
    else {  
      srx_expand(next_beat_buffer,read_beat_buffer,SR_MEASURE_MAX_SIZE);
      }
//    fprintf(stderr,"old	%s\nnew %s\n",read_beat_buffer,next_beat_buffer);
      
    int is_control = 1;
    char *p = next_beat_buffer;
    while(*p) {
      if ((*p < '0')||(*p == '|')||(*p == '+')||(*p == '.')||(*p == '-'))
        {
	p++;
	}
      else {
        is_control=0;
	break;
	}
      }
    
    if (is_control) {
      process_state = 2;
      done_measure_chunk();
      goto lewp;
      }
    memset(mc->line_buffer[mc->number_line_buffers],0,SR_MEASURE_MAX_SIZE);
    strcpy(mc->line_buffer[mc->number_line_buffers],next_beat_buffer);
    mc->line_buffer_len[mc->number_line_buffers] = strlen(mc->line_buffer[mc->number_line_buffers]);
    if (mc->number_line_buffers<SR_MEASURE_NUM_LINES)     mc->number_line_buffers++;    
    }
  process_state = 3;
  done_measure_chunk();    
  }
else if (process_state==2) {
  file_chunk_number++;
  strcpy(mc->beat_buffer,next_beat_buffer);
  mc->beat_buffer_length = strlen(mc->beat_buffer)-1;
  mc->beat_buffer_current_frame = block_next_current_frame;
  mc->sent=0;
  block_next_current_frame = mc->beat_buffer_current_frame + mc->beat_buffer_length;
  process_state=1;
  mc->number_line_buffers=0;
  goto lewp;
  } 
else if (process_state==3) {
 return;
 }
}  






void send_next_bunch(long current_frame) { /* current_frame is the_fraem - both delays */
int m;
m = measurechunkhead;
while (m != measurechunktail) {
  struct sr_measure_chunk *mc = measure_chunks+m;
  if (current_frame > (mc->beat_buffer_current_frame+mc->beat_buffer_length)) {
    measurechunkhead = (measurechunkhead+1) % MEASURE_CHUNK_COUNT;
    current_chunk_number++;
    gather_srx_next_bunch();
    }
  else break;
  m = (m+1) % MEASURE_CHUNK_COUNT;    
  }
while (m != measurechunktail) {
  struct sr_measure_chunk *mc = measure_chunks+m;
//  - turned this off to get multiple lines dumped ahead of time
// - now we dont
//  else if (current_frame < mc->beat_buffer_current_frame) {
//    break;
//    }
  if (current_frame <= mc->beat_buffer_current_frame+mc->beat_buffer_length) {
    if (!mc->sent) {
      add_command_line_to_shared_memory(mc);
      int i;
      for (i=0;i<mc->number_line_buffers;i++) {
        add_line_to_shared_memory(mc,i);
        }
      mc->sent = 1;
      break; // dont overload the memory
      }
    } /* if it is time */
  m = (m+1) % MEASURE_CHUNK_COUNT;  
  }
if ((current_frame&255)==0) {
  set_frame_number_in_shared_memory(current_frame);
  }
}  



int output_info=0; // set to 1 to print out the lyrics

void gather_srx_handle2(long the_frame) { /* the_frame is - the me delay, not the server delay */
gather_srx_next_bunch();
int m=measurechunkhead;
while  (measurechunktail!=m) {
  struct sr_measure_chunk *mc = measure_chunks+m;
  if (the_frame > (mc->beat_buffer_current_frame+mc->beat_buffer_length)) { // save it for 
    }
  else if (the_frame < mc->beat_buffer_current_frame) {
//    fprintf(stderr,"frame %ld < %ld\n",the_frame,mc->beat_buffer_current_frame);
    return;
    }
  else {
    long indent = (the_frame-mc->beat_buffer_current_frame);
    int i;
//    fprintf(stderr,"f %ld pos %ld\n",the_frame,indent);
    for (i=0;i<mc->number_line_buffers;i++) {
      char *buf = mc->line_buffer[i];
      if ( (buf[indent])&&(buf[indent] != ' ')&&((indent==0)||(buf[indent-1] == ' ')) ) { /* if we found a new word */
        int indent2=indent;
        if (buf[indent2]== '_') { indent2++;}
        else if (!indent2) {}
        else if (output_info) fputc(' ',stderr);
        char ch;
        while ((ch=buf[indent2])&&(ch != ' ')) { if (ch != '_')  if (output_info) putc(ch,stderr); indent2++; }
        }
      }    
    if (mc->number_line_buffers==0) {
//      if (output_info) fputc(mc->beat_buffer[indent],stderr);
      }
    return;
    }
  m = (m+1) % MEASURE_CHUNK_COUNT;
  }
return;
}



/*-------------------------- shared memory stuff */




void setup_shared_memory() {
sr_c = get_silent_radiance_shared_memory("default");
if (!sr_c) return;
sr_client = silent_radiance_get_client(sr_c);
if (!sr_client) {
  fprintf(stderr,"NOSHAREDMEM LEFT");
  detatch_slient_radiance_shared_memory(sr_c);
  sr_c=NULL;
  return;
  }

}

int flush_send_lines();

void finish_shared_memory() {
if (sr_c) {
  if (sr_client) {
    flush_send_lines();
    silent_radiance_release_client(sr_c,sr_client);
    sr_client=NULL;
    }
  //silent_radiance_finish_up_client_after_all_commands_sent(sr_c,sr_client,0);
  detatch_slient_radiance_shared_memory(sr_c);
  sr_c=NULL;
  }
}

//int fixonce=1;


void not_moving_queue_axnay(char *command) {
if (!sr_c) return;
if (!sr_client) return;
if (!command) goto nevermind;
/*if (fixonce) {
  fprintf(stderr,"fixonce\n");
  silent_radiance_clear_client_command_queue(sr_client);
  fixonce=0;
  }*/
int s = (tail_send_line+1)%NUMBER_SEND_LINES;
if (s==head_send_line) {
nevermind:
  silent_radiance_clear_client_command_queue(sr_client);
  fprintf(stderr,"NEVERMIND, BROADCAST SERVER NOT RESPONDING\n");
  finish_shared_memory();
  }
send_line[tail_send_line] = strdup(command);
tail_send_line = s;
}



int add_command_if_can_now() {
if (!sr_c) return 0;
if (!sr_client) return 0;
if (head_send_line==tail_send_line) return 1;
if (sr_client->command_head == ((sr_client->command_tail+1)% SILENT_RADIANCE_MAX_CLIENT_COMMANDS)) {
  return 0;
  }

while (head_send_line!=tail_send_line) {
  if (sr_client->command_head == ((sr_client->command_tail+1)% SILENT_RADIANCE_MAX_CLIENT_COMMANDS)) {
    return(1);
    }
  fprintf(stderr,"unhook\n");
  silent_radiance_add_client_command_queue(sr_client,send_line[head_send_line],strlen(send_line[head_send_line]));
  free(send_line[head_send_line]);
  send_line[head_send_line] = NULL;
  head_send_line = (head_send_line+1)%NUMBER_SEND_LINES;
  if (sr_client)  silent_radiance_clean_client_command_queue(sr_client,1);
  }
return 1;
}			

						
					
int flush_send_lines() {
add_command_if_can_now();
// could waiy, but why?
}
												

	  

void add_command_line_to_shared_memory(sr_measure_chunk *c) {
if (!sr_c) return;
char buf[400];

if ((c->beat_buffer_length==0)&&(c->beat_buffer_current_frame != 0)) { return;}
//fprintf(stderr,"\n		V %ld %ld",c->beat_buffer_current_frame,c->beat_buffer_length);
sprintf(buf,"V %ld %ld",c->beat_buffer_current_frame,c->beat_buffer_length);

if (sr_client)  silent_radiance_clean_client_command_queue(sr_client,1);

  add_command_if_can_now();
  if (sr_client->command_head == ((sr_client->command_tail+1)% SILENT_RADIANCE_MAX_CLIENT_COMMANDS)) {
    not_moving_queue_axnay(buf);
    }
  else {
    silent_radiance_add_client_command_queue(sr_client,buf,strlen(buf));
    }
  
}	

		  
void set_frame_number_in_shared_memory(long frame_number) {
if (!sr_c) return;
char buf[400];
//fprintf(stderr,"\nf %ld\n",frame_number);
sprintf(buf,"f %ld",frame_number);
if (sr_client) silent_radiance_clean_client_command_queue(sr_client,1);
add_command_if_can_now();
if (sr_client->command_head == ((sr_client->command_tail+1)% SILENT_RADIANCE_MAX_CLIENT_COMMANDS)) {
  not_moving_queue_axnay(buf);
  }
else {
  silent_radiance_add_client_command_queue(sr_client,buf,strlen(buf));
  }
}



	  
void add_line_to_shared_memory(sr_measure_chunk *c,int lineno) {
if (!sr_c) return;
int extension=0;
char buf1[SR_MEASURE_MAX_SIZE];
char buf[SR_MEASURE_MAX_SIZE];
int start=0;
char line_buffer2[SR_MEASURE_MAX_SIZE];
//srx_compress(line_buffer2,c->line_buffer[lineno],SR_MEASURE_MAX_SIZE);
int len=c->line_buffer_len[lineno];
//int len=strlen(line_buffer2);
while (len) {
  int sublen=len;
  if (sublen>200) sublen=200;
  strncpy(buf1,c->line_buffer[lineno]+extension,sublen);
//  strncpy(buf1,line_buffer2+extension,sublen);
  buf1[sublen]='\0';
//  char ttt[10000];srx_200_to_srx_100(ttt,buf1,10000);
//  fprintf(stderr,"\n		v %ld %1.1d %d %s\n",c->beat_buffer_current_frame,lineno,extension,ttt);
  
  sprintf(buf,"v %ld %1.1d %d %s",c->beat_buffer_current_frame,lineno,extension,buf1);
  if (sr_client)  silent_radiance_clean_client_command_queue(sr_client,1);

  add_command_if_can_now();
  if (sr_client->command_head == ((sr_client->command_tail+1)% SILENT_RADIANCE_MAX_CLIENT_COMMANDS)) {
    not_moving_queue_axnay(buf);
    }
  else {
    silent_radiance_add_client_command_queue(sr_client,buf,strlen(buf));
    }
  
  extension += sublen;
  len -= sublen;
  }
}




		  
/* 
  $Log: sr_handling_srx_common.c,v $
  Revision 1.2  2018/12/22 00:42:42  hib
  wip for new sr player right to sirad.  Made music_broadcast quit if cant write to stdout (or any file) any more.
  (untested)

  Revision 1.1  2018/12/21 17:27:06  hib
  Pulled out the srx stuff for reading/processing a sound file from srplayer into sr_handling_srx_common
  The reason for doing this, is we are going to make an srplayer who does raw - although we might already have that,  we dont have a way to do a non default public program.
  Anyways, this is to get closer to make a demo movie for a song.

  
  from srplayer.c 1.27
  Revision 1.27  2018/10/31 02:48:27  hib
  Set up sr000 for full and incremental logging
  
*/
