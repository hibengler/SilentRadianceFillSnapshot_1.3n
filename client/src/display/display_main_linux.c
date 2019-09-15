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

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

//#include "raylib.h"
#include "not_rl.h"
#include "display_main_linux.h"
/*$Revision: 1.44 $*/
#include "default.h"
#include "display_set.h"
#include "packet_summary.h"
#include "purple_string.h"
/* this is the glue from java to c for 3gl es stuff yeah */



	    
// Turn this off if you wan a debug not full screen
#ifdef LINUX_SIMULATE_APPLE
  //#define SR_LINUX_FULLSCREEN 1
#else
#  define SR_LINUX_FULLSCREEN 1
#endif

/* Graphics calls */
int lightning_flag=1;
GLFWwindow* window;

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


void new_size(GLFWwindow* window,int width,int height) {
display_set_surface_changed(width,height,screen_orientation,screen_layout,screen_drawmode);
save_width=width;
save_height=height;
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      double linux_xpos,linux_ypos;
      glfwGetCursorPos(window,&linux_xpos,&linux_ypos);
//      logit("got %lf,%lf\n",linux_xpos,linux_ypos);
      display_set_touchEvent(1,linux_xpos,linux_ypos);
      }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
      double linux_xpos,linux_ypos;
      glfwGetCursorPos(window,&linux_xpos,&linux_ypos);
//      logit("got %lf,%lf\n",linux_xpos,linux_ypos);
      display_set_touchEvent(10001,linux_xpos,linux_ypos);
      }
}







void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
if (action==GLFW_PRESS) {
  if ((key == GLFW_KEY_Q)||(key==GLFW_KEY_ESCAPE)||(key==GLFW_KEY_BACKSPACE)) {
    if (display_set_keyEvent(6)==-1) {
      exit(0);
      }
    }
  else if ((mods== GLFW_MOD_CONTROL)&&(key==GLFW_KEY_C))  {
    exit(0);
    }
  else if ((key==GLFW_KEY_ENTER)||(key==GLFW_KEY_KP_ENTER)||(key==GLFW_KEY_SPACE)||(key==GLFW_KEY_KP_5)) {
    display_set_keyEvent(1);
    }
  else if ((key==GLFW_KEY_KP_8)||(key==GLFW_KEY_W)||(key==GLFW_KEY_UP)) {
    display_set_keyEvent(2);
    }
  else if ((key==GLFW_KEY_KP_2)||(key==GLFW_KEY_S)||(key==GLFW_KEY_DOWN)) {
    display_set_keyEvent(3);
    }
  else if ((key==GLFW_KEY_KP_4)||(key==GLFW_KEY_A)||(key==GLFW_KEY_LEFT)) {
    display_set_keyEvent(4);
    }
  else if ((key==GLFW_KEY_KP_6)||(key==GLFW_KEY_D)||(key==GLFW_KEY_RIGHT)) {
    display_set_keyEvent(5);
    }
  /*??? need to implement hold key */
  }
else if (action==GLFW_RELEASE) {
  if ((key==GLFW_KEY_ENTER)||(key==GLFW_KEY_KP_ENTER)||(key==GLFW_KEY_SPACE)||(key==GLFW_KEY_KP_5)) {
    display_set_keyEvent(10001);
    }
  else if ((key==GLFW_KEY_KP_8)||(key==GLFW_KEY_W)||(key==GLFW_KEY_UP)) {
    display_set_keyEvent(10002);
    }
  else if ((key==GLFW_KEY_KP_2)||(key==GLFW_KEY_S)||(key==GLFW_KEY_DOWN)) {
    display_set_keyEvent(10003);
    }
  else if ((key==GLFW_KEY_KP_4)||(key==GLFW_KEY_A)||(key==GLFW_KEY_LEFT)) {
    display_set_keyEvent(10004);
    }
  else if ((key==GLFW_KEY_KP_6)||(key==GLFW_KEY_D)||(key==GLFW_KEY_RIGHT)) {
    display_set_keyEvent(10005);
    }
  }
}

#ifdef SR_LINUX_FULLSCREEN
    int display_is_fullscreen=1;
#else
    int display_is_fullscreen=0;
#endif

void Renderer_Init(int width, int height)
{
    int widthMM,heightMM;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    if (display_is_fullscreen) {
      GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
      glfwGetMonitorPhysicalSize(primary_monitor, &widthMM, &heightMM);
      logit("physical size %d,%d\n",widthMM,heightMM);
      const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);
      xdpi = (double)(mode->width)/((double)(widthMM)/25.4);
      ydpi = (double)(mode->height)/((double)(heightMM)/25.4);
      logit("dpi %lf,%lf\n",xdpi,ydpi);
      glfwWindowHint(GLFW_RED_BITS, mode->redBits);
      glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
      glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
      glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
      glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);  
      save_width = mode->width;
      save_height = mode->height;
