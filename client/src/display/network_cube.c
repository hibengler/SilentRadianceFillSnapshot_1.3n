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






/*$Revision: 1.29 $*/

#ifdef ANDROID
#include <jni.h>
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
#include "main_menu.h"


struct user_portal {

 float a; /* used for scaling the roomforyourhead */
 
 float point; /* point between 0 and 1 for the from to looking */
 
 GLfloat from_eye[3];      /* where we are looking from */
 GLfloat from_center[3];    /* the center */
 GLfloat from_up[3];        /* where up[ is */
 GLfloat to_eye[3];      /* where we are looking to be for our eye*/
 GLfloat to_center[3];  /* where to will be */
 GLfloat to_up[3];    /* the up vectoer we are changing to */
 int beat;
 GLfloat original_where_in_tempo;
 
 };

static struct user_portal portal;

static struct user_portal button_portal;

static glMatrix modelMatrix;
static glMatrix effectMatrix; /* used to do effects like zoom near/zoom away */
static glMatrix viewMatrix;
static glMatrix projectionMatrix;
static glMatrix perspectiveMatrix;
static glMatrix MVMatrix;
static glMatrix MVPMatrix;



static int colorHandle;





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
static GLuint full_color_program;
static GLuint gvPositionHandle;
static GLuint mPositionHandle;

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










static void new_to_point(struct user_portal *this_portal) {
  GLfloat *from_eye = this_portal->from_eye;
  GLfloat *to_eye = this_portal->to_eye;
  GLfloat *from_center = this_portal->from_center;
  GLfloat *to_center = this_portal->to_center;
  GLfloat *from_up = this_portal->from_up;
  GLfloat *to_up = this_portal->to_up;
  int i;
  for (i=0;i<3;i++) {
    int r=2000;
    int f = from_eye[i]- (r/2);
    int t = from_eye[i]+ (r/2);
    if (f<-3000) f=-3000;
    if (t>3000) t=3000;
    r = t-f;
    to_eye[i] = (f+t)/2 + rand()%r;
    }
  for (i=0;i<3;i++) {
    int r=2000;
    int f = from_center[i]- (r/2);
    int t = from_center[i]+ (r/2);
    if (f<-3000) f=-3000;
    if (t>3000) t=3000;
    r = t-f;
    to_center[i] = (f+t)/2 + rand()%r;
    }
  for (i=0;i<3;i++) {
    int r=2000;
    int f = from_up[i]- (r/2);
    int t = from_up[i]+ (r/2);
    if (f<-3000) f=-3000;
    if (t>3000) t=3000;
    r = t-f;
    to_up[i] = (f+t)/2 + rand()%r;
    }
}

        
static void init_model(struct user_portal *this_portal) {
  GLfloat *from_eye = this_portal->from_eye;
//  GLfloat *to_eye = this_portal->to_eye;
  GLfloat *from_center = this_portal->from_center;
//  GLfloat *to_center = this_portal->to_center;
  GLfloat *from_up = this_portal->from_up;
//  GLfloat *to_up = this_portal->to_up;
  from_eye[0] = rand() % 6000 - 3000;
  from_eye[1] = rand() % 6000 - 3000;
  from_eye[2] = rand() % 6000 - 3000;
  from_center[0] = rand() % 6000 - 3000;
  from_center[1] = rand() % 6000 - 3000;
  from_center[2] = rand() % 6000 - 3000;
  from_up[0] =  rand() % 6000 - 3000;
  from_up[1] =  rand() % 6000 - 3000;
  from_up[2] =  rand() % 6000 - 3000;
  new_to_point(this_portal);


  this_portal->a = 1.;

  this_portal->beat=0;
  if (packet_summary.where_in_tempo<0.f) {
    this_portal->original_where_in_tempo = 0.f;
    }
  else {
    this_portal->original_where_in_tempo = packet_summary.where_in_tempo;
    }
}



