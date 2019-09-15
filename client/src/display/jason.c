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





/*$Revision: 1.18 $*/

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
#include "helper.h"
#include "display_set.h"
#include "default.h"

#define ANGLES_TO_DO 2400
#define FRAMES_TO_SHOW 300
#define FRAMES_TO_SHOWX3 900
#define FRAMES_TO_SHOWX4 1200

static float colors[FRAMES_TO_SHOWX4];
static int frames[FRAMES_TO_SHOW];
static int queue_start;
static int queue_end;
static int right_phases[FRAMES_TO_SHOW];
static int left_phases[FRAMES_TO_SHOW];
static float z_offset=0.f;
static float right_points[FRAMES_TO_SHOW][720];
static float left_points[FRAMES_TO_SHOW][720];
static float z_finish[FRAMES_TO_SHOW];
static float angle;

static glMatrix modelMatrix;
static glMatrix effectMatrix; /* used to do effects like zoom near/zoom away */
static glMatrix viewMatrix;
static glMatrix perspectiveMatrix;
static glMatrix MVMatrix;
static glMatrix MVPMatrix;



static int colorHandle;


static float cheap_cos[ANGLES_TO_DO];
static float cheap_sin[ANGLES_TO_DO];


static float pixel_width=1.f; /* increase for bigger screens */


#define BUTTON_RINGS 70
#define BUTTON_POINTS 67
#define BUTTON_POINTS_X3 201
static float button_colors[BUTTON_RINGS];
static float button_spirals[BUTTON_RINGS][BUTTON_POINTS_X3];
static float button_angle=0.f;

static char gVertexShaderOneColor[2000];


// WORLD_INTERNATIONAL off

static char * gFragmentShaderOneColor =
"precision mediump float;\n"
	"uniform vec4 v_Color;\n"
        "void main() {\n"
        "  gl_FragColor = v_Color;\n"
        "}\n";




static GLuint createOneColorProgram() {
    sprintf(gVertexShaderOneColor,"uniform mat4 u_MVPMatrix;      \n"		// A constant representing the combined model/view/projection matrix.

		   "attribute vec4 vPosition;     \n"		// Per-vertex position information we will pass in.
		   "void main()                    \n"		// The entry point for our vertex shader.
		   "{                              \n"
		   "   gl_PointSize=%3.2f;           \n"   // this goofed up on ios 12.9 inch so added this.
		   "   gl_Position = u_MVPMatrix   \n" 	// gl_Position is a special variable used to store the final position.
		   "               * vPosition;   \n"     // Multiply the vertex by the matrix to get the final point in 
		   "}                              \n"    // normalized screen coordinates.
                   ,pixel_width);
// WORLD_INTERNATIONAL on

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, gVertexShaderOneColor);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, gFragmentShaderOneColor);
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
//        glBindAttribLocation(program, 0, "a_Color");

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
                    logit("Could not link program:\n%s\n", buf);
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

static GLuint one_color_program;
static GLuint gvPositionHandle;

static float width_in_pixels;
static float height_in_pixels;
static float width_in_inches;
static float height_in_inches;
static float height_to_inches;
static float rotation_in_radians;
static float relative_width;
static float relative_height;
static float width_to_inches;



int jason_setupGraphics(int w, int h,int graphics_type) {
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


//    scaleMatrix(&projectionMatrix,1.0,width_in_inches/height_in_inches,1.0);
    
//    loadIdentity(&projectionMatrix);  
    loadIdentity(&perspectiveMatrix);  
    perspectiveGL(&perspectiveMatrix,45.f, ((GLfloat)w)/((GLfloat)h),  0.1f, 15000.f);
    
if (ran_setup_graphics_once_before) return 1;
    loadIdentity(&modelMatrix);  
    loadIdentity(&effectMatrix);  
    loadIdentity(&viewMatrix);  
   
    one_color_program = createOneColorProgram();
    if (!one_color_program) {
        logit("Could not create one color program.");
        return 0;
    }
    gvPositionHandle = glGetAttribLocation(one_color_program, "vPosition");
    checkGlError("glGetAttribLocation");

    // get handle to fragment shader's v_Color member
    colorHandle = glGetUniformLocation(one_color_program, "v_Color");
    checkGlError("glGetUniformLocation");


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
      y =  cheap_sin[xangle]*d;
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
  }    /* setup buttons */

    
    return 1;
}



