/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler and Luis Santiago Casanova

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







/*$Revision: 1.10 $*/

#ifdef ANDROID
#include <jni.h>
#endif





#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glue.h"
#include "default.h"
#include "packet_summary.h"
#include "display_set.h"
#include "display_main.h"
#include "helper.h"
#include "letters_render_opengles2.h"


static lcontext *lc;

/* These are used to do cheap rotation of x,y around a circle once every packet.  This will make the liesegu look better */
static float cheap_cos_240[240];
static float cheap_sin_240[240];
static float cheap_cos_480[480];
static float cheap_sin_480[480];
static float cheap_cos_720[720];
static float cheap_sin_720[720];
static float cheap_cos_960[960];
static float cheap_sin_960[960];
static float cheap_cos_1200[1200];
static float cheap_sin_1200[1200];
static float cheap_cos_1440[1440];
static float cheap_sin_1440[1440];
static float cheap_cos_1680[1680];
static float cheap_sin_1680[1680];
static float cheap_cos_1920[1920];
static float cheap_sin_1920[1920];
static float cheap_cos_2160[2160];
static float cheap_sin_2160[2160];
static float cheap_cos_2400[2400];
static float cheap_sin_2400[2400];
static float *the_sines[11] = {cheap_sin_240,cheap_sin_240,cheap_sin_480,cheap_sin_720,
                        cheap_sin_960,cheap_sin_1200,cheap_sin_1440,cheap_sin_1680,cheap_sin_1920,
    cheap_sin_2160,cheap_sin_2400};
static float *the_cosines[11] = {cheap_cos_240,cheap_cos_240,cheap_cos_480,cheap_cos_720,
                        cheap_cos_960,cheap_cos_1200,cheap_cos_1440,cheap_cos_1680,cheap_cos_1920,
    cheap_cos_2160,cheap_cos_2400};




static int mColorHandle;
 



static GLuint gProgram;
static GLuint gvPositionHandle;



int luis1_setupGraphics(int w, int h,int graphics_type) {
if (ran_setup_graphics_once_before) return(1);
    { int k;
        /* comput the cheap sin and cosign based on the position in the packet */
      int i;
        for (k=1;k<11;k++) {
            float factor =6.28318530718/(240. * ((float)k));
                                          int length = 240*k;
                                          float *sines = the_sines[k];
                                          float *cosines = the_cosines[k];
      for (i=0;i<length;i++) {
        cosines[i] = cosf(factor*((float)i));
          sines[i] = sinf(factor*((float)i));
	     }
      } /* for k */
                                          } /* compute sines and cosines */
      
      /*
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    logit("setupGraphics(%d, %d)", w, h);
    */
    gProgram = sr_create_program(gStandard2dVertexOneColorShader,gStandard2dFragmentOneColorShader);
    if (!gProgram) {
        logit("Could not create program.");
        return 0;
    }
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    checkGlError("glGetAttribLocation");



    mColorHandle =     glGetUniformLocation(gProgram, "a_Color");
    checkGlError("glGetUniformLocation");
    
    
    
    
    // letters init
    lc = linit_context(); // we will get our own letter context
    
    // vvv this is done in main_menu.c
    //letters_opengles_setgraphics(one_color_program,colorHandle,gvPositionHandle,mPositionHandle); // needs to be set up once, with a single color program
    

    return 1;
}




void luis1_renderBackground(int screen,glMatrix *eye) {
}

void luis1_step() {
if (packet_summary.current_frame==-1) return;

}




static void draw_a_line(float xfrom,float yfrom,float xto,float yto,float red,float green,float blue) {
int n=0;
float verticies[4];
glUseProgram(gProgram);
checkGlError("glUseProgram1L");
glLineWidth(1.f);

glUniform4f(mColorHandle,red,green,blue,1.0f);
checkGlError("glUniform4fwrL");

verticies[n++]=xfrom;
verticies[n++]=yfrom;
verticies[n++]=xto;
verticies[n++]=yto;

glEnableVertexAttribArray(gvPositionHandle);
checkGlError("glEnableVertexAttribArrayL");
glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, verticies);
checkGlError("glVertexAttribPointer");

glDrawArrays(GL_LINE_STRIP,0,n>>1);
checkGlError("glDrawArraysL");


glDisableVertexAttribArray(gvPositionHandle);
checkGlError("glDrawElementsPL");
}



