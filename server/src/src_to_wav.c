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

This example reads standard from input and writes
to the default PCM device for 5 seconds of data.

*/

#include <stdio.h> /* for printf() and fprintf() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"

#include "wav.h"



OpusDecoder *opusdecoder=NULL;
static int frame=0;
  wav_t *wav_file;








#define MAXFRAMES 12




unsigned char checksum(unsigned char *compress,int compress_length) {
unsigned char sum = 0;
int i;
for (i=0;i<compress_length;i++) {
  sum +=compress[i];
  }
} 
    


int convert_to_wav(unsigned char *packet,int sampleSize,wav_t *wav_file) {

unsigned int flag;
unsigned int newframe;
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
        int l = sampleSize-20;                  /* -20 is frame */
    unsigned int i1=packet[l]&0xff;
    unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;

    newframe = i1+(i2<<8)+(i3<<16)+(i4<<24);
}       



{/* Opus frame - including recovery */
  int packets_needed;
  int look;
  look=0;
  /* figure out how many opus packets are there. */
  opus_frames = 0;
  while ((packet [look])&&(opus_frames<MAXFRAMES)&&(look<sampleSize)) {
            /* found a frame. */
            framestart[opus_frames]=look+1;
            framesize[opus_frames]= ((int)(packet[look]))& 0xff;
	    actual_checksums[opus_frames]=checksum(packet+framestart[opus_frames],framesize[opus_frames]);
            look=look+1+framesize[opus_frames];
            opus_frames++;
            }
  end_of_frames = look;
  packets_needed=opus_frames;
  int backup_frame_number;
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
      unsigned short s;
      s= (int)(pcm[i+i]*32768.);
	     
      soundBuffer[soundBufferSize++]= s&0xff;
      soundBuffer[soundBufferSize++]= (s>>8)&0xff;
      s= (int)(pcm[i+i+1]*32768.);
      soundBuffer[soundBufferSize++]= s&0xff;
      soundBuffer[soundBufferSize++]= (s>>8)&0xff;
      }
    void *a[1];
    a[0]=soundBuffer;
    wav_write(wav_file,240,FALSE,FALSE,(void **)(&a));
    soundBufferSize=0;
    frame++;
    } /* for each frame */
  frame = newframe;
  }
     
}





int main(int argc,char *argv[]) {
if (argc<3) {
  fprintf(stderr,"usage: src_to_wav source_file.src dest_file.wav\n");
  exit(2);
  }
FILE *read_file_spec =fopen(argv[1],"r");
if (!read_file_spec) {
  fprintf(stderr,"error - cannot read input file\n");
  }

{
  wav_err_t wav_err;
  wav_file = wav_open(argv[2],TRUE,FALSE,&wav_err);
  if (!wav_file) {
    fprintf(stderr,"Error %d (%s) opening wave file\n",wav_err,wav_get_err_string(wav_err));
    exit(-1);
    }
  
  wav_fmt_t format = WAV_FMT_PCM_TYPE_1;
  format = 1;
  int num_channels = 2;
  int sample_freq=48000;
  int bits_per_sample=16;
  wav_err=wav_write_header(wav_file,format,num_channels,sample_freq,bits_per_sample);
  if (wav_err) {
    fprintf(stderr,"Error %d (%s) writing header for wave file\n",wav_err,wav_get_err_string(wav_err));
    exit(-1);
    }
  }




{ /* opus */
    int err;
    opusdecoder = opus_decoder_create((opus_int32)(48000),2,&err);
    if (err != OPUS_OK)
       {
         fprintf(stderr,"OPUS decoder fail %d\n",err);
       opusdecoder = NULL; /* error - decode will just be off */
       }

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
  if (strncmp(buffer+packet_size-4,"sr01",4)!=0) {
      fprintf(stderr,"PAcket error: %c%c%c%c\n",buffer[packet_size-4],
           buffer[packet_size-3],buffer[packet_size-2],buffer[packet_size-1]);
      }
  convert_to_wav(buffer,packet_size,wav_file);
  }
fclose(read_file_spec);
wav_close(wav_file);  
exit(0);
}

