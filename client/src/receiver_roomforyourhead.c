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

//define SR_NOSOUND
#include <stdio.h>
#include "glue.h"
#ifdef _WIN32
#include "win64_network_receiver.h"
#else
#include "network_receiver.h"
#endif

#include "processor.h"
#include "packet_summary.h"
#include <math.h>
#include <stdlib.h>
#include <pthread.h>


// copied from rooforyouhead on 2017-03-24
// Save as:
//    roomforyourhead.c
//
// Compile with:
//    gcc roomforyourhead.c -o roomforyourhead -lSDL -lGLU -lGL
//
// Execute with:
//    ./roomforyourhead

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

/* structure for a user portal.  Usually just one - but for cooker, who knows? */
struct user_portal {
 SDL_Surface *sdl_surface;
 SDL_VideoInfo *sdl_video_info;
 SDL_SysWMinfo sdl_system_wm_info;
 

 float a; /* used for scaling the roomforyourhead */
 
 float point; /* point between 0 and 1 for the from to looking */
 
 GLdouble from_eye[3];
 GLdouble from_center[3];
 GLdouble from_up[3];
 GLdouble to_eye[3];
 GLdouble to_center[3];
 GLdouble to_up[3];
 
 };
 
 


// Initialize opengl...
void init_opengl(struct user_portal *this_portal,int width, int height)
{
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   gluPerspective(45., ((GLdouble)width)/((GLdouble)height),  1., 15000.);

   glMatrixMode(GL_MODELVIEW);
   glEnable(GL_DEPTH_TEST);
  
   
}





void new_to_point(struct user_portal *this_portal) {
  GLdouble *from_eye = this_portal->from_eye;
  GLdouble *to_eye = this_portal->to_eye;
  GLdouble *from_center = this_portal->from_center;
  GLdouble *to_center = this_portal->to_center;
  GLdouble *from_up = this_portal->from_up;
  GLdouble *to_up = this_portal->to_up;
  int i;
  for (i=0;i<3;i++) {
    int r=2000;
    int f = from_eye[i]- (r/2);
    int t = from_eye[i]+ (r/2);
    if (f<-3000) f=-3000;
    if (t>3000) t=3000;
    r = t-f;
    to_eye[i] = (f+t)/2 + rand()%r;
    }
  for (i=0;i<3;i++) {
    int r=2000;
    int f = from_center[i]- (r/2);
    int t = from_center[i]+ (r/2);
    if (f<-3000) f=-3000;
    if (t>3000) t=3000;
    r = t-f;
    to_center[i] = (f+t)/2 + rand()%r;
    }
  for (i=0;i<3;i++) {
    int r=2000;
    int f = from_up[i]- (r/2);
    int t = from_up[i]+ (r/2);
    if (f<-3000) f=-3000;
    if (t>3000) t=3000;
    r = t-f;
    to_up[i] = (f+t)/2 + rand()%r;
    }
}

        
void init_model(struct user_portal *this_portal) {
  GLdouble *from_eye = this_portal->from_eye;
  GLdouble *to_eye = this_portal->to_eye;
  GLdouble *from_center = this_portal->from_center;
  GLdouble *to_center = this_portal->to_center;
  GLdouble *from_up = this_portal->from_up;
  GLdouble *to_up = this_portal->to_up;
  from_eye[0] = rand() % 6000 - 3000;
  from_eye[1] = rand() % 6000 - 3000;
  from_eye[2] = rand() % 6000 - 3000;
  from_center[0] = rand() % 6000 - 3000;
  from_center[1] = rand() % 6000 - 3000;
  from_center[2] = rand() % 6000 - 3000;
  from_up[0] =  rand() % 6000 - 3000;
  from_up[1] =  rand() % 6000 - 3000;
  from_up[2] =  rand() % 6000 - 3000;
  new_to_point(this_portal);


  this_portal->a = 1.;

}