static void draw_a_triangle(float xfrom,float yfrom,float xto,float yto,
 float x3,float y3,
  float red,float green,float blue) {
int n=0;
float verticies[6];
glUseProgram(gProgram);
checkGlError("glUseProgram1L");
glLineWidth(1.f);

glUniform4f(mColorHandle,red,green,blue,1.0f);
checkGlError("glUniform4fwrL");

verticies[n++]=xfrom;
verticies[n++]=yfrom;
verticies[n++]=xto;
verticies[n++]=yto;
verticies[n++]=x3;
verticies[n++]=y3;

glEnableVertexAttribArray(gvPositionHandle);
checkGlError("glEnableVertexAttribArrayL");
glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, verticies);
checkGlError("glVertexAttribPointer");

glDrawArrays(GL_TRIANGLES,0,n>>1);
checkGlError("glDrawArraysL");


glDisableVertexAttribArray(gvPositionHandle);
checkGlError("glDrawElementsPL");
}



void luis1_renderFrame(int screen,glMatrix *eye) {    

    glUseProgram(gProgram);
    checkGlError("glUseProgram1");
    glLineWidth(1.f);
    
    if (packet_summary.current_frame==-1) return;

    int try_forward=4;
    int to_frame = packet_summary.current_frame+try_forward;
    if (to_frame >packet_summary.end_frame) to_frame=packet_summary.end_frame;
    
    int from_frame=to_frame-9; 
    if (from_frame<packet_summary.no_gap_frame) from_frame=packet_summary.no_gap_frame;
    
    int f;
    int flag=0;
    GLfloat x,y;
    GLfloat verticies[482];
 	   x=0.f;y=0.f;
    #ifdef hfdjfskhs
    /* find the pitches to estimate colors */
    float pitches[12];
    {
      int n=0;
      float pitch=-1.;
      for (f=from_frame;f<to_frame;f++) {
        int index = (packet_summary.start_index + f - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
        if (index<0) index += PACKET_SUMMARY_SIZE;
        struct packet *p = packet_summary.packets+index;
        if (p->has_statistics) {
          pitches[n]=p->pitch;
  	  }
        else {
          pitches[n]=pitch;
	  }
        pitch=pitches[n];
        n++;
        }
      int i;
      pitch=-1.;
      for (i=n;i>=0;i--) {
        if (pitches[i]==-1.) pitches[i]=pitch;
	    pitch=pitches[i];
	}
      for (i=n;i>=0;i--) {
        if (pitches[i]<=0.) pitches[i]=100.;
	}
      /* normalize the pitches */
      for (i=n;i>=0;i--) {
        while (pitches[i]>512.) pitches[i]  = pitches[i]*0.5;
        while (pitches[i]>=256.) pitches[i]  = pitches[i]*2.0;
	}
      /* pitches are now in the range of 256 to 511 - subtract 255 and we can use the pitch colors to set the color */

      /* todo: use pitch to variate color with a cool shader or something like that */ 
      }
   #endif

    int n=0;
    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, verticies);
    checkGlError("glVertexAttribPointer");
    float *sine;
    float *cosine;
    float *sines;
    float *cosines;
    int number_of_frames = to_frame-from_frame+1;
    if (number_of_frames>10) {
        logit("number of frames too big: %d",number_of_frames);
        number_of_frames=0;
        from_frame=to_frame+1;
    }

    sines = the_sines[number_of_frames];
    cosines = the_cosines[number_of_frames];


    for (f=from_frame;f<=to_frame;f++) {
      int index = (packet_summary.start_index + f - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
      if (index<0) index += PACKET_SUMMARY_SIZE;
      struct packet *p = packet_summary.packets+index;
        sine = sines+240*(f-from_frame);
        cosine=cosines+240*(f-from_frame);
	 
      float pos = ((float)(f-from_frame)) / ((float)(to_frame+1-from_frame));
      /* position from 0 to 1 */
      pos = pos*0.12 + 0.00;
      /* now from .0 to .5 */
      GLfloat rgb[4];
      hslToRgb(pos,1.0,0.5,rgb);
      rgb[3]=1.0;
      glUniform4f(mColorHandle,rgb[0],rgb[1],rgb[2],1.0f);
      checkGlError("glUniform4fss");
	
      int i;
      if (p->has_music_samples) {
        n=0;
        if (flag) {
	      verticies[n++]=x;
	      verticies[n++]=y;
	      }
	flag=1;
	for (i=0;i<240;i++) {
	  float xo;
	  float yo;
	  xo=p->sample[i+i]*0.0000305175781250000f; /* /32768*/
	  yo=p->sample[i+i+1]*0.0000305175781250000f; /* /32768*/
	  
	  /* rotate around to make the liesegu pattern look better */
        float thecos = cosine[i];
        float thesin = sine[i];
	  x = xo*thecos - yo*thesin;
	  y = yo*thecos + xo*thesin;
	  verticies[n++]=x;
	  verticies[n++]=y;
	  }
        glEnableVertexAttribArray(gvPositionHandle);
        checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_LINE_STRIP,0,n>>1);
        checkGlError("glDrawArrays");
	glDisableVertexAttribArray(gvPositionHandle);
        checkGlError("glDrawElementsP");
	}
      else flag=0;
      } /* for each frame we want to draw */
    glDisableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray");
    

//    checkGlError("glVertexAttribPointer");
//    glEnableVertexAttribArray(gvPositionHandle);
//    checkGlError("glEnableVertexAttribArray");
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    checkGlError("glDrawArrays");
//    glDisableVertexAttribArray(gvPositionHandle);



// celica
/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvHerevvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/    

/* draw_a_line goes from_x,from_y,   to_x, to_y,   Red,  green,  blue.
x, and y are in the range of -1.0 to 1.0
red green blue are 
in the range 0 to 1.0 */

 //draw_a_line(-1.,-1,1.,1.,    1.,1.,1.);
 draw_a_line (-0.2,-0.2,0.43,0.43,0.,0.5,1.);
 /* red square is actually 2 red triangles
the flag is 1.8 instead of 400, so 1.8/5 is .. 0.36
*/
draw_a_triangle(-0.9, -0.9,   0.9,-0.9,  0.9,-0.9+0.36, 
                 1., 0.,0.); 
draw_a_triangle(-0.9, -0.9,     0.9,-0.9+0.36,   -0.9,-0.9+0.36,
                 1., 0.,0.);



draw_a_triangle(-0.9, -0.9, -0.5, 0., -0.9, 0.9, 0., 0.,1.); 
 //                 x1  y1    x2   y2   x3   y3   r   g  b
 
//*drrect(0, 0, 399, 400);
//triangle(0, 0, 0, 400, 200, 200);
//line(70, 0, 0, 30);

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Here^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
}






