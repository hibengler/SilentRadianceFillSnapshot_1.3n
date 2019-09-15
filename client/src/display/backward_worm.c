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





/*$Revision: 1.46 $*/

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define  LOG_TAG    "default"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#endif


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "glue.h"
#include "packet_summary.h"
#include "display_main.h"
#include "display_set.h"
#include "helper.h"
#include "default.h"

#define ANGLES_TO_DO 2439
#define ANGLES_TO_DO_OVER_2 1219
#define FRAMES_TO_SHOW 300
#define FRAMES_TO_SHOWX3 900
#define FRAMES_TO_SHOWX4 1200



extern int heavy_metal_flag;

static float freqs[480];

//static float colors[FRAMES_TO_SHOWX4];
static int frames[FRAMES_TO_SHOW];
static int queue_start;
static int queue_end;
static int right_phases[FRAMES_TO_SHOW];
static int left_phases[FRAMES_TO_SHOW];
static float z_offset=0.f;
static float lr_points[FRAMES_TO_SHOW][1440];
static float z_finish[FRAMES_TO_SHOW];
static float angle;

static glMatrix modelMatrix;
static glMatrix effectMatrix; /* used to do effects like zoom near/zoom away */
static glMatrix viewMatrix;
static glMatrix perspectiveMatrix;
static glMatrix MVMatrix;
static glMatrix MVPMatrix;
static GLuint mMVPMatrixHandle;

//static int colorHandle;
static int abciHandle;   // a is prev freq, b is current freq, c is next freq

static float cheap_cos[ANGLES_TO_DO];
static float cheap_sin[ANGLES_TO_DO];

 
static float pixel_width=1.f; /* increase for bigger screens */


#define BUTTON_RINGS 70
#define BUTTON_POINTS 67
#define BUTTON_POINTS_X3 201
static float buttonGradient[BUTTON_RINGS];
static float button_colors[BUTTON_RINGS];
static float button_spirals[BUTTON_RINGS][BUTTON_POINTS_X3];
static float button_angle=0.f;

static char gVertexShaderFullColor[2000];


// WORLD_INTERNATIONAL off

static char * gFragmentShaderFullColor =
"precision mediump float;\n"
	"varying vec4 v_Color;\n"
        "void main() {\n"
        "  gl_FragColor = v_Color;\n"
        "}\n";




static GLuint createFullColorProgram() {
    sprintf(gVertexShaderFullColor,"uniform mat4 u_MVPMatrix;      \n"		// A constant representing the combined model/view/projection matrix.
		   "uniform vec4 v_abci;\n"   // orev current and next freq and intensity	
		   "attribute vec4 vPosition;     \n"		// Per-vertex position information we will pass in.
		   "attribute float aGradient;     \n"		// gradient from 0 to 1
		   "varying vec4 v_Color;\n"
		   "void main()                    \n"		// The entry point for our vertex shader.
		   "{                              \n"
		   "float nGradient = aGradient;\n"           // from 0 -> .5 -> 1 to 0 -> 1, 1-> 2 for the three differetn points a,b,c
		   "int i1 = int(floor(nGradient));\n"            // note that they can be the same alot too.  i1 is the index will be 0, or 1, never 2
		   "i1 = (i1>1)?1:i1;\n"
		   "nGradient = nGradient-floor(nGradient);\n"    // now Gradient is 0-1 - linear between the two colors , either color a and b, or b and c
		   "float H = v_abci[i1] + ((v_abci[i1+1]-v_abci[i1]) * nGradient);\n" // So H is in the range something to something else, it could go up
		                                                  // or down octaves, we normalize a to be zero I guess we shouldn't do that because the value
								  // can go negative that way -- I will fix that. so h is 4.5 to 6.9 or something
		   "H = H - floor(H);\n"			  // now h is [0,1)
		   "H = H * 6. ;\n     "         // now we split hue into the 6 possibilities [0,59999999)
		   "float V=1.;"
		   "float fract = H - floor(H);\n"
		   "float S = v_abci[3];\n"
		   "float P = V*(1. - S); float Q= V*(1. - S*fract);\n"
		   "float T = V*(1. - S*(1. - fract));\n"
		   "v_Color = vec4(  (H<3.)?(     \n"
		  "                           (H<2.)? (H<1.?V:Q) :P)    \n"
		"			   :(    \n"
		"			     (H<5.?(H<4.?P:T):V))    \n"
		"		   , (H<3.)?(     \n"
		"                             (H<2.)? (H<1.?T:V) :V)    \n"
		"			   :(    \n"
		"			     (H<5.?(H<4.?Q:P) :P))    \n"
		"		   , (H<3.)?(     \n"
		"                             (H<2.)? P :T)    \n"
		"			   :(    \n"
		"			     (H<5.)? V :Q) ,1.);    \n"
//		   "   v_Color = vec4(H,H,H,1.);\n"   /// for debugging
		   "   gl_PointSize=%3.2f;           \n"   // this goofed up on ios 12.9 inch so added this.
		   "   gl_Position = u_MVPMatrix   \n" 	// gl_Position is a special variable used to store the final position.
		   "                * vPosition;   \n"     // Multiply the vertex by the matrix to get the final point in 			                                            			 
		   "}                              \n"    // normalized screen coordinates.
                   ,pixel_width);
// WORLD_INTERNATIONAL on

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, gVertexShaderFullColor);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, gFragmentShaderFullColor);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glBindAttribLocation(program, 0, "a_Position");
        glBindAttribLocation(program, 1, "aGradient");

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



