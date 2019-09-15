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
/* $Revision: 1.4 $ */

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>  

#include <netdb.h>
#include <arpa/inet.h>  /* for sockaddr_in */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <poll.h>
#include <errno.h>
#include "aubio_utils.h"

#include "transmitter.h"
#include "crypt.h"
/*

This example reads from the default PCM device
and writes to standard output for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
  FILE *write_file_spec = NULL;
  long loops;
  int rc;
  int size;
  unsigned int val;
  int dir;
//#define MTU_MINUS_OVERHEAD 1468
#define MTU_MINUS_OVERHEAD 1452
#define FRAMES_TO_DO 80


/* This makes 1 out of three sets go missing.  This is a kludge to deal with wireless server cards that cannot broadcast faster than 1M,
it bearly works. */
int number_savings=5;
int flake_xx=-1;
int flake=0;



    int sock;                         /* Socket */
    struct sockaddr_in6 broadcastAddr; /* Broadcast address */
    char *broadcastIP;                /* IP broadcast address */
    unsigned broadcastPort;     /* Server port */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */

void DieWithError(char *errorMessage);  /* External error handling function */
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}


static unsigned short flip_bits(unsigned short x)
{
unsigned int a=x;
unsigned int b; 
b=((a&0xFF)<<8)+((a&0xFF00)>>8);
return b;
}


int init_internet(int argc, char *argv[])
{
    size = 0;
    if (argc < 3)                     /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <IP Address> <interface>\n", argv[0]);
        fprintf(stderr,"This always transmits on port 6969\n");
        exit(1);
    }

    broadcastIP = argv[1];            /* First arg:  broadcast IP address */
    char *interface=argv[2];
    broadcastPort = 6969;
    fprintf(stderr,"init on %s port %d\n",broadcastIP,broadcastPort);

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(AF_INET6, SOCK_DGRAM|SOCK_NONBLOCK, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Set socket to allow broadcast */
    broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,
          sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed");
	
     u_char ttl=1;
     if ((setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl))) <0) 
        DieWithError("set multicast ttl failed");


         struct ifreq ifr;
       memset(&ifr, 0, sizeof(struct ifreq));
       snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s",interface);
       ioctl(sock, SIOCGIFINDEX, &ifr);

 printf("[[%d]]\n", ifr.ifr_ifindex );
       if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,  (void*)&ifr, sizeof(struct ifreq)) <0) 
           DieWithError("setsockopt() bind to interface failed");

    /* Construct local address structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    struct addrinfo* result;
    struct addrinfo* res;
/*
    struct addrinfo hints;
    hints.ai_flags=0;
    hints.ai_family=PF_INET6;
    hints.ai_socktype=*/
    int error=getaddrinfo(broadcastIP,NULL,NULL,&result);
    if (error !=0) {
       fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
       exit(-1);
       }
       
       
    /* loop over all returned results and do inverse lookup */
    for (res = result; res != NULL; res = res->ai_next) {   
        char hostname[NI_MAXHOST];
	if (res->ai_family != PF_INET6) continue;
	if (res->ai_socktype != SOCK_DGRAM) continue;
	if (res->ai_protocol != IPPROTO_UDP) continue;
	fprintf(stderr,"flags %x family %d socktype %d protocol %d addrlen %d\n",res->ai_flags,
              res->ai_family,res->ai_socktype,res->ai_protocol,res->ai_addrlen);
        error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST | NI_NUMERICSERV); 
        if (error != 0) {
            fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(error));
            exit(-1);
        }
        if (*hostname != '\0')
           fprintf(stderr,"hostname: %s\n", hostname);


    broadcastAddr.sin6_family = AF_INET6;                 /* Internet address family */
    struct sockaddr_in6 *addr6;
    addr6 = (struct sockaddr_in6 *) res->ai_addr;