//#define WORLD_Silent_Radiance "Silent Radiance"
      window = glfwCreateWindow(mode->width, mode->height, WORLD_Silent_Radiance,glfwGetPrimaryMonitor() , NULL);
      glfwMakeContextCurrent(window);
      logit("wwwwww %d hhhhh %d\n",save_width,save_height);
      if (save_width > save_height*5) {
        save_width = save_width/3;
        }
      else if (save_width > save_height*3) {
        save_width = save_width/2;
        }    
      logit("reduced to sensible wwwwww %d hhhhh %d\n",save_width,save_height);
      } // if fullscreen
    else {
      
#ifdef LINUX_SIMULATE_APPLE
      glfwWindowHint(GLFW_DECORATED, False);
# ifdef LINUX_SIMULATED_APPLE_IS_6_POINT_5_VERTICAL
     save_width= 1242;
     save_height=2688; // 6.5 inch iphone
     screen_orientation = 1; 
     screen_layout = 0; 
   # endif
   # ifdef LINUX_SIMULATED_APPLE_IS_6_POINT_5_HORIZONTAL
     save_width= 2688;
     save_height=1242; // 6.5 inch iphone
   # endif
   # ifdef LINUX_SIMULATED_APPLE_IS_12_POINT_9_GEN_3_VERTICAL
      save_width=2048;
      save_height=2732;
      screen_orientation = 1; 
      screen_layout = 0; 
   #endif
   # ifdef LINUX_SIMULATED_APPLE_IS_12_POINT_9_GEN_3_HORIZONTAL
      save_width=2732;
      save_height=2048;
   # endif
#else 
      save_width=640;
      save_height=480;

#endif
      if (make_demo_mode) {
        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS,8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
        }

      window = glfwCreateWindow(save_width,save_height, WORLD_Silent_Radiance,NULL , NULL);
      glfwMakeContextCurrent(window);
      glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);  
      xdpi=75.f;
      ydpi=75.f;
      } // if not fullscreen

// FULLSCREEN
//#define WORLD_Silent_Radiance "Silent Radiance"

    glfwSetWindowSizeCallback(window,new_size);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    logit("glgetstr %lx\n",(unsigned long int)(glGetString));
    logit("glgetstra %lx\n",(unsigned long int)(&glGetString));
    
    logit("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    logit("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );


//    init_packet_summary();
display_set_init();
//display_set_surface_changed(1920,1080,screen_orientation,screen_drawmode);
display_set_surface_changed(save_width,save_height,screen_orientation,screen_layout,screen_drawmode);
}

FILE *screenshot_file;
unsigned char *screenshot_bitmap;
unsigned char *screenshot_bitmap2;

void demo_open_screenshot_file(int w,int h) {
//#define WORLD_display_display_main_linux_a_dot_rawrgb "a.rawrgb"
//#define WORLD_display_display_main_linux_a "a"
screenshot_file = fopen(WORLD_display_display_main_linux_a_dot_rawrgb,WORLD_display_display_main_linux_a);
if (!screenshot_file) {
  logit("cannot write to a.rawrgb\n");
  exit(-1);
  }
  screenshot_bitmap = malloc(sizeof(char)*w*h*4);
  screenshot_bitmap2 = malloc(sizeof(char)*w*h*3);
}

void demo_dump_screenshot() {
if (screenshot_file) {
  long st=-1;
  long et=-1;
  
  struct timeval demo_time_of_day;
  sr_gettimeofday(&demo_time_of_day,NULL);
  if (demo_start_video_frame_time.tv_sec != -1) {
    st = time_difference_in_milliseconds(demo_time_of_day,demo_start_video_frame_time);         
    if (st<0) return;
    }
  if (demo_end_video_frame_time.tv_sec != -1) {
    et = time_difference_in_milliseconds(demo_time_of_day,demo_end_video_frame_time);  
    logit("demo %ld %ld    et %ld\n",(long)(demo_end_time.tv_sec),(long)(demo_end_time.tv_usec),
       et);
    if (et>=0) exit(0);
    }
  
  logit(".");
  glFlush();
  checkGlError("flush1\n");
//Bind the FBO
//glBindFramebufferOES(GL_FRAMEBUFFER_OES, offscreen_framebuffer);
  // set the viewport as the FBO won't be the same dimension as the screen
//  glViewport(0, 0, save_width, save_height);
//  checkGlError("vp\n");

    

	      glReadPixels(0, 0, save_width, save_height, GL_RGBA, GL_UNSIGNED_BYTE,(void *)(screenshot_bitmap));
  checkGlError("readpizel\n");	        
	      
  
  glFlush();
  checkGlError("flush\n");
  { /* convert from rgba to rgb and flip upside right */
    int i;
    for (i=0;i<save_height;i++) {
      int ii=save_height-i;
      unsigned char *lineshot_startmap = screenshot_bitmap+(i*save_width*4);
      unsigned char *lineshot_endmap = screenshot_bitmap+((i+1)*save_width*4);
      unsigned char *lineshot_startmap2 = screenshot_bitmap2+(ii*save_width*3);
//      unsigned char *lineshot_endmap2 = screenshot_bitmap2+((ii+1)*save_width*3);
      
      unsigned char *d=lineshot_startmap2;
      unsigned char *s=lineshot_startmap;

      while (s<lineshot_endmap) {
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        s++;
        }
      } // for
    }    
  fwrite(screenshot_bitmap2,save_width*save_height*3,1,screenshot_file);
  }
}


