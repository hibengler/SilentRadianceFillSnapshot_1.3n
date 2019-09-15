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
#endif

#include "processor.h"
#include "display_main.h"


int main(int argc,char *argv[]) {
#ifdef _WIN32
logit("aaa\n");
//NativeInitThreadAll(argc,argv);
logit("bbb\n");
NativeInitVisualizer(argc,argv);
Renderer_Init(640,480);
//Renderer_Init(3840,2160);
logit("made it \n");
do {
  NativeWorkVisualizer();
  } while (Renderer_Step());
Renderer_Finish();
//NativeFinish();

#else
logit("aaa\n");
NativeInitVisualizerThreadAll(argc,argv);
logit("bbb\n");

Renderer_Init(1920,1080);
//Renderer_Init(3840,2160);
logit("made it \n");
do {
  } while (Renderer_Step());
Renderer_Finish();
//NativeFinish();
#endif
}
