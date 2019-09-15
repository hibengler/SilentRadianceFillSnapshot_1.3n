#ifndef __ANALYZER_HEADER_H
#define __ANALYZER_HEADER_H 1
/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler

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








#include "glue.h"
#include "processor.h"
#include "analyzer.h"
#ifdef _WIN32
#include "win64_network_receiver.h"
#else
#include "network_receiver.h"
#endif


/* This is kinda instead of processor.c 
Where processor.c generates sound packets from looking at the internet stuff,
analyzer reads the sound packets that are digitized, and processes them through tools
to figure out the following:
beat onset information
tempo point information
db level
and pitch
*/

extern int analyzing;

extern void init_analyzer(void);

extern int analyze(void);

extern void shutdown_analyzer(void);




#endif
