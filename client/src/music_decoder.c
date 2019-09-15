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




/*

This example reads standard from input and writes
to the default PCM device for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <errno.h>

/*#define THREADING_ON 1 
^^^ for threadding example - doesnt help much just burns a cpu */

#ifdef THREADING_ON
#include <pthread.h>
#endif

#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"
#include "music_decoder.h"
#include "sound_generic.h"

#include <alsa/asoundlib.h>


extern int getpacket();
extern int addBlankSound();
extern int playOneSoundBuffer();
extern int processOneSampleMustWriteOne();
extern int processOneSampleMustWriteOne();
extern int processSamplesPossiblyPlay();
extern int processSamplesIgnorePackets();

extern int play_direct (unsigned char *buf);


OpusDecoder *opusdecoder=NULL;
int frame=0;
unsigned int last_microseconds=0;
FILE *write_file_spec= NULL;
int soundringhead=0;
int soundringtail=0;
char *soundring[SOUNDRING_COUNT];
int soundlen[SOUNDRING_COUNT];  // always 960

char *commandring[COMMANDRING_COUNT];
int commandlen[COMMANDRING_COUNT];
int commandringhead=0;

float minpcm=0.;
float maxpcm=0.;
int deltasum=0;
int deltacount=0;
int underruncount=0;
int missingcount=0;
int majormissingcount=0;
int blankedcount=0;


int init_music_decoder() {

{ /* opus */
    int err;
    opusdecoder = opus_decoder_create((opus_int32)(48000),2,&err);
    if (err != OPUS_OK)
       {
         fprintf(stderr,"OPUS decoder fail %d\n",err);
       opusdecoder = NULL; /* error - decode will just be off */
       }

    }

return 0;
}





unsigned char checksum(unsigned char *compress,int compress_length) {
unsigned char sum = 0;
int i;
for (i=0;i<compress_length;i++) {
  sum +=compress[i];
  }
return(sum);
} 

    
	
	    
		
		    
			
			        


