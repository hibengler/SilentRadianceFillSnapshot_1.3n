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
// $Revision: 1.11 $
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


#define OUR_DEMO_MODE 1

extern int demo_screen_width;  // in glue defaults to 480
extern int demo_screen_height; // in glue - defaults to 480
extern double demo_fps; //=30.f;
extern struct timeval demo_start_time;// =15.f;
extern struct timeval demo_end_time; // 20.f;

extern int we_are_reading_file;
extern char *demo_display_set_order;
/* demo_mode width height fps start_time length_time displat_set_string*/
int main(int argc,char *argv[]) {
fprintf(stderr,"demo %dx%d\n",demo_screen_width,demo_screen_height);
NativeInitDemo(argc,argv,OUR_DEMO_MODE);
Renderer_Init_Demo(demo_screen_width,demo_screen_height);
fprintf(stderr,"demo2 %dx%d\n",demo_screen_width,demo_screen_height);
do {}
  while (NativeWorkDemo()&&we_are_reading_file==1);

Renderer_Finish_Demo();
NativeFinish();
}
