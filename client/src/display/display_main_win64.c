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

//
//  display_main_osx.c
//  SilentRadiance
//
//  Created by hib on 6/29/16.
//  Copyright © 2016 Killer Cool Development, LLC. All rights reserved.
//

#include "glue.h"
#include "helper.h"

#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>

#include "SDL_gles2_stubcall_overs_wrapper.h"



#include <stdlib.h>
#include <stdio.h>


#include "display_main_linux.h"
/*$Revision: 1.14 $*/
#include "default.h"
#include "display_set.h"
#include "packet_summary.h"
#include "purple_string.h"
/* this is the glue from java to c for 3gl es stuff yeah */

/* Graphics calls */
int lightning_flag=1;
SDL_Window* window;
SDL_GLContext*sdl_context;

GLES2_Context gles2_context;
 SDL_Renderer *rdr;
 
int save_width;
int save_height;
int screen_orientation = 1; /* 1 - top */
int screen_layout = 1; /* landscape */
int screen_drawmode = 0; /* vr */
extern float xdpi;
extern float ydpi;



/* buttons assume portrait top orientation.  with a +1 to -1 range in x, and a +1 to +1 range in y 
They are then corrected to aspect ratio coordinates with the top as top. one direction is -1 to +1, but the other direction is bigger left, right, up,down is cartesian.
Then to real coordinates 
For landscape (top or left) - this gets rotated 90 degrees left.
For landscape (top or right) - this gets rotated 90 degrees right.
*/


void new_size(SDL_Window* window,int width,int height) {
display_set_surface_changed(width,height,screen_orientation,screen_layout,screen_drawmode);
save_width=width;
save_height=height;
}





void key_callback(SDL_Window* window, int key, int scancode, int action, int mods)
{
if (action==SDL_KEYDOWN) {
  if ((key == SDLK_q)||(key==SDLK_ESCAPE)||(key==SDLK_BACKSPACE)) {
    if (display_set_keyEvent(6)==-1) {
      exit(0);
      }
    }
  else if ((key==SDLK_RETURN)||(key==SDLK_KP_ENTER)||(key==SDLK_SPACE)||(key==SDLK_KP_5)) {
    display_set_keyEvent(1);
    }
  else if ((key==SDLK_KP_8)||(key==SDLK_w)||(key==SDLK_UP)) {
    display_set_keyEvent(2);
    }
  else if ((key==SDLK_KP_2)||(key==SDLK_s)||(key==SDLK_DOWN)) {
    display_set_keyEvent(3);
    }
  else if ((key==SDLK_KP_4)||(key==SDLK_a)||(key==SDLK_LEFT)) {
    display_set_keyEvent(4);
    }
  else if ((key==SDLK_KP_6)||(key==SDLK_d)||(key==SDLK_RIGHT)) {
    display_set_keyEvent(5);
    }
  /*??? need to implement hold key */
  }
}



extern float xdpi;
extern float ydpi;



/* Call this instead of exit(), so we can clean up SDL: atexit() is evil. */
static void
quit(int rc)
{
    int i;

    if (sdl_context != NULL) {
                SDL_GL_DeleteContext(sdl_context);
            }

        SDL_free(sdl_context);

    exit(rc);
}



