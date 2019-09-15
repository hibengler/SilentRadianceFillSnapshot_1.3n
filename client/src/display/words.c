/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Library Public License   
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Library Public License for more details.

You should have received a copy of the GNU Library Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


/* button says disco info for this one */





/*$Revision: 1.25 $*/

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
#include "main_menu.h"
#include "default.h"

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

static int colorHandle;
static int mMVPMatrixHandle;

int words_setupGraphics(int w, int h,int graphics_type) {
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
  //letters_opengles_setgraphics(one_color_program,colorHandle,gvPositionHandle,mPositionHandle); // needs to be set up once, with a single color program
      
  glUseProgram(one_color_program);
    
  colorHandle = glGetUniformLocation(one_color_program, "v_Color");
  checkGlError("glgetruni1w");

  mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");
  checkGlError("glgetpos1");
  mMVPMatrixHandle = glGetUniformLocation(one_color_program, "u_MVPMatrix");
  checkGlError("glgetmvp");
  
  return 1;
  }
    return 1;
}




void words_renderBackground(int screen,glMatrix *eye) {
}

static void double_letters_out(lcontext *lc,fl scale,fl x,fl y,fl z,char *string) {
//#ifndef ANDROID
//glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);
//lc->needed_points->line_width=0.128f;
//letters_out(lc,scale,x,y,z,string);
//lc->needed_points->line_width=0.35f;
//glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
//letters_out(lc,scale,x,y,z-0.001f,string);
//#else 
lc->needed_points->line_width=0.35f;
glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
letters_out(lc,scale,x,y,z-0.001f,string);
glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);
lc->needed_points->line_width=0.128f;
letters_out(lc,scale,x,y,z,string);
//#endif

}




static float words_scale = 1.f;
static float words_x = 0.f;
static float words_y = 0.f;


void words_step() {

}

void words_finishStreming() {
current_display_title_text[0]='\0';
}

void words_renderFrame(int screen,glMatrix *eye) {    
if (packet_summary.current_frame==-1) return;

glUseProgram(one_color_program);
checkGlError("glup44");

// was enable - needed to be there?
glDisable(GL_DEPTH_TEST);
//  glLineWidth(1.0f);
    

// Apply the projection and view transformation
glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&myMatrix));
checkGlError("glmatm4p");
	    
    if ((change_display_title_text)&&(new_display_title_text[0])) {
//      logit("new text display it");
      strcpy(current_display_title_text,new_display_title_text);
      lc->max_boundary.lower_left.y = -10000000.f;
      letters_size(lc,1.f,0.f,0.f,0.f,current_display_title_text);
      float bigscalex = lc->vertical_string_boundary.upper_right.x-lc->vertical_string_boundary.lower_left.x;
      float bigscaley = lc->vertical_string_boundary.upper_right.y-lc->vertical_string_boundary.lower_left.y;
      float bigscale=30.f;
      if (bigscalex>bigscale) {bigscale=bigscalex;}
      if (bigscaley>bigscale) {bigscale=bigscaley;}
      bigscale *= 0.5;
      if (bigscale!= 0.f) {
        words_scale = 1.f/bigscale;
	}
      else words_scale = 1.f;
      if (make_demo_mode) {
        words_scale *=1.6;
	}
//      logit("scale %f %f big %f 1/ %f",bigscalex,bigscaley,bigscale,words_scale);
      words_x = -lc->vertical_string_boundary.lower_left.x * words_scale;
      words_y = -lc->vertical_string_boundary.lower_left.y * words_scale;
      
      
//      logit("range from %f,%f to %f,%f\n",lc->vertical_string_boundary.lower_left.x,lc->vertical_string_boundary.lower_left.y,
//             lc->vertical_string_boundary.upper_right.x,lc->vertical_string_boundary.upper_right.y);	    
      
      change_display_title_text=0.f;
      }
    if (current_display_title_text[0]) {
      float x,y,scale;
#ifdef __APPLE__      
 #define REL_FACTORW relative_height
#else
 #define REL_FACTORW relative_width
 //??? probably a big bug
#endif
  
#define WW 1.f
      if (screen_layout==0) { /* portrait */
	scale = words_scale * WW;
        x= -1. + 0.25 * scale;
	y= -relative_height + 0.35 * scale;
	if (last_draw_mode==1) {
	  y +=  0.20 * scale;
	  }
	}
      else {
	scale = words_scale * WW;
        x= -REL_FACTORW + 0.25 * scale;
	y= -1. + 0.35 * scale;
	}

      /* bloack shadow */
      glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);
      checkGlError("kout2");
      double_letters_out(lc,scale,x+(words_x*WW),y+(words_y*WW),0.f,current_display_title_text);
      checkGlError("kout1");
      }
      
    
    glLineWidth(1.f);
    
}