void demo_finish_screenshot() {
if (screenshot_file) {
  fclose(screenshot_file);
  screenshot_file = NULL;
  }
if (screenshot_bitmap) {
  free(screenshot_bitmap);
  screenshot_bitmap = NULL;
  }
}
  

extern int splash_frame_count;

/* The demo does a sub window - and in fact will later not even be shown.
It will call the get packets and step more often than real time, or less often,
as it has to do a screen dump each frame.
So the time computations need to be played with
200 times a second does not go into 30 times a second, so well, we will have to
do some fudging
Basically, we will fudge with the previous time, so that it looks like it is real time
might have to do stuff with the temp crap as well.
*/
void Renderer_Init_Demo(int width, int height)
{

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);    
    if (make_demo_mode) {
      //glfwWindowHint(GLFW_VISIBLE, GL_FALSE); // special to not show the window at all
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // from https://stackoverflow.com/questions/19596916/unable-to-increase-color-bit-depth-beyond-8-bits-for-glfw-opengl-window-ubuntu
      glfwWindowHint(GLFW_RED_BITS, 8);
      glfwWindowHint(GLFW_GREEN_BITS,8);
      glfwWindowHint(GLFW_BLUE_BITS, 8);
      glfwWindowHint(GLFW_ALPHA_BITS, 8);
      }
      
//#define WORLD_Silent_Radiance "Silent Radiance"
    window = glfwCreateWindow(width, height, WORLD_Silent_Radiance,NULL , NULL);
    glfwMakeContextCurrent(window);
    save_width = width;
    save_height = height;

    splash_frame_count = SPLASH_TOTAL_TIME+1; // no splash screen
    demo_open_screenshot_file(width,height);
// no resizing, keying or any callback

display_set_init();
checkGlError("display_fddffdsset_render_sdone\n");   
display_set_surface_changed(save_width,save_height,screen_orientation,screen_layout,screen_drawmode);

checkGlError("display_fddffdsset_render_done\n");   

} //Renderer_Init_Demo




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
checkGlError("start_renderer_step\n");

if (glfwWindowShouldClose(window)) {
  return(0);
  }
glfwPollEvents();
compute_packet_summary();
display_set_render();
checkGlError("display_set_render_done\n");   

glfwSwapBuffers(window);  
checkGlError("swapbuffers\n");   

return 1;
}





int Renderer_Step_Demo()
{
checkGlError("start_renderer_step_demo\n");

if (glfwWindowShouldClose(window)) {
  return(0);
  }
glfwPollEvents();
compute_packet_summary();
checkGlError("computr_[p_simm\n");   

display_set_render();
checkGlError("display_set_render_done\n");   
// snapshot


demo_dump_screenshot();
checkGlError("demo_dump_screenshot\n");   

glfwSwapBuffers(window);  
checkGlError("swapbuffers\n");   


return 1;
}



void Renderer_Finish() 
{
// ?? finish packet?
display_set_finish();
demo_finish_screenshot();
}

void Renderer_Finish_Demo() 
{
// ?? finish packet?
display_set_finish();
}



// for static projectm missing glXGetProcAddress:
//code/projectm/src/libprojectM/Renderer/SOIL2/SOIL2.c:253: undefined 
// reference to `glXGetProcAddress'
// from https://github.com/lunixbochs/glshim/blob/master/src/glx/glx.c
// didnt work

#define SIMULATE_GLX_ON_EGL
#ifdef SIMULATE_GLX_ON_EGL
void *glXGetProcAddress(GLubyte *a) {
return NULL;
}
#endif 
// SIMULATE_GLX_ON_EGL


