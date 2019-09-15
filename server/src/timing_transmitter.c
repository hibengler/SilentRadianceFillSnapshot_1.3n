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
/* $Revision: 1.2 $ */

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <sys/time.h> /* for socket() and bind() */
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

#include "timing_transmitter.h"
#include "crypt.h"
/*

This example reads from the default PCM device
and writes to standard output for 5 seconds of data.

*/


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
    struct sockaddr_in broadcastAddr; /* Broadcast address */
    char *broadcastIP;                /* IP broadcast address */
    char *sound_broadcastIP;                /* IP broadcast address */
    unsigned short sound_broadcastPort;     /* Server port */
    int sound_broadcastPermission;          /* Socket opt to set permission to broadcast */
    unsigned broadcastPort;     /* Server port */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */

void DieWithError(char *errorMessage);  /* External error handling function */
void DieWithError(char *errorMessage)
{
int e=errno;
    perror(errorMessage);
    fprintf(stderr,"error number %d\n",e);
    exit(1);
}

int init_internet(int argc, char *argv[])
{
    size = 0;
    if (argc < 2)                     /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <IP Address>\n", argv[0]);
        fprintf(stderr,"This always transmits on port 6969\n");
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






/* 
$Log: timing_transmitter.c,v $
Revision 1.2  2017/07/05 13:07:00  hib
copyleft everything

Revision 1.1  2016/11/02 19:10:47  hib
better and better.

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


