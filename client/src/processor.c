/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2018 Hibbard M. Engler

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
// Created by hib on 6/17/16.
//


#include "glue.h"
#include "processor.h"
#ifdef _WIN32
#include "win64_network_receiver.h"
#else
#include "network_receiver.h"
#endif


OpusDecoder *opusdecoder=NULL;

static int frame=0;
FILE *write_file_spec= NULL;

volatile int soundringhead=0; /* start of the soundringqueue. normally incremented by sound.  But could overflow */
volatile int soundringfirst=0; /* also incrmented by processor.c - probably the first viable sound in the ring */
volatile int soundringtail=0; /* the end on the sound ring. this is incremented by processor.c whenever a packet is added */
volatile int soundringcurrent=-1; /* we are playing this RIGHT NOW: -1 if we are not playing.  This is updated by the sound subsystem */

/*
collections :  either char opus or short, depending on if the length is negative (opus) or positive (PCM). ordered by frame number. has gaps.
            collectioncommands - only filled on a few.

commands -> the commands active via frame

collectioncommands - commands that were loaded for the packet but not yet assigned to their commandring/soundring.
This is for SR01 and SR00.
When the soundring is incremented, the commandring is swapped to point to a collection command, again saving us to copy.

soundring - goes to sound.
commandring - list of latest commands - useful for SR01, SR00 not so much
commandring is congruent with soundring.   Where collectioncommands is loose.

	     Note - the buffers in collections are swapped with the buffers in soundring.   This is done as a quick way to change soundrings without having to copy memory - again.
	                  
            -> sound
*/	    


volatile short *soundring[SOUNDRING_COUNT];  /* points to soundring_area */
volatile short soundring_area[SOUNDRING_COUNT][960];




  
  
volatile struct processor_stat soundstat_area[SOUNDRING_COUNT];
volatile struct processor_stat *soundstat[SOUNDRING_COUNT];

volatile char *commandring[SOUNDRING_COUNT]; /* points to commandring_area */
volatile char commandring_area[SOUNDRING_COUNT][2000]; /* commands for the sound ring */
volatile int commandlen[SOUNDRING_COUNT];





/* This handles the multi-packet stuff */
int base_frame=-1;
int collectionhead=0;
short *collections[COLLECTION_COUNT]; /* points to colections_area */
volatile short collections_area[COLLECTION_COUNT][960];

volatile struct processor_stat collectionstat_area[COLLECTION_COUNT];
volatile struct processor_stat *collectionstat[COLLECTION_COUNT];


unsigned char *collectioncommands[COLLECTION_COUNT]; /* points to collectioncommands_area */
volatile char collectioncommands_area[COLLECTION_COUNT][2000]; /* shared with commandring_area */
int collectioncommandlen[COLLECTION_COUNT];
int collectioncommandcount=0;
int command_frame[COLLECTION_COUNT];/* -1 if unknown and need to be processed */

int collection_size;



#ifdef future_or_something
static int float_from_command(char *l) {
float value;
unsigned char *pt = (unsigned char *)&value;
pt[0]=l[0];
pt[1]=l[1];
pt[2]=l[2];
pt[3]=l[3];
return value;
}
#endif


void init_processor_buffers() {

{
  int i;
  for (i=0;i<SOUNDRING_COUNT;i++) {
      soundring[i]=soundring_area[i];
      volatile struct processor_stat *stat;
      stat =soundstat[i]=soundstat_area+i;
      stat->len=0;
      stat->frame=-1;
      stat->version=0;
      
      commandring[i]=commandring_area[i];
      collections[i]=(short *)collections_area[i];
      collectioncommands[i]=(unsigned char *)collectioncommands_area[i];
      
      //volatile struct processor_stat *stat;
      stat = collectionstat[i]=collectionstat_area+i;
      stat->len=0;
      stat->frame=-1;
      stat->version=0;
      
      collectioncommandlen[i]=0;
      command_frame[i]=-1;
      }
  collection_size=0;
      
  }

    soundringhead=0;
    soundringtail=0;
    soundringfirst=0;
    soundringcurrent=-1;
    base_frame=-1;
    collectionhead=0;
    collectioncommandcount=0;
    
    
    frame = -1;
}




int opus_frame=-1;
float minpcm=0.;
float maxpcm=0.;

void init_processor() {
    logit("init_processor start");
    { /* opus */
        int err;
        opusdecoder = opus_decoder_create((opus_int32)(48000),2,&err);
        if (err != OPUS_OK)
        {
            logit("OPUS decoder fail %d\n",err);
            opusdecoder = NULL; /* error - decode will just be off */
        }

    }

    /* init soundrings */
init_processor_buffers();
    logit("init_processor done");
}

void shutdown_processor() {
}





unsigned char checksum(unsigned char *compress,int compress_length) {
unsigned char sum = 0;
int i;
for (i=0;i<compress_length;i++) {
  sum +=compress[i];
  }
return(sum);
} 






static void process_packet_sr01(int flag_override);





void *memcpy1(void *dest, const void *src, size_t n) {return memcpy(dest,src,n);}
void *memcpy2(void *dest, const void *src, size_t n) {return memcpy(dest,src,n);}
void *memcpy3(void *dest, const void *src, size_t n) {return memcpy(dest,src,n);}
void *memcpy4(void *dest, const void *src, size_t n) {return memcpy(dest,src,n);}
void *memcpy5(void *dest, const void *src, size_t n) {return memcpy(dest,src,n);}
void *memcpy6(void *dest, const void *src, size_t n) {return memcpy(dest,src,n);}
void *memcpy7(void *dest, const void *src, size_t n) {return memcpy(dest,src,n);}
void *memcpy8(void *dest, const void *src, size_t n) {return memcpy(dest,src,n);}
void *memcpy9(void *dest, const void *src, size_t n) {return memcpy(dest,src,n);}
void *memcpy10(void *dest, const void *src, size_t n) {return memcpy(dest,src,n);}