//    broadcastAddr.sin6_addr = *((struct in6_addr *)(res->ai_addr));
    struct in6_addr ad6 = addr6->sin6_addr;
    broadcastAddr.sin6_addr = ad6;
    broadcastAddr.sin6_port = htons(broadcastPort);         /* Broadcast port */
    freeaddrinfo(result);

    fprintf(stderr,"got broadcast addr %x:%x:%x:%x:%x:%x:%x:%x socket %d\n",
       flip_bits(ad6.s6_addr16[0]),
       flip_bits(ad6.s6_addr16[1]),
       flip_bits(ad6.s6_addr16[2]),
       flip_bits(ad6.s6_addr16[3]),
       flip_bits(ad6.s6_addr16[4]),
       flip_bits(ad6.s6_addr16[5]),
       flip_bits(ad6.s6_addr16[6]),
       flip_bits(ad6.s6_addr16[7]),sock
       );
    return(0);
    }   
    freeaddrinfo(result);
    fprintf(stderr,"cannot resolve address as ipv6 address\n");
exit(-1);    
    
    
	    
}


unsigned char checksum(unsigned char *compress,int compress_length) {
unsigned char sum = 0;
int i;
for (i=0;i<compress_length;i++) {
  sum +=compress[i];
  }
return(sum);
}


#define TRANSMISSION_MAXSIZE 512
unsigned char beep_buffer[TRANSMISSION_MAXSIZE];
void beep(float dblevel,int onset) {
fprintf(stderr,"should not be used\n");
return; // not used
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int microseconds;
    microseconds = tv.tv_sec & 1048575; /* 277 hours*/
    microseconds = microseconds*1000 + (tv.tv_usec/1000);
    if (onset)  {
       microseconds = microseconds + 1048576000;
       if (size<TRANSMISSION_MAXSIZE)  size=size+8;
       int i;
       for (i=size-8;i>=0;i--) {
         beep_buffer[i]=beep_buffer[i-8];
         }
       char *db;
       db=(char *)&dblevel;
       beep_buffer[0]=microseconds&255;
       beep_buffer[1]= (microseconds>>8)&255;
       beep_buffer[2]= (microseconds>>16)&255;
       beep_buffer[3]= (microseconds>>24)&255;
       beep_buffer[4]=db[0];
       beep_buffer[5]=db[1];
       beep_buffer[6]=db[2];
       beep_buffer[7]=db[3];
//       fprintf(stderr,"Sending %d size %d\n",a,size);
       if (sendto(sock, beep_buffer, size, 0, (struct sockaddr *)
               &broadcastAddr, sizeof(broadcastAddr)) != size)
             DieWithError("sendto() sent a different number of bytes than expected");
       }
     else {
       unsigned char beep_buffer2[8000];
       char *db;
       db=(char *)&dblevel;
       beep_buffer2[0]=microseconds&255;
       beep_buffer2[1]= (microseconds>>8)&255;
       beep_buffer2[2]= (microseconds>>16)&255;
       beep_buffer2[3]= (microseconds>>24)&255;
       beep_buffer2[4]=db[0];
       beep_buffer2[5]=db[1];
       beep_buffer2[6]=db[2];
       beep_buffer2[7]=db[3];
       if (sendto(sock, beep_buffer2, 8, 0, (struct sockaddr *)
               &broadcastAddr, sizeof(broadcastAddr)) != 8)
             DieWithError("sendto() sent a different number of bytes than expected");
       }       
}




float lo=10000.;
float hi=-10000.;


unsigned char boop1[16384];
unsigned char boop2[16384];
unsigned char *boop_buffer=boop1;

int boop_len;



