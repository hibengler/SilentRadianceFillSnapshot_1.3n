/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler and Mathias Werowicks

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







/*$Revision: 1.13 $*/

#ifdef ANDROID
#include <jni.h>
#endif

#define THE_NUMBER 32
#define THE_BUTTON_NUMBER 12
int start_color[THE_NUMBER][THE_NUMBER]; // 0-3

int button_start_color[THE_BUTTON_NUMBER][THE_BUTTON_NUMBER];




#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glue.h"
#include "default.h"
#include "packet_summary.h"
#include "display_set.h"
#include "display_main.h"
#include "helper.h"
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

static int colorHandle;
static int mMVPMatrixHandle;

static int has_beat=0;
static int has_onset=0;
static int try_forward=1;

static void change_squares()
{
  int i,j;
  for (i=0;i<THE_NUMBER;i++) {
     for (j=0;j<THE_NUMBER;j++) {
      start_color[i][j]=(rand()&3);
      }
    }
}    


static void button_change_squares() 
{
  int i,j;
  for (i=0;i<THE_BUTTON_NUMBER;i++) {
    for (j=0;j<THE_BUTTON_NUMBER;j++) {
      button_start_color[i][j]=(rand()&3);
      }
    }
}    

int mathias1_setupGraphics(int w, int h,int graphics_type) {

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
  checkGlError("glgetruni1m");

  mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");
  checkGlError("glgetpos1");
  mMVPMatrixHandle = glGetUniformLocation(one_color_program, "u_MVPMatrix");
  checkGlError("glgetmvp");


  change_squares();

  button_change_squares();
  
  }
    return 1;
}




void mathias1_renderBackground(int screen,glMatrix *eye) {
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

void mathias1_step() {
compute_beat_and_stuff();
if (packet_summary.current_frame==-1) return;

if (has_beat) {
  change_squares();
  }
else if (has_onset) {
  int x,y;
  for (y=0;y<THE_NUMBER;y++) {  
    for (x=0;x<THE_NUMBER;x++) {  
      start_color[y][x]= (start_color[y][x]+1)&3;
      }
    }
  }
}

void mathias1_stepButton() {
compute_beat_and_stuff();
if (packet_summary.current_frame==-1) return;

if (has_beat||has_onset) {
  button_change_squares();
  }

}





static void draw_a_triangle_a(float xfrom,float yfrom,float xto,float yto,
 float x3,float y3,
  float red,float green,float blue,float alpha) {
int n=0;
float verticies[6];
glUseProgram(one_color_program);
checkGlError("glUseProgram1mt");
glLineWidth(1.f);

glUniform4f(colorHandle,red,green,blue,alpha);
checkGlError("glUniform4fwrmt");

verticies[n++]=xfrom;
verticies[n++]=yfrom;
verticies[n++]=xto;
verticies[n++]=yto;
verticies[n++]=x3;
verticies[n++]=y3;

glEnableVertexAttribArray(gvPositionHandle);
checkGlError("glEnableVertexAttribArraymt");
glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, verticies);
checkGlError("glVertexAttribPointer");

glDrawArrays(GL_TRIANGLES,0,n>>1);
checkGlError("glDrawArraysmt");


glDisableVertexAttribArray(gvPositionHandle);
checkGlError("glDrawElementsPmt");
}




static void draw_a_square_a(float x1,float y1,float x2,float y2,
  float red,float green,float blue,float alpha) {
draw_a_triangle_a(x1,y1,x2,y1,x2,y2,red,green,blue,alpha);
draw_a_triangle_a(x1,y1,x2,y2,x1,y2,red,green,blue,alpha);
}

void mathias1_renderFrame(int screen,glMatrix *eye) {    

    glUseProgram(one_color_program);
    checkGlError("glUseProgram1");
    glLineWidth(1.f);
    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&myMatrix));
    checkGlError("glmatm4p");
    
//    if (packet_summary.current_frame==-1) return;


/* draw_a_line goes from_x,from_y,   to_x, to_y,   Red,  green,  blue.
x, and y are in the range of -1.0 to 1.0
red green blue are 
in the range 0 to 1.0 */
GLfloat xx,yy;
int xi,yi;
float divide_factor = 4.0/(float)(THE_NUMBER);
for (yi=0;yi<THE_NUMBER;yi++) {
  yy = ((float)(yi)*divide_factor)-2.0;
  for (xi=0;xi<THE_NUMBER;xi++) {
    int tcolor = start_color[xi][yi];
    xx = ((float)(xi)*divide_factor)-2.0;
    float setcolor=0.3;
    float r,g,b;
    if (tcolor==0) {
      b=setcolor;
      r=0.;
      g=0.;
      }
    else if (tcolor==1) {
      b=0.;
      r=0.;
      g=setcolor;
      }
    else if (tcolor==2) {
      b=0.;
      r=setcolor;
      g=0.;
      }
    else /*if (tcolor==3)*/ {
      b=setcolor;
      r=setcolor;
      g=0.;
      }
    draw_a_square_a(xx,yy,xx+divide_factor,yy+divide_factor,r,g,b,0.2);
    tcolor = (tcolor+1)&3;
    } // xx
  } // yy


