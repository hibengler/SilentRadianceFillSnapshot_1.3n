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

/*#define THREADING_ON 1 
^^^ for threadding example - doesnt help much just burns a cpu */

#ifdef THREADING_ON
#include <pthread.h>
#endif

#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"



#include <alsa/asoundlib.h>


extern int getpacket();
extern int addBlankSound();
extern int playOneSoundBuffer();
extern int processOneSampleMustWriteOne();
extern int processOneSampleMustWriteOne();
extern int processSamplesPossiblyPlay();
extern int processSamplesIgnorePackets();




OpusDecoder *opusdecoder=NULL;
  int init_send_sound=1;
  int useRaw=0;
  long loops;
  int rc;
  int size;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  char *buffer;
  struct pollfd *ourpolls;
  int soundpolls_count;
  int socket_poll;
  
  static int frame=0;
int no_sound = 0;
unsigned int last_microseconds=0;


#define NETRING_COUNT 200
volatile int netringhead=0;
volatile int netringtail=0;
char *netring[NETRING_COUNT];
int netlen[NETRING_COUNT];

#define SOUNDRING_COUNT 400
volatile int soundringhead=0;
volatile int soundringtail=0;
char *soundring[SOUNDRING_COUNT];
int soundlen[SOUNDRING_COUNT];  // always 960


int sock; /* Socket */
struct sockaddr_in broadcastAddr; /* Broadcast Address */
unsigned short broadcastPort; /* Port */





int init_receiver_sound() {
  last_microseconds=0;
  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle, "default",
                    SND_PCM_STREAM_PLAYBACK,SND_PCM_NONBLOCK);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }
  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);
    

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,
                              SND_PCM_FORMAT_S16_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle, params, 2);

  val = 48000;
  snd_pcm_hw_params_set_rate_near(handle, params,
                                  &val, &dir);

int err;

  /* Set period size to 32 frames. */
  frames = 240;
  rc=snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, &dir);
			      
  snd_pcm_uframes_t f;
  rc = snd_pcm_hw_params_get_period_size(params, &f, &dir);
          if (rc < 0) {
	                  fprintf(stderr,"Unable to get period size for playback: %s\n", snd_strerror(err));
         exit(-1);
         }
  fprintf(stderr,"period size is %d\n",(int)f);




       /* set the buffer time */
	unsigned int time_buffer = 250000;
        err = snd_pcm_hw_params_set_buffer_time(handle, params,time_buffer,dir);
        if (err < 0) {
                fprintf(stderr,"Unable to set buffer size: %s\n",snd_strerror(err));
                exit(-1);
        }

snd_pcm_uframes_t acsize;
        err = snd_pcm_hw_params_get_buffer_size(params, &acsize);
        if (err < 0) {
                fprintf(stderr,"Unable to get buffer size for playback: %s\n", snd_strerror(err));
            exit(-1);
        }
  fprintf(stderr,"buffer size is %d\n",(int)acsize);

  
						  

if (rc<0) 			      
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames,
                                    &dir);
				    
{ /* get the poll descriptiors */
  int err;
  soundpolls_count = snd_pcm_poll_descriptors_count(handle);
  fprintf(stderr,"count is %d\n",soundpolls_count);
  ourpolls = malloc(sizeof(struct pollfd)*(soundpolls_count+1)); /* extra one for listening to the socket. */
  socket_poll = soundpolls_count;
  if ((err = snd_pcm_poll_descriptors(handle,ourpolls,soundpolls_count))<0) {
    fprintf(stderr,"unable to obtain poll descriptors for playback: %s\n",snd_strerror(err));
    exit(-1);
    }
  int i;
  for (i=0;i<soundpolls_count;i++) {
    ourpolls[i].events = POLLERR|POLLOUT;
    }
  }

  size = 4000;
  buffer = (char *) malloc(size);
  
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



int finish_play_sound() {
  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);
return(0);
}


void DieWithError(char *errorMessage); /* External error handling function */

void DieWithError(char *errorMessage)
{
perror(errorMessage);
exit(1);
}




