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

// WORLD_INTERNATIONAL off

/*

This example reads standard from input and writes
to the default PCM device for 5 seconds of data.

*/

/* Use the newer ALSA API */
#include <stdio.h> /* for printf() and fprintf() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <errno.h>

/*#define THREADING_ON 1 
^^^ for threadding example - doesnt help much just burns a cpu */

#include "glue.h"
#include "processor.h"
#include "sound_generic.h"

#include "sound_alsa.h"
#include "processor.h"
#include "packet_summary.h"
#include <math.h>

static volatile int wipe_sound=0;  // quick buffer clear

long sound_delay_frames=0;
short zero_buffer[480];


short * getNextQueue240() {

if (wipe_sound) { // quick to help clear when changing the channel
  wipe_sound=0;
  soundringhead = soundringtail;
  soundringcurrent = -1;
  return NULL;
  }
  
if (soundringcurrent==-1) {  /* was emptied. Check to see if we have enough */
  soundringcurrent=soundringhead;
  soundringfirst=soundringhead;
  
  if (soundringcurrent == soundringtail) { /* Empty zero it! */
	return NULL;
	}
  }
else {
  int nsoundringcurrent = (soundringcurrent + 1) % SOUNDRING_COUNT;
  if (nsoundringcurrent == soundringtail) { /* Empty zero it! */
	return NULL;
	}
  soundringcurrent = nsoundringcurrent;
  }
short *nextBuffer = (short *)soundring[soundringcurrent];
return  nextBuffer;
}


  int init_send_sound=1;
  long loops;
  int rc;
  int size;
  unsigned int val;
  int dir;
  char *buffer;
  struct pollfd *ourpolls;
  int soundpolls_count;
  int socket_poll;
  
  int sound_state = 0;
  
  static int frame=0;
int no_sound = 0;




FILE *sound_file=NULL;


int init_receiver_sound(int block) {
sound_file = fopen("a.rawsound","w");
if (!sound_file) {
  fprintf(stderr,
            "unable to open a.rawsound\n");
  exit(1);
  }

size = 4000;
buffer = (char *) malloc(size);
  

sound_state=0;
{
  int i;
  for (i=0;i<480;i++) {
//    zero_buffer[i]=(i*80)%32768;   /* for debugging */
    zero_buffer[i]=0;
    }
  }
sound_delay_frames=0;

return 0;
}




int finish_play_sound() {
  free(buffer);
  sound_delay_frames=0;
return(0);
}


void DieWithError(char *errorMessage); /* External error handling function */

void DieWithError(char *errorMessage)
{
perror(errorMessage);
exit(1);
}


void finish_stream_sound() {
wipe_sound=1;
soundringhead = soundringtail-1;
if (soundringhead<0) soundringhead +=SOUNDRING_COUNT;
}


/* ramp up and down sound at ends of demo */
static int ramp_sound(short *sb,int millipoints) {
double mp = millipoints* 0.000001;
if (mp<0.) mp=0.;
if (mp>1.) mp=1.;
mp = sqrt(mp);
for (int i=0;i<480;i++) {
  sb[i] = (short)(((float)(sb[i]))*mp);
  }
}


  
 


int playOneSoundBuffer() {
char *soundBuffer;
soundBuffer = (char *)getNextQueue240();
if (!soundBuffer) return(0);
int soundBufferSize = 960;

/* handle sound if not in range */

if (!sound_file) return(1);


// skip if we are not in the time window
long st = -1;
long et = -1;
struct timeval demo_time_of_day;
sr_gettimeofday(&demo_time_of_day,NULL);
if (demo_start_time.tv_sec != 0) {
  st = time_difference_in_milliseconds(demo_time_of_day,demo_start_time);
  if (st<0) return(1);
  if (st<500000) ramp_sound((short *)soundBuffer,st*2);
  
  }
if (demo_end_time.tv_sec != 0) {  
  et = time_difference_in_milliseconds(demo_time_of_day,demo_end_time);
  if (et>-500000) ramp_sound((short *)soundBuffer,(-et*2));
  if (et>=0) return(1);
  }
  

rc = fwrite(soundBuffer,960,1,sound_file);
if (rc==0) {
      fprintf(stderr,"??");
      return 0;
      }
int delay=0;
sound_delay_frames = (delay+239)/240;
return 1;
}

