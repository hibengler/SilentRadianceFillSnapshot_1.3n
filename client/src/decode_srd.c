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

#include "world_internationalization.h"


/*

This example reads standard from input and writes
to the default PCM device for 5 seconds of data.

*/

/* Use the newer ALSA API */
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <errno.h>

#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"
#include "music_decoder.h"
#include "sound_generic.h"

#include <alsa/asoundlib.h>

OpusDecoder *opusdecoder=NULL;

unsigned char checksum(unsigned char *compress,int compress_length) {
unsigned char sum = 0;
int i;
for (i=0;i<compress_length;i++) {
  sum +=compress[i];
  }
return(sum);
} 


  int init_send_sound=1;
  int useRaw=1;
  long loops;
  int rc;
  int size;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  char *buffer;
  int soundpolls_count;
  int socket_poll;
  
  int sound_state = 0;
  char zero_buffer[4000];
  
  static int frame=0;
int no_sound = 0;


#define NETRING_COUNT 200
volatile int netringhead=0;
volatile int netringtail=0;
char *netring[NETRING_COUNT];
int netlen[NETRING_COUNT];


int sock; /* Socket */
struct sockaddr_in broadcastAddr; /* Broadcast Address */
unsigned short broadcastPort; /* Port */





int init_receiver_sound(int block) {
} 
int finish_play_sound() {
return(0);
}


void DieWithError(char *errorMessage); /* External error handling function */

void DieWithError(char *errorMessage)
{
perror(errorMessage);
exit(1);
}











    
	
	    
		
		    
			
			        














  


int dump_sound_from_file_packet(unsigned char *packet,int sampleSize) {

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

//if (newframe != header_frame) {
//  fprintf(stderr,"frame %d header %d\n",newframe,header_frame);
//  return 0;
//  }
  
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
      fprintf(stderr,"missing frame %d, newfreame %d needed %d  opus_frames %d\n",frame,newframe,packets_needed,opus_frames);
      for (i=0;i<240;i++) {
        soundBuffer[soundBufferSize++]= 0;
        soundBuffer[soundBufferSize++]= 0;
        soundBuffer[soundBufferSize++]= 0;
        soundBuffer[soundBufferSize++]= 0;
        }

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
/*
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
  */    
    soundBufferSize=0;
    fprintf(stdout,"frame %6.6d check %x\n",frame,actual_checksums[backup_frame_number]);
    frame++;
    } /* for each frame */
  frame = newframe;
  } /* opus section */
     
}

  
    
    
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

  
    
    
int main_file(int argc,char *argv[]) {
init_music_decoder();
//#define WORLD_decode_srd_r "r"
FILE *read_file_spec =fopen(argv[1],WORLD_decode_srd_r);
if (!read_file_spec) {
  fprintf(stderr,"error - cannot read input file %s\n",argv[1]);
  exit(-1);
  }




unsigned char bytes_array[2];
while (fread(bytes_array,2,1,read_file_spec)) {
  int packet_size;
  int read_size;
  char buffer[65536]; 

  packet_size = bytes_array[0] + ((int)((bytes_array[1]))<<8);

  read_size = fread(buffer,packet_size,1,read_file_spec);
  if (!read_size) {
    break;
    }
//#define WORLD_SR_PROTOCOL_SR01 "sr01"
  if ((strncmp(buffer+packet_size-4,WORLD_SR_PROTOCOL_SR01,4)!=0)&& 
//#define WORLD_SR_PROTOCOL_SR00 "sr00"
      (strncmp(buffer+packet_size-4,WORLD_SR_PROTOCOL_SR00,4)!=0))    {
      fprintf(stderr,"Packet error: %c%c%c%c\n",buffer[packet_size-4],
           buffer[packet_size-3],buffer[packet_size-2],buffer[packet_size-1]);
      }
  dump_sound_from_file_packet(buffer,packet_size); 
  }
fclose(read_file_spec);
exit(0);
}





int main(int argc,char *argv[]) {
    return main_file(argc,argv);
}