int network_cube_setupGraphics(int w, int h, int graphics_mode) {
    loadIdentity(&modelMatrix);  
    loadIdentity(&effectMatrix);  
    loadIdentity(&viewMatrix);  
    loadIdentity(&projectionMatrix);  
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
 
    
    loadIdentity(&perspectiveMatrix);  
    perspectiveGL(&perspectiveMatrix,45.f, ((GLfloat)w)/((GLfloat)h),  1.f, 15000.f);

if (ran_setup_graphics_once_before) return(1);    
    loadIdentity(&modelMatrix);  
    loadIdentity(&effectMatrix);  
    loadIdentity(&viewMatrix);  
    loadIdentity(&projectionMatrix);  
    
//    scaleMatrix(&projectionMatrix,1.0,width_in_inches/height_in_inches,1.0);
    
    loadIdentity(&projectionMatrix);  
    
    gvPositionHandle = glGetAttribLocation(one_color_program, "vPosition");
    checkGlError("glGetAttribLocation");

    // get handle to fragment shader's v_Color member
    colorHandle = glGetUniformLocation(one_color_program, "v_Color");
    

    glEnable(GL_DEPTH_TEST);
init_model(&portal);
init_model(&button_portal);
    return 1;
}

#define interpolate(from,to,point,now) {now[0]=(to[0]-from[0])*point + from[0]; now[1]=(to[1]-from[1])*point + from[1]; now[2]=(to[2]-from[2])*point + from[2]; }



void network_cube_renderBackground(int screen,glMatrix *eye) {
}


/* just for reference 
																	    
GetLookAtMatrix(Vector3 eye, Vector3 at, Vector3 up){
    Vector3 forward, side;
    forward = at - eye;
    normalize(forward);
    side = cross(forward, up);
    normalize(side);
    up = cross(side, forward);

    Matrix4 res = Matrix4(
                          side.x, up.x, -forward.x, 0,
                          side.y, up.y, -forward.y, 0,
                          side.z, up.z, -forward.z, 0,
                          0, 0, 0, 1);
    translate(res, Vector3(0 - eye));
    return res;
}
*/


static void glColor3f(GLfloat r,GLfloat g,GLfloat b) {
glUniform4f(colorHandle,r,g,b,1.f);
checkGlError("glUnidform4f");
}



void draw_screen(struct user_portal *this_portal,glMatrix *modelMat,GLfloat line_width,int eye,
  glMatrix *projectionAlter) { 
  GLfloat verticies[1600];
  float point=this_portal->point; 
  GLuint mMVPMatrixHandle;
   
  GLfloat now_eye[3];
  GLfloat now_up[3];
  GLfloat now_center[3];
  GLfloat *from_eye = this_portal->from_eye;
  GLfloat *to_eye = this_portal->to_eye;
  GLfloat *from_up = this_portal->from_up;
  GLfloat *to_up = this_portal->to_up;
  GLfloat *from_center = this_portal->from_center;
  GLfloat *to_center = this_portal->to_center;
  interpolate(from_eye,to_eye,point,now_eye);
  interpolate(from_up,to_up,point,now_up);
  interpolate(from_center,to_center,point,now_center);


  if (projectionAlter) {
    GLfloat now_eye2[3];
    GLfloat now_up2[3];
    GLfloat now_center2[3];
    transformVector(now_eye,now_eye2,projectionAlter);
    transformVector(now_up,now_up2,projectionAlter);
    int i;
    for (i=0;i<3;i++) {
      now_eye[i]=now_eye2[i];
      now_center[i]=now_center2[i];
      }
    }


  ViewLookAt(&viewMatrix,now_eye,now_center,now_up);

/*  
  gluLookAt(now_eye[0],now_eye[1],now_eye[2],
            now_up[0],now_up[1],now_up[2],
	    now_center[0],now_center[1],now_center[2]
	    );
*/	    
  glLineWidth(line_width);
  
    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, verticies);
    checkGlError("glVertexAttribPointernc");

    mMVPMatrixHandle = glGetUniformLocation(one_color_program, "u_MVPMatrix");



//    loadIdentity(&modelMatrix);
    glMatrix *xmatrix=modelMat;
