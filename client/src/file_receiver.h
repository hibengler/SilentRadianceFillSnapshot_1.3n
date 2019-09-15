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

#ifndef _FILE_RECEIVER_H
#define _FILE_RECEIVER_H



#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <errno.h>

#ifdef _WIN32
#include "win64_network_receiver.h"
#else
#include "network_receiver.h"
#endif


extern int we_are_reading_file; /* 1 if we are reading file instead of network */

extern void init_receiver_file(char *filename);
extern int get_packet_from_file();
extern void shutdown_file_reader();


#endif //_FILE_RECEIVER_H