int do_end_packet(char *boop_buffer,int l,int frame_number, float db_level,float the_pitch,
   unsigned int flags,int microseconds,unsigned char *checksums,int header_frame_number) {
int i;
/* MAXFRAMES_TRANSMIT checksums */
for (i=0;i<MAXFRAMES_TRANSMIT;i++) {
  boop_buffer[l++]=checksums[i];
  }

/* 28 overhead */
boop_buffer[l++]=frame_number&255;					/* E-28 frame number of the first frame in packet */
boop_buffer[l++]= (frame_number>>8)&255;
boop_buffer[l++]= (frame_number>>16)&255;
boop_buffer[l++]= (frame_number>>24)&255;
/* frame_number,time,db_level,flags,version */
float dummy_pitch=the_pitch;
unsigned char *pt = (unsigned char *)&dummy_pitch;
boop_buffer[l++]=pt[0];							/* E-24 pitch */
boop_buffer[l++]=pt[1];
boop_buffer[l++]=pt[2];
boop_buffer[l++]=pt[3];
boop_buffer[l++]=header_frame_number&255;					/* E-20 header frame number */
boop_buffer[l++]= (header_frame_number>>8)&255;
boop_buffer[l++]= (header_frame_number>>16)&255;
boop_buffer[l++]= (header_frame_number>>24)&255;
boop_buffer[l++]=microseconds&255;					/* E-16 microseconds */
boop_buffer[l++]= (microseconds>>8)&255;
boop_buffer[l++]= (microseconds>>16)&255;
boop_buffer[l++]= (microseconds>>24)&255;
if (db_level <-11000.) {
  db_level = -11000;
  }
int db_level_int = (int)(db_level *1000);
boop_buffer[l++]=db_level_int&255;		  	/*E-12 - db level */
boop_buffer[l++]=(db_level_int>>8)&255;
boop_buffer[l++]=(db_level_int>>16)&255;
boop_buffer[l++]=(db_level_int>>24)&255;
boop_buffer[l++]=flags&255;  			    	/*E-8 flags */
boop_buffer[l++]= (flags>>8)&255;
boop_buffer[l++]= (flags>>16)&255;
boop_buffer[l++]= (flags>>24)&255;
boop_buffer[l++]= 's';				   	/*E-4 sr01 for silent radiance version 01 */
boop_buffer[l++]= 'r';
boop_buffer[l++]= '0';
boop_buffer[l++]= '1';
return l;
}