int init_receiver_internet(int argc, char *argv[])
{

if (argc < 2) /* Test for correct number of arguments */
{
fprintf(stderr,"Usage: %s <Broadcast Port>\n", argv[0]);
exit(1);
}

if (argc>2) {
  no_sound = 1;
  }

broadcastPort = atoi(argv[1]); /* First arg: broadcast port */

/* Create a best-effort datagram socket using UDP */
if ((sock = socket(PF_INET, SOCK_DGRAM|SOCK_NONBLOCK, IPPROTO_UDP)) < 0)
DieWithError("socket() failed");

/* Construct bind structure */
memset(&broadcastAddr, 0, sizeof(broadcastAddr)); /* Zero out structure */
broadcastAddr.sin_family = AF_INET; /* Internet address family */
broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
broadcastAddr.sin_port = htons(broadcastPort); /* Broadcast port */

/* Bind to the broadcast port */
if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
  DieWithError("bind() failed");

fprintf(stderr,"socket it %d\n",sock);
{ /* set up polling */
  struct pollfd *p1;
  p1=ourpolls+socket_poll;
  p1->fd = sock;
  p1->events = POLLIN;
  p1->revents=0;
  }
}





int finish_internet_receiver() {
close(sock);
}






int deltasum=0;
int deltacount=0;
int underruncount=0;
int missingcount=0;
int majormissingcount=0;
int blankedcount=0;

#define MAXFRAMES 12



int play_sound(int *psoundBufferSize, int *punderruncount,
int mustWriteOne_flag,int possibly_play_flag,int ignore_packets_flag) {
unsigned char *soundBuffer;
int soundBufferSize;

soundlen[soundringtail] = *psoundBufferSize;
if (((soundringtail+1)%SOUNDRING_COUNT)==soundringhead) { 
  fprintf(stderr,"Major error - adding a sound with the queue full\n");
  exit(-1);
  }
soundringtail = (soundringtail+1)%SOUNDRING_COUNT;

soundBufferSize = *psoundBufferSize;
int underruncount = *punderruncount;

if (mustWriteOne_flag) {
  soundBuffer = soundring[soundringhead];
  rc = snd_pcm_writei(handle, soundBuffer, (soundBufferSize/4));
  if (rc == -EPIPE) {
    /* EPIPE means underrun */
    underruncount++;
    snd_pcm_prepare(handle);
    } 
  else if (rc < 0) {
    fprintf(stderr,
            "error from writei: %s\n",
            snd_strerror(rc));
    exit(-1);   
    }  
  else if (rc!= (soundBufferSize/4)) {
    fprintf(stderr,
            "Play_sound failed when really shouldnt\n");
    exit(-1);   
    }
  soundBufferSize=0;
  soundringhead = (soundringhead+1)%SOUNDRING_COUNT;
  }
  
else {   /* Lets do a poll to see if there is a need to write or collect packets */  
  int r;
  int i;
  int polls_to_look_at;
  
  /* we ignore looking at packets very rarely - when we have too many packets and need to process them.
    So we have the ignore_packet_flag to keep us from POLLOUTg, processing packets, then repolling/processing in this inner poll
    */
  if (ignore_packets_flag) polls_to_look_at=soundpolls_count; else  polls_to_look_at=soundpolls_count+1;
  
  for (i=0;i<=socket_poll;i++) {ourpolls[i].revents=0;}
  
  r=poll(ourpolls,polls_to_look_at,0); /* return immediately */

  int count_polls=0;
  unsigned short sound_revents;
  snd_pcm_poll_descriptors_revents(handle, ourpolls, soundpolls_count, &sound_revents);
  
/*  int c=0;
  if  (ourpolls[socket_poll].revents & POLLIN) c++;
  if (sound_revents & POLLOUT) c++;
  fprintf(stderr,"%d %d %d	.\n",c,sound_revents,ourpolls[socket_poll].revents);*/
  
  if (sound_revents & POLLERR) {
    fprintf(stderr,"poll play_sound had an error\n");
    exit(-1);
    }
  /* unlike the other poll area, we always do sound first and net second, because we have the sound and thats why
     play_sound was called. */
  if (sound_revents & POLLOUT) {
    playOneSoundBuffer();
    }
  if (ourpolls[socket_poll].revents & POLLIN) {
    getpacket();
    }
  soundBufferSize=0;
  }
*psoundBufferSize = soundBufferSize;
*punderruncount = underruncount;
}





int addSample(int sampleSize,unsigned char *packet, int useRaw,int mustWriteOne_flag,int possibly_play_flag,int ignore_packets_flag) {

unsigned int flag;
unsigned int newframe;
unsigned int microseconds;
unsigned char *soundBuffer;

soundBuffer = soundring[soundringtail];
int soundBufferSize = 0;

int opus_frames=0;
int framestart[MAXFRAMES];
int framesize[MAXFRAMES];
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
        int l = sampleSize-20;                  /* -20 is frame */
    unsigned int i1=packet[l]&0xff;
    unsigned int i2=packet[l+1]&0xff;
        unsigned int i3=packet[l+2]&0xff;
        unsigned int i4=packet[l+3]&0xff;

    newframe = i1+(i2<<8)+(i3<<16)+(i4<<24);
}       

