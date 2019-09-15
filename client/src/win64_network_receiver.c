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
#include "win64_network_receiver.h"
#include <winsock.h>
#include <time.h>

unsigned char packetbuffer[PACKETMAXSIZE];
int recvStringLen=0; /* Length of received string */
int sock=-1; /* Socket */
int the_network_block = 0;

int timer_on_polling_flag=0; /* Android 6.0 freezes on the poll, so this has us do a microsleep instead */

struct sockaddr_in broadcastAddr; /* Broadcast Address */
unsigned short broadcastPort; /* Port */


int soundpolls_count;

int we_polled=1;


unsigned long nonblocking = 1;  


WSADATA wsaData;   
/* Thank you http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/code/BroadcastReceiverNonblockingWS.c */

/* note - this might be re-called by the receive loop based on various errors
because we sometimes get a recvfrrom failed error on android, when switching 
networks */
void init_receiver_internet(int block) {
    broadcastPort = 6969;
#define HEY_NONBLOCK 00004000
the_network_block=block;

if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        logit( "WSAStartup() failed");
        return;
    }
   /* Create a best-effort datagram socket using UDP */

//AF_INET,  IPPROTO_UDPSOCK_DGRAM

     sock = socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP);
    if (sock  < 0) {
        logit( "socket() failed");
        return;
    }
BOOL bSockoptTrue = TRUE;
 DWORD dwSockoptTrue = TRUE;
 int iBsockTimeout = 5000;
 int iSockRet;
  int iSockErr;
  #define IP_ONESBCAST 23
#define IP_RECEIVE_BROADCAST 22 // Allow/block broadcast reception. #   


    struct sockaddr_in r1,r2;
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    r1.sin_family = AF_INET; /* Internet address family */
//#define WORLD_255_dot_255_dot_255_dot_255 "255.255.255.255"
    r1.sin_addr.s_addr = inet_addr(WORLD_255_dot_255_dot_255_dot_255);
    r1.sin_port = htons(broadcastPort);
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    r2.sin_family = AF_INET; /* Internet address family */
    r2.sin_port = htons(broadcastPort);
//#define WORLD_win64_network_receiver_8_dot_8_dot_8_dot_8 "8.8.8.8"
    r2.sin_addr.s_addr = inet_addr(WORLD_win64_network_receiver_8_dot_8_dot_8_dot_8);
	
    iSockRet = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *) &bSockoptTrue, sizeof bSockoptTrue);

   
/* Set the socket to nonblocking */
    if (ioctlsocket(sock, FIONBIO, &nonblocking) != 0) {
        logit( "socket() failed");
        return;
    }



/* Construct bind structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr)); /* Zero out structure */
    broadcastAddr.sin_family = AF_INET; /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    broadcastAddr.sin_port = htons(broadcastPort);
    

	// maybe opt(SO_BINDTODEVICE) instead, and still bind() to INADDR_ANY
iSockRet = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *) &bSockoptTrue, sizeof bSockoptTrue);
 iSockRet = setsockopt(sock, IPPROTO_IP, IP_RECEIVE_BROADCAST, (char *) &dwSockoptTrue, sizeof dwSockoptTrue);
iSockRet = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &bSockoptTrue, sizeof bSockoptTrue);
iSockRet = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &iBsockTimeout, sizeof iBsockTimeout);
// iSockRet = setsockopt(sock, SOL_SOCKET, SO_CONDITIONAL_ACCEPT
// , (char *) &dwSockoptTrue, sizeof dwSockoptTrue);
// undefined but here https://msdn.microsoft.com/en-us/library/ms740476(VS.85).aspx	
// Maybe so_RCVBUF?
//   iSockRet = setsockopt(sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *) &iBsockTimeout, sizeof iBsockTimeout);
// ^ error




//#define SO_EXCLUSIVEADDRUSE ((int)(~SO_REUSEADDR))
//  iSockRet = setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &bSockoptTrue, sizeof bSockoptTrue);

// iSockRet = setsockopt(sock, IPPROTO_IP, IP_ONESBCAST, (char *) &dwSockoptTrue, sizeof dwSockoptTrue);




/* Bind to the broadcast port */
    if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0) {
        logit( "bind() failed");
        close(sock);
        sock = 0;
        }
//#define WORLD_win64_network_receiver_42 "42"
    int r=sendto(sock,WORLD_win64_network_receiver_42,3,0,(SOCKADDR *)&r1,sizeof(r1));
    if (r == SOCKET_ERROR) {
//#define WORLD_sendto_failed_with_error_colon___pct_d_lf_ "sendto failed with error: %d\n"
        wprintf(LWORLD_sendto_failed_with_error_colon___pct_d_lf_, WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return;
        }
/*
     r=sendto(sock,"laser",6,0,(SOCKADDR *)&r2,sizeof(r2));
    if (r == SOCKET_ERROR) {
        wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return;
        }
*/


    we_polled=0;
return;
}

int get_internet_receiver_packet() {
/* here we know that we will get one packet - because the polls told us */
/* Receive a single datagram from the server */
if (sock == -1) return(0);
char *buffer = (char *)packetbuffer;


recvStringLen = recvfrom(sock, buffer, PACKETMAXSIZE, 0, NULL, 0);
watchdog_point=2;
if (recvStringLen < 0) {
  recvStringLen=0;
  int e;
  if ((e=WSAGetLastError()) == WSAEWOULDBLOCK) {
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
//            struct timespec thislong;
  //           thislong.tv_sec = 0;
    //         thislong.tv_nsec = 10000000; /* 10 milliseconds */
      //       nanosleep(&thislong, &thislong);
     
        
      }
  }
else if (recvStringLen==0) {
  return(0); /* double check */
  }
if (recvStringLen>4) {
  decrypt1((unsigned char *)buffer,recvStringLen-4,recvStringLen);
  }
else {
  logit("weeiwoej\n");
  }
return(1);
}







void shutdown_receiver_internet() {
closesocket(sock);
    sock=0;
WSACleanup();
}
