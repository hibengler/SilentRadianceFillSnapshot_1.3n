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

#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"

extern OpusDecoder *opusdecoder;
extern unsigned int last_microseconds;
extern FILE *write_file_spec;

#define MAXFRAMES 12

#define SOUNDRING_COUNT 400
#define COMMANDRING_COUNT 1400

extern int soundringhead;
extern int soundringtail;
extern char *soundring[SOUNDRING_COUNT];
extern int soundlen[SOUNDRING_COUNT];  // always 960


extern char *commandring[COMMANDRING_COUNT];
extern int commandlen[COMMANDRING_COUNT];
extern int commandringhead;


extern float minpcm;
extern float maxpcm;
extern int deltasum;
extern int deltacount;
extern int underruncount;
extern int missingcount;
extern int majormissingcount;
extern int blankedcount;

extern int init_music_decoder();
extern unsigned char checksum(unsigned char *compress,int compress_length);
extern int addSample(int sampleSize,unsigned char *packet, int useRaw,int mustWriteOne_flag,int possibly_play_flag,int ignore_packets_flag);
extern int make_sound_from_file_packet(unsigned char *packet,int sampleSize);