// Initialize an SDL_Surface as the window surface
// Most of this should explain itself...
SDL_Surface* init_sdl(struct user_portal *this_portal,int width, int height)
{
   SDL_Surface* surface;
   if(SDL_Init(SDL_INIT_VIDEO)< 0)
   {
      return NULL;
   }
   const SDL_VideoInfo* info = SDL_GetVideoInfo();
   if(!info)
   {
      return NULL;
   }
   this_portal->sdl_video_info = (SDL_VideoInfo*)info;
   
/*   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
*/		       
		       
   
//   int flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER  | SDL_ASYNCBLIT | SDL_FULLSCREEN;
   int flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_ASYNCBLIT;
   if (info->hw_available)
   {
     logit("hw!\n");
      flags |= SDL_HWSURFACE;
   }
	else
   {
     logit("sw!\n");
      flags |= SDL_SWSURFACE;
   }
   surface=SDL_SetVideoMode(width, height, info->vfmt->BitsPerPixel, flags);
   if(surface==0)
   {
      return NULL;
   }
   this_portal->sdl_surface = surface;
   
   /* lower the window  = doesnt work*/
#ifdef dshjkdafhlad
   {
   SDL_SysWMinfo *info;
   info = &(this_portal->sdl_system_wm_info);
   SDL_VERSION(&(info->version));
   if (SDL_GetWMInfo(info)) {
     logit("cool.\n");
     (*info).info.x11.lock_func();
     
     int l=XLowerWindow(info->info.x11.display,info->info.x11.window);
     logit("el %d\n",l);
     (*info).info.x11.unlock_func();
     }
   }   
#endif

/* Version 2.0 stuff
   int flag = SDL_GL_SetSwapInterval(1);
   if (!flag) {
     return(NULL);
     }
 */
 
   return surface;
}




static void quit( int code )
{
    /*
     * Quit SDL so we can release the fullscreen
     * mode and restore the previous video settings,
     * etc.
     */
    SDL_Quit( );

    /* Exit program. */
    exit( code );
}




void handle_key_down( SDL_keysym* keysym )
{
    switch( keysym->sym ) {
    case SDLK_ESCAPE:
        quit( 0 );
        break;
    default:
        break;
    }
}


void process_events(struct user_portal *this_portal) {
   /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
        case SDL_KEYDOWN:
	    handle_key_down(&event.key.keysym);
            break;
        case SDL_QUIT:
            /* Handle quit requests (like Ctrl-c). */
            quit( 0 );
            break;
        }

    }



  if (this_portal->a>=4.) this_portal->a=1.;
  else this_portal->a = this_portal->a * 1.01;


  this_portal->point = this_portal->point + 1./(30.*9.9310344);
  if (this_portal->point>1.) {
    this_portal->point = this_portal->point-1.;
    /* make the from = the to */
    this_portal->from_eye[0] = this_portal->to_eye[0];
    this_portal->from_eye[1] = this_portal->to_eye[1];
    this_portal->from_eye[2] = this_portal->to_eye[2];
    this_portal->from_center[0] = this_portal->to_center[0];
    this_portal->from_center[1] = this_portal->to_center[1];
    this_portal->from_center[2] = this_portal->to_center[2];
    this_portal->from_up[0] = this_portal->to_up[0];
    this_portal->from_up[1] = this_portal->to_up[1];
    this_portal->from_up[2] = this_portal->to_up[2];
    new_to_point(this_portal);
    }
}





#define interpolate(from,to,point,now) {now[0]=(to[0]-from[0])*point + from[0]; now[1]=(to[1]-from[1])*point + from[1]; now[2]=(to[2]-from[2])*point + from[2]; }