void process_packet_main(int flag_override) {
//fprintf(stderr,"size %d ",recvStringLen);
int justwrite=0;
short *soundBuffer;
int soundBufferSize = 0;
watchdog_point=3;
//fprintf(stderr,"process_packet_main override %d sampleSize %d\n",flag_override,sampleSize);
if (!opusdecoder) return;
if (!soundring[soundringtail]) {
        logit("bad sound ring not allocated %d",soundringtail);
        return;
  }
if (!commandring[soundringtail]) {
        logit("bad command ring not allocated");
        return;
  }

if (!collections[soundringtail]) {
        logit("bad collection sound ring not allocated");
        return;
  }
if (!collectioncommands[soundringtail]) {
        logit("bad collection command ring not allocated");
        return;
  }
    
    
int sampleSize=recvStringLen;

unsigned char *packet = packetbuffer;


if (sampleSize<12+2) return;
if (sampleSize >PACKETMAXSIZE-2) {
  logit("sampleSize too big");
  return;
  }

int header_frame;  /* the frame number for the first frame in the packet */
int newframe;  /* the frame number for the last frame in the packet */
int opus_frames=0; /* a count of the number of opus frames in the packet. We compute this and then extract the frames
                      in an order - so we can decode opus in order so it sounds good */
int framestart[MAXFRAMES];  /* the start byte position of each opus frame */
int framesize[MAXFRAMES];   /* the size in bytes of each opus feame - from 0 to opus_frames */
int end_of_frames;          /* the byte offset in the packet where there are no more frames - after the 00 00 size 
                               This could be the entire sample size */


    
//logit("version %c%c%c%c",packet[sampleSize-4]    ,packet[sampleSize-3]    ,packet[sampleSize-2]    ,packet[sampleSize-1]    );
/* handle the SR01 packets if this is SR01 */    
if ((packet[sampleSize-1])=='1') { /* SR01 older transmission */
  process_packet_sr01(flag_override);
  return;
  }

/* get the command frame number (for last frame), header_frame, and flag */
{
    int l = sampleSize-12;
    unsigned int i1=packet[l]&0xff;
    unsigned int i2=packet[l+1]&0xff;
    unsigned int i3=packet[l+2]&0xff;
    unsigned int i4=packet[l+3]&0xff;

    newframe = i1+(i2<<8)+(i3<<16)+(i4<<24);
    }
    
{
  int l = sampleSize-8;
  unsigned int i1=packet[l]&0xff;
  unsigned int i2=packet[l+1]&0xff;
  unsigned int i3=packet[l+2]&0xff;
  unsigned int i4=packet[l+3]&0xff;

  header_frame = i1+(i2<<8)+(i3<<16)+(i4<<24);
  }

{
  int sanity = newframe - header_frame;
  if (sanity<0) sanity = -sanity;
  if (sanity >400) {
     logit ("newframe to header_frame is too big: %d",sanity);
     return;
     }
  }
//fprintf(stderr,"newframe %d header_frame %d\n",newframe,header_frame);

unsigned int flag=0;
{ /* compute flag */
        int l = packet[0]+packet[1]*256+PER_FRAME_OVERHEAD;
	if (l<=PER_FRAME_OVERHEAD) {
	  logit("bad collection no first frame");
	  return;
	  }
	if (l>=sampleSize) {
	  logit("bad collection first length too big: %d",l);
	  // qqq if this happens when streaming, we can get in a bad place where we cant get out
	  return;
	  }
	l = l+2-4; /* flags position for first packet */
        unsigned int i1=packet[l]&0xff;
        unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;
        flag = i1+(i2<<8)+(i3<<16)+(i4<<24);
        }


    
    
    
// fprintf(stderr,"   first %d bytes:\n      ",sampleSize);
//  {int i; for (i=0;i<sampleSize;i++) fprintf(stderr,"%d ",packet[i]);}
//  fprintf(stderr,"\n");




int look;
look=0;
/* figure out how many opus packets are there. */
opus_frames = 0;
while ( ( (packet [look])||(packet[look+1]) )&&(opus_frames<MAXFRAMES)&&(look<sampleSize)) {
            /* found a frame. */
            framestart[opus_frames]=look+2;
	    unsigned int pl1 = packet[look];
	    unsigned int pl2 = packet[look+1];
	    pl2 = pl2 <<8;
	    
            framesize[opus_frames]= pl1+pl2;
//	    fprintf(stderr,"framestart %d opus_frames %d framesize[opus_frames] %d\n",
//	        framestart[opus_frames],opus_frames,framesize[opus_frames]);
	    // if framesize is too big, we will skip this frame
	    if ( framesize[opus_frames] +look+2+PER_FRAME_OVERHEAD >=sampleSize) {
// WORLD_INTERNATIONAL off
	      logit("newframe %d framesize[%d](%d) + %d + 2"
	      " + %d > sample size (%d)",newframe,opus_frames, framesize[opus_frames],look,PER_FRAME_OVERHEAD ,sampleSize);
// WORLD_INTERNATIONAL on
	      return;
	      }
            look=look+2+framesize[opus_frames]+PER_FRAME_OVERHEAD;
            opus_frames++;
            }
	
end_of_frames = look+2;
if (end_of_frames >sampleSize) end_of_frames=sampleSize;


	
		
			
	
		
//logit("Frame %d-%d head %d flag %x	base %d	opus_frame %d\n",newframe-opus_frames+1,newframe,header_frame,flag,base_frame,opus_frame);


/* flag_override is for non-broadcast packets */
flag=flag|flag_override;


/* check to see if we are out of bounds */
if ((base_frame==-1)||(newframe <base_frame-SOUNDRING_COUNT)||(newframe>=base_frame+SOUNDRING_COUNT)) { /* starting out */
      if (flag&4) {
//        logit("frame %d head %d flag %x\n",newframe,header_frame,flag);
//           logit("Reset base_frame %d newframe %d (not between %d and %d )\n",base_frame,newframe,base_frame-SOUNDRING_COUNT,base_frame+SOUNDRING_COUNT);
	  
        base_frame = newframe - MAXFRAMES;
        if (base_frame<0) base_frame=0;
        collectionhead=0;
        int i;
        for (i=0;i<SOUNDRING_COUNT;i++) {
          collectionstat[i]->len=0;  
	  collectioncommandlen[i]=0;
	  command_frame[i]=0;
	  }
        collectioncommandcount=0;
	opus_frame=-1;
	}
  else {
        justwrite=1;
	}
  }
/* check to see if we are out of opus decoding range */
else if ((flag==4) && (newframe-opus_frames+1 > opus_frame) && (opus_frame >0)) {
//  logit("Reset opus_frames %d base_frame %d newframe %d never reach %d  !!!!!!!!!!!!!!!!!!",opus_frames,base_frame,newframe,opus_frame);
//  logit("Reset (newframe-opus_frames+1) is %d-%d+1  and opus_frame is %d\n",newframe,opus_frames,opus_frame);
  opus_frame = -1;
  }

			
						
												
	
if (justwrite==1) {
//      logit("Skipping, cant reset because not a starter frame");
  return;
  }	
        




/* copy the sub-packet into collections - based on the frame number.  
In collections, the length is negative if we did not run opus yet
We do that so that we call the opus decode in order 

Also - we add to soundbuffer
*/

int backup_frame_number;

for (backup_frame_number=opus_frames-1;backup_frame_number>=0;backup_frame_number--) {
  int frame = newframe-backup_frame_number;
//	    logit("process frame %d base %d",frame,base_frame);
  if (frame<base_frame) {
    continue;
    }
  int framepos=(collectionhead+frame-base_frame)%SOUNDRING_COUNT;
  if (framepos<0) {logit("QQQ %d+%d-%d %d",collectionhead,frame,base_frame,framepos);}
  if (collectionstat[framepos]->len) {
    continue; /* already decoded this one */
    }
  if (collectionstat[framepos]->frame==frame) { /* should be there */
    continue; /* already decoded this one */
    }

	      
  int opus_result;
  float pcm[480];
  minpcm=0.;
  maxpcm=0.;
  soundBuffer = collections[framepos];
  soundBufferSize = 0;
//            logit("opus %d frame %d",opus_frame,frame);
  if ((opus_frame !=-1)&&(opus_frame != frame-1)) { 
    //	      logit("qq frame %d to %d ",opus_frame,frame);
    //	      logit("backup_frame_number %d opus_frames %d  base %d cframe %d\n",backup_frame_number,opus_frames,base_frame,cframe);
    unsigned char *use_sound_buffer_for_opus = (unsigned char *)collections[framepos];
    memcpy1(use_sound_buffer_for_opus,((unsigned char *)(packet))+framestart[backup_frame_number],framesize[backup_frame_number]);
    memcpy2((void *)(collectionstat[framepos]->stats_area),
          ((unsigned char *)(packet))+framestart[backup_frame_number]+framesize[backup_frame_number],PER_FRAME_OVERHEAD);
	  
//    unsigned char *zz=collectionstat[framepos]->stats_area;
//    logit("frmepos %d frame %d statarea %2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x\n",
//       framepos,frame,zz[0],zz[1],zz[2],zz[3],zz[4],zz[5],zz[6],zz[7],zz[8],zz[9],zz[10],zz[11],zz[12],zz[13],zz[14],zz[15]);
    
    collectionstat[framepos]->frame=frame;
    collectionstat[framepos]->len= -framesize[backup_frame_number]; /* flag - don't decode this yet because we want to decode in order */
    memcpy3((void *)(&(collectionstat[framepos]->version)),packet+sampleSize-4,4*sizeof(char)); /* version of the packet */
    continue;
    }

  /* here the last opus frame was the previous frame. Cool. lets process it */	 
  opus_frame = frame;
  opus_result = opus_decode_float(opusdecoder,((unsigned char *)(packet))+framestart[backup_frame_number],
                                            (opus_int32)(framesize[backup_frame_number]),pcm,(int)240,(int)0);
  int i;
  if (opus_result != 240) {
    logit("opus_Result is %d not 240\n",opus_result);
    }
    //          logit("packlet %d,%d,%d\n",((unsigned char *)(packet))[framestart[backup_frame_number]],
    //           ((unsigned char *)(packet))[framestart[backup_frame_number]+1],((unsigned char *)(packet))[framestart[backup_frame_number]+2]);

  for (i=0;i<480;i++) {
    short s;
                    //     if (i<3)   	        logit("test %d %f,%f\n",i,pcm[i+i],pcm[i+i+1]);
    float ma;
    float m;
    ma=pcm[i];
    m=ma*32768.;
    if (m>32767.) {
      s=32767;
      maxpcm=1.;
      }
    else if (m<-32768.) {
      s=-32768;
      minpcm=-1.;
      }
    else {
      s=(int)m;
      if (ma>maxpcm) maxpcm=ma;
      if (ma<minpcm) minpcm=ma;
      }
    soundBuffer[soundBufferSize++]=s;
    } /* for each sample * 2 channels */
	    
  memcpy4((void *)(collectionstat[framepos]->stats_area),
          ((unsigned char *)(packet))+framestart[backup_frame_number]+framesize[backup_frame_number],PER_FRAME_OVERHEAD);
	  
//  unsigned char *zz=collectionstat[framepos]->stats_area;
//  logit("grmepos %d frame %d statarea %2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x %2.2x%2.2x%2.2x%2.2x%2.2x%2.2x\n",
//       framepos,frame,zz[0],zz[1],zz[2],zz[3],zz[4],zz[5],zz[6],zz[7],zz[8],zz[9],zz[10],zz[11],zz[12],zz[13],zz[14],zz[15]);
  /* set the collection frame number */
  collectionstat[framepos]->frame=frame;
  collectionstat[framepos]->len=soundBufferSize;
  memcpy5((void *)(&(collectionstat[framepos]->version)),packet+sampleSize-4,4*sizeof(char)); /* version of the packet */
  } /* for each compressed buffer */
	  
/* sanity checks */
if (collectioncommands[collectioncommandcount]==NULL) {
  logit("error null");
  }
if (packet==NULL) {
  logit("packet null");
  }
  
  



if (collectioncommandcount>SOUNDRING_COUNT) {
  collectioncommandcount=0;
  logit("overflow collection command count");
  }
/* place the last chunk into collectioncommands - collectioncommandcount - is a modulo counter */
memcpy6(collectioncommands[collectioncommandcount],(packet+end_of_frames),sampleSize-end_of_frames);
collectioncommandlen[collectioncommandcount]=sampleSize-end_of_frames;
command_frame[collectioncommandcount]=header_frame;
//	logit("record frame %d",header_frame);
collectioncommandcount++;
if (collectioncommandcount>SOUNDRING_COUNT) {
  collectioncommandcount=0;
  logit("overflow collection command counta");
  }
	


/* handle frames that are negative lengths - those are not compressed */
{
  int xframe=opus_frame+1;
  if (xframe<base_frame) {
//    logit("did I reset?");
    xframe=base_frame;
    }
  while (1) {
    int framepos=(collectionhead+xframe-base_frame)%SOUNDRING_COUNT;
    if (framepos<0) {logit("QQQ2 %d+%d-%d %d",collectionhead,xframe,base_frame,framepos);}
    if (collectionstat[framepos]->len>=0) break; /* only do if this is negative - meaning it is a cached packet */
    
    /* Cool - here we call opus to decode the packet in order */
    int opus_result;
    float pcm[480];
    minpcm=0.;
    maxpcm=0.;
    soundBuffer = collections[framepos];
    soundBufferSize = 0;
    opus_frame = xframe;
//	    logit("Special decode frame %d",opus_frame);
    opus_result = opus_decode_float(opusdecoder,((unsigned char *)soundBuffer),
                                            (opus_int32)(-collectionstat[framepos]->len)
                    ,pcm,(int)240,(int)0);
    int i;
    if (opus_result != 240) {
      logit("opus_Result is %d not 240\n",opus_result);
      }
  //          logit("packlet %d,%d,%d\n",((unsigned char *)(packet))[framestart[backup_frame_number]],
    //           ((unsigned char *)(packet))[framestart[backup_frame_number]+1],((unsigned char *)(packet))[framestart[backup_frame_number]+2]);

    for (i=0;i<480;i++) {
      short s;
                    //     if (i<3)   	        logit("test %d %f,%f\n",i,pcm[i+i],pcm[i+i+1]);
                float ma;
                float m;
                ma=pcm[i];
                m=ma*32768.;
                if (m>32767.) {
                    s=32767;
                    maxpcm=1.;
                    }
                else if (m<-32768.) {
                    s=-32768;
                    minpcm=-1.;
                }
                else {
                    s=(int)m;
                    if (ma>maxpcm) maxpcm=ma;
                    if (ma<minpcm) minpcm=ma;
                }
      //          if (i<3)   	        logit("SB %d ss %d\n",(int)soundBuffer,soundBufferSize);
                soundBuffer[soundBufferSize++]=s;
                } /* for each sample * 2 channels */
	    
    /* set the collection frame number */
    collectionstat[framepos]->len=soundBufferSize;
	    
	    
    xframe = opus_frame+1;
    } /* while doing stashed packets */
  } /* block to handle the stashed packets that came out of order. We call opus_decode in order */
	    
	  
	
	
{
       /* figure out if we have enough frames to plow through */
       int start; /* start is the start frame number */
       for (start=base_frame;start<base_frame+SOUNDRING_COUNT;start++) {
         int framepos=(collectionhead+start-base_frame)%SOUNDRING_COUNT;
	 if (framepos<0) {logit("QQQ3 %d+%d-%d %d",collectionhead,start,base_frame,framepos);}
         if (collectionstat[framepos]->len>0) break;
	 }

       int end; /* just after the end*/
       for (end=start;end<base_frame+SOUNDRING_COUNT;end++) {
         int framepos=(collectionhead+end-base_frame)%SOUNDRING_COUNT;
	 if (framepos<0) {logit("QQQ4 %d+%d-%d %d",collectionhead,end,base_frame,framepos);}
         if (collectionstat[framepos]->len<=0) break;
	 }
       collection_size=end-start;
//       logit("from %d to %d oldframe %d newframe %d base %d",start,end,frame,newframe,base_frame);       
       /* We will now add if the subsequent frame number is in our range,  or if we have more than 40 loaded up */
       if ( (end-start >= NUMBER_OF_PACKETS_BEFORE_ACTIVATING_SOUND)||((frame+1 >=start)&&(frame+1<end)) ) {
         int i;
         /* great - we can load up the ring buffer quickly now  - first - we can get rid of anything too far in the past */
	 if ((frame+1 >start)&&(frame+1<end)) {
//	   logit("flush %d from %d(%d)",end-frame-1,start,frame+1);
	   for (i=start;i<frame+1;i++) {
	     int framepos=(collectionhead+i-base_frame)%SOUNDRING_COUNT;
	     if (framepos<0) {logit("QQQ5 %d+%d-%d %d",collectionhead,i,base_frame,framepos);}
	     collectionstat[framepos]->len=0;
	     collectionstat[framepos]->frame=0;
	     }
	   start=frame+1;
	   }
//	 else  	     logit("flush %d from %d",end-start,start);

	 
	 /* process the collection frames to figure out the actual frames 
	   This is done by guessing forward and backward for records close to the original 
	 */
//	 int j;
//	 for (j=0;j<collectioncommandcount;j++) {
//	   logit("_ %d %d",j,command_frame[j]);
//	   }
	   
	 /* Move these over quickly */
	 int tail=soundringtail;
	 for (i=start;i<end;i++) {
	   int framepos=(collectionhead+i-base_frame)%SOUNDRING_COUNT;
	   if (framepos<0) {logit("QQQ6 %d+%d-%d %d",collectionhead,i,base_frame,framepos);}
	   /* set the sound frame */
	   frame = collectionstat[framepos]->frame;  
//	   logit("ffffffffffffff %d at %d q %d",collectionstat[framepos]->frame,framepos,tail);
	   { /* swap the buffers for the collections to the ring buffer */
	     short *t;
	     t=(short *)soundring[tail];
	     soundring[tail]=collections[framepos];
	     collections[framepos]=t;
	     if (!soundring[tail]) {
	       logit("found bug %d \n",framepos);
	       }
	     volatile struct processor_stat *tstat;
	     tstat=soundstat[tail];soundstat[tail]=collectionstat[framepos];collectionstat[framepos]=tstat;
	         
//             unsigned char *zz=soundstat[tail]->stats_area;
//    logit("move to tail %d frmepos %d frame %d statarea %2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x\n",
//       tail,framepos,frame,zz[0],zz[1],zz[2],zz[3],zz[4],zz[5],zz[6],zz[7],zz[8],zz[9],zz[10],zz[11],zz[12],zz[13],zz[14],zz[15]);

	     }
	   
	   collectionstat[framepos]->frame=0;
	   collectionstat[framepos]->len=0;
	   
	   /* try to pin the collection to the frame */
           {
	   int j;
	   for (j=0;j<collectioncommandcount;j++) {
	     if (command_frame[j]==i) {
	       /* got the command frame for this one */
	       { /* first swap with the commandstring */
	         volatile unsigned char *t;
	         t=collectioncommands[j];collectioncommands[j]=(unsigned char *)commandring[tail];commandring[tail]=(volatile char *)t;
		 commandlen[tail]=collectioncommandlen[j];
		 collectioncommandlen[j]=0;
		 command_frame[j]=-1;
		 /* redo the frame number */
		 int l=commandlen[tail];
		 volatile unsigned char *x=(volatile unsigned char *)commandring[tail];
//		 logit("setting frame number from %d to %d (header is %d, len is %d)\n",
//		        int_from_command((char *)x+l-12),i,int_from_command((char *)x+l-8),l);
		 if (l>12) {
		   x[l-12] = i&255;
		   x[l-11] =  (i>>8)&255;
		   x[l-10] = (i>>16)&255;
		   x[l-9] = (i>>24)&255;
		   }
		 } /* block to do the command string */
	       break; /* we found it */
	       } /* if we found it */
	     }
	   if (j==collectioncommandcount) {
//	     logit("cant find frame header %d\n",i);
	     commandlen[tail]=0;
	     }
	   
           tail = (tail+1)% SOUNDRING_COUNT;
           if (soundringhead==tail) {
//???              logit("Soundring overflow!!!!!!!!!!!");
              soundringhead = (soundringhead+1) % SOUNDRING_COUNT;
              }
           if (soundringfirst==tail) {
              soundringfirst = (soundringfirst+1) % SOUNDRING_COUNT;
              }
	   
          } /* pin blovk */
	 
         } /* for each start to finish */	 
	   
	   
	 
	 
	 soundringtail=tail; /* instant queue add */
//	 logit("soundring %d to %d",soundringhead,soundringtail);  
	 /* ok - now clean up  */
	 {
	   int j=0;
	   int k=0;
	   for (j=0;j<collectioncommandcount;j++) {
	     if (command_frame[j]!=-1) {
	       if (j!=k) {
	         collectioncommandlen[k]=collectioncommandlen[j];
		 unsigned char *t;
		 t = (unsigned char *)collectioncommands[j];collectioncommands[j]=(unsigned char *)collectioncommands[k];
		 collectioncommands[k]=t;
		 command_frame[k]=command_frame[j];
		 k++;
		 } /* if we are not the same and needed to swap */
	       } /* if we are not a -1 used up frame */
	     } /* for each command */
	   collectioncommandcount=k;
	   int framepos=(collectionhead+end-base_frame)%SOUNDRING_COUNT;
	   if (framepos<0) {logit("QQQ4 %d+%d-%d %d",collectionhead,end,base_frame,framepos);}
	   collectionhead=framepos; /* move the head on up, we are good */
	   base_frame=end;   /* dont care about any packets before */
	   } /* clean up */
	 } /* if we have a block to move over */


}


} /* process_packet_main */