void Renderer_Init(int width, int height)
{


    int widthMM,heightMM;
    if (SDL_WasInit(SDL_INIT_VIDEO)) {return;}
    SDL_Init(SDL_VIDEO_OPENGL_ES2|SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_HAPTIC|SDL_INIT_EVENTS|SDL_INIT_GAMECONTROLLER);
//#define WORLD_Silent_Radiance "Silent Radiance"
    window = SDL_CreateWindow(WORLD_Silent_Radiance,
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          0,0,
//                             SDL_WINDOW_OPENGL );
                             SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_BORDERLESS|SDL_WINDOW_OPENGL );
//SDL_WINDOW_BORDERLESS 
//                          SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL_ES2 | SDL_WINDOW_BORDERLESS );
    
 SDL_Renderer *rdr = SDL_CreateRenderer(
         window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
 if (!rdr) {
   fprintf(stderr,"error creating a renderer - \n%s\n",SDL_GetError());
   quit(2);
   }

int status;

/* block to deal with width and height */
{
  int displays = SDL_GetNumVideoDisplays();
  int status;
  if (displays>1) {}
  SDL_Rect display_bounds;
  status = SDL_GetDisplayBounds(0,&display_bounds);
  if (status !=0) {
//#define WORLD_SDL_GetDisplayBounds_failed_colon___pct_s "SDL_GetDisplayBounds failed: %s"
    SDL_Log(WORLD_SDL_GetDisplayBounds_failed_colon___pct_s, SDL_GetError());
    quit(2);
    }
  save_width = display_bounds.w;
  save_height = display_bounds.h;
  int real_width,real_height;
  status=SDL_GetRendererOutputSize(rdr,&real_width,&real_height);
  fprintf(stderr,"got %d,%d for pixels\n",save_width,save_height);
  fprintf(stderr,"but for real(retina)- it is %d,%d)\n",real_width,real_height); // We can ignore retina for now, I think.
  
  float ddpi,hdpi,vdpi;
  status = SDL_GetDisplayDPI(0,&ddpi,&hdpi,&vdpi);
  if (status !=0) {
//#define WORLD_SDL_GetDisplayDPI_failed_colon___pct_s "SDL_GetDisplayDPI failed: %s"
    SDL_Log(WORLD_SDL_GetDisplayDPI_failed_colon___pct_s, SDL_GetError());
    xdpi = 100.;
    ydpi = 100.;
    }
  else {
    xdpi =hdpi;
    ydpi= vdpi; 
    }
  fprintf(stderr,"we got %lf %lf for dpi\n",xdpi,ydpi);
  
  fprintf(stderr,"now adjust for real...\n");
  xdpi = xdpi * ((double)real_width)/((double)save_width);
  ydpi = ydpi * ((double)real_height)/((double)save_height);
  fprintf(stderr,"now dpi is %lf,%lf\n",xdpi,ydpi);
  
  save_width = real_width;
  save_height = real_height;
     
  widthMM =(double)((double)(save_width))*25.4/(xdpi);  
  heightMM= (double)((double)(save_height))*25.4/(ydpi);
  fprintf(stderr,"we know actial width and height. %lfx%lf mm  o5 %lfx%lf inch\n",(double)(widthMM),(double)(heightMM),widthMM/25.4,heightMM/25.4);
  fprintf(stderr,"Tested it. Still off a bit. Oh well \n");
  } // block to deal with width an d height
  
  

sdl_context = SDL_GL_CreateContext(window);
if (!sdl_context) {
//#define WORLD_SDL_GL_CreateContext_oparen__cparen__colon___pct_s_lf_ "SDL_GL_CreateContext(): %s\n"
  SDL_Log(WORLD_SDL_GL_CreateContext_oparen__cparen__colon___pct_s_lf_, SDL_GetError());
  quit(2);
  }
	 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        status = SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
        status = SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
        status = SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
        status = SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);



// hook
   if (LoadContext(&gles2_context)<0) {
//#define WORLD_Could_not_load_GLES2_functions_lf_ "Could not load GLES2 functions\n"
      SDL_Log(WORLD_Could_not_load_GLES2_functions_lf_);
      exit(2);
     }
			  
			  


       SDL_GL_SetSwapInterval(1);
//        SDL_GL_SetSwapInterval(0);



//#define WORLD_Vendor______colon___pct_s_lf_ "Vendor     : %s\n"
    SDL_Log(WORLD_Vendor______colon___pct_s_lf_, gles2_context.glGetString(GL_VENDOR));
//#define WORLD_Renderer____colon___pct_s_lf_ "Renderer   : %s\n"
    SDL_Log(WORLD_Renderer____colon___pct_s_lf_, gles2_context.glGetString(GL_RENDERER));
//#define WORLD_Version_____colon___pct_s_lf_ "Version    : %s\n"
    SDL_Log(WORLD_Version_____colon___pct_s_lf_, gles2_context.glGetString(GL_VERSION));
//    SDL_Log("Extensions : %s\n", gles2_context.glGetString(GL_EXTENSIONS));

if (SDL_GL_MakeCurrent(window,sdl_context)<0) {
  fprintf(stderr,"Cant make current: %s\n",SDL_GetError());
  quit(2);
  }


// 
// this is where GLEW would go
/*
	// Init GLEW
	#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
	#endif
*/



//    init_packet_summary();

// start with blank?
// no - we will start with the flash, and be up to date.
//    gles2_context.glClearColor(1.0, 0.0, 1.0, 1.0);
//    gles2_context.glClear(GL_COLOR_BUFFER_BIT);
//   SDL_GL_SwapWindow(window);


display_set_init();
//display_set_surface_changed(save_width,save_height,screen_orientation);
display_set_surface_changed(save_width,save_height,screen_orientation,screen_layout,screen_drawmode);
}



int button_last_state=0;




void display_main_set_screen_drawmode(int newmode) {
if (newmode) {
  if (screen_drawmode==0) {
    screen_drawmode = 1;
    display_set_surface_changed(save_width,save_height,screen_orientation,screen_layout,screen_drawmode);
    }
  }
else {  
  if (screen_drawmode==1) {
    screen_drawmode = 0;
    display_set_surface_changed(save_width,save_height,screen_orientation,screen_layout,screen_drawmode);
    }
  }  
}