//    scaleMatrix(modelMat,0.0003f,0.0003f,0.0003f);
    multMatrix(&MVMatrix,xmatrix,&viewMatrix);
    multMatrix(&MVPMatrix,&MVMatrix,&perspectiveMatrix);
    // Apply the projection and view transformation
    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&MVPMatrix));


  GLfloat i;
  GLfloat from;
  GLfloat to;
  GLfloat interval;
  GLfloat from2;
  GLfloat to2;
  GLfloat interval2;
  int n;
  from=-4000+1.+(line_width*0.5)+40;
  to=-from;
  interval = ((to-from)/29.5f);
  
  from2=-4000+1.+(line_width*0.5f);
  to2=-from2;
  interval2 = ((to-from)/30.001f);

#define glVertex3fv(a) {GLfloat *mm=a;verticies[n++]=mm[0];verticies[n++]=mm[1];verticies[n++]=mm[2];}  
  /* RED */
  glColor3f(0.7,0.0, 0.0);
  n=0;
  for (i=from;i<=to;i+=interval) {
    float from[3];
    float to[3];
    from[0] = -4000;
    from[1] = i;
    from[2] = -4000;
    to[0]=4000;
    to[1] = i;
    to[2] = -4000;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }
        glEnableVertexAttribArray(gvPositionHandle);
        checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_LINES,0,n/3);
        checkGlError("glDrawArrays");
	glDisableVertexAttribArray(gvPositionHandle);
        checkGlError("glDisableVertexAttribArray");

  /* green */
  glColor3f(0.0,1.0*0.7, 0.0);
  n=0;
  for (i=from2;i<=to2;i+=interval2) {
    float from[3];
    float to[3];
    from[0] = -4000;
    from[1] = -4000;
    from[2] = i;
    to[0] = 4000-(line_width*0.5);
    to[1]= -4000;
    to[2] = i;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }
        glEnableVertexAttribArray(gvPositionHandle);
        checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_LINES,0,n/3);
        checkGlError("glDrawArrays");
	glDisableVertexAttribArray(gvPositionHandle);
        checkGlError("glDisableVertexAttribArray");
  /* yellow across from green */
  glColor3f(1.0*0.7,1.0*0.7, 0.0);
  n=0;
  for (i=from2;i<=to2;i+=interval2) {
    float from[3];
    float to[3];
    from[0] = -4000;
    from[1] = 4000;
    from[2] = i;
    to[0] = 4000-(line_width*0.5);
    to[1]= 4000;
    to[2] = i;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }
        glEnableVertexAttribArray(gvPositionHandle);
        checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_LINES,0,n/3);
        checkGlError("glDrawArrays");
	glDisableVertexAttribArray(gvPositionHandle);
        checkGlError("glDisableVertexAttribArray");
  /* blue */
  glColor3f(0.0,0.0, 1.0*0.7);
  n=0;
  for (i=from2;i<=to2;i+=interval2) {
    float from[3];
    float to[3];
    from[0] = 4000;
    from[1] = -4000;
    from[2] = i;
    to[0] = 4000;
    to[1]=4000-(line_width*0.5);
    to[2] = i;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }    
        glEnableVertexAttribArray(gvPositionHandle);
        checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_LINES,0,n/3);
        checkGlError("glDrawArrays");
	glDisableVertexAttribArray(gvPositionHandle);
        checkGlError("glDisableVertexAttribArray");
  double f=i-interval2;
  /* orange on other side */
  glColor3f(1.0*0.7,(165./256.)*0.7, 0.0);
  n=0;
  for (i=from;i<=to;i+=interval) {
    float from[3];
    float to[3];
    from[0] = -4000;
    from[1] = i;
    from[2] = f;
    to[0]=4000;
    to[1] = i;
    to[2] = f;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }
        glEnableVertexAttribArray(gvPositionHandle);
        checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_LINES,0,n/3);
        checkGlError("glDrawArrays");
	glDisableVertexAttribArray(gvPositionHandle);
        checkGlError("glDisableVertexAttribArray");
  /* purple */
  glColor3f(1.0*0.7,0.0, 1.0*0.7);
  n=0;
  for (i=from2;i<=to2;i+=interval2) {
    float from[3];
    float to[3];
    from[0] = -4000;
    from[1] = -4000;
    from[2] = i;
    to[0] = -4000;
    to[1]=4000-(line_width*0.5);
    to[2] = i;
    glVertex3fv((GLfloat *) from);
    glVertex3fv((GLfloat *) to);
    }    
        glEnableVertexAttribArray(gvPositionHandle);
        checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_LINES,0,n/3);
        checkGlError("glDrawArrays");
	glDisableVertexAttribArray(gvPositionHandle);
        checkGlError("glDisableVertexAttribArray");

    glDisableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray");
  glLineWidth(1.0f);
}






