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
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


#ifdef __EMSCRIPTEN__

int main(int argc,char *argv[]) {
NativeInit(argc,argv);
fprintf(stderr,"start work loop\n");
// emscripten_set_main_loop(NativeWorkx,60,0);
fprintf(stderr,"bye\n");
//NativeFinish();
return 0;
}

#else 


int main(int argc,char *argv[]) {
NativeInit(argc,argv);
while (1) {
  NativeWork();
  }
NativeFinish();
}
#endif