/* borrowed from http://stackoverflow.com/questions/12943164/replacement-for-gluperspective-with-glfrustrum */
static void perspectiveGL( glMatrix *projection,GLfloat fovY, GLfloat aspect, GLfloat zNear, GLfloat zFar )
{
    const GLfloat pi = 3.1415926535897932384626433832795;
    GLfloat fW, fH;

    //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
    fH = tan( fovY / 360.f * pi ) * zNear;
    fW = fH * aspect;

    //glFrustum( -fW, fW, -fH, fH, zNear, zFar );
    frustumMatrix(projection,-fW, fW, -fH, fH, zNear, zFar );
}

static GLuint full_color_program;
static GLuint gvPositionHandle;
static GLuint aGradientHandle;

static float width_in_pixels;
static float height_in_pixels;
static float width_in_inches;
static float height_in_inches;
static float height_to_inches;
static float rotation_in_radians;
static float relative_width;
static float relative_height;
static float width_to_inches;



static float fixedGradient[240];


int backward_worm_setupGraphics(int w, int h,int screen_type) {
/* same code as in main menu */      
    /* adjust projection for scrren dpi */
    width_in_pixels=w;
    height_in_pixels=h;
    
    
    
    if ((width_in_pixels)>=2560||(height_in_pixels>=2560)) {
      pixel_width=1.79;
      }
    else if ((width_in_pixels)>=1920||(height_in_pixels>=1920)) {
      pixel_width=1.3;
      }
    else if ((width_in_pixels)>=1280||(height_in_pixels>=1280)) {
      pixel_width=1.;
      }
    else if ((width_in_pixels)>=800||(height_in_pixels>=800)) {
      pixel_width=1.;
      }
    else {
      pixel_width=1.;
      }
    
    
    width_in_inches = ((float)w)/xdpi; if (width_in_inches <=0.0f) width_in_inches = 1.0f;
    height_in_inches = ((float)h)/ydpi; if (height_in_inches <=0.0f) height_in_inches = 1.0f;
    
//    logit("w %d h %d xdpi %f ydpi %f width %f height %f\n",w,h,xdpi,ydpi,width_in_inches,height_in_inches);
    /* adjust to be portrait */
    rotation_in_radians = 0.;
    if (width_in_inches > height_in_inches) {
      float t;
      t=width_in_inches;
      width_in_inches = height_in_inches;
      height_in_inches = t;
      t = xdpi;
      xdpi = ydpi;
      ydpi = t;
//      rotation_in_radians = 1.57079632679; /* pi/2 */
//      rotationMatrix(&projectionMatrix,90.f,0.f,0.f,1.f);
      }
    /* width is to be from -1 to +1 */
    relative_width = 1.0;
    relative_height = height_in_inches / width_in_inches;
    width_to_inches = 1.0/width_in_inches;
    height_to_inches = 1.0/height_in_inches;
    
    
    
    loadIdentity(&perspectiveMatrix);  
    perspectiveGL(&perspectiveMatrix,45.f, ((GLfloat)w)/((GLfloat)h),  0.1f, 15000.f);
    
    if (ran_setup_graphics_once_before)  return(1);   
    
//    scaleMatrix(&projectionMatrix,1.0,width_in_inches/height_in_inches,1.0);
    
//    loadIdentity(&projectionMatrix);  


    loadIdentity(&modelMatrix);  
    loadIdentity(&effectMatrix);  
    loadIdentity(&viewMatrix);  
    
    full_color_program = createFullColorProgram();
    if (!full_color_program) {
//        logit("Could not create one color program.");
        return 0;
    }
    gvPositionHandle = glGetAttribLocation(full_color_program, "vPosition");
    checkGlError("glGetAttribLocation");
    aGradientHandle = glGetAttribLocation(full_color_program, "aGradient");
    checkGlError("glGetgradLocation");

    // get handle to fragment shader's v_Color member
    abciHandle = glGetUniformLocation(full_color_program, "v_abci");
    checkGlError("glGetUniformLocation");
    
    mMVPMatrixHandle = glGetUniformLocation(full_color_program, "u_MVPMatrix");
    checkGlError("glGetUniformLocation_bw");

    { 
        /* compute the cheap sin and cosign based on the position in the packet */
      int i;
            float factor =6.28318530718/(((float)(ANGLES_TO_DO)));
            int length = ANGLES_TO_DO;
                                          float *sines = cheap_sin;
                                          float *cosines = cheap_cos;
      for (i=0;i<length;i++) {
        cosines[i] = cosf(factor*((float)i));
          sines[i] = sinf(factor*((float)i));
	     }
    } /* compute sines and cosines */
    
    { /* compute the fixed gradient */
      int i;
      for (i=0;i<240;i++) {
        if (i<60) {
          fixedGradient[i] = (float)(i)/60.; /* 0 to 1 in the first ramp */
	  }
	else if (i<180) {
          fixedGradient[i] = (float)(1.); /* index 1 */
	  }
	else {
          fixedGradient[i] = (float)(i-180)/60. +1.; /* index 1 */
	  }
	  
        }
      }	
    queue_start = 0;
    queue_end = 0;
    z_offset=0.f;
    angle=0.f;
    
    
/* setup buttons */
{
  int i;
  int xangle=0;
  for (i=0;i<BUTTON_RINGS;i++) {
    button_colors[i]=(float)(rand()&255)/256.;
    
    int j;
    float *spiral=button_spirals[i];
    for (j=0;j<BUTTON_POINTS;j++) {
      float d;
      float x,y,z;
      d=0.14f + ((float)i)/((float)(BUTTON_RINGS-1))*1.35f;
      if (j&1) {
        d = d+1.25;
	}
      x = cheap_cos[xangle]*d;
      y = cheap_sin[xangle]*d;
      z= 0.f;
//      logit("i %d j %d d %f angle %d x %f y %f",i,j,d,xangle,x,y);
      int j3 = j+j+j;
      spiral[j3++] = x;
      spiral[j3++] = y;
      spiral[j3++] = z;
      xangle = (xangle + ANGLES_TO_DO/BUTTON_POINTS) % ANGLES_TO_DO;
      }
    xangle = (xangle + ANGLES_TO_DO/(BUTTON_POINTS*BUTTON_RINGS)) % ANGLES_TO_DO;
    }
    
    { /* compute the fixed button gradient */
      int i;
      for (i=0;i<BUTTON_POINTS;i++) {
        if (i<(BUTTON_POINTS/4)) {
          buttonGradient[i] = (float)(i*4)/(float)(BUTTON_POINTS); /* 0 to 1 in the first ramp */
	  }
	else if (i<(BUTTON_POINTS*3)/4) {
          buttonGradient[i] = (float)(1.); /* index 1 */
	  }
	else {
          buttonGradient[i] = (float)(i*4-(BUTTON_POINTS*3))/(float)(BUTTON_POINTS) +1.; /* index 1 */
	  }
	  
        }
      }	

  }    /* setup buttons */
    
    return 1;
}


