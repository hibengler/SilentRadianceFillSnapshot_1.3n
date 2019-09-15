#include <stdio.h>
#include <stdlib.h>
/* this assumes 480x480, and goes forward and backward.
The raw video is 480x480RGB
the video to incorporate is 480x480RGBA
The result is 480x480RGB
*/
#include "world_internationalization.h"

// quick way to incorporate a background screen to the make_demo stuff

#define WIDTH 480
#define HEIGHT 480
struct rgb {
unsigned char r;
unsigned char g;
unsigned char b;
};

struct rgba {
unsigned char r;
unsigned char g;
unsigned char b;
unsigned char a;
};

#define FULL_RGB 691200
#define FULL_RGBA 921600

struct frame_background {
struct rgb pixel[WIDTH*HEIGHT];
};

struct frame_foreground {
struct rgba pixela[WIDTH*HEIGHT];
};

struct frame_background back;

struct frame_foreground fore;

struct frame_background final;


// usage : use_raw_vid_for_layer_background a.rawrgb main.rgba
int main (int argc,char *argv[]) {
if (argc !=3) {
  fprintf(stderr,"Usage: %s [back.rawrgb] [fore.rawrgba] stdout is result, could be appending back.rawrgb, we dont care\n",argv[0]);
  }
FILE *fback;
FILE *ffore;

//#define WORLD_use_raw_vid_for_layer_background_rb "rb"
fback=fopen(argv[1],WORLD_use_raw_vid_for_layer_background_rb);
//#define WORLD_use_raw_vid_for_layer_background_rb "rb"
ffore=fopen(argv[2],WORLD_use_raw_vid_for_layer_background_rb);
if (!fback) { fprintf(stderr,"cant read background file %s\n",argv[1]); exit(-1);}
if (!ffore) { fprintf(stderr,"cant read foreground file %s\n",argv[2]); exit(-1);}

// count the back frames
int back_frames=0;
while (fread(&back.pixel,1,FULL_RGB,fback)!=0) {
  back_frames++;
  }

fprintf(stderr,"%d frames in background. starting at frame %d\n"
 ,back_frames,back_frames-1);
// start going backward
int current_back_frame;
int current_direction;
current_back_frame=back_frames-2; 
current_direction=-1;

while (fread(&fore.pixela,FULL_RGBA,1,ffore)) {
  fseek(fback, (long)(current_back_frame*FULL_RGB),SEEK_SET);
  fread(&back.pixel,1,FULL_RGB,fback);
  for (int i=0;i<WIDTH*HEIGHT;i++) {
    unsigned int f1,f2;
    f1 = fore.pixela[i].a;
    f2 = 255 -f1;
    unsigned int t,t1,t2;
    t1 = ((unsigned int)fore.pixela[i].r) * f1;
    t2 = ((unsigned int)back.pixel[i].r) * f2;
    t = (t1+t2)/255;
    final.pixel[i].r = t;
    
    t1 = ((unsigned int)fore.pixela[i].g) * f1;
    t2 = ((unsigned int)back.pixel[i].g) * f2;
    t = (t1+t2)/255;
    final.pixel[i].g = t;
    
    t1 = ((unsigned int)fore.pixela[i].b) * f1;
    t2 = ((unsigned int)back.pixel[i].b) * f2;
    t = (t1+t2)/255;
    final.pixel[i].b = t;

    }


  fwrite(&final.pixel,FULL_RGB,1,stdout);
  if (current_direction >0) fprintf(stderr,".");
  else fprintf(stderr,">");
  if ((current_back_frame+current_direction) == 0) {
    current_back_frame = 0;
    current_direction= 1;
    }
  else if (current_back_frame+current_direction>=back_frames) {
    current_back_frame = back_frames-2;
    current_direction = -1;
    }
  else {
    current_back_frame += current_direction;
    }
  }
fprintf(stderr,"\n");
fclose(fback);
fclose(ffore);
return 0;
}
