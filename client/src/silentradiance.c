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
#include <stdio.h>
#include "glue.h"
#ifdef _WIN32
#include "win64_network_receiver.h"
#else
#include "network_receiver.h"
#include <time.h>
#endif

#include "processor.h"
#include "display_main.h"


int main(int argc,char *argv[]) {

NativeInitThreadAll(argc,argv);

//NativeInit();
//Renderer_Init(1920,1080);
//Renderer_Init(3840,2160);
#ifdef LINUX_SIMULATE_APPLE
# ifdef LINUX_SIMULATED_APPLE_IS_6_POINT_5_VERTICAL
   Renderer_Init(1242,2688); // 6.5 inch iphone
# endif
# ifdef LINUX_SIMULATED_APPLE_IS_6_POINT_5_HORIZONTAL
   Renderer_Init(2688,1242); // 6.5 inch iphone
# endif
# ifdef LINUX_SIMULATED_APPLE_IS_12_POINT_9_GEN_3_VERTICAL
   Renderer_Init(2048,2732); // 12.9 3rd generation ipad
# endif
# ifdef LINUX_SIMULATED_APPLE_IS_12_POINT_9_GEN_3_HORIZONTAL
   Renderer_Init(2732,2048); // 12.9 3rd generation ipad
# endif
#else
  Renderer_Init(1920,1080);
#endif

do {

//  NativeWork();
#ifdef LINUX_SIMULATE_APPLE
/* this is used to get screenshot demos:
*/
           struct timespec thislong;
             thislong.tv_sec = 0;
             thislong.tv_nsec = 400000000; /* 400 milliseconds */
             nanosleep(&thislong, &thislong);
/**/
#endif


  } while (Renderer_Step());
Renderer_Finish();
//NativeFinish();
}