int addSample(int sampleSize,unsigned char *packet, int useRaw,int mustWriteOne_flag,int possibly_play_flag,int ignore_packets_flag) {

unsigned int flag;
unsigned int newframe;
unsigned int header_frame;
unsigned int microseconds;
unsigned char *soundBuffer;
soundBuffer = soundring[soundringtail];
int soundBufferSize = 0;
int no_sound=0;
int header_flag;

int opus_frames=0;
int framestart[MAXFRAMES];
int framesize[MAXFRAMES];
int end_of_frames = 0;
unsigned char recorded_checksums[MAXFRAMES];
unsigned char actual_checksums[MAXFRAMES];

int missed=0;
int major_missed=0;

{ /* compute flag */
  int l = sampleSize-8;           /*-8 is flags */
  unsigned int i1=packet[l]&0xff;
  unsigned int i2=packet[l+1]&0xff;
  unsigned int i3=packet[l+2]&0xff;
  unsigned int i4=packet[l+3]&0xff;
  flag = i1+(i2<<8)+(i3<<16)+(i4<<24);
}

{ /* compute newFrame whatever that is */
        int l = sampleSize-16;                  /* -16 is frame */
    unsigned int i1=packet[l]&0xff;
    unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;

    microseconds = i1+(i2<<8)+(i3<<16)+(i4<<24);
}       


{ /* compute newFrame whatever that is */
        int l = sampleSize-28;                  /* -28 is frame */
    unsigned int i1=packet[l]&0xff;
    unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;

    header_frame = i1+(i2<<8)+(i3<<16)+(i4<<24);
}       

{ /* compute newFrame whatever that is */
        int l = sampleSize-20;                  /* -20 is frame */
    unsigned int i1=packet[l]&0xff;
    unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;

    newframe = i1+(i2<<8)+(i3<<16)+(i4<<24);
}       

{ /* read recorded checksums */
  int l = sampleSize-40;
  int i;
  for (i=0;i<MAXFRAMES;i++) {
    recorded_checksums[i]=packet[l++];
    actual_checksums[i]=0;
    }
  }
  
  
/* skip the redundant frames */
if (header_frame != newframe) {
  return 0;
  }
    
        useRaw = 0;



{/* Opus frame - including recovery */
  int packets_needed;
  int look;
  if (frame==0) {
          frame = newframe-1;
          packets_needed = 2000;
          }
  else {
      packets_needed = newframe-frame;
      if (packets_needed<0) packets_needed=2000;
      }
  if (flag&4) look=960; else look=0;

    /* figure out how many opus packets are there. */
  opus_frames = 0;
  while ( ( (packet [look])||(packet[look+1]) )&&(opus_frames<MAXFRAMES)&&(look<sampleSize)) {
            /* found a frame. */
            framestart[opus_frames]=look+2;
            framesize[opus_frames]= ((int)(packet[look]))& 0xff + (((int)(packet[look+1]))<<8);
	    actual_checksums[opus_frames]=checksum(packet+framestart[opus_frames],framesize[opus_frames]);
            look=look+2+framesize[opus_frames];
            opus_frames++;
            }
  end_of_frames = look;

  if (packets_needed >MAXFRAMES) {
        major_missed=1;
//	opus_frames = 1;
//	soundBufferSize=0;
//        snd_pcm_prepare(handle);	
//	fprintf(stderr,"major missed %d\n",packets_needed);
//	int i;
//  	for (i=0;i<10;i++) {
//  	  for (;soundBufferSize<960;soundBufferSize++) {
//	    soundBuffer[soundBufferSize]=0;
//	    }
//	  play_sound(&soundBufferSize,&underruncount,mustWriteOne_flag,possibly_play_flag,ignore_packets_flag);  
//	  blankedcount++;
//          soundBuffer = soundring[soundringtail];
//	  mustWriteOne_flag=0;
//	  soundBufferSize=0;
//	  }
        }
  else if (packets_needed >1) {
      missed=1;
      }
  if (opus_frames>packets_needed) {
      opus_frames = packets_needed;
      }
  missingcount += missed;
  majormissingcount += major_missed;

  /* lets look at our code and see what we got */
  if (!major_missed) {
    int mdelta = microseconds-last_microseconds;
    last_microseconds = microseconds;
  
    if ((mdelta <-900000)&&(mdelta>-1000000)) mdelta +=1000000;
    { //  if ((mdelta>0)&&(mdelta<4000000)) {
      mdelta = mdelta/packets_needed;
      deltasum += mdelta;
      deltacount++;
      if (deltacount==200) {
        int queue=0;
        queue=soundringtail-soundringhead;
        if (queue<0) queue += SOUNDRING_COUNT;

//        fprintf(stderr,"delta is %d underrun	%d	missing	%d	major	%d	blanked	%d\n"	"      queue %d        range   %d to %d\n"
//	   ,deltasum/deltacount,underruncount,
//           missingcount,majormissingcount,blankedcount,queue,(int)(minpcm*32768.),(int)(maxpcm*32768.));
        deltasum=0;
        deltacount=0;
        underruncount=0;
        missingcount=0;
        majormissingcount=0;
	blankedcount=0;
	minpcm=0.;
	maxpcm=0.;
        }
      }
    } /* if we are not major missing frame */
    
    
  if (opus_frames && write_file_spec) {
    unsigned char buffer[4000];
    unsigned int size;
    unsigned char *buf = buffer;
    buf +=2;
    int i;
    for (i=0;i<opus_frames;i++) {
      *buf++ = framesize[i]&0xff;
      *buf++ = framesize[i]>>8;
      int j;
      for (j=0;j<framesize[i];j++) {
        *buf++ = packet[framestart[i]+j];
        }
      }
    *buf++=0;  	
    *buf++=0;  	
    for (i=end_of_frames;i<sampleSize;i++) {
      *buf++=packet[i];
      }
    size = (buf-buffer)-2;
    buffer[0]=size&0xff;
    buffer[1]= (size>>8)&0xff;
    fwrite(buffer,size+2,1,write_file_spec);
    }
  
  
  /* checksum crap - didnt help */
  {
    int i;
    for (i=0;i<MAXFRAMES;i++) {
      if (recorded_checksums[i]!=actual_checksums[i]) {
        fprintf(stderr,"checksum err %d %x,%x\n",i,recorded_checksums[i],actual_checksums[i]);
	}
      }
    }
  
  int backup_frame_number;
  int raw_offset;
  if (useRaw) raw_offset = 1; else raw_offset = 0;
  
  
  if (no_sound==0) {
    /* do the uncompressed frame */
    if (useRaw) {
      int n;
      for (n = 0; n < 960; n+=4) {       
        soundBuffer[soundBufferSize++]=packet[n];
        soundBuffer[soundBufferSize++]=packet[n+1];
        soundBuffer[soundBufferSize++]=packet[n+2];
        soundBuffer[soundBufferSize++]=packet[n+3];
        }
      play_sound(&soundBufferSize,&underruncount,mustWriteOne_flag,possibly_play_flag,ignore_packets_flag);
      soundBuffer = soundring[soundringtail];
 
      mustWriteOne_flag=0;
      } /* if we use the raw packet */

    frame=newframe-opus_frames+1;
    for (backup_frame_number=opus_frames-1;backup_frame_number>=raw_offset;backup_frame_number--) {
            int opus_result;
            float pcm[4096];


            opus_result = opus_decode_float(opusdecoder,((unsigned char *)(packet))+framestart[backup_frame_number],
                             (opus_int32)(framesize[backup_frame_number])
                            ,pcm,(int)240,(int)0);
            int i;
            if (opus_result != 240) {
                fprintf(stderr,"opus_Result is %d not 240\n",opus_result);
              }

            for (i=0;i<240;i++) {
	     float m;
             short s;
	     if (pcm[i+i]>maxpcm) maxpcm=pcm[i+i];
	     if (pcm[i+i]<minpcm) minpcm=pcm[i+i];
	     m=pcm[i+i]*32768.;
	     if (m>32767.) s=32767;
	     else if (m<-32768.) s=32768;
	     else s=(int)m;
	     
             soundBuffer[soundBufferSize++]= s&0xff;
             soundBuffer[soundBufferSize++]= (s>>8)&0xff;
	     
	     m=pcm[i+i+1]*32768.;
	     if (m>32767.) s=32767;
	     else if (m<-32768.) s=32768;
	     else s=(int)m;
             soundBuffer[soundBufferSize++]= s&0xff;
             soundBuffer[soundBufferSize++]= (s>>8)&0xff;
             }
          /* here we added a frame to the buffer */
	  
	  /* so lets play it right away */
	  play_sound(&soundBufferSize,&underruncount,mustWriteOne_flag,possibly_play_flag,ignore_packets_flag);
          soundBuffer = soundring[soundringtail];
	  mustWriteOne_flag=0;	  
          } /* for each compressed buffer to add */
    } /* if we are making sound */
  } /* frame recovery block */

        

soundBufferSize = soundBufferSize;
if (no_sound) {
  soundBufferSize=0;
    frame = newframe;
    if (opus_frames != 1) {
//    fprintf(stderr,"Frame %d  flag %x   of %d  s %d  samplesize %d    \n",(int)frame,flag,opus_frames,soundBufferSize/4,sampleSize);
//      fprintf(stderr,"%d",opus_frames);
      }
  }
else {
  frame = newframe;
  if (opus_frames != 1) {
//    fprintf(stderr,"Frame %d  flag %x   of %d  s %d  samplesize %d    \n",(int)frame,flag,opus_frames,soundBufferSize/4,sampleSize);
//      fprintf(stderr,"%d",opus_frames);
    }
  } /* if we have sound */
	  
     
}
















  

