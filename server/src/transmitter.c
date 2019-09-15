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
/* $Revision: 1.15 $ */

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/types.h>
#include <sys/time.h> /* for socket() and bind() */
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

int maxframes_transmit_override = MAXFRAMES_TRANSMIT; /* we turn this down to 1 for streaming */

//#define clean_write
/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
  FILE *write_file_spec = NULL;
  long loops;
  int rc;
  int size;
  unsigned int val;
  int dir;
#define MTU_MINUS_OVERHEAD 1480
//#define MTU_MINUS_OVERHEAD 1480
#define FRAMES_TO_DO 80
#define FRAMES_TO_DO_IN_POOR_MODE 80

/* This makes 1 out of three sets go missing.  This is a kludge to deal with wireless server cards that cannot broadcast faster than 1M,
it bearly works. */
int number_savings=4;
int flake_xx=-1;
int flake=0;



    int sock;                         /* Socket */
    int sound_sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast address ipv4 */
    struct sockaddr_in6 broadcastAddr6; /* Broadcast address ipv6 */
    char *broadcastIP;                /* IP broadcast address */
    unsigned broadcastPort;     /* Server port */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */



float lo=10000.;
float hi=-10000.;


unsigned char stroop1[16384];
unsigned char stroop2[16384];
unsigned char *stroop_buffer=stroop1;

unsigned char boop1[16384];
unsigned char boop2[16384];
unsigned char *boop_buffer=boop1;

int boop_len;



int boop_current_spot=0;
int boop_number_frames_to_do=FRAMES_TO_DO;
int boop_state=0;

int poor_mode=0;
int sr_version=0;
int ipv6_flag=0;




void DieWithError(char *errorMessage);  /* External error handling function */
void DieWithError(char *errorMessage)
{
int e=errno;
    perror(errorMessage);
    fprintf(stderr,"error number %d\n",e);
    exit(1);
}



static unsigned short flip_bits(unsigned short x)
{
unsigned int a=x;
unsigned int b; 
b=((a&0xFF)<<8)+((a&0xFF00)>>8);
return b;
}

static int init_internet_ipv4(int argc,char *argv[]);
static int init_internet_ipv6(int argc,char *argv[]);