int boop_basic(fmat_t *mat,
   int compress_lengths[],
   unsigned char *compress[],
   unsigned int frame_number,
   int frame_numbers[],
   float db_level,float the_pitch,
   unsigned int flags,int microseconds,
   unsigned char *command,  
   int command_length) {
int header_frame_number = frame_numbers[0];
unsigned char checksums[MAXFRAMES];
int i;
int l;
//fprintf(stderr,"basic flag %x frame %d header_frame %d\n",flags,frame_number,header_frame_number);
/* double buffer */
if (boop_buffer==boop1) boop_buffer=boop2; else boop_buffer=boop1;

l=0;
if (1==0) {
  /* kludge to make transmitter send only 2 out of three "sets" of data. 
     It doesnt work very well.  But sometimes a phone will catch the signal.
     Writen to deal with the intel chipset drivers that can only do !M broadcast transmissions.
     */
  if (flags&4) {
    flake_xx = (flake_xx+1) %number_savings;
    if ((flake_xx%3)==0) flake=0; else flake=1;
    if (flake) {
      fprintf(stderr,"f");
      }
    else {
      fprintf(stderr,".");
      }
    }
  }

for (i=0;i<MAXFRAMES_TRANSMIT;i++) {
  checksums[i]=0;
  }

int number_compressed=0;
int offset=0;
while(frame_number != frame_numbers[offset]) offset++;

//fprintf(stderr,"offset 1 is %d header %d frame %d\n",offset,header_frame_number,frame_number);
for (number_compressed=0;number_compressed<MAXFRAMES_TRANSMIT;number_compressed++) {
//  fprintf(stderr,"%d l %d + %d+2 ->  %d? frame %d\n",number_compressed,l,
//                 compress_lengths[offset-number_compressed],
//             l+compress_lengths[offset-number_compressed]+2,
//                          frame_numbers[offset-number_compressed]);
  if ((compress_lengths[offset-number_compressed])&&(l+compress_lengths[offset-number_compressed]+2<MTU_MINUS_OVERHEAD-command_length)
     &&(header_frame_number>=frame_number+number_compressed)) {    
     l++;l++;
     l += compress_lengths[offset-number_compressed];
     }
  else
    break;
  }
//fprintf(stderr,"compressed %d frame from %d to %d\n",number_compressed,frame_number,frame_number+number_compressed-1);
frame_number = frame_number + number_compressed-1;

offset=0;
while(frame_number != frame_numbers[offset]) offset++;

int nc;
l=0;
//fprintf(stderr,"offset 2 is %d number_compressed %d\n",offset,number_compressed);
for (nc=0;nc<number_compressed;nc++) {
//  fprintf(stderr,"frame %6.6d check %x l %d main %d\n",frame_numbers[nc+offset],
//        checksum(compress[nc+offset],compress_lengths[nc+offset]),compress_lengths[nc+offset],frame_number);  
  if (frame_number-nc != frame_numbers[nc+offset]) {
    fprintf(stderr,"frame error %d,%d nc is %d\n",frame_number-nc, frame_numbers[nc+offset],nc);
    }
//  fprintf(stderr,"%d %d\n",nc,compress_lengths[nc+offset]);
  boop_buffer[l++]=compress_lengths[nc+offset]&0xff;
  boop_buffer[l++]=compress_lengths[nc+offset]>>8;
  unsigned char *compress1 = compress[nc+offset];
  int compress_length1 = compress_lengths[nc+offset];
  for (i=0;i<compress_length1;i++) {
    boop_buffer[l++] = compress1[i];
    }
  checksums[nc] = checksum(compress1,compress_length1);
  }
if (l>MTU_MINUS_OVERHEAD-command_length) {
  fprintf(stderr,"overflow %d greater than %d-%d (%d)\n",l,MTU_MINUS_OVERHEAD,command_length,MTU_MINUS_OVERHEAD-command_length);
  }

boop_buffer[l++]=0;
boop_buffer[l++]=0;



/* add command - note - we have no command length for the whole section because we dont need it.  The end of the packet always has the 
  stats */
{ int i;
  for (i=0;i<command_length;i++) {
    boop_buffer[l++] = command[i];
    }
  }


//fprintf(stderr,"start frame is %d to %d header %d",frame_number+number_compressed-1,frame_number,header_frame_number);
l =do_end_packet(boop_buffer,l,frame_number,db_level,the_pitch,flags,microseconds,checksums,header_frame_number);
//fprintf(stderr,"checksum %d %x\n",frame_number,checksums[0]);

boop_len = l;

if (write_file_spec) {   
  unsigned char buffer[40];
  unsigned int size;
  unsigned char *buf = buffer;
  unsigned char checksums[MAXFRAMES_TRANSMIT]; /* only one packet, so only one checksum */
  buffer[0]=boop_len&0xff;   
  buffer[1]= (boop_len>>8)&0xff;
  fwrite(buffer,2,1,write_file_spec);
  fwrite(boop_buffer,boop_len,1,write_file_spec);
  }

encrypt1((unsigned char *)boop_buffer,l-4,l);

if (!flake) {
 //fprintf(stderr,"db is %d\n",db_level_int);
 if (sendto(sock, boop_buffer, l, MSG_DONTWAIT, (struct sockaddr *)
               &broadcastAddr, sizeof(broadcastAddr)) != l)
	       {
  if (errno == EWOULDBLOCK) {
    fprintf(stderr,"would block\n");
    struct pollfd p1;
    p1.fd = sock;
    p1.events = POLLOUT;
    p1.revents=0;
    int r;
    r = poll(&p1,1,10000);
    if (sendto(sock, boop_buffer, l, MSG_DONTWAIT, (struct sockaddr *)
               &broadcastAddr, sizeof(broadcastAddr)) != l)
	 {
         DieWithError("sendto2() sent a different number of bytes than expected");
	 }
    
    }
  else {  
             DieWithError("sendto() sent a different number of bytes than expected");
	     }
  } /* if we are still sending */
 }

//fprintf(stderr,".");
return(number_compressed);
}



int boop_current_spot=0;
int boop_number_frames_to_do=FRAMES_TO_DO;
int boop_state=0;

