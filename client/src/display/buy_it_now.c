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
#include "buy_it_now.h"


#ifdef ANDROID
#include <jni.h>
#endif





#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glue.h"
#include "helper.h"
#include "packet_summary.h"
#include "display_set.h"
#include "display_main.h"
#include "default.h"
#include "store.h"

#include "main_menu.h"

#include "letters_render_opengles2.h"
  
  static glMatrix projectionMatrix;
  static glMatrix myMatrix;
  static GLuint one_color_program;
  static GLuint full_color_program;
  static GLuint gvPositionHandle;
  static GLuint mPositionHandle ;
  
  static float width_in_pixels;
  static float height_in_pixels;
  static float width_in_inches;
  static float height_in_inches;
  static float height_to_inches;
  static float rotation_in_radians;
  static float relative_width;
  static float relative_height;
  static float width_to_inches;
  static int circle_factor;
  static lcontext *lc;
  
  
static int has_beat=0;
static int has_onset=0;
static int try_forward=1;
  

extern int station_has_free_songs; 


int buy_it_now_setupGraphics(int w, int h,int graphics_mode) {

    pass_menu_setup_info(
        &width_in_pixels,
        &height_in_pixels,
        &width_in_inches,
        &height_in_inches,
        &height_to_inches,
        &rotation_in_radians,
        &relative_width,
        &relative_height,
        &width_to_inches,
        &circle_factor,
        &one_color_program,   
        &full_color_program, 
        &gvPositionHandle,
        &projectionMatrix,
        &mPositionHandle
 );

myMatrix = origin_matrix;
//myMatrix = projectionMatrix;
// was projection_matrix
  
#ifdef __APPLE__
      if (screen_orientation==2) {
//        rotationMatrix(&myMatrix,90.f,0.f,0.f,1.f);
        }
      else if (screen_orientation==3) {
//        rotationMatrix(&myMatrix,180.f,0.f,0.f,1.f);
        }
      else if (screen_orientation==4) {
//        rotationMatrix(&myMatrix,270.f,0.f,0.f,1.f);
        } 
#endif
    
if (!ran_setup_graphics_once_before) {
       
  // letters init
  lc = linit_context(); // we will get our own letter context

  // vvv this is done in main_menu.c
  //letters_opengles_setgraphics(one_color_program,colorHandle,gvPositionHandle$
      
  glUseProgram(one_color_program);
  
       


  }
    return 1;
}



void buy_it_now(void) {
/* to be defined */
//#define WORLD_display_buy_it_now_buynow "buynow"
glue_show_url_and_keep_playing(compute_store_munged_url(WORLD_display_buy_it_now_buynow));
}
void buy_it_now_renderBackground(int screen,glMatrix *additionalProjection) {}
void buy_it_now_renderFrame(int screen,glMatrix *additionalProjection) {}
void buy_it_now_renderForeground(int screen,glMatrix *additionalProjection) {}

void buy_it_now_touchEvent(int action,float x,float y) {
}