/* mode:
IPV4_SR00_POOR - this is the standard mode for wifi drivers that broadcast at only 1Mb/s
IPV6_SR00_POOR - this is the mode for Apple testing. IPV6, right?
IPV4_SR00 - Probably the best - as it goes fast.
IPV4_SR01 - Second best.
IPV6_SR01

argv - mode, ip address    or   mode ip addresss interface for ipv6
*/
int init_internet(int argc, char *argv[]) {
if ((argc<=2)||
    ((argc>1)&&(strncmp(argv[1],"IPV6",4)==0)&&(argc<=3))||
    ((argc>1)&&(strncmp(argv[1],"IPV4",4)==0)&&(argc>3))
   )  {
  fprintf(stderr,"Usage: %s <mode> <ip_address> [<interface>]\n"
    "  where:\n"
    "  <mode> is one of the following:\n"
    "      IPV4_SR00_POOR - this is the standard mode for wifi drivers that broadcast at only 1Mb/s\n"
    "      IPV6_SR00_POOR - this is the mode for Apple testing. IPV6, right?\n"
    "      IPV4_SR00 - Probably the best quality - as it goes fast.\n"
    "      IPV6_SR00 - Good for the IOS community.\n"
    "      IPV4_SR01 - Second best.\n"
    "      IPV6_SR01 - Only if you are ipv6\n"
    "	   STREAM_SR00 - sent to a stream - not meant for broadcasting on wifi\n"
    "	   STREAM_SR01 - sent to a stream with protocol SR01 - not meant for broadcasting on wifi\n"
    "\n"
    "  <ip_address> is the ip address to broadcast.  This is usually 10.70.255.255 for ipv4, or ff01::1 for ipv6\n"
    "\n"
    "  <interface> is the network interface name - mandatory for ipv6, and ignored for ipv4\n"
    "\n"
    "The SR01 protocol is older, more likely to loose statistics on packets, but 20%% more efficient.\n"
    "The SR00 protocol does not have sequentical frames ( they skip in poor mode), and this allows \n"
    "us to transmit in 1Mb/s wifi cards.\n"
    "The only card found to transmit faster than 1Mb/s is the Broadcom BCM4311 802.11b.g WLAN (rev 01)\n"
    "card, found int Gateway MT64xx, MX64xx, and similar laptops.  Some HP laptops also sport this card\n"
    "But the HP laptops CPU locks at a slower speed on linux platforms.\n"
    "\n"
    "Example runs:\n"
    "      %s IPV4_SR00_POOR 10.70.255.255\n"
    "      %s IPV6_SR00_POOR ff01::1 wlan0\n"
    "      %s IPV4_SR00 10.69.255.255\n"
    "      %s IPV6_SR00 ff01::1 wlan0\n"
    "      %s IPV4_SR01 10.71.255.255\n"
    "      %s IPV6_SR01 ff01::1 wlan0\n"
    "      %s STREAM_SR00 ok >file.sirad\n"
    "\n",argv[0],argv[0],argv[0],argv[0],argv[0],argv[0],argv[0],argv[0]);
    fprintf(stderr,"This always transmits on port 6969\n");
    exit(1);
    }
int the_ipv6_flag;
char *version;
if (strcmp(argv[1],"IPV4_SR00_POOR")==0) {
  poor_mode=1;
  the_ipv6_flag=0;
  version="SR00";
  }
else if (strcmp(argv[1],"IPV6_SR00_POOR")==0) {
  poor_mode=1;
  the_ipv6_flag=1;
  version="SR00";
  }
else if (strcmp(argv[1],"IPV4_SR00")==0) {
  poor_mode=0;
  the_ipv6_flag=0;
  version="SR00";
  }
else if (strcmp(argv[1],"IPV6_SR00")==0) {
  poor_mode=0;
  the_ipv6_flag=1;
  version="SR00";
  }
else if (strcmp(argv[1],"IPV4_SR01")==0) {
  poor_mode=0;
  the_ipv6_flag=0;
  version="SR01";
  }
else if (strcmp(argv[1],"IPV6_SR01")==0) {
  poor_mode=0;
  the_ipv6_flag=1;
  version="SR01";
  }
else if (strcmp(argv[1],"STREAM_SR00")==0) {
  poor_mode=0;
  the_ipv6_flag=0;
  version="SR00";
  write_file_spec=stdout;
  maxframes_transmit_override=1;
  }
else if (strcmp(argv[1],"STREAM_SR01")==0) {
  poor_mode=0;
  the_ipv6_flag=0;
  version="SR01";
  write_file_spec=stdout;
  maxframes_transmit_override=1;
  }
else {
  fprintf(stderr,"Invalid mode\n");
  exit(1);
  }

if (strcmp(version,"SR01")==0) {
  sr_version=1;
  }
else {
  sr_version=0;
  }
ipv6_flag = the_ipv6_flag;
if (ipv6_flag) return init_internet_ipv6(argc-1,argv+1);
else return init_internet_ipv4(argc-1,argv+1);
}






static int init_internet_ipv4(int argc, char *argv[])
{
    size = 0;
    if (argc < 2)                     /* Test for correct number of parameters */
    {
        fprintf(stderr,"Internal error 1\n");
        exit(1);
    }

    broadcastIP = argv[1];            /* First arg:  broadcast IP address */
    broadcastPort = 6969;
//    sound_broadcastIP = argv[3];
//    sound_broadcastPort = atoi(argv[4]);    /* Second arg:  broadcast port */
    fprintf(stderr,"init on %s port %d\n",broadcastIP,broadcastPort);

    /* Create socket for sending/receiving datagrams */
//    if ((sock = socket(PF_INET, SOCK_DGRAM|SOCK_NONBLOCK, IPPROTO_UDP)) < 0)
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Set socket to allow broadcast */
    broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,
          sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed");

    /* Construct local address structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);/* Broadcast IP address */
    broadcastAddr.sin_port = htons(broadcastPort);         /* Broadcast port */
}