void luis1_renderButton(int program,int mColorHandle,int menu_position_handle,glMatrix *MVPMatrix,int screen,glMatrix *eye) {
glUseProgram(program);
    glLineWidth(1.f);
    
    if (packet_summary.current_frame==-1) return;

    int try_forward=4;

    int to_frame = packet_summary.current_frame+try_forward;
    if (to_frame >packet_summary.end_frame) to_frame=packet_summary.end_frame;
    
    int from_frame=to_frame-9; 
    if (from_frame<packet_summary.no_gap_frame) from_frame=packet_summary.no_gap_frame;
    
    int f;
    int flag=0;
    GLfloat x,y;
    GLfloat verticies[723];
    x=0.f;y=0.f;
    
    glEnableVertexAttribArray(menu_position_handle);
    checkGlError("glEnableVertexAttribArray(men");
    
    
    int n=0;
    glVertexAttribPointer(menu_position_handle, 3, GL_FLOAT, GL_FALSE, 0, verticies);
    checkGlError("glVertexAttribPointer");
    
    
//    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&MVPMatrix));
    
    
    
    
    float *sine;
    float *cosine;
    float *sines;
    float *cosines;
    int number_of_frames = to_frame-from_frame+1;
    if (number_of_frames>10) {
        number_of_frames=0;
        from_frame=to_frame+1;
    }

    sines = the_sines[number_of_frames];
    cosines = the_cosines[number_of_frames];


    for (f=from_frame;f<=to_frame;f++) {
      int index = (packet_summary.start_index + f - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
      if (index<0) index += PACKET_SUMMARY_SIZE;
      struct packet *p = packet_summary.packets+index;
        sine = sines+240*(f-from_frame);
        cosine=cosines+240*(f-from_frame);
	 
      float pos = ((float)(f-from_frame)) / ((float)(to_frame+1-from_frame));
      /* position from 0 to 1 */
      pos = pos*0.12 + 0.00;
      /* now from .0 to .5 */
      GLfloat rgb[4];
      hslToRgb(pos,1.0,0.5,rgb);
      rgb[3]=1.0;
      glUniform4f(mColorHandle,rgb[0],rgb[1],rgb[2],1.0f);
      checkGlError("glUniform4fwr");
	
      int i;
      if (p->has_music_samples) {
        n=0;
        if (flag) {
	      verticies[n++]=x;
	      verticies[n++]=y;
	      verticies[n++]=0.f;
	      }
	flag=1;

	for (i=0;i<30;i++) {
	  float xo;
	  float yo;
	  xo=p->sample[i*16]*0.0000305175781250000f; /* /32768*/
	  yo=p->sample[i*16+1]*0.0000305175781250000f; /* /32768*/
	  
	  /* rotate around to make the liesegu pattern look better */
        float thecos = cosine[i*8];
        float thesin = sine[i*8];
	  x = xo*thecos - yo*thesin;
	  y = yo*thecos + xo*thesin;
	  
	  /* clip it */
	  if (x>1.f) x=1.f;
	  if (x<-1.f) x=-1.f;
	  if (y>1.f) y=1.f;
	  if (y<-1.f) y=-1.f;
	  
	  verticies[n++]=x;
	  verticies[n++]=y;
	  verticies[n++]=0.; // z
	  }
	glDrawArrays(GL_LINE_STRIP,0,n/3);
        checkGlError("glDrawArrays");
	}
      else flag=0;

      } /* for each frame we want to draw */
    glDisableVertexAttribArray(menu_position_handle);
    checkGlError("glEnableVertexAttribArray");
    
    {
      glUseProgram(letters_opengles_program_handle());
      checkGlError("koutds");
      float scale = 0.5;
      float x=-0.9f;
      float y=0.15f;
      glUniform4f(letters_opengles_color_handle(), 1.0f,1.0f,0.7f,1.0f);
      lc->needed_points->line_width=0.128f;
//#define WORLD_display_luis1_luis1 "luis1"
      letters_out(lc,scale,x,y,0.f,WORLD_display_luis1_luis1);
      }
    
}


void luis1_renderForeground(int screen,glMatrix *eye) {
}



void luis1_touchEvent(int action,float x,float y) {
if (action==1) {
  push_display_set(2);
  }
}




// WORLD_INTERNATIONAL off
const display_set luis1_display_set = {
  .name = "luis1",
  .setupGraphics = luis1_setupGraphics,
  .init = NULL,
  .setupGL = NULL,
  .changeScreen = NULL,
  .step = luis1_step,
  .renderBackground = luis1_renderBackground,
  .renderMain  = luis1_renderFrame,
  .renderForeground = luis1_renderForeground,
  .renderButton = luis1_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = NULL
  };
// WORLD_INTERNATIONAL on


/*
$Log: luis1.c,v $
Revision 1.10  2019/04/28 04:11:16  hib
wip - changing display_set to separate init, glsetup, and screen changed

Revision 1.9  2019/04/15 11:29:48  hib
OK. NOW this is the correct version for internationalization on linux!

Revision 1.8  2019/04/15 10:40:50  hib
WORLD_ macros have now set up silent radiance to be internationalized.
It is easier to implement a system where there are separate builds,
but I could handle a weird multi-languge on one build with more trickiness.

The world_internatiolization.h file sets it up
Currently just english

Revision 1.7  2019/04/15 08:13:56  hib
cleaning international

Revision 1.6  2019/03/17 12:58:25  hib
Updated copyrights

Revision 1.5  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

Revision 1.4  2019/03/01 00:01:04  hib
helper cleanup and initial setup of vr stuff

Revision 1.3  2019/01/18 21:53:51  casanova
started the flag

Revision 1.2  2018/11/18 02:10:05  casanova
put my name on it

Revision 1.1  2018/11/17 22:29:46  casanova
start out

Revision 1.2  2018/09/29 11:31:07  hib
Set up ivonne1 and luis1

Revision 1.1  2018/09/29 08:48:58  hib
luis1 and ivonne1

Revision 1.1  2018/09/29 08:12:42  hib
luis1 made from purple_string

*/

