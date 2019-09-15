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
#include <stdio.h>
#include <stdlib.h>
#include "world_internationalization.h"
// quick way to incorporate a background screen to the make_demo stuff

#define WIDTH 1920
#define HEIGHT 1080
struct rgb {
unsigned char r;
unsigned char g;
unsigned char b;
};

#define FULL 6220800

struct frame {
struct rgb pixel[WIDTH*HEIGHT];
};

struct frame a;
struct frame b;
struct frame c;


int main () {
FILE *f1;
FILE *f2;
FILE *f3;
//#define WORLD_combine_raw_vids_a_dot_rgb "a.rgb"
//#define WORLD_combine_raw_vids_rb "rb"
f1=fopen(WORLD_combine_raw_vids_a_dot_rgb,WORLD_combine_raw_vids_rb);
if (!f1) { fprintf(stderr,"cant read a.rgb\n"); exit(-1);}
fread(&a.pixel,FULL,1,f1);
fclose(f1);
  for (int i=0;i<WIDTH*HEIGHT;i++) {
    a.pixel[i].r =  (unsigned char) ( ( ((int)a.pixel[i].r) * 60 ) / 100);
    a.pixel[i].g =  (unsigned char) ( ( ((int)a.pixel[i].g) * 60 ) / 100);
    a.pixel[i].b =  (unsigned char) ( ( ((int)a.pixel[i].b) * 60 ) / 100);
    }

//#define WORLD_a0_dot_rawrgb "a0.rawrgb"
//#define WORLD_combine_raw_vids_r "r"
f2 = fopen(WORLD_a0_dot_rawrgb,WORLD_combine_raw_vids_r);
//#define WORLD_combine_raw_vids_a_dot_rawrgb "a.rawrgb"
//#define WORLD_combine_raw_vids_w "w"
f3 = fopen(WORLD_combine_raw_vids_a_dot_rawrgb,WORLD_combine_raw_vids_w);
if (!f2) { fprintf(stderr,"cant read a0.rawrgb\n"); exit(-1);}

while (fread(&b.pixel,1,FULL,f2)!=0) {
  for (int i=0;i<WIDTH*HEIGHT;i++) {
    c.pixel[i] = (b.pixel[i].r|b.pixel[i].g|b.pixel[i].b)?b.pixel[i]:a.pixel[i];
    }
  fprintf(stderr,".");
  fwrite(&c.pixel,FULL,1,f3);
  }
fclose(f3);
fclose(f2);
return 0;
}