static void compute_beat_and_stuff() {
has_beat=0;
has_onset=0;
try_forward=1;
int to_frame = packet_summary.current_frame+try_forward;
if (to_frame >packet_summary.end_frame) to_frame=packet_summary.end_frame;
int from_frame=to_frame-7;
if (from_frame<packet_summary.no_gap_frame) from_frame=packet_summary.no_gap_frame;
int f;
for (f=from_frame;f<to_frame;f++) {
            int index = (packet_summary.start_index + f -
                     packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
            if (index<0) index += PACKET_SUMMARY_SIZE;
            struct packet *p; 
            p = packet_summary.packets+index;
            if (p->has_beat) {
                has_beat=1;
                }
            if (p->has_onset) {
                has_onset=1;
                }
            }
 
}
  
static int bnum=0;
void buy_it_now_renderButton(int program,int mColorHandle,int menu_position_handle,
  glMatrix *MVPMatrix,int screen,glMatrix *additionalProjection) {
//    glLineWidth(1.f);
    if (packet_summary.current_frame==-1) return;
  
    
    {
//#define WORLD_display_buy_it_now_Buy "Buy"
      char *buy_or_get = WORLD_display_buy_it_now_Buy;
      if (station_has_free_songs) {
//#define WORLD_display_buy_it_now_Get "Get"
        buy_or_get = WORLD_display_buy_it_now_Get;
	}
      glUseProgram(letters_opengles_program_handle());
      checkGlError("koutds");
      float scale = WORLD_display_buy_it_now_Buy_scale;  //0.6
      float x= WORLD_display_buy_it_now_Buy_x; //-0.5f;
      float y= WORLD_display_buy_it_now_Buy_y; //0.13f;
      if (has_beat&&(bnum==0)) {
        lc->needed_points->line_width=WORLD_display_buy_it_now_beat_width;
checkGlError("ws1");
        if (!station_has_free_songs) {
          glUniform4f(letters_opengles_color_handle(), 1.f,1.0f,1.f,1.0f);
//#define WORLD_display_buy_it_now__dlr__dlr__dlr_ "$$$"
          letters_out(lc,1.f,WORLD_display_buy_it_now__dlr__dlr__dlr_x,
	    WORLD_display_buy_it_now__dlr__dlr__dlr_y,
	    0.f,WORLD_display_buy_it_now__dlr__dlr__dlr_);
	  }
        glUniform4f(letters_opengles_color_handle(), 0.8f,0.0f,0.7f,1.0f);
        letters_out(lc,scale,x,y,0.f,buy_or_get);
//#define WORLD_display_buy_it_now_Now "Now"
        letters_out(lc,WORLD_display_buy_it_now_Buy_scale
	  ,WORLD_display_buy_it_now_Now_x,
	  WORLD_display_buy_it_now_Now_y,0.f,WORLD_display_buy_it_now_Now);
	}
      else if (has_onset) {
        lc->needed_points->line_width=WORLD_display_buy_it_now_onset_width;
        glUniform4f(letters_opengles_color_handle(), 1.f,0.4f,0.6f,1.0f);
checkGlError("ws2");
      letters_out(lc,scale,x,y,0.f,buy_or_get);
//#define WORLD_display_buy_it_now_Now "Now"
        letters_out(lc,WORLD_display_buy_it_now_Buy_scale
	  ,WORLD_display_buy_it_now_Now_x,
	  WORLD_display_buy_it_now_Now_y,0.f,WORLD_display_buy_it_now_Now);
        }
      else {
      
        lc->needed_points->line_width=WORLD_display_buy_it_now_normal_width;
        glUniform4f(letters_opengles_color_handle(), 0.5f,1.0f,0.5f,1.0f);
checkGlError("ws3");
      letters_out(lc,scale,x,y,0.f,buy_or_get);
//#define WORLD_display_buy_it_now_Now "Now"
        letters_out(lc,WORLD_display_buy_it_now_Buy_scale
	  ,WORLD_display_buy_it_now_Now_x,
	  WORLD_display_buy_it_now_Now_y,0.f,WORLD_display_buy_it_now_Now);
        }
      }  
}


void buy_it_now_step(void) {}
void buy_it_now_stepButton(void) {
compute_beat_and_stuff();
if (has_beat) bnum = (bnum+1)&15;
}
void buy_it_now_finishStreaming() {}





// WORLD_INTERNATIONAL off
const display_set buy_it_now_display_set = {
  .name = "buy_it_now",
  .setupGraphics = buy_it_now_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .step = buy_it_now_step,
  .renderBackground = buy_it_now_renderBackground,
  .renderMain  = buy_it_now_renderFrame,
  .renderForeground = buy_it_now_renderForeground,
  .stepButton = buy_it_now_stepButton,
  .renderButton = buy_it_now_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = buy_it_now_finishStreaming,
  .clickedOnMenuItemEvent = buy_it_now
  };
// WORLD_INTERNATIONAL on



