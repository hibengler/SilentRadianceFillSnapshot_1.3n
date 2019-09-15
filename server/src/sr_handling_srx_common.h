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

Pulled from rev 1.27 of srplayer.c - this is the srx specific stuff
- that stuff that generates singalong words for now, but maybe more later.

*/
#include "aubio_utils.h"

#include "srx_format.h"



/* note - these structures also exist in packet_summary (client) and srplayer - sr_handling_srx_common.h (server)
with slightly different names.
packet_summary reads what we are writing,  both can print out text 
but from packet summary, we can have the blue bounding ball!
*/


#include "shm_communication.h"
/* yup - we communicate to the server backend */

/* note - these structures also exist in packet_summary (client) and srplayer - sr_handling_srx_common.h (server)
with slightly different names.
packet_summary reads what we are writing,  both can print out text 
but from packet summary, we can have the blue bounding ball!
*/
/* size of text */
//#define SR_MEASURE_MAX_SIZE 1600
#define SR_MEASURE_MAX_SIZE 800
// silent radiance 1.2 is fixed at 800, but we want to expand to 1600
//#define SR_MEASURE_NUM_LINES 9
#define SR_MEASURE_NUM_LINES 4

typedef struct sr_measure_chunk {
  long beat_buffer_current_frame;
  char beat_buffer[SR_MEASURE_MAX_SIZE];
  long beat_buffer_length; // length in 1/100ths of a second
  int number_line_buffers;
  char line_buffer[SR_MEASURE_NUM_LINES][SR_MEASURE_MAX_SIZE];
  int line_buffer_len[SR_MEASURE_NUM_LINES];
  int sent;
  } sr_measure_chunk;

extern char next_beat_buffer[SR_MEASURE_MAX_SIZE];



#define NUMBER_SEND_LINES 400
/* sending command queue up */
extern char *send_line[NUMBER_SEND_LINES];
extern int head_send_line;
extern int tail_send_line;

#define MEASURE_CHUNK_COUNT 40
extern sr_measure_chunk measure_chunks[MEASURE_CHUNK_COUNT];
extern volatile int measurechunkhead;
extern volatile int measurechunktail;



extern struct silent_radiance *sr_c;
extern struct silent_radiance_client *sr_client;

extern FILE *srxfile;




extern  int current_chunk_number;
extern  int file_chunk_number;
extern   long file_frame;
extern  int number_chunks;

/* shared memory header stuff */
void setup_shared_memory();
void add_command_line_to_shared_memory(sr_measure_chunk *c);
/* if command line is 0 - we reset frame number  to 0 */
void add_line_to_shared_memory(sr_measure_chunk *c,int lineno);
/* might be multiple commands */

void set_frame_number_in_shared_memory(long frame_number);
/* send periodically to keep things in check - especially if they change channels */

void finish_shared_memory();

#define SOUNDRING_COUNT 4000

extern volatile int soundringhead; /* start of the soundringqueue. normally incremented by sound.  But could overflow */
extern volatile int soundringfirst; /* also incrmented by processor.c - probably the first viable sound in the ring */
extern volatile int soundringtail; /* the end on the sound ring. this is incremented by processor.c whenever a packet is added */
extern volatile int soundringcurrent; /* we are playing this RIGHT NOW: -1 if we are not playing.  This is updated by the sound subsystem */
extern volatile short soundring[SOUNDRING_COUNT][960];  /* points to soundring_area */

extern volatile long soundframe[SOUNDRING_COUNT];



void gather_srx_handle2(long the_frame);
void gather_srx_handle(long the_frame,long delayme,long delayserver);


extern int delay_frames;
extern int delay_frames_server;
extern long last_srx_handle_frame;

extern int process_state;
extern int output_info; // set to 1 to print out the lyrics

void send_next_bunch(long current_frame); /* current_frame is the_frame - both delays */


#define BUNDLE_TO_GATHER 5


void done_measure_chunk();
void gather_srx_next_bunch();
void send_next_bunch(long current_frame);
void gather_srx_handle2(long the_frame); /* the_frame is - the me delay, not the server delay */


/*-------------------------- shared memory stuff */
void setup_shared_memory();
int flush_send_lines();
void finish_shared_memory();
void not_moving_queue_axnay(char *command);
int add_command_if_can_now();
void add_command_line_to_shared_memory(sr_measure_chunk *c);
void set_frame_number_in_shared_memory(long frame_number);
void add_line_to_shared_memory(sr_measure_chunk *c,int lineno);