static int button_state=0;
static int button_timer=10;

void words_stepButton() {
button_timer = (button_timer+1)%12;
if (button_timer==0) {
  button_state = (button_state+1)%3;
  }
}


void words_renderButton(int program,int mColorHandle,int menu_position_handle,glMatrix *MVPMatrix,int screen,glMatrix *eye) {
glUseProgram(one_color_program);
      checkGlError("kout1ssd");
if (button_state>0) {
  glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);
  lc->needed_points->line_width=WORLD_display_words_Info_line_width;
//#define WORLD_display_words_Disco "Disco"
  letters_out(lc,WORLD_display_words_Disco_scale,
    WORLD_display_words_Disco_x,
    WORLD_display_words_Disco_y,
    0.f,WORLD_display_words_Disco);
  }
if (button_state>1) {
  glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);
  lc->needed_points->line_width=WORLD_display_words_Info_line_width;
//#define WORLD_display_words_Info "Info"
  letters_out(lc,WORLD_display_words_Info_scale,
    WORLD_display_words_Info_x,
    WORLD_display_words_Info_y,0.f,WORLD_display_words_Info);
  }  
}


void words_renderForeground() {
}



void words_touchEvent(int action,float x,float y) {
if (action==1) {
  push_display_set(2);
  }
}

// WORLD_INTERNATIONAL off
const display_set words_display_set = {
  .name = "words",
  .setupGraphics = words_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen=NULL,
  .step = words_step,
  .renderBackground = words_renderBackground,
  .renderMain  = words_renderFrame,
  .renderForeground = words_renderForeground,
  .stepButton = words_stepButton,
  .renderButton = words_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = words_finishStreming
  };
// WORLD_INTERNATIONAL on

/*
$Log: words.c,v $
Revision 1.25  2019/05/04 00:23:57  hib
wip but visual voice looks like crap

Revision 1.24  2019/05/03 16:28:10  hib
wip. the impact font is goofed up again.

Revision 1.23  2019/04/28 05:14:53  hib
cleaned it up

Revision 1.22  2019/04/28 04:15:05  hib
bobo on words - now has the stuff

Revision 1.21  2019/04/15 18:01:11  hib
OK this is linux silentradiance and resplendor version 1.3 official.

Revision 1.20  2019/04/15 11:29:48  hib
OK. NOW this is the correct version for internationalization on linux!

Revision 1.19  2019/04/15 10:40:50  hib
WORLD_ macros have now set up silent radiance to be internationalized.
It is easier to implement a system where there are separate builds,
but I could handle a weird multi-languge on one build with more trickiness.

The world_internatiolization.h file sets it up
Currently just english

Revision 1.18  2019/04/15 08:13:56  hib
cleaning international

Revision 1.17  2019/03/17 12:58:25  hib
Updated copyrights

Revision 1.16  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

Revision 1.15  2019/02/11 05:45:11  hib
demo mode - make font bigger for the title

Revision 1.14  2019/01/29 15:03:03  hib
start converting rlgl into proprietary sr. Step 1. combined rlgl.c and core.c

Revision 1.13  2018/09/28 17:46:46  hib
Version 1.2 - after apple. Linting warnings and errors.
Also - trying to fix perspective of larryharvey in android

Revision 1.12  2018/02/24 21:56:38  hib
minor again

Revision 1.11  2018/02/24 15:28:50  hib
Got the VR to work initially

Revision 1.10  2018/02/22 21:57:55  hib
Help modularize the display visualizers much more - for display_set - need to do for the main menu too
Also - added step/draw initial logic, to make it more likely that we can render split screen for goggles.

Revision 1.9  2018/01/21 01:44:28  hib
menus are better but not perfect for apple.CVS: ----------------------------------------------------------------------

Revision 1.8  2018/01/20 22:10:07  hib
added 2018

Revision 1.7  2018/01/18 07:37:51  hib
got the display_worm to show up

Revision 1.6  2018/01/16 06:28:13  hib
Got the Silent Radiance word splash to show up pretty well.
Got the words better than before.
fixed baloon head in main menu I think.

Revision 1.5  2018/01/16 02:32:21  hib
Got words working on linux and every where else - glenable(depth test was the key

Revision 1.4  2017/12/30 19:58:58  hib
the opt_menu does not turn around correctly on pple,

But - the logo is real nice

Revision 1.3  2017/12/30 16:58:19  hib
OK - now to the apple for real

Revision 1.2  2017/12/30 16:26:44  hib
Ready to test on iphone

Revision 1.1  2017/12/30 04:29:49  hib
better


*/
