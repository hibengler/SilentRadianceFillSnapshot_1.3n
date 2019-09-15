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






/*$Revision: 1.31 $*/
// a
//{{{ static vars + shaders

#ifdef ANDROID
#include <jni.h>
#endif



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "glue.h"
#include "packet_summary.h"
#include "justin2.h"
#include "helper.h"
#include "default.h"
#include "flowers_geo.h"
#include "crystals_geo.h"
#include "display_set.h"




static GLuint program;
static GLuint flowerVertexID;
static GLuint flowerColorID;
static GLuint flower_vIndexID;
//static GLuint geo1VertexID;
//static GLuint geo1ColorID;
//static GLuint geo1_vIndexID;

//static GLfloat flowerMultiColors[][4];
//static GLfloat colorHop;

static long anim_lastFired_timestamps[20];





 
static GLuint mvpLoc;
static GLuint positionLoc;
static GLuint colorLoc;
static glMatrix mprojectionMat1;
static glMatrix *projectionMat1=&mprojectionMat1;
//static glMatrix mprojectionMat2;
//static glMatrix *projectionMat2=&mprojectionMat2;
static glMatrix mmodelviewMat1;
static glMatrix *modelviewMat1=&mmodelviewMat1;
static glMatrix mmvpMat1;
static glMatrix *mvpMat1=&mmvpMat1;

//static glMatrix *appliedModelTranformMatrix;
static GLuint matrixAttrLoc;
 
static GLint surface_width, surface_height;

static GLfloat animStates[20];//going to use 20 numeric slots for now


//projection
static GLfloat near = -20.0f;
static GLfloat far  = 20.0f;
static GLfloat yFOV  = 75.0f;

static GLuint numberOfPetals = 5;
static GLfloat flowerScale = 1.5f;




//}}} end static vars + shaders       
        
//{{{ geo       
        
// // // // // // // // // // one way of notating the tetra.  don't know how to combine with GLSL approach  vvvvvvvvvvv
//static const GLfloat tetrahedronVerts[4][3] = {
//        {  0.5,  0.5,  0.5  },   /* index 0 */
//        { -0.5, -0.5,  0.5  },   /* index 1 */
//        { -0.5,  0.5, -0.5  },   /* index 2 */
//        {  0.5, -0.5, -0.5  } }; /* index 3 */
#ifdef future_or_something
static const GLfloat tetrahedronColors[4][4] = {
        {  1.0,  1.0,  0.0, 1.0  },   /* red */
        {  0.0,  1.0,  0.0, 0.4   },   /* green */
        {  0.0,  0.0,  1.0, 1.0   },   /* blue */
        {  1.0,  1.0,  1.0, 1.0   } }; /* white */

static const GLushort tetrahedronIndices[12] = //GL_TRIANGLE_STRIP version- see glDrawElements { 0, 1, 2, 3, 0, 1 } remember [6];
{
 0,1,2,//rgb
 2,1,3,
 2,3,0,//rwb
 0,3,1};//


//}}} end geo
#endif

void justin2_rewriteColors(GLfloat targetVertices[], GLfloat colorVertices[]){
//	int numVertices = arraySlotCountf(colorVertices,4);
	
}

void justin2_makeColorVariations(int numVariations, int startingVariation) {
  //  GLfloat petal1_24hues[24][petal1VertCount][4];
	GLfloat colorHop = 15.f/360.f;
  //  GLfloat petal1ColorsTemp[petal1VertCount][4];
//    logit("array counter test ::::.. petal1ColorsTemp is %d", arraySlotCountf(petal1Colors,4));//
    for(int hueCounter = startingVariation; hueCounter < (numVariations+startingVariation); hueCounter ++){//set up for 24 turned # down for testing
       for(int vertexCounter = 0; vertexCounter < petal1VertCount; vertexCounter++){

//           logit("a color vertex # %d::.. %f   %f    %f ", vertexCounter, petal1Colors[vertexCounter][0], petal1Colors[vertexCounter][1],  petal1Colors[vertexCounter][2]);
		   float hsv[3];
		   rgbToHsv(petal1Colors[vertexCounter][0], petal1Colors[vertexCounter][1],  petal1Colors[vertexCounter][2],hsv);
           static rgb RGB;//change hsv / rgbToHsv() to be the same pattern using struct
		   static GLfloat newHue;
           newHue = (colorHop * hueCounter) + hsv[0];
		   if(newHue>1.0f) newHue = newHue - 1.0f;
//		   logit("new Hue is ::.. %f", newHue);
		   
		   RGB = hsv2rgb((double) newHue, (double) hsv[1], (double) hsv[2]);
//           logit("original vertex Value-r::.. %f    new Vertex Value-r   %f",petal1Colors[vertexCounter][0],RGB.r);
		   petal1Colors[vertexCounter][0] = RGB.r;
		   petal1Colors[vertexCounter][1] = RGB.g;
		   petal1Colors[vertexCounter][2] = RGB.b;
		   petal1Colors[vertexCounter][3] = 1.0f;
		   
       } 

    }

    
}