static struct user_portal *this_portal = &portal;

static void network_cube_stepper(struct user_portal *this_portal) {
this_portal = &portal;
if (this_portal->a>=4.) this_portal->a=1.;
else this_portal->a = this_portal->a * 1.02;
#ifdef junk
/* junk for timing with the tempo - but tempo position does not work so well */
//   logit("%f -> %f",packet_summary.where_in_tempo,this_portal->original_where_in_tempo);
  if (packet_summary.where_in_tempo >=0.f) {
    if ((this_portal->original_where_in_tempo>packet_summary.where_in_tempo)||
        ((this_portal->original_where_in_tempo==0.f)&&(packet_summary.where_in_tempo==0.f)) ) {
      this_portal->beat = (this_portal->beat+1) % 4;
      if (this_portal->beat==0) {
        this_portal->original_where_in_tempo = packet_summary.where_in_tempo;    
        /* make the from = the to */
        this_portal->from_eye[0] = this_portal->to_eye[0];
        this_portal->from_eye[1] = this_portal->to_eye[1];
        this_portal->from_eye[2] = this_portal->to_eye[2];
        this_portal->from_center[0] = this_portal->to_center[0];
        this_portal->from_center[1] = this_portal->to_center[1];
        this_portal->from_center[2] = this_portal->to_center[2];
        this_portal->from_up[0] = this_portal->to_up[0];
        this_portal->from_up[1] = this_portal->to_up[1];
        this_portal->from_up[2] = this_portal->to_up[2];
        new_to_point(this_portal);
	}
      }
    this_portal->point = ((GLfloat)this_portal->beat)*0.25+packet_summary.where_in_tempo;
    } /* if we have a tempo */
  else */
#endif  
  
  {
    this_portal->point = this_portal->point + 1./(30.*5.);
    if (this_portal->point>1.) {
      this_portal->point = this_portal->point-1.;
      /* make the from = the to */
      this_portal->from_eye[0] = this_portal->to_eye[0];
      this_portal->from_eye[1] = this_portal->to_eye[1];
      this_portal->from_eye[2] = this_portal->to_eye[2];
      this_portal->from_center[0] = this_portal->to_center[0];
      this_portal->from_center[1] = this_portal->to_center[1];
      this_portal->from_center[2] = this_portal->to_center[2];
      this_portal->from_up[0] = this_portal->to_up[0];
      this_portal->from_up[1] = this_portal->to_up[1];
      this_portal->from_up[2] = this_portal->to_up[2];
      new_to_point(this_portal);
      }
    }

}


void network_cube_step(void) {
network_cube_stepper(&portal);
}



void network_cube_renderFrame(int screen,glMatrix *projectionAlter) {    
glUseProgram(one_color_program);
checkGlError("glUseProgram");
glEnable(GL_DEPTH_TEST);
    

loadIdentity(&modelMatrix);   
draw_screen(this_portal,&modelMatrix,7.f,screen,projectionAlter);
}