void draw_screen(struct user_portal *this_portal) { 
  float point=this_portal->point; 
  float a=this_portal->a;
   
  glLoadIdentity();
  GLdouble now_eye[3];
  GLdouble now_up[3];
  GLdouble now_center[3];
  GLdouble *from_eye = this_portal->from_eye;
  GLdouble *to_eye = this_portal->to_eye;
  GLdouble *from_up = this_portal->from_up;
  GLdouble *to_up = this_portal->to_up;
  GLdouble *from_center = this_portal->from_center;
  GLdouble *to_center = this_portal->to_center;
  interpolate(from_eye,to_eye,point,now_eye);
  interpolate(from_up,to_up,point,now_up);
  interpolate(from_center,to_center,point,now_center);
  
  gluLookAt(now_eye[0],now_eye[1],now_eye[2],
            now_up[0],now_up[1],now_up[2],
	    now_center[0],now_center[1],now_center[2]
	    );
	    
  GLdouble line_width = 7.;
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(line_width);
  glBegin(GL_LINES);
  
  GLdouble i;
  GLdouble from;
  GLdouble to;
  GLdouble interval;
  GLdouble from2;
  GLdouble to2;
  GLdouble interval2;
  from=-4000+1.+(line_width*0.5)+40;
  to=-from;
  interval = ((to-from)/29.5);
  
  from2=-4000+1.+(line_width*0.5);
  to2=-from2;
  interval2 = ((to-from)/30.001);
  
  /* RED */
  glColor3f(0.3,0.0, 0.0);
  for (i=from;i<=to;i+=interval) {
    float from[3];
    float to[3];
    from[0] = -4000;
    from[1] = i;
    from[2] = -4000;
    to[0]=4000;
    to[1] = i;
    to[2] = -4000;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }
  /* green */
  glColor3f(0.0,1.0*0.3, 0.0);
  for (i=from2;i<=to2;i+=interval2) {
    float from[3];
    float to[3];
    from[0] = -4000;
    from[1] = -4000;
    from[2] = i;
    to[0] = 4000-(line_width*0.5);
    to[1]= -4000;
    to[2] = i;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }
  /* yellow across from green */
  glColor3f(1.0*0.3,1.0*0.3, 0.0);
  for (i=from2;i<=to2;i+=interval2) {
    float from[3];
    float to[3];
    from[0] = -4000;
    from[1] = 4000;
    from[2] = i;
    to[0] = 4000-(line_width*0.5);
    to[1]= 4000;
    to[2] = i;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }
  /* blue */
  glColor3f(0.0,0.0, 1.0*0.3);
  for (i=from2;i<=to2;i+=interval2) {
    float from[3];
    float to[3];
    from[0] = 4000;
    from[1] = -4000;
    from[2] = i;
    to[0] = 4000;
    to[1]=4000-(line_width*0.5);
    to[2] = i;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }    
  double f=i-interval2;
  /* orange on other side */
  glColor3f(1.0*0.3,(165./256.)*0.3, 0.0);
  for (i=from;i<=to;i+=interval) {
    float from[3];
    float to[3];
    from[0] = -4000;
    from[1] = i;
    from[2] = f;
    to[0]=4000;
    to[1] = i;
    to[2] = f;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }
  /* purple */
  glColor3f(1.0*0.3,0.0, 1.0*0.3);
  for (i=from2;i<=to2;i+=interval2) {
    float from[3];
    float to[3];
    from[0] = -4000;
    from[1] = -4000;
    from[2] = i;
    to[0] = -4000;
    to[1]=4000-(line_width*0.5);
    to[2] = i;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }    
  glEnd();
   
/*  glLoadIdentity();
  glTranslatef(0, 0, -5);
  glBegin(GL_TRIANGLES);
  glColor3ub(0, 255, 0); glVertex2f( 0, a);
  glColor3ub(255, 0, 0); glVertex2f(-a,-a);
  glColor3ub(0, 0, 255); glVertex2f( a,-a);
  glEnd();
*/
   
}


typedef struct {
    double r;       //  [0, 1]
    double g;       //  [0, 1]
    double b;       //  [0, 1]
} rgb;                     
																		       
																		       
//   inconsistent pattern with rgbToHSV (like this approach better) but it's basically working for now http$
rgb hsv2rgb(double H, double S, double V)
{
    rgb RGB;
    double /* H = HSV.h, S = HSV.s, V = HSV.v, */
            P, Q, T,
            fract;
     H *= 360;//adapt for 0-1 hue input
    (H == 360.)?(H = 0.):(H /= 60.);
    fract = H - floor(H);
 
    P = V*(1. - S);
    Q = V*(1. - S*fract);   
    T = V*(1. - S*(1. - fract));

    if      (0. <= H && H < 1.)
        RGB = (rgb){.r = V, .g = T, .b = P};
    else if (1. <= H && H < 2.)
        RGB = (rgb){.r = Q, .g = V, .b = P};   
    else if (2. <= H && H < 3.)
        RGB = (rgb){.r = P, .g = V, .b = T};
    else if (3. <= H && H < 4.)
        RGB = (rgb){.r = P, .g = Q, .b = V};   
    else if (4. <= H && H < 5.)
        RGB = (rgb){.r = T, .g = P, .b = V};
    else if (5. <= H && H < 6.)
        RGB = (rgb){.r = V, .g = P, .b = Q};
    else
        RGB = (rgb){.r = 0., .g = 0., .b = 0.};

    return RGB;
}

     
#define PN 36

float from_pitch=0.;
float to_pitch=0.;
int from_to_pos=0;
int direction=1;