void justin2_switchDirection(){
	 //	
	 GLfloat newSpeed = ((rand() % 600)*0.01)-3.0f;//-3 to +3
      // logit("newSpeed=============== %f", newSpeed);
		animStates[0] = newSpeed;
		GLuint hueHop = (rand() % 5);
//		logit("hue Hop number %d", hueHop);
		justin2_makeColorVariations(1,hueHop);
		
}

void justin2_newTimingInterval(){
		justin2_switchDirection();

}





          
static int justin2_init() {
//    glClearDepthf(1.0f);
//    checkGlError("glClearDepth");
//    glEnable(GL_DEPTH_TEST); 
//    checkGlError("glDepthTest");
//    glDepthRangef(0.0,1.0);
//    checkGlError("glDepthRangef");
GLint status;

 
  
 
program = sr_create_program(gStandard3dVertexVaryingColorShader,gStandard3dFragmentVaryingColorShader);

 
// We don't need the shaders anymore - the program is enough


mvpLoc = glGetUniformLocation(program, "u_mvpMat");
checkGlError("glGetUniformLoaction");
positionLoc = glGetAttribLocation(program, "a_position");
    checkGlError("glGetUniformLoaction");
colorLoc = glGetAttribLocation(program, "a_color");
    checkGlError("glGetUniformLoaction");
matrixAttrLoc = glGetAttribLocation(program, "myMatrix");
    checkGlError("glGetAttribLocation");
//}}} end env + shader setup

//{{{ main Model View matrix setup

    //Model View matrix setup
    //loads blank matrix
    loadIdentity( modelviewMat1 );

//}}} end modelView matrix setup

animStates[0] = 1.0f; //0 - rotation speed layer1 (flower)




//generate the empty buffers - may want to move per instance transforms into a buffer, for now just handling at render output with changes to uniform
 glGenBuffers(1, &flowerVertexID);
    checkGlError("glGenBuffers1");
 glGenBuffers(1, &flowerColorID);
    checkGlError("glGenBuffers2");
    glGenBuffers(1, &flower_vIndexID);
    checkGlError("glGenBuffers3");
    
justin2_makeColorVariations(1,19);
    
//{{{ send geo to buffers 
 //make depth buffer
    
	
    glBindBuffer(GL_ARRAY_BUFFER, flowerVertexID);
    checkGlError("glbindbuffer");


    /*vvvvvvvvv*/
    
    //ORIG glBufferData(GL_ARRAY_BUFFER, sizeof(petal1Verts), petal1Verts,GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(petal1Verts), petal1Verts,GL_STATIC_DRAW);
    checkGlError("glbufferDatabb");
  
/*^^^^^^^^^^^^^*/
    glBindBuffer(GL_ARRAY_BUFFER, flowerColorID);
    checkGlError("glbindbuffer2");

    glBufferData(GL_ARRAY_BUFFER, sizeof(petal1Colors), petal1Colors,
                 GL_STATIC_DRAW);//means it can be optimized for not being changed
    checkGlError("glbufferdata2");


////////index buffer "IBO"/////////////////////////////////////////////////////
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flower_vIndexID);
    checkGlError("glbindbuffer3");
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(petal1Indices), petal1Indices, GL_STATIC_DRAW);
    checkGlError("glbufferdata3");
  
 // end per instance loop