int Renderer_Step()
{
if (SDL_GL_MakeCurrent(window,sdl_context)<0) {
  fprintf(stderr,"cant make current\n%s\n",SDL_GetError());
  }
SDL_Event event;
if (button_last_state) button_last_state--; // debouncing
while (SDL_PollEvent(&event)) {
  if (event.type == SDL_QUIT) {
    fprintf(stderr,"qqqqq\n");
    exit(-6);
    }
  if (event.type == SDL_KEYDOWN)
			{
									key_callback(window,event.key.keysym.sym,0,event.type,0);
			}
  else if (event.type==SDL_MOUSEBUTTONDOWN) {
    if (event.button.button==SDL_BUTTON_LEFT) {
      if (!(button_last_state)) {
        display_set_touchEvent(1,event.button.x,event.button.y);
	button_last_state=1; 
	}
      }
    }
  else if (event.type=SDL_WINDOWEVENT) {
    if (event.window.event==SDL_WINDOWEVENT_RESIZED) {
      if (event.window.windowID ==0) { /* only do first screen for now */
                            int w, h;
                            int status = SDL_GL_MakeCurrent(window,sdl_context);
                            if (status) {
//#define WORLD_SDL_GL_MakeCurrent_oparen__cparen__colon___pct_s_lf_ "SDL_GL_MakeCurrent(): %s\n"
                                SDL_Log(WORLD_SDL_GL_MakeCurrent_oparen__cparen__colon___pct_s_lf_, SDL_GetError());
                                }
                            else {
                              /* Change view port to the new window dimensions */
//                              SDL_GL_GetDrawableSize(state->windows[i], &w, &h);
// 			      status=SDL_GetRendererOutputSize(rdr,&w,&h);
//                               glViewport(0, 0, w, h);
			      new_size(window,w,h);
			      }

                            /* Update window content */
                            //Render(event.window.data1, event.window.data2, &datas[i]);
                            }
      }
    } /* resize screen */
  } /* while getting these events. */

//compute_packet_summary(); I think its already done?


display_set_render();
    

SDL_GL_SwapWindow(window);	

return 1;
}



void Renderer_Finish() 
{
// ?? finish packet?
display_set_finish();
// Delete our OpengL context
	SDL_GL_DeleteContext(sdl_context);

	// Destroy our window
	SDL_DestroyWindow(window);

	// Shutdown SDL 2
SDL_Quit();
}






/* kudos to http://headerphile.com/sdl2/opengl-part-1-sdl-opengl-awesome/ */
/*
 $Log: display_main_win64.c,v $
 Revision 1.14  2019/04/15 11:29:48  hib
 OK. NOW this is the correct version for internationalization on linux!

 Revision 1.13  2019/04/15 10:40:50  hib
 WORLD_ macros have now set up silent radiance to be internationalized.
 It is easier to implement a system where there are separate builds,
 but I could handle a weird multi-languge on one build with more trickiness.

 The world_internatiolization.h file sets it up
 Currently just english

 Revision 1.12  2019/03/17 12:58:24  hib
 Updated copyrights

 Revision 1.11  2018/03/07 03:24:33  justin
 Fixes to get raylib to compile on windows

 Revision 1.10  2017/11/04 11:58:40  hib
 PK, we are about to publish the training radio station

 Revision 1.9  2017/11/04 03:58:10  hib
 SDL thread example - switch to pthreads

 Revision 1.8  2017/11/02 06:34:16  hib
 Well, this kinda works. Shucks.  Now for curl,or get the web site up.
 I choose curl

 Revision 1.7  2017/11/01 23:34:41  hib
 ready to fix this

 Revision 1.6  2017/11/01 07:50:58  hib
 closer. got a trianlge to work

 Revision 1.5  2017/11/01 06:59:11  hib
 no movement - I can set it purple

 Revision 1.4  2017/11/01 00:22:51  hib
 Yeah, this is a branch, but it is a very important branch, as it is taming the windows beast.

 Revision 1.3  2017/10/31 21:57:56  hib
 wip

 Revision 1.2  2017/10/31 08:40:52  hib
 compiled!

 Revision 1.1  2017/10/31 06:31:42  hib
 there it runs again, but probably is a blank screen.
 We are getting closer.

from display_main_linux:  Revision 1.7  2017/07/05 12:59:10  hib
 added copyleft to the client

 Revision 1.6  2017/04/06 11:16:55  hib
 minor fixes

 Revision 1.5  2017/04/06 02:25:21  hib
 

 Revision 1.4  2017/04/05 19:17:02  hib
 Got the keys to work, though their positions are off and the enter key doesn't work yet.
 Now just got to fix the fucking problemw with translation from the fake world to the real world of points.

 Revision 1.3  2017/04/04 19:18:28  hib
 Getting there. mouse is starting to work, but width and height are weird.

 Revision 1.2  2017/04/04 09:08:37  hib
 Got it to run on linux as a client! yo!

 Revision 1.1  2016/12/20 23:45:37  hib
 consolidating

 
 */