int make_sound_from_file_packet(unsigned char *packet,int sampleSize) {

unsigned int flag;
unsigned int newframe;
unsigned int header_frame;
unsigned int microseconds;
unsigned char soundBuffer[960];
int soundBufferSize=0;

int opus_frames=0;
int framestart[MAXFRAMES];
int framesize[MAXFRAMES];
int end_of_frames = 0;
unsigned char recorded_checksums[MAXFRAMES];
unsigned char actual_checksums[MAXFRAMES];

int major_missed=0;

{ /* compute flag */
  int l = sampleSize-8;           /*-8 is flags */
  unsigned int i1=packet[l]&0xff;
  unsigned int i2=packet[l+1]&0xff;
  unsigned int i3=packet[l+2]&0xff;
  unsigned int i4=packet[l+3]&0xff;
  flag = i1+(i2<<8)+(i3<<16)+(i4<<24);
}

{ /* compute newFrame whatever that is */
        int l = sampleSize-16;                  /* -16 is frame */
    unsigned int i1=packet[l]&0xff;
    unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;
    microseconds = i1+(i2<<8)+(i3<<16)+(i4<<24);
}       


{ /* compute newFrame whatever that is */
        int l = sampleSize-20;                  /* -20 is header_frame */
    unsigned int i1=packet[l]&0xff;
    unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;

    header_frame = i1+(i2<<8)+(i3<<16)+(i4<<24);
}       
{ /* compute newFrame whatever that is */
        int l = sampleSize-28;                  /* -28 is frame */
    unsigned int i1=packet[l]&0xff;
    unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;

    newframe = i1+(i2<<8)+(i3<<16)+(i4<<24);
}       





int packets_needed;
if (frame==0) {
  frame = newframe-1;
  packets_needed = 2000;
  }
else {
  packets_needed = newframe-frame;
  if (packets_needed<0) packets_needed=2000;
  }

if (newframe != header_frame) {
//  fprintf(stderr,"frame %d header %d\n",newframe,header_frame);
  return 0;
  }
  
{/* Opus frame - including recovery */
  int look;
  look=0;
  /* figure out how many opus packets are there. */
  opus_frames = 0;
  while ((packet [look])&&(opus_frames<MAXFRAMES)&&(look<sampleSize)) {
            /* found a frame. */
            framestart[opus_frames]=look+2;
            framesize[opus_frames]= ((int)(packet[look]))& 0xff + (((int)(packet[look+1]))<<8);
	    actual_checksums[opus_frames]=checksum(packet+framestart[opus_frames],framesize[opus_frames]);
            look=look+2+framesize[opus_frames];
            opus_frames++;
            }
  end_of_frames = look;
  if ((packets_needed != 2000)&&(packets_needed>opus_frames)) {
    while (packets_needed>opus_frames) {
      int i;
      fprintf(stderr,"p");
      for (i=0;i<240;i++) {
        soundBuffer[soundBufferSize++]= 0;
        soundBuffer[soundBufferSize++]= 0;
        soundBuffer[soundBufferSize++]= 0;
        soundBuffer[soundBufferSize++]= 0;
        }
	
      play_direct(soundBuffer);

      soundBufferSize=0;
      packets_needed--;
      } /* while zeroing out for the missed section */
    fprintf(stderr,"\n");
    } /* if we should pad this a few frames */
  packets_needed=opus_frames;
  int backup_frame_number;
  
  frame=newframe-opus_frames+1;
  for (backup_frame_number=opus_frames-1;backup_frame_number>=0;backup_frame_number--) {
    int opus_result;
    float pcm[4096];

    opus_result = opus_decode_float(opusdecoder,((unsigned char *)(packet))+framestart[backup_frame_number],
                             (opus_int32)(framesize[backup_frame_number])
                            ,pcm,(int)240,(int)0);
            int i;
            if (opus_result != 240) {
                fprintf(stderr,"opus_Result is %d not 240\n",opus_result);
              }

    for (i=0;i<240;i++) {
      short s;
      short s2;
      s= (int)(pcm[i+i]*32768.);
	     
      soundBuffer[soundBufferSize++]= s&0xff;
      soundBuffer[soundBufferSize++]= (s>>8)&0xff;
      s= (int)(pcm[i+i+1]*32768.);
      soundBuffer[soundBufferSize++]= s&0xff;
      soundBuffer[soundBufferSize++]= (s>>8)&0xff;
      }
      
    play_direct(soundBuffer);
    soundBufferSize=0;
    frame++;
    } /* for each frame */
  frame = newframe;
  } /* opus section */
     
}

  
    
    
