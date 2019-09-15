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
#include "world_internationalization.h"
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
#include "crypt.h"

extern int getpacket();
extern int addBlankSound();
extern int playOneSoundBuffer();
extern int processOneSampleMustWriteOne();
extern int processOneSampleMustWriteOne();
extern int processSamplesPossiblyPlay();
extern int processSamplesIgnorePackets();

#ifndef logit
#define logit(...) {fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\n");}
#endif

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
  struct pollfd *ourpolls;
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
  
  last_microseconds=0;
  /* Open PCM device for playback. */
  if (!block) {
//#define WORLD_receiver_alsa_default "default"
    rc = snd_pcm_open(&handle, WORLD_receiver_alsa_default,
                    SND_PCM_STREAM_PLAYBACK,SND_PCM_NONBLOCK);
    }
  else {
//#define WORLD_receiver_alsa_default "default"
    rc = snd_pcm_open(&handle, WORLD_receiver_alsa_default,0,
                    SND_PCM_STREAM_PLAYBACK);
    }
  
  if (rc < 0) {
    logit(
//#define WORLD_unable_to_open_pcm_device_colon___pct_s_lf_ "unable to open pcm device: %s\n"
            WORLD_unable_to_open_pcm_device_colon___pct_s_lf_,
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
	                  logit("Unable to get period size for playback: %s\n", snd_strerror(err));
         exit(-1);
         }
  logit("period size is %d\n",(int)f);




       /* set the buffer time */
	unsigned int time_buffer = 250000;
        err = snd_pcm_hw_params_set_buffer_time(handle, params,time_buffer,dir);
        if (err < 0) {
                logit("Unable to set buffer size: %s\n",snd_strerror(err));
                exit(-1);
        }

snd_pcm_uframes_t acsize;
        err = snd_pcm_hw_params_get_buffer_size(params, &acsize);
        if (err < 0) {
                logit("Unable to get buffer size for playback: %s\n", snd_strerror(err));
            exit(-1);
        }
  logit("buffer size is %d\n",(int)acsize);

  
						  

if (rc<0) 			      
    logit(
//#define WORLD_unable_to_set_hw_parameters_colon___pct_s_lf_ "unable to set hw parameters: %s\n"
            WORLD_unable_to_set_hw_parameters_colon___pct_s_lf_,
            snd_strerror(rc));

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    logit(
//#define WORLD_unable_to_set_hw_parameters_colon___pct_s_lf_ "unable to set hw parameters: %s\n"
            WORLD_unable_to_set_hw_parameters_colon___pct_s_lf_,
            snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames,
                                    &dir);
				    
    snd_pcm_prepare(handle);
    
{ /* get the poll descriptiors */
  int err;
  soundpolls_count = snd_pcm_poll_descriptors_count(handle);
  logit("count is %d\n",soundpolls_count);
  ourpolls = malloc(sizeof(struct pollfd)*(soundpolls_count+1)); /* extra one for listening to the socket. */
  socket_poll = soundpolls_count;
  if ((err = snd_pcm_poll_descriptors(handle,ourpolls,soundpolls_count))<0) {
    logit("unable to obtain poll descriptors for playback: %s\n",snd_strerror(err));
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
         logit("OPUS decoder fail %d\n",err);
       opusdecoder = NULL; /* error - decode will just be off */
       }

    }

sound_state=0;
{
  int i;
  for (i=0;i<4000;i++) {
    zero_buffer[i]=0;
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
logit("Usage: %s <Broadcast Port>\n", argv[0]);
exit(1);
}

if (argc>2) {
  no_sound = 1;
  }

broadcastPort = atoi(argv[1]); /* First arg: broadcast port */

/* Create a best-effort datagram socket using UDP */
if ((sock = socket(PF_INET, SOCK_DGRAM|SOCK_NONBLOCK, IPPROTO_UDP)) < 0)
//#define WORLD_socket_oparen__cparen__failed "socket() failed"
DieWithError(WORLD_socket_oparen__cparen__failed);

/* Construct bind structure */
memset(&broadcastAddr, 0, sizeof(broadcastAddr)); /* Zero out structure */
broadcastAddr.sin_family = AF_INET; /* Internet address family */
broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
broadcastAddr.sin_port = htons(broadcastPort); /* Broadcast port */

/* Bind to the broadcast port */
if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
//#define WORLD_bind_oparen__cparen__failed "bind() failed"
  DieWithError(WORLD_bind_oparen__cparen__failed);

logit("socket it %d\n",sock);
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








static int xrun_recovery(snd_pcm_t *handle, int err)
{
        if (err == -EPIPE) {    /* under-run */
                err = snd_pcm_prepare(handle);
                if (err < 0)
//#define WORLD_Can_qt_t_recovery_from_underrun_comma__prepare_failed_colon___pct_s_lf_ "Can't recovery from underrun, prepare failed: %s\n"
                        printf(WORLD_Can_qt_t_recovery_from_underrun_comma__prepare_failed_colon___pct_s_lf_, snd_strerror(err));
                underruncount++;
                return 0;
        } else if (err == -ESTRPIPE) {
                while ((err = snd_pcm_resume(handle)) == -EAGAIN)
                        logit("sleep????\n");
                        sleep(1);       /* wait until the suspend flag is released */
                if (err < 0) {
                        err = snd_pcm_prepare(handle);
                        if (err < 0)
//#define WORLD_Can_qt_t_recovery_from_suspend_comma__prepare_failed_colon___pct_s_lf_ "Can't recovery from suspend, prepare failed: %s\n"
                                printf(WORLD_Can_qt_t_recovery_from_suspend_comma__prepare_failed_colon___pct_s_lf_, snd_strerror(err));
                }
                return 0;
        }
        return err;
}





int play_direct(unsigned char *soundBuffer) {
int soundBufferSize=960;
int rc;

    { 
      char *p = soundBuffer;
      int size=soundBufferSize/4;
      while (rc = snd_pcm_writei(handle, soundBuffer, size) !=  size) {
        if (rc<0) {
          if (xrun_recovery(handle, rc) < 0) {
//#define WORLD_Write_error_colon___pct_s_lf_ "Write error: %s\n"
                                        printf(WORLD_Write_error_colon___pct_s_lf_, snd_strerror(rc));
                                        exit(EXIT_FAILURE);
                                }
          }
        else if (rc ==0) {
          logit("playonesoundbuffer failed when it shhouldn't %d\n",rc);
          }
        else if (rc ==1) {
//          logit("1 error\n");
          }
        else {   /* AOK */
          size -= rc;
          p += (rc*4);
          }
        } /* while makins sure the write succeeded */
      }

}




int play_sound(int *psoundBufferSize, int *punderruncount,
int mustWriteOne_flag,int possibly_play_flag,int ignore_packets_flag) {
unsigned char *soundBuffer;
int soundBufferSize;

soundlen[soundringtail] = *psoundBufferSize;
if (((soundringtail+1)%SOUNDRING_COUNT)==soundringhead) { 
  logit("Major error - adding a sound with the queue full\n");
  exit(-1);
  }
soundringtail = (soundringtail+1)%SOUNDRING_COUNT;

soundBufferSize = *psoundBufferSize;
int underruncount = *punderruncount;

if (mustWriteOne_flag) {
  playOneSoundBuffer();
  }
  
else {   /* Lets do a poll to see if there is a need to write or collect packets */  
  int r;
  int i;
  int polls_to_look_at;
  
  /* we ignore looking at packets very rarely - when we have too many packets and need to process them.
    So we have the ignore_packet_flag to keep us from POLLOUTg, processing packets, then repolling/processing in this inner poll
    */
  if (ignore_packets_flag) polls_to_look_at=soundpolls_count; else  polls_to_look_at=soundpolls_count+1;
  
  repoll_here:
  for (i=0;i<=socket_poll;i++) {ourpolls[i].revents=0;}
  
  r=poll(ourpolls,polls_to_look_at,0); /* return immediately */

  int count_polls=0;
  unsigned short sound_revents;
  snd_pcm_poll_descriptors_revents(handle, ourpolls, soundpolls_count, &sound_revents);
  
/*  int c=0;
  if  (ourpolls[socket_poll].revents & POLLIN) c++;
  if (sound_revents & POLLOUT) c++;
  logit("%d %d %d	.\n",c,sound_revents,ourpolls[socket_poll].revents);*/
  
  if (sound_revents & POLLERR) {
    logit("poll play_sound had an error\n");
    exit(-1);
    }
  /* unlike the other poll area, we always do sound first and net second, because we have the sound and thats why
     play_sound was called. */
  if (sound_revents & POLLOUT) {
    playOneSoundBuffer();
    if (ourpolls[socket_poll].revents & POLLIN) {
      getpacket();
      }
    goto repoll_here;
    }
  else if (ourpolls[socket_poll].revents & POLLIN) {
    getpacket();
    }  
  soundBufferSize=0;
  }
*psoundBufferSize = soundBufferSize;
*punderruncount = underruncount;
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
    logit("sound had an error\n");
    exit(-1);
    }
  if (sound_revents & POLLOUT) {
    count_polls++;
    }
  if (ourpolls[socket_poll].revents & POLLIN) {
    count_polls++;
    }
//  logit("%d %d %d	%d\n",count_polls,sound_revents,ourpolls[socket_poll].revents,work_to_do_flag-1);
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
  logit("warning - overflow packet reception");
  processSamplesIgnorePackets();
  }
if ((recvStringLen = recvfrom(sock, buffer, size, 0, NULL, 0)) < 0) {
  if (errno==EWOULDBLOCK) {
    logit("warning - packet blocked even though we thouigh it wouldn't\n");
    return 0; /* should not happen, but we coded for it anyways */
    }
  else {
//#define WORLD_recvfrom_oparen__cparen__failed "recvfrom() failed"
      DieWithError(WORLD_recvfrom_oparen__cparen__failed);
  //  logit("received %d frames %d\n",recvStringLen,(int)(frames));
      }
  }
netlen[netringtail]=recvStringLen;
if (recvStringLen>4) {
  decrypt1(buffer,recvStringLen-4,recvStringLen);
  }
netringtail = (netringtail+1)%NETRING_COUNT;
}

