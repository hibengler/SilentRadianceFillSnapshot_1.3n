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






//
// Created by hib on 6/16/16.
//
#include "glue.h"
#include "file_receiver.h"
#include "processor.h"
#include <time.h>
#include <fcntl.h>

int we_are_reading_file=0;
FILE *receiver_file;
unsigned long bytes_read=0;



/* note - this might be re-called by the receive loop based on various errors
because we sometimes get a recvfrrom failed error on android, when switching 
networks */
void init_receiver_file(char *filename) {
#define HEY_NONBLOCK 00004000
//the_network_block=block;

//#define WORLD_file_receiver__minus_ "-"
if (strcmp(filename,WORLD_file_receiver__minus_)==0) {
  receiver_file=stdin;
  }
else {
//#define WORLD_file_receiver_r "r"
  receiver_file = fopen(filename,WORLD_file_receiver_r);
  if (!receiver_file) {
    logit("cant open the file %s error %d\n",filename,errno);
    exit(-1);
    }
#ifdef __EMSCRIPTEN__
   /* for emscriptem, we turn async files. the read is much more resiliant - good code for handling slow pipes */
   int receiver_file_no = fileno(receiver_file);
   int f=fcntl(receiver_file_no, F_SETFL, O_NONBLOCK);
   if (!f) {
     logit("receiver failed\n");
     }
#endif     
    
  }
we_are_reading_file=1;
bytes_read=0;
//    we_polled=0;
return;
}


int slide_until_sr_indicator(char *startbuf,int number_in_buf) 
{
int i=0;
int state = 0;
char ch;
while (1) {
  if (i<number_in_buf) {
    ch=startbuf[i];
    }
  else {
    int f=fread((void *)startbuf,1,1,receiver_file);
    if (!f) return 0;
    bytes_read++;
    ch = startbuf[0];
    number_in_buf++;
    }
  if (state==0) {
    if (ch=='s') state = 1; else state=0;
    }
  else if (state==1) {
    if (ch=='r') state = 2; else state=0;
    }
  else if (state==2) {
    if (ch=='0') state = 3; else state=0;
    }
  else if (state==3) {
    if ((ch=='0')||(ch=='1')) return(1); else state=0;
    }
  i++;
  }
return number_in_buf;
}

#ifdef __EMSCRIPTEN__
/* this read_file is much more cleaner - it is not pipe loop closed. And it uses asynchronous reads */
/* might help with the curl implementation in non-em mode */

static unsigned char bufferbig[4000];
static int save_buffer=0;
static int save_len=0;



