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
// Created by hib on 6/16/16.
//
#include "glue.h"
#include "network_receiver.h"
#include <time.h>
#include <sched.h>
#include <unistd.h>

unsigned char packetbuffer[PACKETMAXSIZE];
ssize_t recvStringLen=0; /* Length of received string */
int sock=-1; /* Socket */
int the_network_block = 0;

int timer_on_polling_flag=0; /* Android 6.0 freezes on the poll, so this has us do a microsleep instead */

struct sockaddr_in6 broadcastAddr; /* Broadcast Address */
unsigned short broadcastPort; /* Port */

#ifndef _WIN32
struct pollfd ourpolls[1];
#endif

int soundpolls_count;

int we_polled=1;







/* note - this might be re-called by the receive loop based on various errors
because we sometimes get a recvfrrom failed error on android, when switching 
networks */
void init_receiver_internet(int block) {
    broadcastPort = 6969;
#define HEY_NONBLOCK 00004000
the_network_block=block;

if (block) {
  sock = socket(PF_INET6, SOCK_DGRAM , IPPROTO_UDP);
  }
else {
  sock = socket(PF_INET6, SOCK_DGRAM | HEY_NONBLOCK, IPPROTO_UDP);
  }
/* Create a best-effort datagram socket using UDP */
    if (sock  < 0) {
        logit( "socket() failed");
        return;
    }

/* Construct bind structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr)); /* Zero out structure */
    broadcastAddr.sin6_family = AF_INET6; /* Internet address family */
//    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    broadcastAddr.sin6_port = htons(broadcastPort); /* Broadcast port */

/* Bind to the broadcast port */
    if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0) {
        logit( "bind() failed");
        close(sock);
        sock = 0;
        }
#ifndef _WIN32
    ourpolls->fd = sock;
    ourpolls->events = POLLERR|POLLIN;
    ourpolls->revents=0;
#endif
    we_polled=0;
return;
}




#ifdef LINUX_CLIENT
#define DOTHIS
#endif
#ifdef _WIN32
#define DOTHIS
#endif

#ifdef DOTHIS
int get_internet_receiver_packet() {
/* here we know that we will get one packet - because the polls told us */
/* Receive a single datagram from the server */
if (sock == -1) return(0);
char *buffer = (char *)packetbuffer;


recvStringLen = recvfrom(sock, buffer, PACKETMAXSIZE, 0, NULL, 0);
watchdog_point=2;
if (recvStringLen < 0) {
  recvStringLen=0;
  if (errno==EWOULDBLOCK) {
    we_polled=0;
    return 0; /* should happen often, we will check later */
    }
  else {
      logit("recvfrom() failed errno %d",errno);
      if ((errno==EBADF)||(errno==ENOTCONN)||(errno==ENOTSOCK)) {
        int oldsock=sock;
	init_receiver_internet(the_network_block);
	logit("Resocket %d -> %d",oldsock,sock);
	}
            struct timespec thislong;
             thislong.tv_sec = 0;
             thislong.tv_nsec = 10000000; /* 10 milliseconds */
             nanosleep(&thislong, &thislong);
     
        
      }
  }
else if (recvStringLen==0) return(0); /* double check */
if (recvStringLen>4) {
  decrypt1((unsigned char *)buffer,recvStringLen-4,recvStringLen);
  }
return(1);
}

#else

//android, apple

int get_internet_receiver_packet() {
/* here we know that we will get one packet - because the polls told us */
/* Receive a single datagram from the server */
if (sock == -1) return(0);
char *buffer = (char *)packetbuffer;
     if (timer_on_polling_flag) {
         watchdog_point = 1;
         ourpolls->revents = 0;
         int r;
#ifdef ANDROID	 
         /*r =*/ poll(ourpolls, 1, 10);
#endif
#ifdef __APPLE__
     /* ios variant */
         /*r =*/ poll(ourpolls, 1, 0);
#endif
         watchdog_point = 14;
         if (!(ourpolls->revents & POLLIN)) {
             return 0; /*waiting*/
         }
     }
 else {
         watchdog_point = 101;
         ourpolls->revents = 0;
         int r;
         /*r =*/ poll(ourpolls, 1, 0);
         watchdog_point = 114;
         if (!(ourpolls->revents & POLLIN)) {
             struct timespec thislong;
             thislong.tv_sec = 0;
#ifdef ANDROID	     
#ifdef fgdfds_POSIX_PRIORITY_SCHEDULING
              sched_yield();
#else	      
              thislong.tv_nsec = 10000000; /* 10 milliseconds */
              nanosleep(&thislong, &thislong);
#endif	      
#else	     
             thislong.tv_nsec = 10000000; /* 10 milliseconds */
             nanosleep(&thislong, &thislong);
#endif	     
             watchdog_point = 115;
             return 0; /*waiting*/
         }
     }



recvStringLen = recvfrom(sock, buffer, PACKETMAXSIZE, 0, NULL, 0);
watchdog_point=2;
if (recvStringLen < 0) {
  recvStringLen=0;
  if (errno==EWOULDBLOCK) {
    we_polled=0;
    return 0; /* should happen often, we will check later */
    }
  else {
      logit("recvfrom() failed errno %d",errno);
      if ((errno==EBADF)||(errno==ENOTCONN)||(errno==ENOTSOCK)) {
        int oldsock=sock;
	init_receiver_internet(the_network_block);
	logit("Resocket %d -> %d",oldsock,sock);
	}
            struct timespec thislong;
             thislong.tv_sec = 0;
             thislong.tv_nsec = 10000000; /* 10 milliseconds */
             nanosleep(&thislong, &thislong);
     
        
      }
  }
else if (recvStringLen==0) return(0); /* double check */
if (recvStringLen>4) {
    decrypt1((unsigned char *)buffer,(int)(recvStringLen-4l),(int)(recvStringLen));
  }
return(1);
}

#endif






void shutdown_receiver_internet() {
close(sock);
    sock=0;
}
