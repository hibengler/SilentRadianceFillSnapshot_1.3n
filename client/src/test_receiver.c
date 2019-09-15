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
// WORLD_INTERNATIONAL off


 

/*

This example reads standard from input and writes
to the default PCM device for 5 seconds of data.

*/

#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */




  long loops;
  int rc;
  int size;
  unsigned int val;
  int dir;
 unsigned char *buffer;


int sock; /* Socket */
struct sockaddr_in broadcastAddr; /* Broadcast Address */
unsigned short broadcastPort; /* Port */
int recvStringLen; /* Length of received string */










int finish_play_sound() {
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

if (argc != 2) /* Test for correct number of arguments */
{
fprintf(stderr,"Usage: %s <Broadcast Port>\n", argv[0]);
exit(1);
}
size=8000;
buffer = (char *) malloc(size);

broadcastPort = atoi(argv[1]); /* First arg: broadcast port */
fprintf(stderr,"receiving on %d\n",broadcastPort);
/* Create a best-effort datagram socket using UDP */
if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
DieWithError("socket() failed");

/* Construct bind structure */
memset(&broadcastAddr, 0, sizeof(broadcastAddr)); /* Zero out structure */
broadcastAddr.sin_family = AF_INET; /* Internet address family */
broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
broadcastAddr.sin_port = htons(broadcastPort); /* Broadcast port */

/* Bind to the broadcast port */
if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
DieWithError("bind() failed");
}





int finish_internet_receiver() {
close(sock);
}




int receiver_loop() {
while (1) {
  int onset;
  /* Receive a single datagram from the server */
  if ((recvStringLen = recvfrom(sock, buffer, size, 0, NULL, 0)) < 0)
  DieWithError("recvfrom() failed");
  float x;
  unsigned char *xc;
  xc = (unsigned char *)&x;
  xc[0]=buffer[4];
  xc[1]=buffer[5];
  xc[2]=buffer[6];
  xc[3]=buffer[7];
  int b = buffer[0]+buffer[1]*256+
      buffer[2]*65536+buffer[3]*16777216;
  onset=0;
  if (b>=1048576000) {
    b=b-=1048576000;
    onset=1;
    }
    
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int a;
    a = tv.tv_sec & 1048575; /* 277 hours*/
    a = a*1000 + (tv.tv_usec/1000);
  fprintf(stderr,"Got %d %f     %d\n",b,x,(a-b));
          
  }

}


int main(int argc,char *argv[]) {
init_receiver_internet(argc,argv);
receiver_loop();
}
