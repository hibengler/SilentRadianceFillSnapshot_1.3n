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

#include "window_shop.h"
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
  

void window_shop() {
/* to be defined */
//#define WORLD_windowshop "windowshop"
glue_show_url_and_stop_playing(compute_store_munged_url(WORLD_windowshop));
}







int window_shop_setupGraphics(int w, int h,int graphics_mode) {

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
   gerr("sdssdaa");  
    
if (!ran_setup_graphics_once_before) {
       
  // letters init
  lc = linit_context(); // we will get our own letter context

  // vvv this is done in main_menu.c
  //letters_opengles_setgraphics(one_color_program,colorHandle,gvPositionHandle$
      
  glUseProgram(one_color_program);
   gerr("sdssda");  
       


  }
   gerr("sdssda");  
    return 1;
}

extern int station_has_free_songs;

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
  

void window_shop_step(void) {}
void window_shop_renderBackground(int screen,glMatrix *additionalProjection) {}
void window_shop_renderFrame(int screen,glMatrix *additionalProjection) {}
void window_shop_renderForeground(int screen,glMatrix *additionalProjection) {}

void window_shop_touchEvent(int action,float x,float y) {
}

void window_shop_renderButton(int program,int mColorHandle,int menu_position_handle,
  glMatrix *MVPMatrix,int screen,glMatrix *additionalProjection) {
  
//    glLineWidth(1.f);
    if (packet_summary.current_frame==-1) return;
  
    
    {
      glUseProgram(letters_opengles_program_handle());
      gerr("koutds");
      float scale = WORLD_display_window_shop_Window_scale; // 0.5
      float x=WORLD_display_window_shop_Window_x;  //-0.9f;
      float y=WORLD_display_window_shop_Window_y;  //-0.05f;
      if (has_beat) {
        lc->needed_points->line_width=0.140f;
        glUniform4f(letters_opengles_color_handle(), 0.8f,1.0f,0.8f,1.0f);
gerr("ws1");
	}
      else if (has_onset) {
        lc->needed_points->line_width=0.130f;
        glUniform4f(letters_opengles_color_handle(), 0.6f,1.0f,0.6f,1.0f);
gerr("ws2");
        }
      else {
      
        lc->needed_points->line_width=0.128f;
        glUniform4f(letters_opengles_color_handle(), 0.5f,1.0f,0.5f,1.0f);
gerr("ws3");
        }
      if (station_has_free_songs==1) {
//#define WORLD_display_window_shop_Window "Window"
        letters_out(lc,scale,x,y,0.f,WORLD_display_window_shop_Window);
//#define WORLD_display_window_shop_Show "Show"
        letters_out(lc,WORLD_display_window_shop_Show_scale,
	  WORLD_display_window_shop_Show_x,
	  WORLD_display_window_shop_Show_y,0.f,WORLD_display_window_shop_Show);
        }
      if (station_has_free_songs==2) {
//#define WORLD_display_window_shop_Window "Window"
        letters_out(lc,scale,x,y,0.f,WORLD_display_window_shop_Window);
//#define WORLD_display_window_shop_Show "Show"
		//#define WORLD_display_window_shop_Show "Show"
		        letters_out(lc,WORLD_display_window_shop_Show_scale,
			          WORLD_display_window_shop_Show_x,
				  WORLD_display_window_shop_Show_y,
				  0.f,WORLD_display_window_shop_Show);
	}
      else {
//#define WORLD_display_window_shop_Window "Window"
        letters_out(lc,scale,x,y,0.f,WORLD_display_window_shop_Window);
//#define WORLD_display_window_shop__dlr_hop "$hop"
        letters_out(lc,scale,WORLD_display_window_shop__dlr_hop_x,
	  WORLD_display_window_shop__dlr_hop_y,0.f,
	  WORLD_display_window_shop__dlr_hop);
	}
      }  
        
  
}

  
      
void window_shop_stepButton(void) {
compute_beat_and_stuff();
}

void window_shop_finishStreaming() {}





// WORLD_INTERNATIONAL off

const display_set window_shop_display_set = {
  .name = "window_shop",
  .setupGraphics = window_shop_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .step = window_shop_step,
  .renderBackground = window_shop_renderBackground,
  .renderMain  = window_shop_renderFrame,
  .renderForeground = window_shop_renderForeground,
  .stepButton = window_shop_stepButton,
  .renderButton = window_shop_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = window_shop_finishStreaming,
  .clickedOnMenuItemEvent = window_shop
  };
// WORLD_INTERNATIONAL on