/* probably cant use raw anyways */
if (flag&4) { /* if have raw */
  useRaw = useRaw;
}
else { /* cant use it, dont have it */
  useRaw = 0;
  }


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
  while ((packet [look])&&(opus_frames<MAXFRAMES)&&(look<sampleSize)) {
            /* found a frame. */
            framestart[opus_frames]=look+1;
            framesize[opus_frames]= ((int)(packet[look]))& 0xff;
            look=look+1+framesize[opus_frames];
            opus_frames++;
            }

  if (packets_needed >MAXFRAMES) {
        opus_frames = 1;
        major_missed=1;
	soundBufferSize=0;
        snd_pcm_prepare(handle);	
//	fprintf(stderr,"major missed %d\n",packets_needed);
	int i;
  	for (i=0;i<10;i++) {
  	  for (;soundBufferSize<960;soundBufferSize++) {
	    soundBuffer[soundBufferSize]=0;
	    }
	  play_sound(&soundBufferSize,&underruncount,mustWriteOne_flag,possibly_play_flag,ignore_packets_flag);  
	  blankedcount++;
          soundBuffer = soundring[soundringtail];
	  mustWriteOne_flag=0;
	  soundBufferSize=0;
	  }
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
      if (deltacount==2000) {
        fprintf(stderr,"delta is %d underrun	%d	missing	%d	major	%d	blanked	%d\n",deltasum/deltacount,underruncount,
           missingcount,majormissingcount,blankedcount);
        deltasum=0;
        deltacount=0;
        underruncount=0;
        missingcount=0;
        majormissingcount=0;
	blankedcount=0;
        }
      }
    } /* if we are not major missing frame */

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
             unsigned short s;
             s= (int)(pcm[i+i]*32768.);
	     
             soundBuffer[soundBufferSize++]= s&0xff;
             soundBuffer[soundBufferSize++]= (s>>8)&0xff;
             s= (int)(pcm[i+i+1]*32768.);
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



/* no thread loop example - not used. */
int loop() {
while (1) {
  /* Either wait forever, or wait for no time because we got work to do */
  int work_to_do_flag;
  work_to_do_flag = 0;
  if (netringhead != netringtail) work_to_do_flag = 1;
  int r;
  int i;
  for (i=0;i<=socket_poll;i++) {ourpolls[i].revents=0;}
  r=poll(ourpolls,soundpolls_count+1,work_to_do_flag-1);  
  /* Tricky - because if there iw work to do, we return right away, otherwise not */
  int count_polls=0;
  unsigned short sound_revents;
  snd_pcm_poll_descriptors_revents(handle, ourpolls, soundpolls_count, &sound_revents);
  
  if (sound_revents & POLLERR) {
    fprintf(stderr,"sound had an error\n");
    exit(-1);
    }
  if (sound_revents & POLLOUT) {
    count_polls++;
    }
  if (ourpolls[socket_poll].revents & POLLIN) {
    count_polls++;
    }
//  fprintf(stderr,"%d %d %d	%d\n",count_polls,sound_revents,ourpolls[socket_poll].revents,work_to_do_flag-1);
  /* now - figure out what to do first. Sound is a premium.  If we got sound, we have to send it
     But if there is no sound, well, we should process a packet first - if there are no packets pending processing
     */
  if (count_polls==2) {
    if (soundringhead == soundringtail) { /* no sound buffered */
      if (netringhead == netringtail) { /* no packets buffered */
        getpacket();
        if (netringhead == netringtail) { /* might have failed */
          if (soundringhead == soundringtail) { /* no sound buffered  - usually the case*/
            addBlankSound();
            playOneSoundBuffer();
	    }
	  else {
            playOneSoundBuffer();
	    }
	  } /* if we still didn't get a packet - rare */
	else { /* we got a packet, lets process it */
  	  processOneSampleMustWriteOne();
	  }
        }
      else { /* packets are buffered but not processed */
	processOneSampleMustWriteOne();
	getpacket();
	}
      }
    else { /* sound is buffered - play it first */
      playOneSoundBuffer();
      getpacket();
      processSamplesPossiblyPlay();
      }
    } /* if we are processing both at once */
  else if (count_polls==1) { /* one of the two events ocurred */
    /* by the way, POLLIN is 1, and POLLOUT is 4 */
    if (sound_revents & POLLOUT) { /* need sound */
      if (soundringhead != soundringtail) {
        playOneSoundBuffer();
        }
      else {
        addBlankSound();
        playOneSoundBuffer();
        }
      }  /* if need sound */
    else { /* got packet */
      getpacket();
      processSamplesPossiblyPlay();
      }
    } /* if we got one event */
  else {
    processSamplesPossiblyPlay();
    }
  } /* main loop forever */
}

  
 
