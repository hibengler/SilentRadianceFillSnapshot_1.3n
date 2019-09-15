/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler and Srikant Voruganti

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







/*$Revision: 1.3 $*/

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


static int mColorHandle;
 static GLuint gProgram;
static GLuint gvPositionHandle;
// WORLD_INTERNATIONAL off

static const char *colorFragmentShader = 
 "precision mediump float;       \n"     // Set the default precision to medium. We don't need as high of a
                                            // precision in the fragment shader.
   "varying vec4 v_Color;          \n"     // This is the color from the vertex shader interpolated across the
                                            // triangle per fragment.
   "void main()                    \n"     // The entry point for our fragment shader.
   "{                              \n"
   "   gl_FragColor = v_Color;     \n"     // Pass the color directly through the pipeline.
   "}                              \n";

static char *colorVertexShader =
"attribute vec4 vPosition;\n"
"uniform vec4 a_Color;\n"
	"varying vec4 v_Color;\n"
        "void main() {\n"
	"  v_Color = a_Color; \n"
        "  gl_Position = vPosition;\n"
        "}\n";

// WORLD_INTERNATIONAL on

static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
//                    logit("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}



int srikant1_setupGraphics(int w, int h,int graphics_type) {
if (ran_setup_graphics_once_before) return(1);

      
      /*
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

//    logit("setupGraphics(%d, %d)", w, h);
    */
    gProgram = createProgram(colorVertexShader, colorFragmentShader);
    if (!gProgram) {
//        logit("Could not create program.");
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


static int to_frame;
static int from_frame;
static int try_forward;
static int number_of_frames;

static GLfloat verticies[482];


void srikant1_step() {
if (packet_summary.current_frame==-1) return;
try_forward=4;
to_frame = packet_summary.current_frame+try_forward;
if (to_frame >packet_summary.end_frame) to_frame=packet_summary.end_frame;
    
from_frame=to_frame-9; 
if (from_frame<packet_summary.no_gap_frame) from_frame=packet_summary.no_gap_frame;

number_of_frames = to_frame-from_frame+1;
if (number_of_frames>10) {
//        logit("number of frames too big: %d",number_of_frames);
  number_of_frames=0;
  from_frame=(to_frame+1);
  }

  

}

void srikant1_renderMain(int screen,glMatrix *eye) {    

if (packet_summary.current_frame==-1) return;
if (!number_of_frames)  return;

glUseProgram(gProgram);
checkGlError("glUseProgram1");
glLineWidth(1.f);
    

    
glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, verticies);
checkGlError("glVertexAttribPointer");

float factor = (3.14159*2.0 )/ ((float)number_of_frames);
float start=0.f;
int flag=0;

float x=0.f;  // last x and y values
float y=0.f;  // used to continue from where left off
int f;
for (f=from_frame;f<=to_frame;f++) {
  int index = (packet_summary.start_index + f - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
  if (index<0) index += PACKET_SUMMARY_SIZE;
  struct packet *p = packet_summary.packets+index;
    
  float pos = ((float)(f-from_frame)) / ((float)(to_frame+1-from_frame));
  /* position from 0 to 1 */
  pos = pos*0.60 + 0.30;

  /* now from .30 to .9 */
  GLfloat rgb[4];
  hslToRgb(pos,1.0,0.66,rgb);
  rgb[3]=1.0;
  glUniform4f(mColorHandle,rgb[0],rgb[1],rgb[2],1.0f);
  checkGlError("glUniform4fss");
	
  if (p->has_music_samples) {
    int i;
    int n=0;
    if (flag) {
      verticies[n++]=x;
      verticies[n++]=y;
      }
    flag=1;
    float factor2 = factor*0.0041666666666;   /* factor/240*/
    float angle=start;
    for (i=0;i<240;i++) {
      float thecos;
      float thesin;
      float xo;
      float yo;
      xo=p->sample[i+i]*0.0000305175781250000f; /* /32768*/
      yo=p->sample[i+i+1]*0.0000305175781250000f; /* /32768*/
	  
      /* rotate around to make the liesegu pattern look better */
      thecos = hcos(angle);
      thesin = hsin(angle);
      
      x = verticies[n++]= xo*thecos - yo*thesin;
      y = verticies[n++]= yo*thecos + xo*thesin;
      angle += factor2;
      }
    glEnableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray");
    
    glDrawArrays(GL_LINE_STRIP,0,(n)>>1);  // this is the draw command
    checkGlError("glDrawArrays");
    
    glDisableVertexAttribArray(gvPositionHandle);
    checkGlError("glDrawElementsP");
    }
  else 
    flag=0;
  start += factor;
  } /* for each frame we want to draw */
  
glDisableVertexAttribArray(gvPositionHandle);
checkGlError("glEnableVertexAttribArray");    
    

}


void srikant1_stepButton(void) {}


void srikant1_renderButton(int program,int mColorHandle,int menu_position_handle,glMatrix *MVPMatrix,int screen,glMatrix *eye) {
glUseProgram(program);
glLineWidth(1.f);
    
if (packet_summary.current_frame==-1) return;

int try_forward=4;
int to_frame = packet_summary.current_frame+try_forward;
if (to_frame >packet_summary.end_frame) to_frame=packet_summary.end_frame;
    
int from_frame=to_frame-9; 
if (from_frame<packet_summary.no_gap_frame) from_frame=packet_summary.no_gap_frame;
    
int f;
GLfloat verticies[723];
    
glEnableVertexAttribArray(menu_position_handle);
checkGlError("glEnableVertexAttribArray(men");
    
    
int n=0;
glVertexAttribPointer(menu_position_handle, 3, GL_FLOAT, GL_FALSE, 0, verticies);
checkGlError("glVertexAttribPointer");
    
    
//    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&MVPMatrix));
    
    
    
    

int number_of_frames = to_frame-from_frame+1;
if (number_of_frames>10) {
    number_of_frames=0;
    from_frame=to_frame+1;
}

float factor = (3.14159*2.0 )/ ((float)number_of_frames);
float start=0.f;
int flag=0;

float x=0.f;  // last x and y values
float y=0.f;  // used to continue from where left off

for (f=from_frame;f<=to_frame;f++) {
  int index = (packet_summary.start_index + f - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
  if (index<0) index += PACKET_SUMMARY_SIZE;
  struct packet *p = packet_summary.packets+index;
    
  float pos = ((float)(f-from_frame)) / ((float)(to_frame+1-from_frame));
  /* position from 0 to 1 */
  pos = pos*0.60 + 0.30;

  /* now from .30 to .9 */
  GLfloat rgb[4];
  hslToRgb(pos,1.0,0.66,rgb);
  rgb[3]=1.0;
  glUniform4f(mColorHandle,rgb[0],rgb[1],rgb[2],1.0f);
  checkGlError("glUniform4fss");
	
  if (p->has_music_samples) {
    int i;
    int n=0;
    if (flag) {
      verticies[n++]=x;
      verticies[n++]=y;
      verticies[n++]=0.f;
      }
    flag=1;
    float factor2 = factor*0.033333333333333;   /* factor/30 */
    float angle=start;
    for (i=0;i<30;i++) {
      float thecos;
      float thesin;
      float xo;
      float yo;
      xo=p->sample[i*16]*0.0000305175781250000f; /* /32768*/
      yo=p->sample[i*16+1]*0.0000305175781250000f; /* /32768*/
	  
      /* rotate around to make the liesegu pattern look better */
      thecos = hcos(angle);
      thesin = hsin(angle);
      
      x = verticies[n++]= xo*thecos - yo*thesin;
      y = verticies[n++]= yo*thecos + xo*thesin;
      verticies[n++]=0.; // z
      angle += factor2;
      }
    glEnableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray");
    
    glDrawArrays(GL_LINE_STRIP,0,(n)/3);  // this is the draw command
    checkGlError("glDrawArrays");
    
    glDisableVertexAttribArray(gvPositionHandle);
    checkGlError("glDrawElementsP");
    }
  else 
    flag=0;
  start += factor;
  } /* for each frame we want to draw */
  
  
  checkGlError("glEnableVertexAttribArray");
    {
      glUseProgram(letters_opengles_program_handle());
      checkGlError("koutds");
      float scale = 0.5;
      float x=-0.9f;
      float y=0.15f;
      glUniform4f(letters_opengles_color_handle(), 1.0f,1.0f,0.7f,1.0f);
      lc->needed_points->line_width=0.128f;
      letters_out(lc,scale,x,y,0.f,"Srikant1");
      }

}











const display_set srikant1_display_set = {
  .name = "Srikant1",
  .setupGraphics = srikant1_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .step = srikant1_step,
  .renderBackground = NULL,
  .renderMain  = srikant1_renderMain,
  .renderForeground = NULL,
  .stepButton = srikant1_stepButton,
  .renderButton = srikant1_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = NULL
  };


/*
$Log: srikant1.c,v $
Revision 1.3  2019/06/30 23:34:54  hib
Addes heavy metal mode, finally.

Revision 1.2  2019/06/30 22:36:20  hib
Cleaned up purple_string by taking out its sign and using hsin from helper
Also amke srikant1.c as an example for our new contrubutor.

Revision 1.1  2019/06/30 21:01:46  hib
copy from purple_graphics for srikant


*/

