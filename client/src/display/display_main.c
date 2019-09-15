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







#include <jni.h>

#include "glue.h"
#include "display_main.h"
#include "display_set.h"
#include "purple_string.h"
#include "default.h"
#include "packet_summary.h"

/* this is the glue from java to c for 3gl es stuff yeah */

/* Graphics calls */
int lightning_flag=1;
int screen_orientation = 1; /* 1 - top */
int screen_layout = 0; /* portrait */
int screen_drawmode = 0; /* vr */
int save_width;
int save_height;



int need_to_call_surface_changed=0; /* set if we changed orientation - need to call during a step because the GLEs context
   will not be set in the thread that knows about the change, or it is unavailable during this window */
static int need_surface_width=0;
static int need_surface_height=0;
static int need_orientation=0;
   

JNIEXPORT void JNICALL Java_com_killercool_silentradiance_DisplayView_00024Renderer_Init(JNIEnv * env, jobject obj);

JNIEXPORT void JNICALL Java_com_killercool_silentradiance_DisplayView_00024Renderer_SurfaceChanged(JNIEnv * env, jobject obj,  jint width, jint height);
JNIEXPORT void JNICALL Java_com_killercool_silentradiance_DisplayView_00024Renderer_Step(JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_com_killercool_silentradiance_DisplayView_00024Renderer_Finish(JNIEnv * env, jobject obj);
JNIEXPORT void Java_com_killercool_silentradiance_VisualizingActivity_RotationChanged
  (JNIEnv *javaEnvironment, jobject self,jint rotation,jint width,jint height);

JNIEXPORT void JNICALL Java_com_killercool_silentradiance_DisplayView_00024Renderer_Init
 (JNIEnv * env, jobject obj)
{
//logit("Diusplayview_RendereerInit!!!!\n");
init_packet_summary();
display_set_init();
}

JNIEXPORT void JNICALL Java_com_killercool_silentradiance_DisplayView_00024Renderer_Finish
 (JNIEnv * env, jobject obj)
{
//logit("Diusplayview_Rendereerfinish!!!!\n");
display_set_finish();
}


JNIEXPORT void JNICALL Java_com_killercool_silentradiance_DisplayView_00024Renderer_SurfaceChanged
(JNIEnv * env, jobject obj,  jint width, jint height)
{
if ((screen_orientation==1)||(screen_orientation==3)) {
  screen_layout=0;
  }     
else {
  screen_layout=1;
  }

need_to_call_surface_changed=1; /* call it the next time we render */
need_surface_width=width;
need_surface_height=height;
need_orientation=screen_orientation;
}

JNIEXPORT void JNICALL Java_com_killercool_silentradiance_DisplayView_00024Renderer_Step(JNIEnv * env, jobject obj)
{
databaseEnvironment = env; /* because we do sqllite in the compute_packet_summary */
compute_packet_summary();

/* handle changing the surface now - where we will have an GLES context, if the orientation did change */
if (need_to_call_surface_changed) {
  need_to_call_surface_changed=0;
  if (screen_layout) {
//    logit("ds1");
    display_set_surface_changed(need_surface_width,need_surface_height,need_orientation,screen_layout,screen_drawmode);
    }   
  else {
//    logit("ds2");
    display_set_surface_changed(need_surface_width,need_surface_height,need_orientation,screen_layout,screen_drawmode);
    }
  save_width = need_surface_width;
  save_height = need_surface_height;
  }

/* render the screen */
display_set_render();
}


        


JNIEXPORT void Java_com_killercool_silentradiance_VisualizingActivity_NativeRotationChanged
  (JNIEnv *javaEnvironment, jobject self,jint rotation,jint width,jint height) {

//logit("uuu %d width height %d,%d\n",rotation,width,height);
if ((rotation==1)||(rotation==3)) {
  screen_layout=0;
  }     
else {
  screen_layout=1;
  }
need_to_call_surface_changed=1; /* call it the next time we render */
need_surface_width=width;
need_surface_height=height;
need_orientation=rotation;
}



JNIEXPORT void Java_com_killercool_silentradiance_VisualizingActivity_NativeSetRotation
  (JNIEnv *javaEnvironment, jobject self,jint rotation,jint width,jint height) {
screen_orientation = rotation;
//logit("uuu %d width height %d,%d\n",rotation,width,height);
if ((rotation==1)||(rotation==3)) {
  screen_layout=0;
  }     
else {
  screen_layout=1;
  }
}
        

void display_main_set_screen_drawmode(int newmode) {  
if (newmode) {
  if (screen_drawmode==0) {
    screen_drawmode = 1;
//    logit("ds3");
    display_set_surface_changed(save_width,save_height,screen_orientation,screen_layout,screen_drawmode);
    }
  }
else {
  if (screen_drawmode==1) {
    screen_drawmode = 0;
//    logit("ds4");
    display_set_surface_changed(save_width,save_height,screen_orientation,screen_layout,screen_drawmode);
    }  
  }  
}
    