//}}} send geo to buffers
return 0;
} //end justin2_init()








int justin2_setupGraphics(int w, int h,int drawtype) {
//logit("justin2_setupGraphiccs w %d h %d dt %d   setup_before %d\n"
// ,w,h,drawtype,ran_setup_graphics_once_before);
	
surface_width = w;
surface_height = h;
if (ran_setup_graphics_once_before==0)  {
     if (justin2_init()==EXIT_FAILURE) {
//         logit("justin2 exit failure\n");
                     return(EXIT_FAILURE);
     }
 }

//{{{ projectionMat1 matrix setup
//*****PROJECTION******** vvvvvvvvvvvvvvv
loadIdentity( projectionMat1 );
 
GLfloat aspect = (float)surface_width/(float)surface_height;
GLfloat height = tanf( yFOV / 360.0f * M_PI ) * near;
GLfloat width = height * aspect;
 

frustumMatrix( projectionMat1, -width, width, -height, height, near,
               far );
if ( surface_width > surface_height ){
    scaleMatrix( projectionMat1, (float)surface_height/(float)surface_width,
                 1.0f, 1.0f );
} else {
    scaleMatrix( projectionMat1, 1.0f,
                 (float)surface_width/(float)surface_height, 1.0f );
}

//}}} end projectionMat1 setup


if (ran_setup_graphics_once_before==0)  {
  //////////////////////////"permanent transforms"///////////////////
//  logit("justin2 set pernanamet transforms\n");

  scaleMatrix(modelviewMat1,flowerScale,flowerScale,flowerScale);
  //translateMatrix( modelviewMat1,-0.0f,-0.4f,0.0f); //pretty good with current matrix setup
  translateMatrix( modelviewMat1,-0.f,-0.4f,0.0f); 
  rotationMatrix( modelviewMat1, 40.0f, 1.0f, 0.2f, 0.0f);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
  }

return 1;
}















void justin2_renderBackground(int screen,glMatrix *eye) {
}

static int loopSec=4;

void justin2_step() {
    rotationMatrix( modelviewMat1, animStates[0], -0.0f, 0.0f, 1.0);//animated spin, using animStates[0]
	multMatrix( mvpMat1, modelviewMat1, projectionMat1);// create actual mvp matrix for use as a uniform

	struct timeval moment;
	sr_gettimeofday(&moment, NULL);
	//
	// // // // // // // // //  
	anim_lastFired_timestamps[0] = (moment.tv_sec%10000)*1000000 + moment.tv_usec;
     loopSec = moment.tv_sec%3;
 	if(loopSec == 0){
 		if(moment.tv_usec < 25000) justin2_newTimingInterval();//cheap way of detecting just one event per new second-based timer.   replace with proper interval.
    }
}



void justin2_renderFrame(int screen,glMatrix *eye) {
//    logit("justin2_renderframe");

    glUseProgram(program);
    checkGlError("glUseProgram");

   if (loopSec==0) {
       //for immediate testing, rewrite the vbo for color each time this runs, then well worry about proper cache management.
    glBindBuffer(GL_ARRAY_BUFFER, flowerColorID);
    checkGlError("glBindBufferaa");
//    logit("flower color id is %d",flowerColorID);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(petal1Colors), petal1Colors,
                 GL_STATIC_DRAW);//means it can be optimized for not being changed
    checkGlError("glBufferDataaa");
    
    }

// Enable and bind the vertex information
glEnableVertexAttribArray(positionLoc);
checkGlError("glEnableVertexAttribArray");

glBindBuffer(GL_ARRAY_BUFFER, flowerVertexID);
checkGlError("glBindBuffer");
glVertexAttribPointer(positionLoc, 4, GL_FLOAT, GL_FALSE,
                      4 * sizeof(GLfloat), 0);//(index, size of component, dataType, normalize_yn, bytesize ie stride, startingPos)
checkGlError("glVertixAttibPointer1");		      
 // Enable and bind the color information
glEnableVertexAttribArray(colorLoc);
checkGlError("glEnableVertexarray colorloc");
glBindBuffer(GL_ARRAY_BUFFER, flowerColorID);
checkGlError("glBindBuffer32");
glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE,
                      4 * sizeof(GLfloat), 0);//(index, size of component, dataType, normalize_yn, bytesize ie stride, startingPos)