void jason_renderBackground(int screen,glMatrix *eye) {
    glClearColor(0.0f, 0.0f, 0.f, 1.0f);
    checkGlError("glClearColor");

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
}




static void compute_frames() {
int i;
int q;
int right_phase;
int left_phase;
if (queue_start==queue_end) {
  q = queue_start;
  i = packet_summary.no_gap_frame;
  left_phase=0;
  right_phase=1200;
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
    right_phase=1200;
    z_offset=0.f;    
    }
  q = queue_end;
  left_phase = left_phases[qm1]+240;
  if (left_phase>=ANGLES_TO_DO) left_phase -=ANGLES_TO_DO;
  right_phase = right_phases[qm1]+240;
  if (right_phase >=ANGLES_TO_DO) right_phase -= ANGLES_TO_DO;
  z_offset = z_finish[qm1];
  if (z_offset>400.) {
    q = queue_start;
    i = packet_summary.no_gap_frame;
    left_phase=0;
    right_phase=1200;
    z_offset=0.f;    
    }
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
    float plog = logf(pitch*0.333333f)*1.44269504088884957607f;  /* first constant is 1/log(2) because ln(x)/ln(2) = log2(x) */
//    the .33333f is just to make jason with different colors thatn backward_worm
    int plogi = (int) plog;
    pitch = plog- (float)plogi;
/*
Old way:
    while (pitch>440.) pitch  = pitch*0.125;
    while (pitch<=220.) pitch  = pitch*2.0;
pitches are now in the range of 256 to 511 - subtract 255 and we can use the pitch colors to set the color 
*/
    

    
    rgb xrgb;
    xrgb=hsv2rgb(pitch,1.f,0.90f);
    colors[q*4]=xrgb.r;
    colors[q*4+1]=xrgb.g;
    colors[q*4+2]=xrgb.b;
    colors[q*4+3]=1.0f;
    }
    
  else {
    carry:
    if (q==queue_start) {
      colors[q*4]=0.5f;
      colors[q*4+1]=0.5f;
      colors[q*4+2]=0.5f;
      colors[q*4+3]=1.0f;
      }
    else {
      int qm1=q-1;
      if (qm1<0) qm1 += FRAMES_TO_SHOW;
      colors[q*4]=colors[qm1*4];
      colors[q*4+1]=colors[qm1*4+1];
      colors[q*4+2]=colors[qm1*4+2];
      colors[q*4+3]=1.0f;
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
      float *pt = left_points[q];
      float x,y,z;
      x = m * cheap_cos[left_phase];
      y = m * cheap_sin[left_phase];
      z = z_offset;
      z_offset += 0.00004166666666666666f; /* 1/38400 */
      
      pt[jj++] = x;
      pt[jj++] = y;
      pt[jj++] = z;
      
      jj-=3;
      pt = right_points[q];
      m = p->sample[j+j+1]*0.0000305175781250000f; /* /32768*/
      m= (1.1f+m)/2.1f;
      x = m * cheap_cos[right_phase];
      y = m * cheap_sin[right_phase];
      z = z_offset;
      z_offset += 0.00004166666666666666f; /* 1/38400 */
      
      pt[jj++] = x;
      pt[jj++] = y;
      pt[jj++] = z;
      
      
      left_phase++;
      right_phase++;
      
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



static void step_screen(glMatrix *modelMat,GLfloat parameter) {
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

static void draw_screen(glMatrix *modelMat,GLfloat parameter,int screen) { 
  GLuint mMVPMatrixHandle;
   GLfloat now_eye[3] = {0.f,0.f,-400.f};
   GLfloat now_center[3] = {0.f,0.f,0.f};
   GLfloat now_up[3] =   {0.f,1.4f,-400.f};



	
//   now_eye[2]=-400.f+z_offset;
//   now_center[2] = +z_offset;
//   now_up[2]=-400.f +z_offset;
  ViewLookAt(&viewMatrix,now_eye,now_center,now_up);
  
  glLineWidth(pixel_width);
  

  mMVPMatrixHandle = glGetUniformLocation(one_color_program, "u_MVPMatrix");

//  perspectiveGL(&perspectiveMatrix,45.f, ((GLfloat)width_in_pixels)/((GLfloat)height_in_pixels),  0.001f, 15000.f);
  glMatrix *xmatrix=modelMat;
//    scaleMatrix(modelMat,0.5f,0.5f,0.5f);
  translateMatrix(xmatrix,0.f,0.f,-z_offset+6.f);
  rotationMatrix(xmatrix,angle,0.f,0.f,1.f);
  multMatrix(&MVMatrix,xmatrix,&viewMatrix);
  multMatrix(&MVPMatrix,&MVMatrix,&perspectiveMatrix);
    // Apply the projection and view transformation
  glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&MVPMatrix));
  checkGlError("glUniformMatrix4fv");
  
    glDisable(GL_DEPTH_TEST);
  

int q=queue_start;  

int c=0;
while (q!=queue_end) { 

  int i4=q*4;
  
  if ((frames[q]-packet_summary.current_frame<=2)&&(frames[q]-packet_summary.current_frame>=-2)) {
    float c[3];
    c[0]=colors[i4];
    c[1]=colors[i4+1];
    c[2]=colors[i4+2];
    float min=0.f;
    if (c[0]>min) min=c[0];
    if (c[1]>min) min=c[1];
    if (c[2]>min) min=c[2];
    if (min) min = 1.0f/min; else min=1.0f;
    glUniform4f(colorHandle,colors[i4]*min,colors[i4+1]*min,colors[i4+2]*min,colors[i4+3]);
    checkGlError("glUniform4fh2");
    }
  else {
    glUniform4f(colorHandle,colors[i4],colors[i4+1],colors[i4+2],colors[i4+3]);
    checkGlError("glUniform4fh");
    }
  glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, left_points[q]);
  checkGlError("glVertexAttribPointer");
  glEnableVertexAttribArray(gvPositionHandle);
  checkGlError("glEnableVertexAttribArray aa");
//  if (q==queue_start) logit("%f,%f,%f",left_points[q][0],left_points[q][1],left_points[q][2]);
  glDrawArrays(GL_POINTS,0,240);
  checkGlError("glDrawArrays");
  glDisableVertexAttribArray(gvPositionHandle);
  checkGlError("glDisableVertexArray");


  glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, right_points[q]);
  checkGlError("glVertexAttribPointer");
  glEnableVertexAttribArray(gvPositionHandle);
  checkGlError("glEnableVertexAttribArray bb");
  glDrawArrays(GL_POINTS,0,240);
  checkGlError("glDrawArrays");
  glDisableVertexAttribArray(gvPositionHandle);
  checkGlError("glDisableVertexArray");

  
  q=q+1;
  if (q>=FRAMES_TO_SHOW) q -= FRAMES_TO_SHOW;
  c=c+1;  
  } /* for each frame we want to draw */

    glEnable(GL_DEPTH_TEST);
