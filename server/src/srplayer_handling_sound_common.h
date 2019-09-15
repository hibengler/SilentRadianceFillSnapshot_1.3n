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




extern int slow_down_speed;

extern int init_receiver_sound(int block);
extern int playOneSoundBuffer();
extern void finish_play_sound();

extern volatile int soundringhead; /* start of the soundringqueue. normally incremented by sound.  But could overflow */
extern volatile int soundringfirst; /* also incrmented by processor.c - probably the first viable sound in the ring */
extern volatile int soundringtail; /* the end on the sound ring. this is incremented by processor.c whenever a packet is added */
extern volatile int soundringcurrent; /* we are playing this RIGHT NOW: -1 if we are not playing.  This is updated by the sound subsystem */
extern volatile short soundring[][960];  /* points to soundring_area */

extern volatile long soundframe[];
extern int delay_frames;


extern void play_one(char *out1);
extern void clear_out_sound_buffer();
extern void exit_handler1(void);