void backward_worm_renderBackground() {
    glClearColor(0.0f, 0.0f, 0.f, 1.0f);
    checkGlError("glClearColor");

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
}




static double slow_color=1.2f;

static int did_2big_reset=0;

static void compute_frames() {
int i;
int q;
int right_phase;
int left_phase;
slow_color = slow_color * 1.03138236435686;
if (slow_color >= 2.) {
  slow_color = slow_color * 0.5;
  }
  
if (queue_start==queue_end) {
  q = queue_start;
  i = packet_summary.no_gap_frame;
  left_phase=0;
  right_phase=ANGLES_TO_DO_OVER_2;
  z_offset=0.f;
  }
else {
  int qm1 = queue_end-1;
  if (qm1<0) qm1 += FRAMES_TO_SHOW;
  i = frames[qm1]+1;
  if (i<packet_summary.no_gap_frame) {
    q = queue_start;
    i = packet_summary.no_gap_frame;
    left_phase=0;
    right_phase=ANGLES_TO_DO_OVER_2;
    z_offset=0.f;    
    }
  else if ((i>packet_summary.no_gap_frame + 20000)&&(did_2big_reset==0)) {
    did_2big_reset=1;
    q = queue_start;
    i = packet_summary.no_gap_frame;
    left_phase=0;
    right_phase=ANGLES_TO_DO_OVER_2;
    z_offset=0.f;    
    }
  else {
    did_2big_reset=0;
    q = queue_end;
    left_phase = left_phases[qm1]+240;
    if (left_phase>=ANGLES_TO_DO) left_phase -=ANGLES_TO_DO;
    right_phase = right_phases[qm1]+240;
    if (right_phase >=ANGLES_TO_DO) right_phase -= ANGLES_TO_DO;
    z_offset = z_finish[qm1];
    if (z_offset>300.) { // was 400
      q = queue_start;
      i = packet_summary.no_gap_frame;
      left_phase=0;
      right_phase=ANGLES_TO_DO_OVER_2;
      z_offset=0.f;    
      }
    }
  }
if (last_draw_mode==1) { /* separate left and right eye from left and right sound */
   right_phase=left_phase;
   }

int try_forward=4;
int to_frame = packet_summary.current_frame+try_forward;
if (to_frame >packet_summary.end_frame) to_frame=packet_summary.end_frame;
	

for (;i<to_frame;i++) {
  frames[q]=i;
  int index = (packet_summary.start_index + i - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
  if (index<0) index += PACKET_SUMMARY_SIZE;
  struct packet *p = packet_summary.packets+index;
  
  if (left_phase>=ANGLES_TO_DO) left_phase -=ANGLES_TO_DO;
  if (right_phase >=ANGLES_TO_DO) right_phase -= ANGLES_TO_DO;
  
//  logit("q %d	i %d	ind %d	end %d	h %d",q,i,index,packet_summary.end_frame,p->has_statistics);
  if (p->has_statistics) {

    float pitch=p->pitch;
    if (pitch<1.) {
      goto carry;
      }
//    float plog = logf(pitch)*1.44269504088884957607f - 0.23521271117f;  /* first constant is 1/log(2) because ln(x)/ln(2) = log2(x) */
                                                                        /* second constant makes 440 hertz the basis instead of 256 hz */
    float plog = logf(pitch)*1.44269504088884957607f;  /* first constant is 1/log(2) because ln(x)/ln(2) = log2(x) */
//    int plogi = (int) plog;
//    pitch = plog- (float)plogi;
    if (heavy_metal_flag) {
      int ilog;
      ilog = plog;
      plog = plog - ((float)ilog); /* get rid of octaves */
      plog = plog * 0.25f;
      plog += 4.f;
      }

    freqs[q]=plog;
/*
Old way:
    while (pitch>440.) pitch  = pitch*0.125;
    while (pitch<=220.) pitch  = pitch*2.0;
pitches are now in the range of 256 to 511 - subtract 255 and we can use the pitch colors to set the color 
*/
    }
   else {



carry: 
     if (q==queue_start) {
       freqs[q]=77.; /* indicates grey */
       }
     else {
       int qm1=q-1;
       if (qm1<0) qm1 += FRAMES_TO_SHOW;
       freqs[q]=freqs[qm1];
       }
    }
     
  { /* block to compute everyting */
    left_phases[q]=left_phase;
    right_phases[q]=right_phase;
    
    int j;
    for (j=0;j<240;j++) {
      float m;
      int jj=j+j+j;
      m = p->sample[j+j]*0.0000305175781250000f; /* /32768*/
      m= (1.05f+m)/2.05f;
      float *pt = lr_points[q];
      float x,y;
      x = m * cheap_cos[left_phase];
      y = m * cheap_sin[left_phase];
      z_offset += 0.00004166666666666666f; /* 1/38400 */
      
      pt[jj++] = x;
      pt[jj++] = y;
      pt[jj++] = z_offset;
      
      freqs[j+240] = freqs[j];
      
      int kk = jj-3+720;
      m = p->sample[j+j+1]*0.0000305175781250000f; /* /32768*/
      m= (1.05f+m)/2.05f;
      x = m * cheap_cos[right_phase];
      y = m * cheap_sin[right_phase];
      z_offset += 0.00004166666666666666f; /* 1/38400 */
      
      pt[kk++] = x;
      pt[kk++] = y;
      pt[kk++] = z_offset+0.01;
      
      
      left_phase= (left_phase+1) % ANGLES_TO_DO;
      right_phase= (right_phase+1) % ANGLES_TO_DO;
      
      }
    }
  z_finish[q]=z_offset;
  q = q + 1;
  if (q >= FRAMES_TO_SHOW) q=q-FRAMES_TO_SHOW;
  queue_end=q;
  if (queue_end==queue_start) {
    queue_start = queue_start + 1;
    if (queue_start >= FRAMES_TO_SHOW) queue_start = queue_start - FRAMES_TO_SHOW;
    }
  }
}



static float direction=1.f;


static void draw_screen(glMatrix *modelMat,GLfloat parameter,glMatrix *projectionAlter,int screen) { 
   GLfloat now_eye[3] = {0.f,0.f,-400.f};
   GLfloat now_center[3] = {0.f,0.f,0.f};
   GLfloat now_up[3] =   {0.f,1.4f,-400.f};

	
//   now_eye[2]=-400.f+z_offset;
//   now_center[2] = +z_offset;
//   now_up[2]=-400.f +z_offset;
  ViewLookAt(&viewMatrix,now_eye,now_center,now_up);
  
  glLineWidth(pixel_width);
  

//  perspectiveGL(&perspectiveMatrix,45.f, ((GLfloat)width_in_pixels)/((GLfloat)height_in_pixels),  0.001f, 15000.f);
  glMatrix *xmatrix=modelMat;
//    scaleMatrix(modelMat,0.5f,0.5f,0.5f);
  translateMatrix(xmatrix,0.f,0.f,-z_offset+6.f);
  rotationMatrix(xmatrix,angle,0.f,0.f,1.f);
  multMatrix(&MVMatrix,xmatrix,&viewMatrix);
  multMatrix(&MVPMatrix,&MVMatrix,&perspectiveMatrix);
  if (projectionAlter) {
    glMatrix alterMatrix;
    multMatrix(&alterMatrix,&MVPMatrix,projectionAlter);
    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&alterMatrix));
    }
  else {
    // Apply the projection and view transformation
    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&MVPMatrix));
    }
  checkGlError("glUniformMatrix4fvb");
  
    glDisable(GL_DEPTH_TEST);
  

