/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2019 Hibbard M. Engler

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
// $Revision: 1.1 $
// built from silentradiance_make_demo
// This one uses projectM(Milkdrop) as the main display
// and randomly will turn on backward_worm, network_cube, or purple_string
// It defaults to 1920x1080 - sets the demo mode to 2
// It also uses different settings for milkdrop to work better.
// it also disbles and re-enables the song name for the first 10 seconds.
// and the last 10 seconds
#include <stdio.h>
#include "glue.h"
#ifdef _WIN32
#include "win64_network_receiver.h"
#else
#include "network_receiver.h"
#endif

#include "processor.h"
#include "display_main.h"
#include "packet_summary.h"

#define OUR_DEMO_MODE 2

extern int demo_screen_width;  // in glue defaults to 1920
extern int demo_screen_height; // in glue - defaults to 1920
/* somewhat  implemented */
extern double demo_fps; //=30.f;
extern struct timeval demo_start_time; // =15.f; we do 0
extern struct timeval demo_end_time; // 20.f;  we do the whole length

extern int we_are_reading_file;
extern char *demo_display_set_order;
/* demo_mode width height fps start_time length_time displat_set_string*/
int main(int argc,char *argv[]) {
//NativeInitThreadAll(argc,argv);

NativeInitDemo(argc,argv,2);
Renderer_Init_Demo(demo_screen_width,demo_screen_height);
do {}
  while (NativeWorkDemo()&&we_are_reading_file==1);

Renderer_Finish_Demo();
NativeFinish();
}
