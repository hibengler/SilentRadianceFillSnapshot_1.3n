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


#define MAXFRAMES 200
#define MAXFRAMES_TRANSMIT 36
#define MAXFRAMES_CHECKSUM 12

int init_internet(int argc, char *argv[]); /* 3,"IPV4_SR00_POOR","ip",0,"SR00" */


void beep(float dblevel,int onset);
int boop(int *compress_lengths,unsigned char *compress[],
   int *frame_numbers,
   unsigned char *command,
   int command_length,int poor_mode);
void reboop();


// stroop outputs to stdout
int stroop(int compress_lengths[],
unsigned char *compress[],
int frame_numbers[],
unsigned char *command,
int command_length,int poor_mode);

extern int poor_mode;

#define PER_FRAME_OVERHEAD 16
#define PACKET_OVERHEAD 16