int getpacket() { 
/* here we know that we will get one packet - because the polls told us */
/* Receive a single datagram from the server */
int recvStringLen; /* Length of received string */
char *buffer = netring[netringtail];
while (((netringtail+1)%NETRING_COUNT)==netringhead) { 
  fprintf(stderr,"warning - overflow packet reception");
  processSamplesIgnorePackets();
  }
if ((recvStringLen = recvfrom(sock, buffer, size, 0, NULL, 0)) < 0) {
  if (errno==EWOULDBLOCK) {
    fprintf(stderr,"warning - packet blocked even though we thouigh it wouldn't\n");
    return 0; /* should not happen, but we coded for it anyways */
    }
  else {
      DieWithError("recvfrom() failed");
  //  fprintf(stderr,"received %d frames %d\n",recvStringLen,(int)(frames));
      }
  }
netlen[netringtail]=recvStringLen;
netringtail = (netringtail+1)%NETRING_COUNT;
}

int addBlankSound() {
char *buffer = soundring[soundringtail];
int default_len=960;
if (((soundringtail+1)%SOUNDRING_COUNT)==soundringhead) { 
  fprintf(stderr,"Major error - adding a blank sound when we shouldnt\n");
  exit(-1);
  }
blankedcount++;
soundlen[soundringtail]=default_len;
int i;
for (i=0;i<default_len;i++) {
  buffer[i]=0;
  }
soundringtail = (soundringtail+1)%SOUNDRING_COUNT;
return 0;
}


int playOneSoundBuffer() {
char *soundBuffer;
int soundBufferSize;
if (soundringtail==soundringhead) { 
  fprintf(stderr,"Major error - playing a sound when we shouldnt\n");
  exit(-1);
  }
soundBuffer = soundring[soundringhead];
soundBufferSize = soundlen[soundringhead];
rc = snd_pcm_writei(handle, soundBuffer, (soundBufferSize/4));
if (rc == -EPIPE) {
  underruncount++;
  /* EPIPE means underrun */
  snd_pcm_prepare(handle);
  }
else if (rc < 0) {
  fprintf(stderr,
            "error from writei: %s\n",
            snd_strerror(rc));
  soundBufferSize=0;
  }  
else if (rc !=  (soundBufferSize/4)) {
  fprintf(stderr,"playonesoundbuffer failed when it shhouldn't %d\n",rc);
  }
else {   /* AOK */
  soundBufferSize=0;
  }
soundringhead = (soundringhead+1)%SOUNDRING_COUNT;
}




int processOneSampleMustWriteOne() {
if (netringtail==netringhead) { 
  fprintf(stderr,"Major error - playing a sound when we shouldnt\n");
  exit(-1);
  }
char *buffer = netring[netringhead];
int recvStringLen = netlen[netringhead]; /* Length of received string */
  
addSample(recvStringLen,buffer,useRaw,1,0,0);
netringhead = (netringhead+1)%NETRING_COUNT;
}


int processSamplesPossiblyPlay() {
while (netringtail!=netringhead) { 
  char *buffer = netring[netringhead];
  int recvStringLen = netlen[netringhead]; /* Length of received string */
  
  addSample(recvStringLen,buffer,useRaw,0,1,0);
  netringhead = (netringhead+1)%NETRING_COUNT;
  }
}

int processSamplesIgnorePackets() {
while (netringtail!=netringhead) { 
  char *buffer = netring[netringhead];
  int recvStringLen = netlen[netringhead]; /* Length of received string */
  
  addSample(recvStringLen,buffer,useRaw,0,0,1);
  netringhead = (netringhead+1)%NETRING_COUNT;
  }
}








int main(int argc,char *argv[]) {
init_receiver_sound();
init_receiver_internet(argc,argv);

/* init netrings */
{
  int i;
  for (i=0;i<NETRING_COUNT;i++) {
    netring[i] = (char *)malloc(2000);
    }
  netringhead=0;
  netringtail=0;
  }
/* init soundrings */
{
  int i;
  for (i=0;i<SOUNDRING_COUNT;i++) {
    soundring[i] = (char *)malloc(4000);
    }
  soundringhead=0;
  soundringtail=0;
  }


loop();

}