int get_packet_from_file() {
recvStringLen=0;
/* here we know that we will get one packet - because the polls told us */
/* Receive a single datagram from the server */
char *buffer = (char *)packetbuffer;
/* wait for processor.c */
  int queue_size=0;
  if (soundringcurrent != -1) {
    queue_size = soundringtail-soundringcurrent;
    if (queue_size<0) queue_size += SOUNDRING_COUNT;
    }
  if (queue_size >180) {
    struct timespec thislong;
    thislong.tv_sec = 0;
    thislong.tv_nsec = 10000000; /* 10 milliseconds */
		  
    return(0);    
    }
    	
if (save_buffer) { /* special read */
  recvStringLen=save_len;
  int f;
  f = fread((void *)buffer+save_buffer,1,save_len-save_buffer,receiver_file);  
  if (!f) {
    logit("error2 %d %ld total\n",errno,bytes_read);
    if (feof(receiver_file)) {
      if (stdin != receiver_file) {
        fclose(receiver_file);
        }
      receiver_file=NULL;
      we_are_reading_file = -1;
      recvStringLen=0;
      }
    return(0);
    }
  else if (f!=save_len-save_buffer) {
    save_buffer+=f;
    save_len=recvStringLen;
    recvStringLen=0;
    return(0);  
    }
  save_buffer = 0;
  save_len = 0;
  if (recvStringLen>4) {
    decrypt1((unsigned char *)buffer,recvStringLen-4,recvStringLen);
    }
  }

        
/*??? needs file work here on out */
reread:
if (!receiver_file) return 0;
int f=fread((void *)bufferbig,2,1,receiver_file);
if (f) {
  bytes_read += 2;
  recvStringLen = bufferbig[0] + (bufferbig[1]<<8);
//  logit("recvStringLen %d\n",recvStringLen);
  if ((recvStringLen<1)||(recvStringLen>2000)) {
    if (!slide_until_sr_indicator((void *)bufferbig,2)) return 0;
    recvStringLen=0;
    return(0);
//    goto reread;
    }
  }
else {
  logit("error %d   total: %ld\n",errno,bytes_read);
  if (feof(stderr)) {
    if (stdin != receiver_file) {
      fclose(receiver_file);
      }
    receiver_file=NULL;
    we_are_reading_file = -1;  
    }
  return(0);
  }
f = fread((void *)buffer,1,recvStringLen,receiver_file);  
bytes_read += f;
if (!f) {
  logit("error2 %d %ld total\n",errno,bytes_read);
  if (feof(stderr)) {
    if (stdin != receiver_file) {
      fclose(receiver_file);
      }
    receiver_file=NULL;
    we_are_reading_file = -1;
    }
  recvStringLen=0;
  return(0);
  }
else if (f!=recvStringLen) {
  save_buffer+=f;
  save_len=recvStringLen;
  recvStringLen=0;
//  logit("mismatch %d to %d\n",f,recvStringLen);
  return(0);  
  }
if (recvStringLen>4) {
  decrypt1((unsigned char *)buffer,recvStringLen-4,recvStringLen);
  }
return(1);
}
#else
/* regular blocked reading */

int get_packet_from_file() {
/* here we know that we will get one packet - because the polls told us */
/* Receive a single datagram from the server */
char *buffer = (char *)packetbuffer;
/* wait for processor.c */
do {
  int queue_size = 0;
  if (soundringcurrent != -1) {
    queue_size = soundringtail-soundringcurrent;
    if (queue_size<0) queue_size += SOUNDRING_COUNT;
    }
  if (queue_size >60) {
    struct timespec thislong;
    thislong.tv_sec = 0;
    thislong.tv_nsec = 10000000; /* 10 milliseconds */
    nanosleep(&thislong, &thislong);
    }	
  else {
    break;
    }					    
  } while (1);
  

unsigned char bufferbig[4000];
reread:
if (!receiver_file) return 0;
ssize_t f=fread((void *)bufferbig,2,1,receiver_file);
if (f) {
  bytes_read += 2;
  recvStringLen = bufferbig[0] + (bufferbig[1]<<8);
//  logit("recvStringLen %d\n",recvStringLen);
  if ((recvStringLen<1)||(recvStringLen>2000)) {
    if (!slide_until_sr_indicator(bufferbig,2)) return 0;
    goto reread;
    }
  }
else {
  logit("error %d   total: %ld\n",errno,bytes_read);
  if (stdin != receiver_file) {
    fclose(receiver_file);
    }
  receiver_file=NULL;
  we_are_reading_file = -1;  
  return(0);
  }
f = fread((void *)buffer,1,recvStringLen,receiver_file);  
bytes_read += f;
if (!f) {
  logit("error2 %d %ld total\n",errno,bytes_read);
  if (stdin != receiver_file) {
    fclose(receiver_file);
    }
  receiver_file=NULL;
  we_are_reading_file = -1;
  return(0);
  }
else if (f!=recvStringLen) {
  logit("mismatch %ld to %ld\n",f,recvStringLen);
  if (stdin != receiver_file) {
    fclose(receiver_file);
    }
  receiver_file=NULL;
  we_are_reading_file = -1;
  return(0);  
  }
if (recvStringLen>4) {
  decrypt1((unsigned char *)buffer,recvStringLen-4,recvStringLen);
  }
return(1);
}
#endif






void shutdown_file_reader() {
if (receiver_file) {
  fclose(receiver_file);
  receiver_file = NULL;
  }
/* almost want to turn off the we_are_reading_file indicator - but some other things might not be initialized
  So we will set it to -1 */
we_are_reading_file = -1;
}