rgb pitch_to_rgb_value(float pitch) {
   float plog = logf(pitch)*1.44269504088884957607f;  /* first constant is 1/log(2) because ln(x)/ln(2)...*/
    int plogi = (int) plog;
    pitch = plog- (float)plogi;
    
    if (from_to_pos==0) {
      from_pitch = to_pitch;
      to_pitch=pitch;
      if (from_pitch==0.) {
        from_pitch=to_pitch;
	}
      if (from_pitch<to_pitch)
        if (to_pitch-from_pitch<.5) direction=1; else direction=-1;
      else if (from_pitch-to_pitch<.5) direction=-1; else direction=1;

      pitch = from_pitch;
      from_to_pos=1.;
      }
    else if ((from_to_pos <PN)) {
      pitch = from_pitch +(to_pitch-from_pitch)*((float)(direction*from_to_pos))/((float)PN);
      if (pitch<0.) pitch += 1.;
      if (pitch>1.) pitch -= 1.;
      
      from_to_pos++;
      }
    else {
      from_pitch = to_pitch;
      to_pitch=pitch;
      if (to_pitch==0.) {
        to_pitch=from_pitch;
	}
      pitch = from_pitch;
      from_to_pos=1.;
      }
    
/*
Old way:
    while (pitch>440.) pitch  = pitch*0.125;
    while (pitch<=220.) pitch  = pitch*2.0;   
pitches are now in the range of 256 to 511 - subtract 255 and we can use the pitch colors to set the color
*/
    
    
      
    rgb xrgb;
    xrgb=hsv2rgb(pitch,1.f,0.99f);
return xrgb;
}

rgb color = {1.f,0.f,1.f};

rgb do_color2(int fb)
{
if (!((packet_summary.state==PACKET_STATE_GOOD)||(packet_summary.state==PACKET_STATE_WARNING))) { return color;}
int index = (packet_summary.start_index + fb - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
if (index<0) index += PACKET_SUMMARY_SIZE;
struct packet *p = packet_summary.packets+index;
if (p->has_statistics) {
//  logit("p->pitch %f\n",p->pitch);
  if (p->pitch!=0.f) {
    rgb c = pitch_to_rgb_value(p->pitch);
    if ((c.r!=0.f)||(c.g!=0.f)||(c.b!=0.f)) {
      color = c;
      }
    }
  }
return(color);
}


int last_frame_number=0;





rgb do_color() {
//    logit(".");
if (!((packet_summary.state==PACKET_STATE_GOOD)||(packet_summary.state==PACKET_STATE_WARNING)||(packet_summary.state==PACKET_STATE_WARMING)) ) { return color;}
if (packet_summary.current_frame == last_frame_number) return color;
//    logit(",");
if (packet_summary.current_frame > last_frame_number+1) {
  if (packet_summary.no_gap_frame > last_frame_number) {
    last_frame_number = packet_summary.no_gap_frame;
    }
  }

//logit("from %d to %d\n",last_frame_number,packet_summary.current_frame);
int i;
for (i=last_frame_number;i<=packet_summary.current_frame;i++) {
  do_color2(i);
//  logit("%d\n",i);
  }
last_frame_number=packet_summary.current_frame;
return color;
}


#define WIDTH 1920
#define HEIGHT 1080





extern volatile int soundPlayCool;




void *MaxheadPlay(void *arg)
{
//    init_receiver_sound(1);
    init_packet_summary();
while (soundPlayCool) {
//  playOneSoundBuffer();  
  compute_packet_summary();
  }

return NULL;
}




int main()
{

struct user_portal this_portal_yeah;
// Create our window and initialize opengl...
SDL_Surface* buffer = init_sdl(&this_portal_yeah,WIDTH, HEIGHT);
init_opengl(&this_portal_yeah,WIDTH, HEIGHT);
init_model(&this_portal_yeah);

init_processor();

{
  pthread_t tid[1];
      soundPlayCool=1;
      int err;
      err = pthread_create(tid,NULL,&MaxheadPlay,NULL);
      if (err != 0)
         logit("\ncan't create thread :[%s]", strerror(err));
      }
    init_receiver_internet(1);

     
	       
while (1) {
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    glClearColor(color.r,color.g,color.b,1.f);
  if (get_internet_receiver_packet()) {
    process_packet();
//    compute_packet_summary();
    do_color();
    }
  else {
    // OpenGL code...
    do_color();
    
    process_events(&this_portal_yeah);
     
    draw_screen(&this_portal_yeah);

    SDL_GL_SwapBuffers();
    }
  }  
       

   // Pause for a couple of seconds...
   SDL_Delay(2000);
}



