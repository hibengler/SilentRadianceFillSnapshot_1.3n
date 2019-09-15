#include "glue.h"
#include "processor.h"
#include "analyzer.h"

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


#include "glue.h"







/* This is kinda instead of processor.c 
Where processor.c generates sound packets from looking at the internet stuff,
analyzer reads the sound packets that are digitized, and processes them through tools
to figure out the following:
beat onset information
tempo point information
db level
and pitch
*/

int analyzing=0;

static int frame=-1;

void init_analyzer() {
init_processor_buffers();

int i;
for (i=0;i<SOUNDRING_COUNT;i++) {
      volatile struct processor_stat *stat;
      stat =soundstat[i];
      stat->len=0;
      stat->frame=-1;
      stat->version=0; /* not set to anything until we have stats - this is a helper for the process stats
                          Note - that the processor stuff always has the stats recorded in the sound buffer. analyzer stats is not that lucky
                          So.... Process stats might call analyze itself -- if the analyzer didn't get to it - so we use mutexes to check.
                          Block by block - we test the mutex. That way,  the video rendering app can do the work, if it is time to do it,
                          or the work app can do the work, if it happens to be there first.
                          Worst case, the video app will ignore a frame or two.
                          Best case - some processing aubio core will be separate from the video core.
                          */                 
      }
			  
			  

/* would have the aubio stuff here as well */
    
    frame=-1;
    analyzing=1;
    logit("init_analyzer done");
			    
}


int analyze() {
if (soundringhead == soundringtail) { /* Empty zero it! */
  if (soundringhead ==soundringfirst) {
    soundringcurrent=-1;
    }
  return(0);
  }
else {
  logit("analyze %d to %d frame %d",soundringhead,soundringtail,frame);

  int counter=0;
  do {
    /* soundringfirst is never set by this - as we always have a stream of sound, I hope */
    soundringcurrent = soundringhead;
//    short *nextBuffer = (short *) soundring[soundringcurrent];
//    logit(" b %d [%d,%d,%d,...]",soundringcurrent,nextBuffer[0],nextBuffer[2],nextBuffer[4]);
    
    
    volatile struct processor_stat *s;
    s=soundstat[soundringcurrent];
    unsigned char *sa = (unsigned char *)(s->stats_area);
    {
    
      float the_pitch=440.f;
      float db_level=-11.f;
      /* microseconds is done by the sound program - as its callback is most timely */
#ifdef never_use_just_to_show    
      int microseconds;
      struct timeval tv;
      gettimeofday(&tv,NULL);
      microseconds = tv.tv_sec & 1024; /* 277 hours*/
      microseconds = microseconds*1000000 + (tv.tv_usec);
#endif
      unsigned int flags=0;

      /* these are default values - as we dont have aubio loaded */
	
	  
	    
	        
      unsigned char *statistics = sa;
      /* this is congruent to code in music_broadcast_server.c on the server side */
    
      int l = 0;
     // float dummy_pitch=the_pitch;
      unsigned char *pt = (unsigned char *)&the_pitch;
      statistics[l++]=pt[0];                                                        /* pitch */
      statistics[l++]=pt[1];
      statistics[l++]=pt[2];
      statistics[l++]=pt[3];
#ifdef never_use_just_to_show    
      statistics[l++]=microseconds&255;                                     /* microseconds - is modulo 1024 seconds */
      statistics[l++]= (microseconds>>8)&255;
      statistics[l++]= (microseconds>>16)&255;
      statistics[l++]= (microseconds>>24)&255;
#else
      l+=4;
#endif
      pt = (unsigned char *) &db_level;                                    /* db level */
      statistics[l++]=pt[0];
      statistics[l++]=pt[1];
      statistics[l++]=pt[2];
      statistics[l++]=pt[3];
      statistics[l++]=flags&255;                                           /* flags */
      statistics[l++]= (flags>>8)&255;
      statistics[l++]= (flags>>16)&255;
      statistics[l++]= (flags>>24)&255;
      } /* analysis block */
        
    /* set the version to "sr00" so that the stats can handle it */
    char *v = (char *)(&(s->version));
    v[0]='s';
    v[1]='r';
    v[2]='0';
    v[3]='0';
    
    
    /* handle the frames number */
    if (frame==-1) frame=1;
    else frame++;
    s->frame = frame;
  
    /* we have analyzed. So set soundringhead to the next one */
    soundringhead = (soundringhead + 1) % SOUNDRING_COUNT;
    counter++;
    } while (soundringhead != soundringtail);

  return(counter);  
  }
	
}


void shutdown_analyzer() {
analyzing=0;
soundringhead=0;
soundringtail=0;
soundringfirst=0;
soundringcurrent=-1;
	    
logit("shutdown_analyzer done");
}