void boop(fmat_t *mat,
   int compress_lengths[],
   unsigned char *compress[],
   int frame_numbers[],
   float db_level,
   float the_pitch,
   unsigned int flags,
   unsigned char *command,  
      int command_length,int poor_mode) {
   /* poor mode not yet implemented in transmitter6 */
int frame_number=frame_numbers[0];
int frame_count; 
int microseconds;
struct timeval tv;
gettimeofday(&tv,NULL);
//microseconds = tv.tv_sec & 1048575; /* 277 hours*/
microseconds = tv.tv_sec & 1024; /* 277 hours*/
microseconds = microseconds*1000000 + (tv.tv_usec);



for (frame_count=MAXFRAMES-1;frame_count>=0;frame_count--) {
  if (compress_lengths[frame_count]!=0) break;
  }
  
  
#ifdef clean_write
if (write_file_spec) {   
  unsigned char buffer[4000];
  unsigned int size;
  unsigned char *buf = buffer;
  unsigned char checksums[MAXFRAMES_TRANSMIT]; /* only one packet, so only one checksum */
  buf +=2;
  int i;
  *buf++=compress_lengths[0]&0xff;
  *buf++=compress_lengths[0]>>8;
  for (i=0;i<compress_lengths[0];i++) {
    *buf++ = compress[0][i];
    }  
  *buf++=0;
  *buf++=0;
  
  
/* add command - note - we have no command length for the whole section because we dont need it.  The end of the packet always has the 
  stats */
  { int i;
    for (i=0;i<command_length;i++) {
      buf[l++] = command[i];
      }
    }
  
  
  
  
  for (i=0;i<MAXFRAMES_TRANSMIT;i++) {
    checksums[i]=0;
    }
  checksums[0]=checksum(compress[0],compress_lengths[0]);
  buf += do_end_packet(buf,0,frame_number,db_level,the_pitch,flags,microseconds,checksums,frame_number);
  size = (buf-buffer)-2;
  buffer[0]=size&0xff;   
  buffer[1]= (size>>8)&0xff;
  fwrite(buffer,size+2,1,write_file_spec);
  }
#endif
//fprintf(stderr,"frame %6.6d check %x l %d\n",frame_number,checksum(compress[0],compress_lengths[0]),compress_lengths[0]);  
if (boop_state==0) { /* initial */
  if (frame_count<FRAMES_TO_DO+MAXFRAMES_TRANSMIT) { /* too early */
    return;
    }
  boop_state=1; 
  boop_number_frames_to_do=FRAMES_TO_DO;
  boop_current_spot=1;
//  int i;
//  for (i=0;i<frame_count;i++) {
//     fprintf(stderr,"Frame %6.6d (%d) check %x l %d\n",frame_numbers[i],i,checksum(compress[i],compress_lengths[i]),compress_lengths[i]);  
//     }
  }
//fprintf(stderr,"s %d %d  ",boop_state,boop_current_spot);
if (boop_state==1) { /* first bunch */
  boop_current_spot +=boop_basic(mat,
      compress_lengths,compress,
       boop_current_spot,frame_numbers
      ,db_level,the_pitch,flags|4,microseconds,command,command_length);
//fprintf(stderr,"   -> %d\n",boop_current_spot);
  boop_state=2;
  return;
  }   
/* here boop_state is 2, so this is an additional packet where only the flags are current */
  boop_current_spot +=boop_basic(mat,
      compress_lengths,compress,
       boop_current_spot,frame_numbers
      ,db_level,the_pitch,flags,microseconds,command,command_length);
//fprintf(stderr,"   -> %d\n",boop_current_spot);
if (boop_current_spot >=frame_number) {
  boop_state=1; 
  boop_number_frames_to_do=boop_number_frames_to_do;
  boop_current_spot=frame_number+1-boop_number_frames_to_do;
  return;
  }
boop_state=2;

return;
}

void reboop() {
  if (sendto(sock, boop_buffer, boop_len, 0, (struct sockaddr *)
               &broadcastAddr, sizeof(broadcastAddr)) != boop_len)
             DieWithError("sendto() sent a different number of bytes than expected");

}




/* 
$Log: transmitter6.c,v $
Revision 1.4  2017/07/05 13:07:00  hib
copyleft everything

Revision 1.3  2016/11/02 19:07:48  hib
Consolidated transmitter ipv6 and ipv4 together. Also made SR00 protocol
and poor_mode so we can somewhat effectively transmit on wifi cards limited to 1MB/s.
This version is still kludgy, and there is a bug where the sr_version gets overwritten - so there is more work to find an overbearing leak that changes things too much.
But - it is a major upgrade.

Revision 1.2  2016/09/10 07:03:42  hib
Added the ability to send commands to the transmitter viw shared memory! wow!

Revision 1.1  2016/08/11 20:47:04  hib
Got transmitter6.c to work.

Revision 1.5  2016/06/29 22:14:48  hib
made the SR01 show up

Revision 1.4  2016/06/29 22:14:18  hib
encrypt

Revision 1.3  2016/06/22 10:20:06  hib
made ip address variable

*/