int addBlankSound() {
char *buffer = soundring[soundringtail];
int default_len=960;
if (((soundringtail+1)%SOUNDRING_COUNT)==soundringhead) { 
  logit("Major error - adding a blank sound when we shouldnt\n");
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
//if (soundringtail==soundringhead) { 
//  logit("Major error - playing a sound when we shouldnt\n");
//  exit(-1);
//  }
  
#define STATE1_TO 1
#define STATE2_TO 24
#define STATE0_TO 1  
playitagainsam:  
{ /* only start playing after we have 24 sounds */
  int queue_size = soundringtail-soundringhead;
  if (queue_size<0) queue_size += SOUNDRING_COUNT;
  if (sound_state==0) {
    if (queue_size>STATE2_TO) {
      sound_state=2;
      }
    else if (queue_size>=STATE1_TO) {
      sound_state=1;
      }
    }
  else if (sound_state==1) {
    if (queue_size <STATE0_TO) {
      sound_state=0;
      }
    else if (queue_size>STATE2_TO) {
      sound_state=2;
      }
    }
  else if (sound_state==2) {
    if (queue_size <STATE0_TO) {
      sound_state=0;
      }
    else if (queue_size<=STATE1_TO) {
      sound_state=1;
      }
    }
  }
  
//sound_state=1; /* ignore sound state anyways */  
if (sound_state) {  
  soundBuffer = soundring[soundringhead];
  soundBufferSize = soundlen[soundringhead];
  rewriteit:
  rc = snd_pcm_writei(handle, soundBuffer, (soundBufferSize/4));
  if (rc<0) {
    if (rc==-11) {
      return 0;
      }
    if (xrun_recovery(handle, rc) < 0) {
//#define WORLD_Write_error_colon___pct_d__pct_s_lf_ "Write error: %d %s\n"
                                        printf(WORLD_Write_error_colon___pct_d__pct_s_lf_,rc, snd_strerror(rc));
                                        exit(EXIT_FAILURE);
                                }
    goto rewriteit;
    }
  else if (rc != (soundBufferSize/4)) {
    logit("playonesoundbuffer failed when it shhouldn't %d\n",rc);
    }
  else {   /* AOK */
    soundBufferSize=0;
    }
  soundringhead = (soundringhead+1)%SOUNDRING_COUNT;
  if (sound_state==2) { goto playitagainsam;}
  }
else {
  soundBufferSize=960;
  rewriteit2:
  rc = snd_pcm_writei(handle, zero_buffer, (soundBufferSize/4)); 
  if (rc<0) {
    if (xrun_recovery(handle, rc) < 0) {
//#define WORLD_Write_error_colon___pct_s_lf_ "Write error: %s\n"
                                        printf(WORLD_Write_error_colon___pct_s_lf_, snd_strerror(rc));
                                        exit(EXIT_FAILURE);
                                }
    goto rewriteit2;
    }
  else if (rc !=  (soundBufferSize/4)) {
    logit("playonesoundbuffer failed when it shhouldn't %d\n",rc);
    }
  else {   /* AOK */
    soundBufferSize=0;
    }
  }
}









int processOneSampleMustWriteOne() {
if (netringtail==netringhead) { 
  logit("Major error - playing a sound when we shouldnt\n");
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








int main_internet(int argc,char *argv[]) {
init_receiver_sound(0);
init_receiver_internet(argc,argv);
//#define WORLD_record_dot_srd "record.srd"
//#define WORLD_receiver_alsa_w "w"
write_file_spec =fopen(WORLD_record_dot_srd,WORLD_receiver_alsa_w);
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











  

  
    
    
int main_file(int argc,char *argv[]) {
init_receiver_sound(1);
//#define WORLD_receiver_alsa_r "r"
FILE *read_file_spec =fopen(argv[1],WORLD_receiver_alsa_r);
logit("opening file...\n");
if (!read_file_spec) {
  logit("error - cannot read input file %s\n",argv[1]);
  exit(-1);
  }


write_file_spec =NULL;

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
  if (strncmp(buffer+packet_size-4,WORLD_SR_PROTOCOL_SR01,4)!=0) {
      logit("Packet error: %c%c%c%c\n",buffer[packet_size-4],
           buffer[packet_size-3],buffer[packet_size-2],buffer[packet_size-1]);
      }
      logit(".");
  make_sound_from_file_packet(buffer,packet_size); 
  }
fclose(read_file_spec);
exit(0);
}





int main(int argc,char *argv[]) {
if (argv[1]) {
  if (atoi(argv[1])) { /* prot number */
    return main_internet(argc,argv);
    }
  else {
    return main_file(argc,argv);
    }
  }
else {
  logit(" need port number or .srd file to read from \n");
  exit(-1);
  }
}