int q=queue_start;  

int c=0;

float prev_freq = freqs[q];
while (q!=queue_end) { 
  float abci[4];
  //int offset;
  
  
  int nextq = q+1;
  if (nextq>=FRAMES_TO_SHOW) nextq -= FRAMES_TO_SHOW;
  if (nextq==queue_end) nextq=q;
    

  float intensity = 1.;
  if ((frames[q]-packet_summary.current_frame<=2)&&(frames[q]-packet_summary.current_frame>=-2)) {
    intensity = 0.45;
    }
  
  abci[0] = prev_freq;
  abci[1] = freqs[q];
  abci[2] = freqs[nextq];
  int yy;
  for (yy=0;yy<3;yy++) {
    if (abci[yy]==77.) {
      abci[yy] = slow_color;
      }
    }
      
  
  abci[0]= (abci[0] + abci[1])*0.5;
  abci[2] = (abci[2] + abci[1])*0.5;
  abci[3] = intensity;    
  prev_freq = abci[1];
  
  
  glUniform4f(abciHandle,abci[0],abci[1],abci[2],abci[3]);
  checkGlError("glUniform4fh1");

  glVertexAttribPointer(aGradientHandle,1,GL_FLOAT,GL_FALSE,0,fixedGradient);
  checkGlError("glVertexAttribPointergradient");
  glEnableVertexAttribArray(aGradientHandle);
  checkGlError("glEnableVertexAttribArray sds");
  
  if (last_draw_mode==1) { /* separate left and right eye from left and right sound */
    if (screen==1) {
      glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, lr_points[q]+720);
      checkGlError("glVertexAttribPointer");
    } else {
      glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, lr_points[q]);
      checkGlError("glVertexAttribPointer");
      }
    glEnableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray aa");
  
    glDrawArrays(GL_POINTS,0,240);
    checkGlError("glDrawArrays");
    glDisableVertexAttribArray(gvPositionHandle);
    checkGlError("glDisableVertexArray");
    }
  else {
    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, lr_points[q]);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray aa");
  
    glDrawArrays(GL_POINTS,0,480);
    checkGlError("glDrawArrays");
    glDisableVertexAttribArray(gvPositionHandle);
    checkGlError("glDisableVertexArray");
    }

  
  glDisableVertexAttribArray(aGradientHandle);
  checkGlError("glDisableVertexArraygrsfshdnd");
  glDisableVertexAttribArray(aGradientHandle);
  checkGlError("glDisableVertexArraygrsfshdnd");

  
  q=q+1;
  if (q>=FRAMES_TO_SHOW) q -= FRAMES_TO_SHOW;
  c=c+1;  
  } /* for each frame we want to draw */

  glEnable(GL_DEPTH_TEST);