checkGlError("glVertexAttribPointer");


 

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flower_vIndexID);
checkGlError("glBindBuffer");


	//draw multiple petals with matrix changed between each
	for(GLuint petalNumber = 0;petalNumber<numberOfPetals;petalNumber++){
		rotationMatrix( mvpMat1, 72.0f, 0.0f, 0.1f, 3.0f);
		glUniformMatrix4fv(mvpLoc, 1, 0, &mvpMat1->mat[0][0]);
        
       //
        glDrawElements(GL_TRIANGLES, 42*3, GL_UNSIGNED_SHORT, (void *)0);//tetra-12 icosa-60
//        glDrawElements(GL_TRIANGLES, 42*3, GL_UNSIGNED_SHORT, flower_vIndexID);//tetra-12 icosa-60
    checkGlError("glDrawElementsj2");
	}

    
	glDisableVertexAttribArray(positionLoc);
checkGlError("glDisableVertexAttribArray1");
	glDisableVertexAttribArray(colorLoc);
checkGlError("glDisableVertexAttribArray2");

//donee:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGlError("glBindBuffer1");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    checkGlError("glBindBuffer2");
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    checkGlError("glBindBuffer3");
//    logit("done justin2_renderframe");

}



void justin2_renderForeground() {
}














static int render_phase=0; /* 0 to 60 frames  just for the button */


void justin2_stepButton() {
render_phase = (render_phase+1)%150;
}

void justin2_renderButton(int xxprogram,int mColorHandle,int menu_position_handle,glMatrix *smallbox,int screen,glMatrix *eye) {
//    logit("justin2_renderbutton");

//if (packet_summary.current_frame==-1) return;

    glUseProgram(program);
    checkGlError("glUseProgram");

// Enable and bind the vertex information
glEnableVertexAttribArray(positionLoc);
checkGlError("glEnableVertexAttribArray");

glBindBuffer(GL_ARRAY_BUFFER, flowerVertexID);
checkGlError("glBindBuffer");
glVertexAttribPointer(positionLoc, 4, GL_FLOAT, GL_FALSE,
                      4 * sizeof(GLfloat), 0);//(index, size of component, 
//		      dataType, normalize_yn, bytesize ie stride, startingPos)
checkGlError("glVertixAttibPointer1");		      
 // Enable and bind the color information
glEnableVertexAttribArray(colorLoc);
checkGlError("glEnableVertexarray colorloc");
glBindBuffer(GL_ARRAY_BUFFER, flowerColorID);
checkGlError("glBindBuffer33");
glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE,
                      4 * sizeof(GLfloat), 0);//(index, size of component, dataType, normalize_yn, bytesize ie stride, startingPos)
checkGlError("glVertexAttribPointer");

 //attempting to add depth buffer for z testing - not sure it's set up right though
 // hib - comment this out because it errors???
// checkGlError("glFramebufferRenderbuffer");

 

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flower_vIndexID);
checkGlError("glBindBuffer");


scaleMatrix(smallbox,flowerScale*0.75f,flowerScale*0.75f,flowerScale*0.75f);
rotationMatrix( smallbox, 40.0f, 1.0f, 0.2f, 0.0f);
rotationMatrix( smallbox, 360.f*(float)render_phase*0.006666f, -0.0f, 0.0f, 1.0);//animated spin, using animStates[0]

	

	//draw multiple petals with matrix changed between each
	for(GLuint petalNumber = 0;petalNumber<numberOfPetals;petalNumber++){
		rotationMatrix( smallbox, 72.0f, 0.0f, 0.1f, 3.0f);
		glUniformMatrix4fv(mvpLoc, 1, 0, (GLfloat *)smallbox);
        
       //
        glDrawElements(GL_TRIANGLES, 42*3, GL_UNSIGNED_SHORT, (void *)0);//tetra-12 icosa-60
//        glDrawElements(GL_TRIANGLES, 42*3, GL_UNSIGNED_SHORT, flower_vIndexID);//tetra-12 icosa-60
    checkGlError("glDrawElements");
	}

    
	glDisableVertexAttribArray(positionLoc);