/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Here^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
}






void mathias1_renderButton(int program,int colorHandle,int menu_position_handle,glMatrix *MVPMatrix,int screen,glMatrix *eye) {
glUseProgram(one_color_program);
    glLineWidth(1.f);
    
    if (packet_summary.current_frame==-1) return;

    
    
    
    
    
//    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&MVPMatrix));
    
    

    glUseProgram(one_color_program);
    checkGlError("glUseProgram1");
    glLineWidth(1.f);
//    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&myMatrix));
//    checkGlError("glmatm4p");
    
//    if (packet_summary.current_frame==-1) return;


/* draw_a_line goes from_x,from_y,   to_x, to_y,   Red,  green,  blue.
x, and y are in the range of -1.0 to 1.0
red green blue are 
in the range 0 to 1.0 */
GLfloat xx,yy;
int xi,yi;
float divide_factor = 2.0/(float)(THE_BUTTON_NUMBER);
for (yi=0;yi<THE_BUTTON_NUMBER;yi++) {
  yy = ((float)(yi)*divide_factor)-1.0;
  for (xi=0;xi<THE_BUTTON_NUMBER;xi++) {
    int tcolor = button_start_color[yi][xi];
    xx = ((float)(xi)*divide_factor)-1.0;
    float r,g,b;
    float setcolor=0.25;
    if (tcolor==0) {
      b=setcolor;
      r=0.;
      g=0.;
      }
    else if (tcolor==1) {
      b=0.;
      r=0.;
      g=setcolor;
      }
    else if (tcolor==2) {
      b=0.;
      r=setcolor;
      g=0.;
      }
    else /*if (tcolor==3)*/ {
      b=setcolor;
      r=setcolor;
      g=0.;
      }
    draw_a_square_a(xx,yy,xx+divide_factor,yy+divide_factor,r,g,b,1.);
    tcolor = (tcolor+1)&3;
    } // xx
  } // yy




    
    glDisableVertexAttribArray(menu_position_handle);
    checkGlError("glEnableVertexAttribArray");
    
//    {
//      glUseProgram(letters_opengles_program_handle());
//      checkGlError("koutds");
//      float scale = 0.5;
//      float x=-0.9f;
//      float y=0.15f;
//      glUniform4f(letters_opengles_color_handle(), 1.0f,1.0f,0.7f,1.0f);
//      lc->needed_points->line_width=0.128f;
//      letters_out(lc,scale,x,y,0.f,"mathias1");
//      }
    
}


void mathias1_renderForeground(int screen,glMatrix *eye) {
}



void mathias1_touchEvent(int action,float x,float y) {
if (action==1) {
  push_display_set(2);
  }
}




// WORLD_INTERNATIONAL off
const display_set mathias1_display_set = {
  .name = "mathias1",
  .setupGraphics = mathias1_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .step = mathias1_step,
  .renderBackground = mathias1_renderBackground,
  .renderMain  = mathias1_renderFrame,
  .renderForeground = mathias1_renderForeground,
  .stepButton = mathias1_stepButton,
  .renderButton = mathias1_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = NULL
  };
// WORLD_INTERNATIONAL on


/*
$Log: mathias1.c,v $
Revision 1.13  2019/05/04 00:23:57  hib
wip but visual voice looks like crap

Revision 1.12  2019/05/03 16:28:10  hib
wip. the impact font is goofed up again.

Revision 1.11  2019/04/28 04:11:16  hib
wip - changing display_set to separate init, glsetup, and screen changed

Revision 1.10  2019/04/15 10:28:13  hib
This could be ready for the first transform to international

Revision 1.9  2019/03/20 15:37:44  hib

works on ios

Revision 1.8  2019/03/19 02:22:48  hib
Diamonds now have button text

Revision 1.7  2019/03/17 12:58:25  hib
Updated copyrights

Revision 1.6  2019/03/17 04:09:04  hib
Ok, now its perfect

Revision 1.5  2019/03/17 04:04:41  hib
added a little spit and shine

Revision 1.3  2019/03/17 02:53:50  hib
Got the squares - but they are rectangles because came from purple string.
Will add the more consistent stuff with the menu inheritance

Revision 1.2  2019/03/17 02:19:13  hib
Added mathias and guttes it.

Revision 1.1  2019/03/17 02:06:07  hib
Added Mathias1 for colored squares in the background



*/