/*
 $Log: display_main_linux.c,v $
 Revision 1.44  2019/08/07 21:24:43  hib
 wip

 Revision 1.43  2019/06/27 14:44:12  hib
 wip - we are getting better

 Revision 1.42  2019/06/07 02:45:04  hib
 wip prime

 Revision 1.41  2019/05/30 22:34:38  hib
 Cleaned up the apple emulation mode for screenshots.  Now, back to the demo stuff

 Revision 1.40  2019/05/30 21:01:47  hib
 *** empty log message ***

 Revision 1.39  2019/05/28 11:45:23  hib
 cool

 Revision 1.38  2019/05/21 04:04:52  hib
 *** empty log message ***

 Revision 1.37  2019/05/21 02:35:57  hib
 wip

 Revision 1.36  2019/05/20 12:45:47  hib
 helped idenify an error - with project m
 makde gerr which ios a better way to trach wtf

 Revision 1.35  2019/05/15 00:28:07  hib
 Got resplandor separated
 got demos separated - and yes, they compile - new frame logic for video frames untested
 Did not finish the mtv like - first make replandor for android
 and very first - add nova effect behind GG

 Revision 1.34  2019/05/03 20:43:58  hib
 Cleaned up alot and hopefully the impact font as well

 Revision 1.33  2019/04/28 05:14:53  hib
 cleaned it up

 Revision 1.32  2019/04/28 04:11:16  hib
 wip - changing display_set to separate init, glsetup, and screen changed

 Revision 1.31  2019/04/15 11:29:48  hib
 OK. NOW this is the correct version for internationalization on linux!

 Revision 1.30  2019/04/15 10:40:50  hib
 WORLD_ macros have now set up silent radiance to be internationalized.
 It is easier to implement a system where there are separate builds,
 but I could handle a weird multi-languge on one build with more trickiness.

 The world_internatiolization.h file sets it up
 Currently just english

 Revision 1.29  2019/04/15 06:09:25  hib
 internationalization wip

 Revision 1.28  2019/04/03 14:22:15  hib
 clean up logit

 Revision 1.27  2019/03/17 20:49:28  hib
 cleaning out raylib

 Revision 1.26  2019/03/17 12:58:24  hib
 Updated copyrights

 Revision 1.25  2019/02/11 05:45:11  hib
 demo mode - make font bigger for the title

 Revision 1.24  2019/01/29 15:03:03  hib
 start converting rlgl into proprietary sr. Step 1. combined rlgl.c and core.c

 Revision 1.23  2018/12/26 04:15:46  hib
 client - does demo.
 Server - does scraping for pt and ds

 Revision 1.22  2018/12/25 17:09:52  hib
 Made sound_to_file use the current to point tot he current + next

 Revision 1.21  2018/12/24 19:11:48  hib
 a little cleanup on larry harvey- the top menu or menu dont show when it is display mode.

 More work on the demo - but still doesnt produce

 Revision 1.20  2018/12/24 09:00:52  hib
 Added 666 and 721

 Revision 1.19  2018/12/22 15:39:11  hib
 Well, silentradiance still works.  the demo maker - got it to dump some files upside down.
 Switched sr_demo to run the file reader in the background, and the sound in the foreground-  switching abck and forth from sound_to_file.c and sound_alsa.c - well sound_alsa.c usually works but doesn t close the app. and the other one freezes.
 Also the video stopped being outputted.
 Need to take a break from it.

 Revision 1.18  2018/12/22 07:26:17  hib
 the demo silentradiance is not processing files for some reason.  but we are progressing further along making a video for a song.

 Revision 1.17  2018/09/18 03:25:05  hib
 I think I got the singalong locked in.  Now try on the apple.

 Revision 1.16  2018/09/18 00:19:31  hib
 Rewrote the visual voice stuff - more stable - but it doesnt work yet.
 Also, it      handle     lines      are     the        segment
          can         the lines that     on      same

 Revision 1.15  2018/02/27 04:30:21  hib
 Cleaned up the vr mode alot

 Revision 1.14  2018/02/24 21:56:38  hib
 minor again

 Revision 1.13  2018/02/24 20:43:19  hib
 minor

 Revision 1.12  2018/02/24 15:28:50  hib
 Got the VR to work initially

 Revision 1.11  2018/02/22 21:57:55  hib
 Help modularize the display visualizers much more - for display_set - need to do for the main menu too
 Also - added step/draw initial logic, to make it more likely that we can render split screen for goggles.

 Revision 1.10  2018/02/14 22:45:43  hib
 cewel

 Revision 1.9  2017/12/21 20:46:52  hib
 looking better

 Revision 1.8  2017/12/13 16:02:19  hib
 Adeded ctrl-c to quit

 Revision 1.7  2017/07/05 12:59:10  hib
 added copyleft to the client

 Revision 1.6  2017/04/06 11:16:55  hib
 minor fixes

 Revision 1.5  2017/04/06 02:25:21  hib
 ll

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