void network_cube_renderForeground(int screen,glMatrix *eye) {
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



void network_cube_stepButton() {  
  {
    button_portal.point = button_portal.point + 2./(30.*5.); /* twice as fast */
    if (button_portal.point>1.) {
      button_portal.point = button_portal.point-1.;
      /* make the from = the to */
      button_portal.from_eye[0] = button_portal.to_eye[0];
      button_portal.from_eye[1] = button_portal.to_eye[1];
      button_portal.from_eye[2] = button_portal.to_eye[2];
      button_portal.from_center[0] = button_portal.to_center[0];
      button_portal.from_center[1] = button_portal.to_center[1];
      button_portal.from_center[2] = button_portal.to_center[2];
      button_portal.from_up[0] = button_portal.to_up[0];
      button_portal.from_up[1] = button_portal.to_up[1];
      button_portal.from_up[2] = button_portal.to_up[2];
      new_to_point(&button_portal);
      }
    }
}

void network_cube_renderButton(int program,int mColorHandle,int menu_position_handle,
   glMatrix *view,int screen,glMatrix *eye) {

glUseProgram(one_color_program);
checkGlError("glUseProgram");
if (view) {
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

// logit("upperl left %f,%f",upper_left[0],upper_left[1]);
// logit("upperr left %f,%f",upper_right[0],upper_right[1]);
// logit("lowerl left %f,%f",lower_left[0],lower_left[1]);
// logit("lowerr left %f,%f",lower_right[0],lower_right[1]);



 GLuint topx,topy,bottomx,bottomy;
 if (last_draw_mode) {
   if (!screen) {
    top[0] = min(min(upper_left[0],upper_right[0]),min(lower_left[0],lower_right[0]));
    bottom[0] = max(max(upper_left[0],upper_right[0]),max(lower_left[0],lower_right[0]));
    top[1] = min(min(upper_left[1],upper_right[1]),min(lower_left[1],lower_right[1]));
    bottom[1] = max(max(upper_left[1],upper_right[1]),max(lower_left[1],lower_right[1]));
    
//  logit("VR L %f,%f -> %f,%f %d",top[0],top[1],bottom[0],bottom[1],(int)xdpi);
    topx = (top[0]+1.0f)*width_in_pixels;
    topy = (top[1]+1.0f)*height_in_pixels*0.50f;
    bottomx = (bottom[0]+1.0f)*width_in_pixels;
    bottomy = (bottom[1]+1.0f)*height_in_pixels*0.50f;
    if ( (((GLint)bottomx-(GLint)topx-2)<=0)|| (((GLint)bottomy-(GLint)topy-2)<=0) ) {
      return;
      }
    glViewport(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    checkGlError("glViewportbw2awl");
    glScissor(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    glEnable(GL_SCISSOR_TEST);
    }
  else {
    top[0] = min(min(upper_left[0],upper_right[0]),min(lower_left[0],lower_right[0]));
    bottom[0] = max(max(upper_left[0],upper_right[0]),max(lower_left[0],lower_right[0]));
    top[1] = min(min(upper_left[1],upper_right[1]),min(lower_left[1],lower_right[1]));
    bottom[1] = max(max(upper_left[1],upper_right[1]),max(lower_left[1],lower_right[1]));
//logit("VR R %f,%f -> %f,%f %d",top[0],top[1],bottom[0],bottom[1],(int)xdpi);
    topx = (top[0]+1.0f)*width_in_pixels+ (width_in_pixels*2);
    topy = (top[1]+1.0f)*height_in_pixels*0.5f;
    bottomx = (bottom[0]+1.0f)*width_in_pixels+ (width_in_pixels*2);
    bottomy = (bottom[1]+1.0f)*height_in_pixels*0.5f;
    if ( (((GLint)bottomx-(GLint)topx-2)<=0)|| (((GLint)bottomy-(GLint)topy-2)<=0) ) {
      return;
      }
    glViewport(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    checkGlError("glViewportbw2ji");
    glScissor(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    glEnable(GL_SCISSOR_TEST);
    }
  }
 else {
  top[0] = min(min(upper_left[0],upper_right[0]),min(lower_left[0],lower_right[0]));
  bottom[0] = max(max(upper_left[0],upper_right[0]),max(lower_left[0],lower_right[0]));
  top[1] = min(min(upper_left[1],upper_right[1]),min(lower_left[1],lower_right[1]));
  bottom[1] = max(max(upper_left[1],upper_right[1]),max(lower_left[1],lower_right[1]));
//  logit("Reg %f,%f -> %f,%f %d",top[0],top[1],bottom[0],bottom[1],(int)xdpi);
  topx = (top[0]+1.0f)*width_in_pixels*0.5f;
  topy = (top[1]+1.0f)*height_in_pixels*0.5f;
  bottomx = (bottom[0]+1.0f)*width_in_pixels*0.5f;
  bottomy = (bottom[1]+1.0f)*height_in_pixels*0.5f;
  if ( (((GLint)bottomx-(GLint)topx-2)<=0)|| (((GLint)bottomy-(GLint)topy-2)<=0) ) {
    return;
    }
  glViewport(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
  if (checkGlError("glViewportbw2iuh")) {
    logit("%d,%d,%d,%d",topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
    }
  glScissor(topx+1, topy+1, bottomx-topx-2, bottomy-topy-2);
  glEnable(GL_SCISSOR_TEST);
  }
  //logit("    %d,%d -> %d,%d",topx,topy,bottomx,bottomy);
 }




loadIdentity(&modelMatrix); 

  
      
draw_screen(&button_portal,&modelMatrix,2.f,screen,NULL);
glDisable(GL_SCISSOR_TEST);
//glViewport(0,0,(GLuint)width_in_pixels,(GLuint)height_in_pixels);
// checkGlError("glViewportenddenet");
 
}


// WORLD_international off
const display_set network_cube_display_set = {
  .name = "network_cube",
  .setupGraphics = network_cube_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .step = network_cube_step,
  .renderBackground = network_cube_renderBackground,
  .renderMain  = network_cube_renderFrame,
  .renderForeground = network_cube_renderForeground,
  .stepButton = network_cube_stepButton,
  .renderButton = network_cube_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = NULL
  };
// WORLD_international on

/*
$Log: network_cube.c,v $
Revision 1.29  2019/05/03 16:28:10  hib
wip. the impact font is goofed up again.

Revision 1.28  2019/05/03 12:26:57  hib
from non cvs

Revision 1.23  2019/03/17 12:58:25  hib
Updated copyrights

Revision 1.22  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

Revision 1.21  2018/09/28 23:08:37  hib
Publish 1.2 time

Revision 1.20  2018/09/28 17:46:46  hib
Version 1.2 - after apple. Linting warnings and errors.
Also - trying to fix perspective of larryharvey in android

Revision 1.19  2018/09/22 17:45:36  hib
wip

Revision 1.18  2018/09/02 21:47:26  hib
wip.  Just need bzip

Revision 1.17  2018/08/22 21:58:51  hib
work in progress

Revision 1.16  2018/07/25 03:54:47  hib
*** empty log message ***

Revision 1.15  2018/03/18 08:03:14  hib
simple receiveer no sound

Revision 1.14  2018/02/27 01:27:44  hib
wip

Revision 1.13  2018/02/24 15:28:50  hib
Got the VR to work initially

Revision 1.12  2018/02/22 21:57:55  hib
Help modularize the display visualizers much more - for display_set - need to do for the main menu too
Also - added step/draw initial logic, to make it more likely that we can render split screen for goggles.

Revision 1.11  2018/01/20 22:10:07  hib
added 2018

Revision 1.10  2018/01/18 07:37:51  hib
got the display_worm to show up

Revision 1.9  2017/12/01 16:57:35  hib
image pretty much done - sometimes doesn't show up

Revision 1.8  2017/12/01 12:48:27  hib
Did a bit of a revamp for the graphics - cleaning out glViewport, added scissors that should clip the buttons on cheap phones.
Better set up to handle 3d, and the "third eye" idea.

Revision 1.7  2017/12/01 04:45:48  hib
Made all the graphical apps check to see if ran before so we don't recreate all the programs, etc.
just before we did it, images was close to working on android.
Thats our main focus right now.

Revision 1.6  2017/11/08 09:38:06  hib
found a problem or two

Revision 1.5  2017/07/05 12:40:10  hib
Added copyleft

Revision 1.4  2017/04/06 11:41:16  hib
clean up linux client ifdef

Revision 1.3  2017/04/06 11:10:31  hib
Works and switches orientation now. Whew!

Revision 1.2  2017/04/04 09:08:37  hib
Got it to run on linux as a client! yo!

Revision 1.1  2016/12/20 20:07:49  hib
Should be enough to build this

Revision 1.5  2016/07/02 00:54:14  hib
Got the justid display to work as well.

Revision 1.3  2016/06/30 03:42:18  hib
made display better

Revision 1.1  2016/06/30 00:03:56  hib
display stuff to go over to apple

*/