static int init_internet_ipv6(int argc, char *argv[])
{
    size = 0;
    if (argc < 3)                     /* Test for correct number of parameters */
    {
        fprintf(stderr,"Internal error 2\n");
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
    memset(&broadcastAddr6, 0, sizeof(broadcastAddr6));   /* Zero out structure */
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


    broadcastAddr6.sin6_family = AF_INET6;                 /* Internet address family */
    struct sockaddr_in6 *addr6;
    addr6 = (struct sockaddr_in6 *) res->ai_addr;
//    broadcastAddr6.sin6_addr = *((struct in6_addr *)(res->ai_addr));
    struct in6_addr ad6 = addr6->sin6_addr;
    broadcastAddr6.sin6_addr = ad6;
    broadcastAddr6.sin6_port = htons(broadcastPort);         /* Broadcast port */
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



int do_end_packet_sr00(char *boop_buffer,int l,int frame_number,int header_frame_number) {
int i;


/* 12 overhead */
boop_buffer[l++]=frame_number&255;					/* E-12 frame number of the first frame in packet */
boop_buffer[l++]= (frame_number>>8)&255;
boop_buffer[l++]= (frame_number>>16)&255;
boop_buffer[l++]= (frame_number>>24)&255;
boop_buffer[l++]=header_frame_number&255;					/* E-8 header frame number */
boop_buffer[l++]= (header_frame_number>>8)&255;
boop_buffer[l++]= (header_frame_number>>16)&255;
boop_buffer[l++]= (header_frame_number>>24)&255;
boop_buffer[l++]= 's';				   	/*E-4 sr00 for silent radiance version 00 */
boop_buffer[l++]= 'r';
boop_buffer[l++]= '0';
boop_buffer[l++]= '0';
return l;
}


int do_end_packet_sr01(char *boop_buffer,int l,int frame_number, float db_level,float the_pitch,
   unsigned int flags,int microseconds,unsigned char *checksums,int header_frame_number) {
int i;
/* MAXFRAMES_TRANSMIT checksums */
for (i=0;i<MAXFRAMES_CHECKSUM;i++) {
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



int boop_basic_sr00(int compress_lengths[],
   unsigned char *compress[],
   unsigned int frame_number,
   int frame_numbers[],int or_flags,
   unsigned char *command,  
      int command_length,int poor_mode) {
int header_frame_number = frame_numbers[0];
unsigned char checksums[MAXFRAMES_CHECKSUM];
int i;
int l;
//fprintf(stderr,"basic flag %x frame %d header_frame %d\n",or_flags,frame_number,header_frame_number);
/* double buffer */
if (boop_buffer==boop1) boop_buffer=boop2; else boop_buffer=boop1;

l=0;

int number_compressed=0;
int offset=0;
int have_to_skip=0;
offset = frame_numbers[0]-frame_number;
if (offset<0) offset=0;
if (offset>=MAXFRAMES) {
  have_to_skip = offset-MAXFRAMES+1;
  fprintf(stderr,"frame %d to far from the top frame %d have to skip by %d!!!!!\n",frame_number,frame_numbers[0],have_to_skip);
  frame_number += have_to_skip;
  }


int actual_maxframes=maxframes_transmit_override;
if (poor_mode) {
//  actual_maxframes=2;
  }


//fprintf(stderr,"offset 1 is %d header %d frame %d\n",offset,header_frame_number,frame_number);
for (number_compressed=0;number_compressed<actual_maxframes;number_compressed++) {
//  fprintf(stderr,"%d l %d + %d+2 ->  %d? frame %d\n",number_compressed,l,
//                 compress_lengths[offset-number_compressed],
//             l+compress_lengths[offset-number_compressed]+2,
//                          frame_numbers[offset-number_compressed]);
  if ((compress_lengths[offset-number_compressed])&&(l+compress_lengths[offset-number_compressed]+2+PER_FRAME_OVERHEAD
          <MTU_MINUS_OVERHEAD-command_length)
     &&(header_frame_number>=frame_number+number_compressed)) {    
     l++;l++;
     l += compress_lengths[offset-number_compressed]+PER_FRAME_OVERHEAD;
     }
  else
    break;
  }
//fprintf(stderr,"compressed %d frame from %d to %d\n",number_compressed,frame_number,frame_number+number_compressed-1);
frame_number = frame_number + number_compressed-1;

offset=0;
int more_have_to_skip=0;
offset = frame_numbers[0]-frame_number;
if (offset<0) offset=0;
if (offset>=MAXFRAMES) {
  more_have_to_skip = offset-MAXFRAMES+1;
  fprintf(stderr,"frame %d to far from the top frame %d have to skip by %d + the older skip (%d)!!!!!\n",frame_number,frame_numbers[0],more_have_to_skip,have_to_skip);
  frame_number += more_have_to_skip;
  have_to_skip += more_have_to_skip;
  }


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
  int compress_length1 = compress_lengths[nc+offset]+PER_FRAME_OVERHEAD;
  for (i=0;i<compress_length1;i++) {
    boop_buffer[l++] = compress1[i];
    }
  if (nc==0) {
    /* fix the flags */
    boop_buffer[l-4] |= or_flags;
    }
  }
if (l>MTU_MINUS_OVERHEAD-command_length) {
  fprintf(stderr,"overflow %d greater than %d-%d (%d)\n",l,MTU_MINUS_OVERHEAD,command_length,MTU_MINUS_OVERHEAD-command_length);
  }
boop_buffer[l++]=0;
boop_buffer[l++]=0;

/* add command - note - we have no command length for the whole section because we dont need it.  The end of the packet 
   always has the 
  stats */
{ int i;
  for (i=0;i<command_length;i++) {
    boop_buffer[l++] = command[i];
    }
  }


//fprintf(stderr,"start frame is %d to %d header %d",frame_number+number_compressed-1,frame_number,header_frame_number);
l =do_end_packet_sr00(boop_buffer,l,frame_number,header_frame_number);
//fprintf(stderr,"checksum %d %x\n",frame_number,checksums[0]);

boop_len = l;


encrypt1((unsigned char *)boop_buffer,l-4,l);

{ /* block to send to the socket */
 //fprintf(stderr,"db is %d\n",db_level_int);
// fprintf(stderr,"[l=%d h=%d]	",l,header_frame_number);
 if (sendto(sock, boop_buffer, l, MSG_DONTWAIT, (struct sockaddr *)
               &broadcastAddr, sizeof(broadcastAddr)) != l)
	       {
  if (errno == EWOULDBLOCK) {
      fprintf(stderr,"would block\n");
       struct timespec thislong;
       thislong.tv_sec = 0;
       thislong.tv_nsec = 1000000 * 3; /* 3 milliseconds */
       nanosleep(&thislong, &thislong);
					      
      return(0+have_to_skip);
      
    struct pollfd p1;
    p1.fd = sock;
    p1.events = POLLOUT;
    p1.revents=0;
    int r;
    r = poll(&p1,1,10000);
    if (p1.revents&POLLERR) {
      fprintf(stderr,"poll error\n");
      }
    if (p1.revents&POLLOUT) {
      fprintf(stderr,"poll out\n");
      }
    if (p1.revents&POLLNVAL) {
      fprintf(stderr,"poll invalid request\n");
      }
    fprintf(stderr,"polled\n");  
    if (sendto(sock, boop_buffer, l, MSG_DONTWAIT, (struct sockaddr *)
               &broadcastAddr, sizeof(broadcastAddr)) != l)
	 {
         DieWithError("sendto2()a sent a different number of bytes than expected");
	 }
    
    fprintf(stderr,"...\n");
    }
  else {  
             DieWithError("sendto()b sent a different number of bytes than expected");
	     }
  } /* if we are still sending */
 } /* send block */

return(number_compressed+have_to_skip);
}

static int int_from_command(unsigned char *l) {
int i1 = l[0] & 0xff;
int i2 = l[1] & 0xff;
int i3 = l[2] & 0xff;
int i4 = l[3] & 0xff;
return i1 + (i2 << 8) + (i3 << 16) + (i4 << 24);
}
            
	                 

static int float_from_command(unsigned char *l) {
float value;
unsigned char *pt = (unsigned char *)&value;
pt[0]=l[0];
pt[1]=l[1];
pt[2]=l[2];
pt[3]=l[3];
return value;
}

				   				   


int boop_basic_sr01(int compress_lengths[],
   unsigned char *compress[],
   unsigned int frame_number,
   int frame_numbers[],int or_flags,
   unsigned char *command,  
      int command_length,int poor_mode) {

/* get the parameters */
   char *e = compress[0]+compress_lengths[0]+PER_FRAME_OVERHEAD;
   float the_pitch = float_from_command(e-16);
   int microseconds = int_from_command(e-12);
   float db_level = float_from_command(e-8);
   unsigned int flags = int_from_command(e-4); 

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

for (i=0;i<MAXFRAMES_CHECKSUM;i++) {
  checksums[i]=0;
  }

int number_compressed=0;
int offset=0;
while(frame_number != frame_numbers[offset]) offset++;

//fprintf(stderr,"offset 1 is %d header %d frame %d\n",offset,header_frame_number,frame_number);
for (number_compressed=0;number_compressed<maxframes_transmit_override;number_compressed++) {
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
l =do_end_packet_sr01(boop_buffer,l,frame_number,db_level,the_pitch,flags|or_flags,microseconds,checksums,header_frame_number);
//fprintf(stderr,"checksum %d %x\n",frame_number,checksums[0]);

boop_len = l;

encrypt1((unsigned char *)boop_buffer,l-4,l);

if (!flake) {
 //fprintf(stderr,"db is %d\n",db_level_int);
 if (sendto(sock, boop_buffer, l, MSG_DONTWAIT, (struct sockaddr *)
               &broadcastAddr, sizeof(broadcastAddr)) != l)
	       {
  if (errno == EWOULDBLOCK) {
    //fprintf(stderr,"would block\n");
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


return(number_compressed);
}







int stroop_sr00(int compress_lengths[],
   unsigned char *compress[],
   int frame_numbers[],
   unsigned char *command,  
   int command_length,int poor_mode) {

int frame_count=1;
int or_flags=4;
int frame_number=frame_numbers[0];
int header_frame_number = frame_numbers[0];
unsigned char checksums[MAXFRAMES_CHECKSUM];
int i;
int l;
//fprintf(stderr,"basic flag %x frame %d header_frame %d\n",or_flags,frame_number,header_frame_number);


l=0;

stroop_buffer[l++]=compress_lengths[0]&0xff;
stroop_buffer[l++]=compress_lengths[0]>>8;

unsigned char *compress1 = compress[0];
int compress_length1 = compress_lengths[0]+PER_FRAME_OVERHEAD;
for (i=0;i<compress_length1;i++) {
  stroop_buffer[l++] = compress1[i];
  }

/* fix the flags */
stroop_buffer[l-4] |= or_flags;

stroop_buffer[l++]=0;
stroop_buffer[l++]=0;

/* add command - note - we have no command length for the whole section because we dont need it.  The end of the packet 
   always has the 
  stats */
{ int i;
  for (i=0;i<command_length;i++) {
    stroop_buffer[l++] = command[i];
    }
  }

l =do_end_packet_sr00(stroop_buffer,l,frame_number,header_frame_number);
if (write_file_spec) {
  unsigned char buffer[40];
  unsigned char *buf = buffer;
  buffer[0]=l&0xff;   
  buffer[1]=(l>>8)&0xff;
  fwrite(buffer,2,1,write_file_spec);
  encrypt1((unsigned char *)stroop_buffer,l-4,l);
  int rc=fwrite(stroop_buffer,l,1,write_file_spec);
  if (rc==0) return(1);
  return(0);
  }

return(0);
}






int stroop_sr01(int compress_lengths[],
   unsigned char *compress[],
   int frame_numbers[],
   unsigned char *command,  
      int command_length,int poor_mode) {

int frame_number=frame_numbers[0];
int or_flags=4;
/* get the parameters */
char *e = compress[0]+compress_lengths[0]+PER_FRAME_OVERHEAD;
float the_pitch = float_from_command(e-16);
int microseconds = int_from_command(e-12);
float db_level = float_from_command(e-8);
unsigned int flags = int_from_command(e-4); 
int header_frame_number = frame_numbers[0];
unsigned char checksums[MAXFRAMES];

int i;
int l;
//fprintf(stderr,"basic flag %x frame %d header_frame %d\n",flags,frame_number,header_frame_number);
/* double buffer */
if (stroop_buffer==boop1) stroop_buffer=boop2; else stroop_buffer=boop1;


for (i=0;i<MAXFRAMES_CHECKSUM;i++) {
  checksums[i]=0;
  }

l=0;
stroop_buffer[l++]=compress_lengths[0]&0xff;
stroop_buffer[l++]=compress_lengths[0]>>8;
unsigned char *compress1 = compress[0];
int compress_length1 = compress_lengths[0];
for (i=0;i<compress_length1;i++) {
  stroop_buffer[l++] = compress1[i];
  }
checksums[0] = checksum(compress1,compress_length1);

stroop_buffer[l++]=0;
stroop_buffer[l++]=0;


/* add command - note - we have no command length for the whole section because we dont need it.  The end of the packet always has the 
  stats */
{ int i;
  for (i=0;i<command_length;i++) {
    stroop_buffer[l++] = command[i];
    }
  }


l =do_end_packet_sr01(stroop_buffer,l,frame_number,db_level,the_pitch,flags,microseconds,checksums,header_frame_number);

if (write_file_spec) {
  unsigned char buffer[40];
  unsigned char *buf = buffer;
  buffer[0]=l&0xff;   
  buffer[1]=(l>>8)&0xff;
  fwrite(buffer,2,1,write_file_spec);
  encrypt1((unsigned char *)stroop_buffer,l-4,l);
  int rc=fwrite(stroop_buffer,l,1,write_file_spec);
  if (!rc) return(1);
  }

return(0);
}








int boop_sr00(int compress_lengths[],
   unsigned char *compress[],
   int frame_numbers[],
   unsigned char *command,  
      int command_length,int poor_mode) {

int frame_number=frame_numbers[0];
int frame_count; 

for (frame_count=MAXFRAMES-1;frame_count>=0;frame_count--) {
  if (compress_lengths[frame_count]!=0) break;
  }
  
  



//fprintf(stderr,"frame %6.6d check %x l %d\n",frame_number,checksum(compress[0],compress_lengths[0]),compress_lengths[0]);  
if (boop_state==0) { /* initial */
  if (frame_count<FRAMES_TO_DO+maxframes_transmit_override) { /* too early */
    return 0;
    }
  boop_state=1; 
  if (poor_mode) {
    boop_number_frames_to_do=FRAMES_TO_DO_IN_POOR_MODE; // probably the same because we have less packets 
    }
  else {
    boop_number_frames_to_do=FRAMES_TO_DO;
    }   
  boop_current_spot=1;
  }
//fprintf(stderr,"s %d %d  ",boop_state,boop_current_spot);
if (boop_state==1) { /* first bunch */
  int  offset =boop_basic_sr00(compress_lengths,compress,
       boop_current_spot,frame_numbers
      ,4,command,command_length,poor_mode);
  if (offset) { /* 0 is returned if we did not transmit */
    boop_current_spot += offset;
//fprintf(stderr,"   -> %d\n",boop_current_spot);
    boop_state=2;
    }
  return 0;
  }   
/* here boop_state is 2, so this is an additional packet where only the flags are current */
  int  offset =boop_basic_sr00(compress_lengths,compress,
       boop_current_spot,frame_numbers
      ,0,command,command_length,poor_mode);
  if (offset) { /* 0 is returned if we did not transmit */
    boop_current_spot += offset;
    }
      
//fprintf(stderr,"   -> %d\n",boop_current_spot);
if (boop_current_spot >=frame_number) {
  boop_state=1; 
  boop_number_frames_to_do=boop_number_frames_to_do;
  boop_current_spot=frame_number+1-boop_number_frames_to_do;
  return 0;
  }
boop_state=2;

return 0;
}




int boop_sr01(int compress_lengths[],
   unsigned char *compress[],
   int frame_numbers[],
   unsigned char *command,  
      int command_length,int poor_mode) {

int frame_number=frame_numbers[0];
int frame_count; 

for (frame_count=MAXFRAMES-1;frame_count>=0;frame_count--) {
  if (compress_lengths[frame_count]!=0) break;
  }
  
  
  

//fprintf(stderr,"frame %6.6d check %x l %d\n",frame_number,checksum(compress[0],compress_lengths[0]),compress_lengths[0]);  
if (boop_state==0) { /* initial */
  if (frame_count<FRAMES_TO_DO+maxframes_transmit_override) { /* too early */
    return 0;
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
  boop_current_spot +=boop_basic_sr01(
      compress_lengths,compress,
       boop_current_spot,frame_numbers,
      4,command,command_length,poor_mode);
//fprintf(stderr,"   -> %d\n",boop_current_spot);
  boop_state=2;
  return 0;
  }   
/* here boop_state is 2, so this is an additional packet where only the flags are current */
  boop_current_spot +=boop_basic_sr01(
      compress_lengths,compress,
       boop_current_spot,frame_numbers
      ,0,command,command_length,poor_mode);
//fprintf(stderr,"   -> %d\n",boop_current_spot);
if (boop_current_spot >=frame_number) {
  boop_state=1; 
  boop_number_frames_to_do=boop_number_frames_to_do;
  boop_current_spot=frame_number+1-boop_number_frames_to_do;
  return 0;
  }
boop_state=2;

return 0;
}


/* stroop - boop the current file right now - this is for for broadcast.  this is for streaming
So we can get rid of the complexities - and just do a one packet per file
And we also don't have to repeat older packets that are out of range */
int stroop(int compress_lengths[],
unsigned char *compress[],
int frame_numbers[],
unsigned char *command,
int command_length,int poor_mode) {
int rc;
if (sr_version==0) {
  rc=stroop_sr00(compress_lengths,compress,frame_numbers,command,command_length,poor_mode);
  }
else if (sr_version==1) {
  rc=stroop_sr01(compress_lengths,compress,frame_numbers,command,command_length,poor_mode);
  }  
else {
  fprintf(stderr,"munged up sr_version %d\n",sr_version);
  exit(1);
  }
return rc;
}
	       

int boop(int compress_lengths[],
   unsigned char *compress[],
   int frame_numbers[],
   unsigned char *command,  
      int command_length,int poor_mode) {
int rc;      
if (sr_version==0) {
  rc=boop_sr00(compress_lengths,compress,frame_numbers,command,command_length,poor_mode);
  }
else if (sr_version==1) {
  rc=boop_sr01(compress_lengths,compress,frame_numbers,command,command_length,poor_mode);
  }  
else {
  fprintf(stderr,"munged up sr_version %d\n",sr_version);
  exit(1);
  }
return rc;
}

/* 
$Log: transmitter.c,v $
Revision 1.15  2018/12/22 00:42:42  hib
wip for new sr player right to sirad.  Made music_broadcast quit if cant write to stdout (or any file) any more.
(untested)

Revision 1.14  2017/10/27 03:24:53  hib
Fixed it - had bad problems with sr001

Revision 1.13  2017/10/20 05:36:09  hib
almost missed this

Revision 1.12  2017/10/18 19:32:18  hib
This program uses curl to upload two streams to a server, with each offset by 5M,
The files are all 10M in size, so they are easy to finaggle.
the idea is to make it easy for a perl program (or other) to stream the info from something like
a generic web server - so an AWS could serve it up. :)

Revision 1.11  2017/10/17 22:39:09  hib
making to broadcast to web server

Revision 1.10  2017/07/05 13:07:00  hib
copyleft everything

Revision 1.9  2016/11/02 19:07:48  hib
Consolidated transmitter ipv6 and ipv4 together. Also made SR00 protocol
and poor_mode so we can somewhat effectively transmit on wifi cards limited to 1MB/s.
This version is still kludgy, and there is a bug where the sr_version gets overwritten - so there is more work to find an overbearing leak that changes things too much.
But - it is a major upgrade.

Revision 1.8  2016/10/13 07:00:06  hib
mde the transmitter build different

Revision 1.7  2016/10/13 06:01:39  hib
cleanup

Revision 1.6  2016/09/10 07:03:42  hib
Added the ability to send commands to the transmitter viw shared memory! wow!

Revision 1.5  2016/06/29 22:14:48  hib
made the SR01 show up

Revision 1.4  2016/06/29 22:14:18  hib
encrypt

Revision 1.3  2016/06/22 10:20:06  hib
made ip address variable

*/