checkGlError("glDisableVertexAttribArray1");
	glDisableVertexAttribArray(colorLoc);
checkGlError("glDisableVertexAttribArray2");

//donee:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGlError("glBindBuffer1");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    checkGlError("glBindBuffer2");
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    checkGlError("glBindBuffer3");
    
//    logit("done justin2_renderbutton");
}






// WORLD_INTERNATIONAL off
const display_set justin2_display_set = {
  .name = "justin2",
  .setupGraphics = justin2_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .step = justin2_step,
  .renderBackground = justin2_renderBackground,
  .renderMain  = justin2_renderFrame,
  .renderForeground = justin2_renderForeground,
  .stepButton = justin2_stepButton,
  .renderButton = justin2_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = NULL
  };
// WORLD_INTERNATIONAL on

/*
$Log: justin2.c,v $
Revision 1.31  2019/05/17 00:39:01  hib
this would be the silentradiance 1/3l version

Revision 1.30  2019/05/03 16:28:10  hib
wip. the impact font is goofed up again.

Revision 1.29  2019/05/03 12:55:04  hib
Cleaned up justin. Added milkdrop back to linux client

Revision 1.28  2019/05/03 12:26:57  hib
from non cvs

Revision 1.22  2019/03/17 12:58:25  hib
Updated copyrights

Revision 1.21  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

Revision 1.20  2018/12/22 07:26:17  hib
the demo silentradiance is not processing files for some reason.  but we are progressing further along making a video for a song.

Revision 1.19  2018/09/28 17:46:46  hib
Version 1.2 - after apple. Linting warnings and errors.
Also - trying to fix perspective of larryharvey in android

Revision 1.18  2018/03/08 00:33:41  hib
minor changes trying to get AMD EGL working.

Revision 1.17  2018/03/04 04:15:35  hib
found a bug the step was doing gl commands!

Revision 1.16  2018/02/22 21:57:55  hib
Help modularize the display visualizers much more - for display_set - need to do for the main menu too
Also - added step/draw initial logic, to make it more likely that we can render split screen for goggles.

Revision 1.15  2018/01/20 22:10:07  hib
added 2018

Revision 1.14  2017/12/01 16:57:35  hib
image pretty much done - sometimes doesn't show up

Revision 1.13  2017/12/01 12:48:27  hib
Did a bit of a revamp for the graphics - cleaning out glViewport, added scissors that should clip the buttons on cheap phones.
Better set up to handle 3d, and the "third eye" idea.

Revision 1.12  2017/12/01 04:45:48  hib
Made all the graphical apps check to see if ran before so we don't recreate all the programs, etc.
just before we did it, images was close to working on android.
Thats our main focus right now.

Revision 1.11  2017/11/08 09:38:06  hib
found a problem or two

Revision 1.10  2017/11/02 03:36:39  hib
got rid of lowp doesnt compile on nvidia hardware.

Revision 1.9  2017/11/02 03:19:03  hib
Took out dept render stuff in justin2.c - caused win to blow up and looks like it isnt needed.

Revision 1.8  2017/07/05 12:40:10  hib
Added copyleft

Revision 1.7  2017/04/06 11:41:16  hib
clean up linux client ifdef

Revision 1.6  2017/04/06 09:37:20  hib
Making multiple direction. Fix the tv icons. Clean up the tv

Revision 1.5  2017/04/04 09:08:37  hib
Got it to run on linux as a client! yo!

Revision 1.4  2016/12/29 19:16:14  justin
Some cleanup - about to get back into the actual coding.   Still needs plenty of tidying up

Revision 1.3  2016/12/29 18:45:15  justin
Moved back in the tweening / multiple anim params work from summer 016 - removed mallocs

Revision 1.2  2016/12/21 01:32:25  hib
consolidating...

Revision 1.1  2016/12/20 23:45:38  hib
consolidating

Revision 1.8  2016/07/21 07:18:26  hib
Fixed bugs in justins code. Yeah!

Revision 1.7  2016/07/14 09:23:10  hib
cool

Revision 1.5  2016/07/14 06:21:12  hib
ran through the mac.

Revision 1.4  2016/07/13 22:01:29  hib
sync with justin

*/