static void process_packet_sr01(int flag_override) {

    int justwrite=0;
    short *soundBuffer;
    int soundBufferSize = 0;
    unsigned char recorded_checksums[MAXFRAMES_CHECKSUM];
    unsigned char actual_checksums[MAXFRAMES_CHECKSUM];

    watchdog_point=3;

    if (!opusdecoder) return;
    if (!soundring[soundringtail]) {
        logit("bad sound ring not allocated %d",soundringtail);
        return;
    }
    if (!commandring[soundringtail]) {
        logit("bad command ring not allocated");
        return;
    }

    if (!collections[soundringtail]) {
        logit("bad collection sound ring not allocated");
        return;
    }
    if (!collectioncommands[soundringtail]) {
        logit("bad collection command ring not allocated");
        return;
    }
    
    
     int sampleSize=recvStringLen;
     
    unsigned char *packet = packetbuffer;


    if (sampleSize<40+2) return;
    int header_frame;
    unsigned int flag;
    int newframe;
    int opus_frames=0;
    int framestart[MAXFRAMES];
    int framesize[MAXFRAMES];
    int end_of_frames;

    { /* compute flag */
        int l = sampleSize-8;
        unsigned int i1=packet[l]&0xff;
        unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;
        flag = i1+(i2<<8)+(i3<<16)+(i4<<24);
    }


    {
        int l = sampleSize-28;
        unsigned int i1=packet[l]&0xff;
        unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;

        newframe = i1+(i2<<8)+(i3<<16)+(i4<<24);
    }
    
    {
        int l = sampleSize-20;
        unsigned int i1=packet[l]&0xff;
        unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;

        header_frame = i1+(i2<<8)+(i3<<16)+(i4<<24);
    }
    
    
    
{
  int sanity = newframe - header_frame;
  if (sanity<0) sanity = -sanity;
  if (sanity >400) {
     logit ("newframe to header_frame is too big: %d",sanity);
     return;
     }
  }
    
    
{ /* read recorded checksums */
  int l = sampleSize-40;
  int i;
  for (i=0;i<MAXFRAMES_CHECKSUM;i++) {
    recorded_checksums[i]=packet[l++];
    actual_checksums[i]=0;
    }
  }
      
  /* Opus frame - including recovery */
        int look;
        look=0;
        /* figure out how many opus packets are there. */
        opus_frames = 0;
        while ( ( (packet [look])||(packet[look+1]) )&&(opus_frames<MAXFRAMES)&&(look<sampleSize)) {
            /* found a frame. */
            framestart[opus_frames]=look+2;
	    unsigned int pl1 = packet[look];
	    unsigned int pl2 = packet[look+1];
	    pl2 = pl2 <<8;
	    
            framesize[opus_frames]= pl1+pl2;
	    
	    if ( framesize[opus_frames] +look+2 >=sampleSize) {
	      logit("framesize[%d](%d) + %d + 2 > sample size (%d)",opus_frames, framesize[opus_frames],look,sampleSize);
	      return;
	      }
            look=look+2+framesize[opus_frames];
            opus_frames++;
        }
	
      end_of_frames = look+2;
      if (end_of_frames >sampleSize) end_of_frames=sampleSize;

        
//logit("frame %d head %d flag %x\n",newframe,header_frame,flag);
    if ((base_frame==-1)||(newframe <base_frame-SOUNDRING_COUNT)||(newframe>=base_frame+SOUNDRING_COUNT)) { /* starting out */
      flag=flag|flag_override;
      if (flag&4) {
//        logit("frame %d head %d flag %x\n",newframe,header_frame,flag);
//        logit("Reset base_frame %d newframe %d (not between %d and %d )\n",base_frame,newframe,base_frame-SOUNDRING_COUNT,base_frame+SOUNDRING_COUNT);
        base_frame = newframe-MAXFRAMES;
        if (base_frame<0) base_frame=0;
        collectionhead=0;
        int i;
        for (i=0;i<SOUNDRING_COUNT;i++) {
          collectionstat[i]->len=0;  
	  collectioncommandlen[i]=0;
	  command_frame[i]=0;
	  }
        collectioncommandcount=0;
	opus_frame=-1;
	}
      else {
        justwrite=1;
	}
      }
    
    



    { 	
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
            size = (unsigned int)(buf-buffer)-2;
            buffer[0]=size&0xff;
            buffer[1]= (size>>8)&0xff;
            fwrite(buffer,size+2,1,write_file_spec);
            }

    if (justwrite==1) {
//      logit("Skipping, cant reset because not a starter frame");
      return;
      }	
        
    {	

        int backup_frame_number;
	int cframe=-1;
        for (backup_frame_number=opus_frames-1;backup_frame_number>=0;backup_frame_number--) {
	    int frame = newframe-backup_frame_number;
//	    logit("process frame %d base %d",frame,base_frame);
	    if (frame<base_frame) {
	      cframe=frame;
	      continue;
	      }
	    int framepos=(collectionhead+frame-base_frame)%SOUNDRING_COUNT;
	    if (framepos<0) {logit("QQQ %d+%d-%d %d",collectionhead,frame,base_frame,framepos);}
	    if (collectionstat[framepos]->len) {
	      cframe=frame;
	      continue; /* already decoded this one */
	      }
	    if (collectionstat[framepos]->frame==frame) { /* should be there */
	      cframe=frame;
	      continue; /* already decoded this one */
	      }
	    
	    if (backup_frame_number<MAXFRAMES_CHECKSUM) { 
	      actual_checksums[backup_frame_number]=checksum(packet+framestart[backup_frame_number],framesize[backup_frame_number]);
              if (recorded_checksums[backup_frame_number]!=actual_checksums[backup_frame_number]) {
                logit("checksum err %d %x,%x  frame %d\n",backup_frame_number
	          ,recorded_checksums[backup_frame_number],actual_checksums[backup_frame_number],newframe);
	        }
	      }
	      
            int opus_result;
            float pcm[480];
            minpcm=0.;
            maxpcm=0.;
	    soundBuffer = collections[framepos];
            soundBufferSize = 0;
//            logit("opus %d",frame);
            if ((opus_frame !=-1)&&(opus_frame != frame-1)) { 
//	      logit("qq frame %d to %d ",opus_frame,frame);
//	      logit("backup_frame_number %d opus_frames %d  base %d cframe %d\n",backup_frame_number,opus_frames,base_frame,cframe);
	      unsigned char *use_sound_buffer_for_opus = (unsigned char *)soundBuffer;
	      memcpy7(use_sound_buffer_for_opus,((unsigned char *)(packet))+framestart[backup_frame_number],framesize[backup_frame_number]);
	      collectionstat[framepos]->frame=frame;
	      collectionstat[framepos]->len=-framesize[backup_frame_number];
	      memcpy8((void *)(&(collectionstat[framepos]->version)),packet+sampleSize-4,4*sizeof(char)); /* version of the packet */
	      continue;
	      }
	 
	    opus_frame = frame;
            opus_result = opus_decode_float(opusdecoder,((unsigned char *)(packet))+framestart[backup_frame_number],
                                            (opus_int32)(framesize[backup_frame_number])
                    ,pcm,(int)240,(int)0);
            int i;
            if (opus_result != 240) {
                logit("opus_Result is %d not 240\n",opus_result);
            }
  //          logit("packlet %d,%d,%d\n",((unsigned char *)(packet))[framestart[backup_frame_number]],
    //           ((unsigned char *)(packet))[framestart[backup_frame_number]+1],((unsigned char *)(packet))[framestart[backup_frame_number]+2]);

            for (i=0;i<480;i++) {
                short s;
                    //     if (i<3)   	        logit("test %d %f,%f\n",i,pcm[i+i],pcm[i+i+1]);
                float ma;
                float m;
                ma=pcm[i];
                m=ma*32768.;
                if (m>32767.) {
                    s=32767;
                    maxpcm=1.;
                    }
                else if (m<-32768.) {
                    s= -32768;
                    minpcm=-1.;
                }
                else {
                    s=(int)m;
                    if (ma>maxpcm) maxpcm=ma;
                    if (ma<minpcm) minpcm=ma;
                }
      //          if (i<3)   	        logit("SB %d ss %d\n",(int)soundBuffer,soundBufferSize);
                soundBuffer[soundBufferSize++]=s;
                } /* for each sample * 2 channels */
	    
            /* set the collection frame number */
	    collectionstat[framepos]->frame=frame;
	    collectionstat[framepos]->len=soundBufferSize;
	    memcpy9((void *)(&(collectionstat[framepos]->version)),packet+sampleSize-4,4*sizeof(char)); /* version of the packet */
	    } /* for each compressed buffer */
	  

	if (collectioncommands[collectioncommandcount]==NULL) {
	  logit("error null");
	  }
	if (packet==NULL) {
	  logit("packet null");
	  }
	if (collectioncommandcount>SOUNDRING_COUNT) {
	  collectioncommandcount=0;
	  logit("overflow collection command count2s");
	  }
        memcpy10(collectioncommands[collectioncommandcount],(packet+end_of_frames),sampleSize-end_of_frames);
        collectioncommandlen[collectioncommandcount]=sampleSize-end_of_frames;
	command_frame[collectioncommandcount]=header_frame;
//	logit("record frame %d",header_frame);
	collectioncommandcount++;
	if (collectioncommandcount>SOUNDRING_COUNT) {
	  collectioncommandcount=0;
	  logit("overflow collection command count2a");
	  }
	
	
	/* handle frames that are negative lengths - those are not compressed */
	{
	  int xframe=opus_frame+1;
	  if (xframe<base_frame) {
	    logit("did I reset?");
	    xframe=base_frame;
	    }
	  while (1) {
	    int framepos=(collectionhead+xframe-base_frame)%SOUNDRING_COUNT;
	    if (framepos<0) {logit("QQQ2 %d+%d-%d %d",collectionhead,xframe,base_frame,framepos);}
	    if (collectionstat[framepos]->len>=0) break; /* only do if this is negative - meaning it is a cached packet */
	    
	    /* Cool - here we call opus to decode the packet in order */
	    int opus_result;
            float pcm[480];
            minpcm=0.;
            maxpcm=0.;
	    soundBuffer = collections[framepos];
            soundBufferSize = 0;
	    opus_frame = xframe;
//	    logit("Special decode frame %d",opus_frame);
            opus_result = opus_decode_float(opusdecoder,((unsigned char *)soundBuffer),
                                            (opus_int32)(-collectionstat[framepos]->len)
                    ,pcm,(int)240,(int)0);
            int i;
            if (opus_result != 240) {
                logit("opus_Result is %d not 240\n",opus_result);
            }
  //          logit("packlet %d,%d,%d\n",((unsigned char *)(packet))[framestart[backup_frame_number]],
    //           ((unsigned char *)(packet))[framestart[backup_frame_number]+1],((unsigned char *)(packet))[framestart[backup_frame_number]+2]);

            for (i=0;i<480;i++) {
                short s;
                    //     if (i<3)   	        logit("test %d %f,%f\n",i,pcm[i+i],pcm[i+i+1]);
                float ma;
                float m;
                ma=pcm[i];
                m=ma*32768.;
                if (m>32767.) {
                    s=32767;
                    maxpcm=1.;
                    }
                else if (m<-32768.) {
                    s=-32768;
                    minpcm=-1.;
                }
                else {
                    s=(int)m;
                    if (ma>maxpcm) maxpcm=ma;
                    if (ma<minpcm) minpcm=ma;
                }
      //          if (i<3)   	        logit("SB %d ss %d\n",(int)soundBuffer,soundBufferSize);
                soundBuffer[soundBufferSize++]=s;
                } /* for each sample * 2 channels */
	    
            /* set the collection frame number */
	    collectionstat[framepos]->len=soundBufferSize;
	    
	    
	    xframe = opus_frame+1;
	    } /* while doing stashed packets */
	  } /* block to handle the stashed packets that came out of order. We call opus_decode in order */
	    
	  
	
	
//	logit("F %d %d",newframe,collectioncommandcount);
        } /* block collecting things */


    {
       /* figure out if we have enough frames to plow through */
       int start; /* start is the start frame number */
       for (start=base_frame;start<base_frame+SOUNDRING_COUNT;start++) {
         int framepos=(collectionhead+start-base_frame)%SOUNDRING_COUNT;
	 if (framepos<0) {logit("QQQ3 %d+%d-%d %d",collectionhead,start,base_frame,framepos);}
         if (collectionstat[framepos]->len>0) break;
	 }

       int end; /* just after the end*/
       for (end=start;end<base_frame+SOUNDRING_COUNT;end++) {
         int framepos=(collectionhead+end-base_frame)%SOUNDRING_COUNT;
	 if (framepos<0) {logit("QQQ4 %d+%d-%d %d",collectionhead,end,base_frame,framepos);}
         if (collectionstat[framepos]->len<=0) break;
	 }
       collection_size=end-start;
//       logit("from %d to %d oldframe %d newframe %d base %d",start,end,frame,newframe,base_frame);       
       /* We will now add if the subsequent frame number is in our range,  or if we have more than 40 loaded up */
       if ( (end-start >= NUMBER_OF_PACKETS_BEFORE_ACTIVATING_SOUND)||((frame+1 >=start)&&(frame+1<end)) ) {
         int i;
         /* great - we can load up the ring buffer quickly now  - first - we can get rid of anything too far in the past */
	 if ((frame+1 >start)&&(frame+1<end)) {
//	   logit("flush %d from %d(%d)",end-frame-1,start,frame+1);
	   for (i=start;i<frame+1;i++) {
	     int framepos=(collectionhead+i-base_frame)%SOUNDRING_COUNT;
	     if (framepos<0) {logit("QQQ5 %d+%d-%d %d",collectionhead,i,base_frame,framepos);}
	     collectionstat[framepos]->len=0;
	     collectionstat[framepos]->frame=0;
	     }
	   start=frame+1;
	   }
//	 else  	     logit("flush %d from %d",end-start,start);

	 
	 /* process the collection frames to figure out the actual frames 
	   This is done by guessing forward and backward for records close to the original 
	 */
//	 int j;
//	 for (j=0;j<collectioncommandcount;j++) {
//	   logit("_ %d %d",j,command_frame[j]);
//	   }
	   
	 /* Move these over quickly */
	 int tail=soundringtail;
	 for (i=start;i<end;i++) {
	   int framepos=(collectionhead+i-base_frame)%SOUNDRING_COUNT;
	   if (framepos<0) {logit("QQQ6 %d+%d-%d %d",collectionhead,i,base_frame,framepos);}
	   
	   frame = collectionstat[framepos]->frame;  
//	   logit("ffffffffffffff %d at %d q %d",collectionstat[framepos]->frame,framepos,tail);
	   { /* swap the buffers for the collections to the ring buffer */
	     short *t;
	     t=(short *)soundring[tail];
	     soundring[tail]=collections[framepos];
	     collections[framepos]=t;
	     if (!soundring[tail]) {
	       logit("found bug %d \n",framepos);
	       }
	     volatile struct processor_stat *tstat;
	     tstat=soundstat[tail];soundstat[tail]=collectionstat[framepos];collectionstat[framepos]=tstat;
	     }
	   
	   collectionstat[framepos]->frame=0;
	   collectionstat[framepos]->len=0;
	   
	   /* try to pin the collection to the frame */
           {
	   int j;
	   for (j=0;j<collectioncommandcount;j++) {
	     if (command_frame[j]==i) {
	       /* got the command frame for this one */
	       { /* first swap with the commandstring */
	         volatile unsigned char *t;
	         t=collectioncommands[j];collectioncommands[j]=(unsigned char *)commandring[tail];commandring[tail]=(volatile char *)t;
		 commandlen[tail]=collectioncommandlen[j];
		 collectioncommandlen[j]=0;
		 command_frame[j]=-1;
		 /* redo the frame number */
		 int l=commandlen[tail];
		 volatile unsigned char *x=(volatile unsigned char *)commandring[tail];
//		 logit("setting frame number from %d to %d (header is %d, len is %d)\n",
//		        int_from_command((char *)x+l-28),i,int_from_command((char *)x+l-20),l);
		 if (l>28) {
		   x[l-28] = i&255;
		   x[l-27] =  (i>>8)&255;
		   x[l-26] = (i>>16)&255;
		   x[l-25] = (i>>24)&255;
		   }
		 } /* block to do the command string */
	       break; /* we found it */
	       } /* if we found it */
	     }
	   if (j==collectioncommandcount) {
//	     logit("cant find frame header %d\n",i);
	     commandlen[tail]=0;
	     }
	   
           tail = (tail+1)% SOUNDRING_COUNT;
           if (soundringhead==tail) {
//???              logit("Soundring overflow!!!!!!!!!!!");
              soundringhead = (soundringhead+1) % SOUNDRING_COUNT;
              }
           if (soundringfirst==tail) {
              soundringfirst = (soundringfirst+1) % SOUNDRING_COUNT;
              }
	   
	   } /* pin blovk */
	 
           } /* for each start to finish */	 
	   
	   
	 
	 
	 soundringtail=tail; /* instant queue add */
//	 logit("soundring %d to %d",soundringhead,soundringtail);  
	 /* ok - now clean up  */
	 {
	   int j=0;
	   int k=0;
	   for (j=0;j<collectioncommandcount;j++) {
	     if (command_frame[j]!=-1) {
	       if (j!=k) {
	         collectioncommandlen[k]=collectioncommandlen[j];
		 unsigned char *t;
		 t = (unsigned char *)collectioncommands[j];collectioncommands[j]=(unsigned char *)collectioncommands[k];
		 collectioncommands[k]=t;
		 command_frame[k]=command_frame[j];
		 k++;
		 } /* if we are not the same and needed to swap */
	       } /* if we are not a -1 used up frame */
	     } /* for each command */
	   collectioncommandcount=k;
	   int framepos=(collectionhead+end-base_frame)%SOUNDRING_COUNT;
	   if (framepos<0) {logit("QQQ4 %d+%d-%d %d",collectionhead,end,base_frame,framepos);}
	   collectionhead=framepos; /* move the head on up, we are good */
	   base_frame=end;   /* dont care about any packets before */
	   } /* clean up */
	 } /* if we have a block to move over */


    } /* block queueing things */
 }




} /* process_packet_SR01 */

void process_packet_ignore_framestart()
{
process_packet_main(4);
}

void process_packet() 
{
process_packet_main(0);
}
