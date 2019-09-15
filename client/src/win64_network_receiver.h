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

#ifndef DASOUND24_WNETWORK_RECEIVER_H
#define DASOUND24_WNETWORK_RECEIVER_H



#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <errno.h>

#include "crypt.h"
#define PACKETMAXSIZE 4000


extern unsigned char packetbuffer[PACKETMAXSIZE];
extern int recvStringLen; /* Length of received string */
extern int sock; /* Socket */
extern int soundpolls_count;

extern int timer_on_polling_flag;
extern void init_receiver_internet(int block);
extern int get_internet_receiver_packet(void);
extern void shutdown_receiver_internet(void);


#endif //DASOUND24_NETWORK_RECEIVER_H