glLineWidth(pixel_width);
}











static void jason_step() {    
step_screen(&modelMatrix,7.f);
}

void jason_renderFrame(int screen,glMatrix *eye) {    
glUseProgram(one_color_program);
checkGlError("glUseProgram");
glEnable(GL_DEPTH_TEST);
    
loadIdentity(&modelMatrix);

draw_screen(&modelMatrix,7.f,screen);
}



void jason_renderForeground(int screen,glMatrix *eye) {
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


void jason_stepButton() {
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


void jason_renderButton(int program,int colorHandle,int menu_position_handle,glMatrix *view,int screen,glMatrix *eye) {
/* we will set the viewport to be smaller */
glUseProgram(one_color_program);
checkGlError("glUseProgram");
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
    glViewport(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    checkGlError("glViewportbw2aqw");
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
    glViewport(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    checkGlError("glViewportbw2huih");
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
  glViewport(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
  checkGlError("glViewportbw2iui");
  glScissor(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
  glEnable(GL_SCISSOR_TEST);
  }
//logit("    %d,%d -> %d,%d",topx,topy,bottomx,bottomy);


loadIdentity(&modelMatrix);




  GLuint mMVPMatrixHandle;
   GLfloat now_eye[3] = {0.f,0.f,-400.f};
   GLfloat now_center[3] = {0.f,0.f,0.f};
   GLfloat now_up[3] =   {0.f,1.4f,-400.f};


  ViewLookAt(&viewMatrix,now_eye,now_center,now_up);
  
  glLineWidth(pixel_width);
  

  mMVPMatrixHandle = glGetUniformLocation(program, "u_MVPMatrix");

  loadIdentity(&modelMatrix); 
  glMatrix *xmatrix=&modelMatrix;
//  translateMatrix(xmatrix,0.f,0.f,-z_offset+6.f);
  rotationMatrix(xmatrix,button_angle,0.f,0.f,1.f);
//    multMatrix(&MVMatrix,xmatrix,&viewMatrix);
//    multMatrix(&MVPMatrix,&MVMatrix,&perspectiveMatrix);
    // Apply the projection and view transformation

  glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&modelMatrix));
  checkGlError("glUniformMatrix4fv");
  
  
int i;
for (i=0;i<BUTTON_RINGS;i++) {
      rgb xrgb;
    xrgb=hsv2rgb(button_colors[i],1.f,1.f);
//    float colors[4];
//    colors[0]=xrgb.r;
//    colors[1]=xrgb.g;
//    colors[2]=xrgb.b;
//    colors[3]=1.0f;

    glUniform4f(colorHandle,xrgb.r,xrgb.g,xrgb.b,1.0f);
    checkGlError("glUniform4fh");
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

glDisable(GL_SCISSOR_TEST);
}


// WORLD_INTERNATIONAL off
const display_set jason_display_set = {
  .name = "jason",
  .setupGraphics = jason_setupGraphics,
  .step = jason_step,
  .changeScreen = NULL,
  .renderBackground = jason_renderBackground,
  .renderMain  = jason_renderFrame,
  .renderForeground = jason_renderForeground,
  .stepButton = jason_stepButton,
  .renderButton = jason_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = NULL
  };
// WORLD_INTERNATIONAL on


/*
$Log: jason.c,v $
Revision 1.18  2019/04/28 04:11:16  hib
wip - changing display_set to separate init, glsetup, and screen changed

Revision 1.17  2019/04/15 09:35:41  hib
Getting closer.

Revision 1.16  2019/04/15 06:09:25  hib
internationalization wip

Revision 1.15  2019/03/17 12:58:24  hib
Updated copyrights

Revision 1.14  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

Revision 1.13  2018/09/28 17:46:46  hib
Version 1.2 - after apple. Linting warnings and errors.
Also - trying to fix perspective of larryharvey in android

Revision 1.12  2018/07/25 03:54:47  hib
*** empty log message ***

Revision 1.11  2018/02/27 01:27:44  hib
wip

Revision 1.10  2018/02/24 21:56:38  hib
minor again

Revision 1.9  2018/02/24 15:28:50  hib
Got the VR to work initially

Revision 1.8  2018/02/22 21:57:55  hib
Help modularize the display visualizers much more - for display_set - need to do for the main menu too
Also - added step/draw initial logic, to make it more likely that we can render split screen for goggles.

Revision 1.7  2018/01/22 12:37:39  jason
Cleaned trailing spaces

Revision 1.6  2018/01/20 22:10:07  hib
added 2018

Revision 1.5  2018/01/18 07:37:51  hib
got the display_worm to show up

Revision 1.4  2017/12/01 16:57:35  hib
image pretty much done - sometimes doesn't show up

Revision 1.3  2017/12/01 12:48:27  hib
Did a bit of a revamp for the graphics - cleaning out glViewport, added scissors that should clip the buttons on cheap phones.
Better set up to handle 3d, and the "third eye" idea.

Revision 1.2  2017/12/01 04:45:48  hib
Made all the graphical apps check to see if ran before so we don't recreate all the programs, etc.
just before we did it, images was close to working on android.
Thats our main focus right now.

Revision 1.1  2017/11/15 03:02:43  hib
Added jason

*/

/*$Revision: 1.18 $*/