glLineWidth(pixel_width);
}









void backward_worm_step() {
compute_frames();


    int has_beat=0;
    int has_onset=0;
    {
        int try_forward=4;
        int to_frame = packet_summary.current_frame+try_forward;
        if (to_frame >packet_summary.end_frame) to_frame=packet_summary.end_frame;
        int from_frame=to_frame-8; 
        if (from_frame<packet_summary.no_gap_frame) from_frame=packet_summary.no_gap_frame;
	
	int f;
	for (f=from_frame;f<to_frame;f++) {	
            int index = (packet_summary.start_index + to_frame - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
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


if (has_beat) {
  direction = direction*-1.f;
  }
if (has_onset) {
  angle = angle+1.5f*direction;
  }
else {
  angle = angle+0.5f*direction;
  }

if (angle>360.f) angle=angle-360.f;
}



void backward_worm_renderFrame(int screen,glMatrix *projectionAlter) {    
glUseProgram(full_color_program);
checkGlError("glUseProgram");
glEnable(GL_DEPTH_TEST);
checkGlError("depth");
    
loadIdentity(&modelMatrix);

draw_screen(&modelMatrix,7.f,projectionAlter,screen);
checkGlError("screen");
glDisable(GL_DEPTH_TEST);
checkGlError("disdepth");

}



void backward_worm_renderForeground() {
}



static float max(float a, float b){
        if(a>=b) return a;
        else return b;
}

static float min(float a, float b){
        if(a<b) return a;
        else return b;
}

static GLfloat upper_left_test[3] = {-1.0f,1.0f,0.0f};
static GLfloat upper_right_test[3] = {1.0f,1.0f,0.0f};
static GLfloat lower_left_test[3] = {-1.0f,-1.0f,0.0f};
static GLfloat lower_right_test[3] = {1.0f,-1.0f,0.0f};




void backward_worm_stepButton(void) {
/* move colors down */
int i;
float prev = (float)(rand()&255)/256.;
for (i=0;i<BUTTON_RINGS;i++) {
  float t=button_colors[i];
  button_colors[i]= prev;
  prev=t;
  }
button_angle += 0.666;
if (button_angle>360.f) button_angle -=360.f;


}

void backward_worm_renderButton(int program,int colorHandle,int menu_position_handle,glMatrix *view,int screen,glMatrix *eye) {
GLint id;
glGetIntegerv(GL_CURRENT_PROGRAM,&id);
glUseProgram(full_color_program);
checkGlError("glUseProgram");
    
glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(view));
  checkGlError("glUniformMatrix4fvbf");
glEnable(GL_DEPTH_TEST);

/* we will set the viewport to be smaller */
GLfloat upper_left[3];
GLfloat upper_right[3];
GLfloat lower_left[3];
GLfloat lower_right[3];
GLfloat top[3];
GLfloat bottom[3];
transformVector(upper_left,upper_left_test,view);
transformVector(upper_right,upper_right_test,view);
transformVector(lower_left,lower_left_test,view);
transformVector(lower_right,lower_right_test,view);


GLuint topx,topy,bottomx,bottomy;
if (last_draw_mode) {
  if (!screen) {
    top[0] = min(min(upper_left[0],upper_right[0]),min(lower_left[0],lower_right[0]));
    bottom[0] = max(max(upper_left[0],upper_right[0]),max(lower_left[0],lower_right[0]));
    top[1] = min(min(upper_left[1],upper_right[1]),min(lower_left[1],lower_right[1]));
    bottom[1] = max(max(upper_left[1],upper_right[1]),max(lower_left[1],lower_right[1]));
//logit("%f,%f -> %f,%f %d",top[0],top[1],bottom[0],bottom[1],(int)xdpi);
    topx = (top[0]+1.0f)*width_in_pixels;
    topy = (top[1]+1.0f)*height_in_pixels*0.50f;
    bottomx = (bottom[0]+1.0f)*width_in_pixels;
    bottomy = (bottom[1]+1.0f)*height_in_pixels*0.50f;
    if ( (((GLint)bottomx-(GLint)topx-2)<=0)|| (((GLint)bottomy-(GLint)topy-2)<=0) ) {
      return;
      }
    glViewport(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    checkGlError("glViewportbw2aop");
    glScissor(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    glEnable(GL_SCISSOR_TEST);
    }
  else {
    top[0] = min(min(upper_left[0],upper_right[0]),min(lower_left[0],lower_right[0]));
    bottom[0] = max(max(upper_left[0],upper_right[0]),max(lower_left[0],lower_right[0]));
    top[1] = min(min(upper_left[1],upper_right[1]),min(lower_left[1],lower_right[1]));
    bottom[1] = max(max(upper_left[1],upper_right[1]),max(lower_left[1],lower_right[1]));
//logit("%f,%f -> %f,%f %d",top[0],top[1],bottom[0],bottom[1],(int)xdpi);
    topx = (top[0]+1.0f)*width_in_pixels+ (width_in_pixels*2);
    topy = (top[1]+1.0f)*height_in_pixels*0.5f;
    bottomx = (bottom[0]+1.0f)*width_in_pixels+ (width_in_pixels*2);
    bottomy = (bottom[1]+1.0f)*height_in_pixels*0.5f;
    if ( (((GLint)bottomx-(GLint)topx-2)<=0)|| (((GLint)bottomy-(GLint)topy-2)<=0) ) {
      return;
      }
    glViewport(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    checkGlError("glViewportbw2eee");
    glScissor(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    glEnable(GL_SCISSOR_TEST);
    }
  }
else {
  top[0] = min(min(upper_left[0],upper_right[0]),min(lower_left[0],lower_right[0]));
  bottom[0] = max(max(upper_left[0],upper_right[0]),max(lower_left[0],lower_right[0]));
  top[1] = min(min(upper_left[1],upper_right[1]),min(lower_left[1],lower_right[1]));
  bottom[1] = max(max(upper_left[1],upper_right[1]),max(lower_left[1],lower_right[1]));
//logit("%f,%f -> %f,%f %d",top[0],top[1],bottom[0],bottom[1],(int)xdpi);
  topx = (top[0]+1.0f)*width_in_pixels*0.5f;
  topy = (top[1]+1.0f)*height_in_pixels*0.5f;
  bottomx = (bottom[0]+1.0f)*width_in_pixels*0.5f;
  bottomy = (bottom[1]+1.0f)*height_in_pixels*0.5f;
  if ( (((GLint)bottomx-(GLint)topx-2)<=0)|| (((GLint)bottomy-(GLint)topy-2)<=0) ) {
    return;
    }
  glViewport(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
  if (checkGlError("glViewportbw2www")) {
//    logit("vp %d,%d,%d,%d\n",topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    }
  glScissor(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
  glEnable(GL_SCISSOR_TEST);
  }
//logit("    %d,%d -> %d,%d",topx,topy,bottomx,bottomy);



//display_set_gl_base_viewport();

 glMatrix modelMatrix;
// glMatrix m2;
// glMatrix m3;
 
 glMatrix viewMatrix;
// glMatrix MVMatrix;



   GLfloat now_eye[3] = {0.f,0.f,-400.f};
   GLfloat now_center[3] = {0.f,0.f,0.f};
   GLfloat now_up[3] =   {0.f,1.4f,-400.f};


  ViewLookAt(&viewMatrix,now_eye,now_center,now_up);
  
  glLineWidth(pixel_width);
  




  loadIdentity(&modelMatrix); 
//  glMatrix *xmatrix=&modelMatrix;
//  translateMatrix(xmatrix,0.f,0.f,-z_offset+6.f);
  rotationMatrix(&modelMatrix,button_angle,0.f,0.f,1.f);
  scaleMatrix(&modelMatrix,2.f,2.f,2.f);
//  multMatrix(&MVMatrix,&modelMatrix,&viewMatrix);
//    multMatrix(&MVPMatrix,&MVMatrix,&perspectiveMatrix);
    // Apply the projection and view transformation
    

  glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&modelMatrix));
  if (checkGlError("glUniformMatrix4fva")) {
//    logit("program %d mathandle is %d\n",program,mMVPMatrixHandle);
    }
  
float prev_freq = button_colors[0];
int i;
for (i=0;i<BUTTON_RINGS;i++) {
  float abci[4];
  //int offset;
  
  
  int nexti = i+1;
  if (nexti>=BUTTON_RINGS) nexti=i;

  float intensity = 1.;

  abci[0] = prev_freq;
  abci[1] = button_colors[i];
  abci[2] = button_colors[nexti];
  
  abci[0]= (abci[0] + abci[1])*0.5;
  abci[2] = (abci[2] + abci[1])*0.5;
  abci[3] = intensity;    
  prev_freq = abci[1];
  
  if (heavy_metal_flag) {
	      float r,g,b;
	      r = abci[0];
	      g = abci[1];
	      b = abci[2];
	      // redden across the board.
//	      r = r + 0.2;
	      if (r>1.0) r=1.0;
	      
	      // green cant be bigger than red
	      if (g>r) g=r;
	      
	      // much less  blue
	      b=b*0.3;
              glUniform4f(abciHandle,r,g,b,abci[3]);
              checkGlError("glUniform4fh1b");
	      
	      }
  else {
  
    glUniform4f(abciHandle,abci[0],abci[1],abci[2],abci[3]);
    checkGlError("glUniform4fh1b");
    }
    
    
  glVertexAttribPointer(aGradientHandle,1,GL_FLOAT,GL_FALSE,0,buttonGradient);
  checkGlError("glVertexAttribPointergradient");
  glEnableVertexAttribArray(aGradientHandle);
  checkGlError("glEnableVertexAttribArray sds");
    
    
  glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, button_spirals[i]);
  checkGlError("glVertexAttribPointer");
  glEnableVertexAttribArray(gvPositionHandle);
  checkGlError("glEnableVertexAttribArray aa");
  glDrawArrays(GL_POINTS,0,BUTTON_POINTS);
  checkGlError("glDrawArrays");
  glDisableVertexAttribArray(gvPositionHandle);
  checkGlError("glDisableVertexArray");
  }
  
/*glulookat */ 
glUseProgram(id);
checkGlError("glUseProgram");
glDisable(GL_DEPTH_TEST);
glDisable(GL_SCISSOR_TEST);

}


void backward_worm_finishStreaming() {
//called when we  change channels
    queue_start = 0;
    queue_end = 0;
    z_offset=0.f;
    angle=0.f;
}

// WORLD_INTERNATIONAL off
const display_set backward_worm_display_set = {
  .name = "backward_worm",
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .setupGraphics = backward_worm_setupGraphics,
  .step = backward_worm_step,
  .renderBackground = backward_worm_renderBackground,
  .renderMain  = backward_worm_renderFrame,
  .renderForeground = backward_worm_renderForeground,
  .stepButton = backward_worm_stepButton,
  .renderButton = backward_worm_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = backward_worm_finishStreaming,
  };
// WORLD_INTERNATIONAL on

/*
$Log: backward_worm.c,v $
Revision 1.46  2019/06/30 23:34:54  hib
Addes heavy metal mode, finally.

Revision 1.45  2019/05/17 00:39:00  hib
this would be the silentradiance 1/3l version

Revision 1.44  2019/05/03 16:28:10  hib
wip. the impact font is goofed up again.

Revision 1.43  2019/05/03 12:26:57  hib
from non cvs

Revision 1.42  2019/04/28 04:11:16  hib
wip - changing display_set to separate init, glsetup, and screen changed

Revision 1.41  2019/04/15 08:13:56  hib
cleaning international

Revision 1.40  2019/04/15 06:09:25  hib
internationalization wip

Revision 1.39  2019/03/17 12:58:24  hib
Updated copyrights

Revision 1.38  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

Revision 1.37  2019/02/09 09:27:57  hib
backward worm reversed eyeballs to sync up

Revision 1.36  2019/02/08 17:28:47  hib
backward worm - give different righty/left eye visuals

Revision 1.35  2018/09/28 23:08:37  hib
Publish 1.2 time

Revision 1.34  2018/09/28 17:46:46  hib
Version 1.2 - after apple. Linting warnings and errors.
Also - trying to fix perspective of larryharvey in android

Revision 1.33  2018/07/29 20:44:08  hib
wip

Revision 1.32  2018/07/25 03:51:19  hib
wip

Revision 1.31  2018/03/08 18:58:47  hib
wip - start of the eye matrix stuff

Revision 1.30  2018/02/27 23:17:14  hib
Fixed this thing!

Revision 1.29  2018/02/27 21:40:18  hib
Got it all

Revision 1.28  2018/02/27 21:33:25  hib
wip

Revision 1.27  2018/02/27 21:29:31  hib
wip

Revision 1.26  2018/02/27 01:27:44  hib
wip

Revision 1.25  2018/02/24 15:28:50  hib
Got the VR to work initially

Revision 1.24  2018/02/22 21:57:55  hib
Help modularize the display visualizers much more - for display_set - need to do for the main menu too
Also - added step/draw initial logic, to make it more likely that we can render split screen for goggles.

Revision 1.23  2018/01/20 22:10:07  hib
added 2018

Revision 1.22  2018/01/20 21:59:40  hib
functionally ready in ios

Revision 1.21  2018/01/18 07:37:51  hib
got the display_worm to show up

Revision 1.20  2018/01/18 04:33:07  hib
This is beautiful - gott to get hte button working though.
We could change v to be low for non red colors in heavy metal land, turning yellow into a tan and orange.

Revision 1.16  2018/01/17 13:12:28  hib
This was a redo of backward worm to smooth out fonts. decided to back out at the last minut
was based on hues

Revision 1.15  2018/01/16 08:54:26  hib
minor

Revision 1.14  2018/01/16 08:49:36  hib
fixed backward worm from freezing - by multiple ways

Revision 1.13  2017/12/01 16:57:35  hib
image pretty much done - sometimes doesn't show up

Revision 1.12  2017/12/01 12:48:27  hib
Did a bit of a revamp for the graphics - cleaning out glViewport, added scissors that should clip the buttons on cheap phones.
Better set up to handle 3d, and the "third eye" idea.

Revision 1.11  2017/12/01 04:45:48  hib
Made all the graphical apps check to see if ran before so we don't recreate all the programs, etc.
just before we did it, images was close to working on android.
Thats our main focus right now.

Revision 1.10  2017/10/06 14:59:36  hib
ready to make a beta

Revision 1.9  2017/09/26 22:36:52  hib
Fixed a problem on ipads where backward_worm had no dots.

Revision 1.8  2017/07/05 12:40:10  hib
Added copyleft

Revision 1.7  2017/04/06 11:41:16  hib
clean up linux client ifdef

Revision 1.6  2017/04/06 11:10:31  hib
Works and switches orientation now. Whew!

Revision 1.5  2017/04/04 09:08:37  hib
Got it to run on linux as a client! yo!

Revision 1.4  2016/12/22 04:23:21  hib
Got it working. Now 6 screens.

Revision 1.3  2016/12/22 04:08:06  hib
removed the extra graphics from the menu, but it crashes

Revision 1.2  2016/12/21 01:32:24  hib
consolidating...

Revision 1.1  2016/12/20 23:45:37  hib
consolidating

Revision 1.5  2016/07
/02 00:54:14  hib
Got the justid display to work as well.

Revision 1.3  2016/06/30 03:42:18  hib
made display better

Revision 1.1  2016/06/30 00:03:56  hib
display stuff to go over to apple

*/

/*$Revision: 1.46 $*/



