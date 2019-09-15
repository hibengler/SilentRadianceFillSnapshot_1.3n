/*
Since this is derived from raylib, this has a dual licese zlib/libpng and GNU LGPL 2.0

*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2018 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*

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

/*$Revision: 1.27 $*/
/* This is starting out as a stub for all the hooks into raylib to "borrow" the 
VR stuff, which should be able to stand alone anyways.
Hopefully I can get this working, and working on the ios platform before
1.3 launch
*/

/* Revision 1.3 is do nothing   we go forward from there */

// WORLD_INTERNATIONAL off
// ^^ hint is unrelated to raylib


/*$Revision: 1.27 $*/

#ifdef ANDROID
#include <jni.h>
#endif


/* matricies:

raymath had in line matrix functions - I brought over the minimum of that
then there are th rl... functions that multiply current matrix with other matrixes
current matric is either the model view matrix, or the prograction matrix, and one can switch back and forth
and there are a bunc of (semi redundant) calls to set that,

The current_matrix is pushed or popped for work on the stack.  We switch to empewrical mode
The matricies should be 1:1 with the openGL Es 2.0 matrixes in the rest of silentradiance, but why chance it.
So I dont switch between the two matrices.



There is alot missing in the initWindow - including setting up the "Shader"
Shader -> program - with the vertex and fragment code.
Took a while to figure that one out.


*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glue.h"
#include "default.h"
#include "packet_summary.h"
#include "display_set.h"
#include "display_main.h"
#include "helper.h"
#include "not_rl.h"
#include "main_menu.h"

#include "letters_render_opengles2.h"






// Default vertex attribute names on shader to set location points
#define NOT_RL_DEFAULT_ATTRIB_POSITION_NAME    "vertexPosition"    // shader-location = 0
#define NOT_RL_DEFAULT_ATTRIB_TEXCOORD_NAME    "vertexTexCoord"    // shader-location = 1
#define NOT_RL_DEFAULT_ATTRIB_NORMAL_NAME      "vertexNormal"      // shader-location = 2
#define NOT_RL_DEFAULT_ATTRIB_COLOR_NAME       "vertexColor"       // shader-location = 3
#define NOT_RL_DEFAULT_ATTRIB_TANGENT_NAME     "vertexTangent"     // shader-location = 4
#define NOT_RL_DEFAULT_ATTRIB_TEXCOORD2_NAME   "vertexTexCoord2"   // shader-location = 5
 


//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static not_rl_Vector2 not_rl_cameraAngle = { 0.0f, 0.0f };          // TODO: Remove! Compute it in UpdateCamera()
static float not_rl_cameratargetdistance = 0.0f;             // TODO: Remove! Compute it in UpdateCamera()
static float not_rl_playerEyesPosition = 1.85f;              // Default player eyes position from ground (in meters)
 
 
static int cameraMode = NOT_RL_CAMERA_CUSTOM;                // Current camera mode

static not_rl_bool not_rl_windowReady = not_rl_false;                // Check if window has been 
// Compressed textures support flags
static not_rl_bool not_rl_texCompDXTSupported = not_rl_false;    // DDS texture compression support  
static not_rl_bool not_rl_texNPOTSupported = not_rl_false;       // NPOT textures full support
static not_rl_bool not_rl_texFloatSupported = not_rl_false;      // float textures support (32 bit p$

// White texture useful for plain color polys (required by shader)
// dont need it static unsigned int not_rl_whiteTexture;   


// Display size-related data
static unsigned int not_rl_displayWidth, not_rl_displayHeight; // Display width and height (monitor, device-screen, LCD, ...)
static int not_rl_screenWidth, not_rl_screenHeight;       // Screen width and height (used render area)
static int not_rl_renderWidth, not_rl_renderHeight;       // Framebuffer width and height (render area, including black bars if required)
static int not_rl_renderOffsetX = 0;               // Offset X from render area (must be divided by 2)
static int not_rl_renderOffsetY = 0;               // Offset Y from render area (must be divided by 2)
static not_rl_Matrix not_rl_downscaleView;                // Matrix to downscale view (in case screen size bigger than display size)

static int not_rl_currentMatrixMode= NOT_RL_PROJECTION;

static int not_rl_vrSimulatorReady = not_rl_false;
static not_rl_bool not_rl_vrStereoRender = not_rl_false; // VR stereo rendering enabled/disabled flag


static float not_rl_currentDepth = -1.0f;


static not_rl_DynamicBuffer not_rl_quads;                 // Default dynamic buffer for quads data (used to draw textures)

// Default buffers draw calls
static not_rl_DrawCall *not_rl_draws;
static int not_rl_drawsCounter;




static not_rl_bool not_rl_vaoSupported = not_rl_false;           // VAO support (OpenGL ES2 could not support VAO extension)

// Extension supported flag: Compressed textures
static not_rl_bool not_rl_texCompETC1Supported = not_rl_false;   // ETC1 texture compression support
static not_rl_bool not_rl_texCompETC2Supported = not_rl_false;   // ETC2/EAC texture compression sup$
static not_rl_bool not_rl_texCompPVRTSupported = not_rl_false;   // PVR texture compression support  
static not_rl_bool not_rl_texCompASTCSupported = not_rl_false;   // ASTC texture compression support

static not_rl_bool not_rl_debugMarkerSupported = not_rl_false;

// Extension supported flag: Anisotropic filtering
static not_rl_bool not_rl_texAnisotropicFilterSupported = not_rl_false;  // Anisotropic texture filt$
static float not_rl_maxAnisotropicLevel = 0.0f;        // Maximum anisotropy level sup$

// Extension supported flag: Clamp mirror wrap mode
static not_rl_bool not_rl_texClampMirrorSupported = not_rl_false;    // Clamp mirror wrap mode suppo$


not_rl_VrStereoConfig not_rl_vrConfig;         // VR stereo configuration for simulator


static unsigned int not_rl_CompileShader(const char *shaderStr, int type);     // Compile custom shader and return shader id
static void not_rl_LoadBuffersDefault(void);       // Load default internal buffers (lines, triangles, quads)
     // well, just quads for us

static not_rl_Matrix not_rl_modelview;
static not_rl_Matrix not_rl_projection;
//static not_rl_Matrix *currentMatrix;

/* stuff from main menu to help set things up */
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






/* distortionshader - this is the magic - 
edited to just be opengles 2.0
*/
// Vertex shader definition to embed, no external file required
static const char not_rl_distortionVShaderStr[] = 
"#version 100                       \n"
"attribute vec3 vertexPosition;     \n"
"attribute vec2 vertexTexCoord;     \n"
"attribute vec4 vertexColor;        \n"
"varying vec2 fragTexCoord;         \n"
"varying vec4 fragColor;            \n"
"uniform mat4 mvp;            \n"
"void main()                        \n"
"{                                  \n"
"    fragTexCoord = vertexTexCoord; \n"
"    fragColor = vertexColor;       \n"
"    gl_Position = mvp*vec4(vertexPosition, 1.0); \n"
"}                                  \n";

// Fragment shader definition to embed, no external file required
static const char not_rl_distortionFShaderStr[] = 
"#version 100                       \n"   
"precision mediump float;           \n"
"varying vec2 fragTexCoord;         \n"
"varying vec4 fragColor;            \n"
"uniform sampler2D texture0;                                     \n"
"uniform vec2 leftLensCenter;       \n"
"uniform vec2 rightLensCenter;      \n"
"uniform vec2 leftScreenCenter;     \n"
"uniform vec2 rightScreenCenter;    \n"
"uniform vec2 scale;                \n"
"uniform vec2 scaleIn;              \n"
"uniform vec4 hmdWarpParam;         \n"
"uniform vec4 chromaAbParam;        \n"
"void main() \n"
"{ \n"
"   vec2 lensCenter = fragTexCoord.x < 0.5 ? leftLensCenter : rightLensCenter; \n"
"   vec2 screenCenter = fragTexCoord.x < 0.5 ? leftScreenCenter : rightScreenCenter; \n"
"   vec2 theta = (fragTexCoord - lensCenter)*scaleIn; \n"
"   float rSq = theta.x*theta.x + theta.y*theta.y; \n"
"   vec2 theta1 = theta*(hmdWarpParam.x + hmdWarpParam.y*rSq + hmdWarpParam.z*rSq*rSq + hmdWarpParam.w*rSq*rSq*rSq); \n"
"   vec2 thetaBlue = theta1*(chromaAbParam.z + chromaAbParam.w*rSq); \n"
"   vec2 tcBlue = lensCenter + scale*thetaBlue; \n"
"   if (any(bvec2(clamp(tcBlue, screenCenter - vec2(0.25, 0.5), screenCenter + vec2(0.25, 0.5)) - tcBlue))) \n"
"   { \n"
"       gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0); \n"
"   } \n"
"   else \n"
"   { \n"
"       float blue = texture2D(texture0, tcBlue).b; \n"
"       vec2 tcGreen = lensCenter + scale*theta1; \n"
"       float green = texture2D(texture0, tcGreen).g; \n"
"       vec2 thetaRed = theta1*(chromaAbParam.x + chromaAbParam.y*rSq); \n"
"       vec2 tcRed = lensCenter + scale*thetaRed; \n"
"       float red = texture2D(texture0, tcRed).r; \n"
"       gl_FragColor = vec4(red, green, blue, 1.0); \n"
"    } \n"
"} \n";

static unsigned int not_rl_LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId);  // Load custom shader program
static void not_rl_SetShaderDefaultLocations(not_rl_Shader *shader); // Bind default shader locations (attributes and uniforms)







// just for quads in not_rl
// Update default internal buffers (VAOs/VBOs) with vertex array data
// NOTE: If there is not vertex data, buffers doesn't need to be updated (vertexCount > 0)
// TODO: If no data changed on the CPU arrays --> No need to re-update GPU arrays (change flag required)
static void not_rl_UpdateBuffersDefault(void)
{
//logit("not_rl_UpdateBuffersDefault\n");
    // Update not_rl_quads vertex buffers
    if (not_rl_quads.vCounter > 0)
    {
        // Activate Quads VAO
        if (not_rl_vaoSupported) glBindVertexArray(not_rl_quads.vaoId);

        // Quads - vertex positions buffer
        glBindBuffer(GL_ARRAY_BUFFER, not_rl_quads.vboId[0]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*4*NOT_RL_MAX_QUADS_BATCH, 
	//    not_rl_quads.vertices, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*3*not_rl_quads.vCounter, not_rl_quads.vertices);

        // not_rl_quads - texture coordinates buffer
        glBindBuffer(GL_ARRAY_BUFFER, not_rl_quads.vboId[1]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*4*NOT_RL_MAX_QUADS_BATCH, not_rl_quads.texcoords, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*2*not_rl_quads.vCounter, not_rl_quads.texcoords);

        // Quads - colors buffer
        glBindBuffer(GL_ARRAY_BUFFER, not_rl_quads.vboId[2]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*4*NOT_RL_MAX_QUADS_BATCH, 
	//  not_rl_quads.colors, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned char)*4*not_rl_quads.vCounter, 
	  not_rl_quads.colors);

        // Another option would be using buffer mapping...
        //not_rl_quads.vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
        // Now we can modify vertices
        //glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    //--------------------------------------------------------------

    // Unbind the current VAO
    if (not_rl_vaoSupported) glBindVertexArray(0);
}


static void not_rl_DrawBuffersDefault(void);



static not_rl_bool not_rl_InitGraphicsDevice(int width, int height);  // Initialize graphics
static void not_rl_SetupViewport(void);





int not_rl_setupGraphics(int w, int h,int graphics_mode) {
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



static void not_rl_SetupFramebufferSize(int displayWidth, int displayHeight) {
if (displayWidth<displayHeight) {
  int t=displayWidth;
  displayWidth = displayHeight;
  displayHeight=t;
  }

        not_rl_renderWidth = not_rl_screenWidth;
        not_rl_renderHeight = not_rl_screenHeight;   
        not_rl_renderOffsetX = 0;
        not_rl_renderOffsetY = 0;
}




void not_rl_InitWindow(int width, int height, void *data)
{
if (width<height) {
  int t=width;
  width = height;
  height=t;
  }
not_rl_screenWidth=width;
not_rl_screenHeight=height;
not_rl_downscaleView = not_rl_MatrixIdentity();
//	call glfw_init
not_rl_displayWidth = width;
not_rl_displayHeight = height;
not_rl_SetupFramebufferSize(not_rl_displayWidth, not_rl_displayHeight);


// Init graphics device (display device and OpenGL context)
// NOTE: returns true if window and graphic device has been initialized suc$
not_rl_windowReady = not_rl_InitGraphicsDevice(width, height);
if (!not_rl_windowReady) {
  logit("not_rl_InitWindow Window is not ready %d,%d\n",width,height);
  return;
  }	    



}



// Get VR device information for some standard devices
not_rl_VrDeviceInfo not_rl_GetVrDeviceInfo(int vrDeviceType)
{
    not_rl_VrDeviceInfo hmd = { 0 };                // Current VR device info
    
    switch (vrDeviceType)
    {
        case NOT_RL_HMD_DEFAULT_DEVICE:
        case NOT_RL_HMD_OCULUS_RIFT_CV1:
        {
            // Oculus Rift CV1 parameters
            // NOTE: CV1 represents a complete HMD redesign compared to previous versions,
            // new Fresnel-hybrid-asymmetric lenses have been added and, consequently,
            // previous parameters (DK2) and distortion shader (DK2) doesn't work any more.
            // I just defined a set of parameters for simulator that approximate to CV1 stereo rendering
            // but result is not the same obtained with Oculus PC SDK.
            hmd.hResolution = 2160;                 // HMD horizontal resolution in pixels
            hmd.vResolution = 1200;                 // HMD vertical resolution in pixels
            hmd.hScreenSize = 0.133793f;            // HMD horizontal size in meters
            hmd.vScreenSize = 0.0669f;              // HMD vertical size in meters
            hmd.vScreenCenter = 0.04678f;           // HMD screen center in meters
            hmd.eyeToScreenDistance = 0.041f;       // HMD distance between eye and display in meters
            hmd.lensSeparationDistance = 0.07f;     // HMD lens separation distance in meters
            hmd.interpupillaryDistance = 0.07f;     // HMD IPD (distance between pupils) in meters
            hmd.lensDistortionValues[0] = 1.0f;     // HMD lens distortion constant parameter 0
            hmd.lensDistortionValues[1] = 0.22f;    // HMD lens distortion constant parameter 1
            hmd.lensDistortionValues[2] = 0.24f;    // HMD lens distortion constant parameter 2
            hmd.lensDistortionValues[3] = 0.0f;     // HMD lens distortion constant parameter 3
            hmd.chromaAbCorrection[0] = 0.996f;     // HMD chromatic aberration correction parameter 0
            hmd.chromaAbCorrection[1] = -0.004f;    // HMD chromatic aberration correction parameter 1
            hmd.chromaAbCorrection[2] = 1.014f;     // HMD chromatic aberration correction parameter 2
            hmd.chromaAbCorrection[3] = 0.0f;       // HMD chromatic aberration correction parameter 3
            
//            logit( "Initializing VR Simulator (Oculus Rift CV1)");
        } break;
        case NOT_RL_HMD_OCULUS_RIFT_DK2:
        {
            // Oculus Rift DK2 parameters
            hmd.hResolution = 1280;                 // HMD horizontal resolution in pixels
            hmd.vResolution = 800;                  // HMD vertical resolution in pixels
            hmd.hScreenSize = 0.14976f;             // HMD horizontal size in meters
            hmd.vScreenSize = 0.09356f;             // HMD vertical size in meters
            hmd.vScreenCenter = 0.04678f;           // HMD screen center in meters
            hmd.eyeToScreenDistance = 0.041f;       // HMD distance between eye and display in meters
            hmd.lensSeparationDistance = 0.0635f;   // HMD lens separation distance in meters
            hmd.interpupillaryDistance = 0.064f;    // HMD IPD (distance between pupils) in meters
            hmd.lensDistortionValues[0] = 1.0f;     // HMD lens distortion constant parameter 0
            hmd.lensDistortionValues[1] = 0.22f;    // HMD lens distortion constant parameter 1
            hmd.lensDistortionValues[2] = 0.24f;    // HMD lens distortion constant parameter 2
            hmd.lensDistortionValues[3] = 0.0f;     // HMD lens distortion constant parameter 3
            hmd.chromaAbCorrection[0] = 0.996f;     // HMD chromatic aberration correction parameter 0
            hmd.chromaAbCorrection[1] = -0.004f;    // HMD chromatic aberration correction parameter 1
            hmd.chromaAbCorrection[2] = 1.014f;     // HMD chromatic aberration correction parameter 2
            hmd.chromaAbCorrection[3] = 0.0f;       // HMD chromatic aberration correction parameter 3
            
//            logit( "Initializing VR Simulator (Oculus Rift DK2)");
        } break; 
        case NOT_RL_HMD_OCULUS_GO:
        {
            // TODO: Provide device display and lens parameters
        }
        case NOT_RL_HMD_VALVE_HTC_VIVE:
        {
            // TODO: Provide device display and lens parameters
        }
        case NOT_RL_HMD_SONY_PSVR:
        {
            // TODO: Provide device display and lens parameters
        }
        default: break;
    }
    
    return hmd;
}




// Configure stereo rendering (including distortion shader) with HMD device parameters
// NOTE: It modifies the global variable: VrStereoConfig not_rl_vrConfig
static void not_rl_SetStereoConfig(not_rl_VrDeviceInfo hmd)
{
    // Compute aspect ratio
    float aspect = ((float)hmd.hResolution*0.5f)/(float)hmd.vResolution;

    // Compute lens parameters
    float lensShift = (hmd.hScreenSize*0.25f - hmd.lensSeparationDistance*0.5f)/hmd.hScreenSize;
    float leftLensCenter[2] = { 0.25f + lensShift, 0.5f };
    float rightLensCenter[2] = { 0.75f - lensShift, 0.5f };
    float leftScreenCenter[2] = { 0.25f, 0.5f };
    float rightScreenCenter[2] = { 0.75f, 0.5f };

    // Compute distortion scale parameters
    // NOTE: To get lens max radius, lensShift must be normalized to [-1..1]
    float lensRadius = fabsf(-1.0f - 4.0f*lensShift);
    float lensRadiusSq = lensRadius*lensRadius;
    float distortionScale = hmd.lensDistortionValues[0] +
                            hmd.lensDistortionValues[1]*lensRadiusSq +
                            hmd.lensDistortionValues[2]*lensRadiusSq*lensRadiusSq +
                            hmd.lensDistortionValues[3]*lensRadiusSq*lensRadiusSq*lensRadiusSq;

    logit( "VR: Distortion Scale: %f", distortionScale);

    float normScreenWidth = 0.5f;
    float normScreenHeight = 1.0f;
    float scaleIn[2] = { 2.0f/normScreenWidth, 2.0f/normScreenHeight/aspect };
    float scale[2] = { normScreenWidth*0.5f/distortionScale, normScreenHeight*0.5f*aspect/distortionScale };

//    logit( "VR: Distortion Shader: LeftLensCenter = { %f, %f }", leftLensCenter[0], leftLensCenter[1]);
//    logit( "VR: Distortion Shader: RightLensCenter = { %f, %f }", rightLensCenter[0], rightLensCenter[1]);
//    logit( "VR: Distortion Shader: Scale = { %f, %f }", scale[0], scale[1]);
//    logit( "VR: Distortion Shader: ScaleIn = { %f, %f }", scaleIn[0], scaleIn[1]);

    // Update distortion shader with lens and distortion-scale parameters
//    logit("support distoreaitoatnb shader\n");
    not_rl_SetShaderValue(not_rl_vrConfig.distortionShader, not_rl_GetShaderLocation(not_rl_vrConfig.distortionShader, "leftLensCenter"), leftLensCenter, 2);
    not_rl_SetShaderValue(not_rl_vrConfig.distortionShader, not_rl_GetShaderLocation(not_rl_vrConfig.distortionShader, "rightLensCenter"), rightLensCenter, 2);
    not_rl_SetShaderValue(not_rl_vrConfig.distortionShader, not_rl_GetShaderLocation(not_rl_vrConfig.distortionShader, "leftScreenCenter"), leftScreenCenter, 2);
    not_rl_SetShaderValue(not_rl_vrConfig.distortionShader, not_rl_GetShaderLocation(not_rl_vrConfig.distortionShader, "rightScreenCenter"), rightScreenCenter, 2);

    not_rl_SetShaderValue(not_rl_vrConfig.distortionShader, not_rl_GetShaderLocation(not_rl_vrConfig.distortionShader, "scale"), scale, 2);
    not_rl_SetShaderValue(not_rl_vrConfig.distortionShader, not_rl_GetShaderLocation(not_rl_vrConfig.distortionShader, "scaleIn"), scaleIn, 2);
    not_rl_SetShaderValue(not_rl_vrConfig.distortionShader, not_rl_GetShaderLocation(not_rl_vrConfig.distortionShader, "hmdWarpParam"), hmd.lensDistortionValues, 4);
    not_rl_SetShaderValue(not_rl_vrConfig.distortionShader, not_rl_GetShaderLocation(not_rl_vrConfig.distortionShader, "chromaAbParam"), hmd.chromaAbCorrection, 4);


    // Fovy is normally computed with: 2*atan2(hmd.vScreenSize, 2*hmd.eyeToScreenDistance)
    // ...but with lens distortion it is increased (see Oculus SDK Documentation)
    //float fovy = 2.0f*atan2(hmd.vScreenSize*0.5f*distortionScale, hmd.eyeToScreenDistance);     // Really need distortionScale?
    float fovy = 2.0f*(float)atan2(hmd.vScreenSize*0.5f, hmd.eyeToScreenDistance);
    lensShift = lensShift * 100;
    // Compute camera projection matrices
    float projOffset = 4.0f*lensShift;      // Scaled to projection space coordinates [-1..1]
    not_rl_Matrix proj = not_rl_MatrixPerspective(fovy, aspect, 0.01, 1000.0);
    not_rl_vrConfig.eyesProjection[0] = not_rl_MatrixMultiply(proj, not_rl_MatrixTranslate(projOffset, 0.0f, 0.0f));
    not_rl_vrConfig.eyesProjection[1] = not_rl_MatrixMultiply(proj, not_rl_MatrixTranslate(-projOffset, 0.0f, 0.0f));

    // Compute camera transformation matrices
    // NOTE: Camera movement might seem more natural if we model the head.
    // Our axis of rotation is the base of our head, so we might want to add
    // some y (base of head to eye level) and -z (center of head to eye protrusion) to the camera positions.
    not_rl_vrConfig.eyesViewOffset[0] = not_rl_MatrixTranslate(-hmd.interpupillaryDistance*0.5f, 0.075f, 0.045f);
    not_rl_vrConfig.eyesViewOffset[1] = not_rl_MatrixTranslate(hmd.interpupillaryDistance*0.5f, 0.075f, 0.045f);

    // Compute eyes Viewports
    not_rl_vrConfig.eyesViewport[0] = (not_rl_Rectangle){ 0, 0, hmd.hResolution/2, hmd.vResolution };
    not_rl_vrConfig.eyesViewport[1] = (not_rl_Rectangle){ hmd.hResolution/2, 0, hmd.hResolution/2, hmd.vResolution };
}




// Set internal projection and modelview matrix depending on eyes tracking data
static void not_rl_SetStereoView(int eye, not_rl_Matrix matProjection, not_rl_Matrix matModelView)
{
    not_rl_Matrix eyeProjection = matProjection;
    not_rl_Matrix eyeModelView = matModelView;

    // Setup viewport and projection/modelview matrices using tracking data
    glViewport(eye*not_rl_screenWidth/2, 0, not_rl_screenWidth/2, not_rl_screenHeight);

    // Apply view offset to modelview matrix
    eyeModelView = not_rl_MatrixMultiply(matModelView, not_rl_vrConfig.eyesViewOffset[eye]);

    // Set current eye projection matrix
    eyeProjection = not_rl_vrConfig.eyesProjection[eye];

    not_rl_SetMatrixModelview(eyeModelView);
    not_rl_SetMatrixProjection(eyeProjection);
}


// Init VR simulator for selected device parameters
// NOTE: It modifies the global variable: VrStereoConfig not_rl_vrConfig
void not_rl_InitVrSimulator(not_rl_VrDeviceInfo info)
{
    // Initialize framebuffer and textures for stereo rendering
    // NOTE: Screen size should match HMD aspect ratio
    not_rl_vrConfig.stereoFbo = not_rl_LoadRenderTexture(not_rl_screenWidth, not_rl_screenHeight);
    
    logit("got distortion shader\n");
    // Load distortion shader
    unsigned int vertexShaderId = not_rl_CompileShader(not_rl_distortionVShaderStr, GL_VERTEX_SHADER);
    checkGlError("777d0");
    unsigned int fragmentShaderId = not_rl_CompileShader(not_rl_distortionFShaderStr, GL_FRAGMENT_SHADER);
    checkGlError("777d1");
    not_rl_vrConfig.distortionShader.id = not_rl_LoadShaderProgram(vertexShaderId, fragmentShaderId);
    checkGlError("777d2");
    if (not_rl_vrConfig.distortionShader.id > 0) not_rl_SetShaderDefaultLocations(&not_rl_vrConfig.distortionShader);


    glUseProgram(not_rl_vrConfig.distortionShader.id);
    checkGlError("extra0\n");
    // Set VR configutarion parameters, including distortion shader
    not_rl_SetStereoConfig(info);

    not_rl_vrSimulatorReady = not_rl_true;


}

void not_rl_PretendNoVr(void) {
/* in the modified raylib this allowed us to reenter into raylib to do 2d stuff or whatever in a visualizer, then reuse it as the vr 3d stuff
here its a non-op because we arent using raylib to draw anymore */
}

void not_rl_StopPretendingNoVr(void) {
/* in the modified raylib this allowed us to reenter into raylib to do 2d stuff or whatever in a visualizer, then reuse it as the vr 3d stuff
here its a non-op because we arent using raylib to draw anymore */
}






// Begin Oculus drawing configuration
void not_rl_BeginVrDrawing(void)
{
//return; // hack
    if (not_rl_vrSimulatorReady)
    {
	glUseProgram(not_rl_vrConfig.distortionShader.id);
	checkGlError("extra3\n");
        
        // Setup framebuffer for stereo rendering
        not_rl_EnableRenderTexture(not_rl_vrConfig.stereoFbo.id);

        // NOTE: If your application is configured to treat the texture as a linear format (e.g. GL_RGBA)
        // and performs linear-to-gamma conversion in GLSL or does not care about gamma-correction, then:
        //     - Require OculusBuffer format to be OVR_FORMAT_R8G8B8A8_UNORM_SRGB
        //     - Do NOT enable GL_FRAMEBUFFER_SRGB
        //glEnable(GL_FRAMEBUFFER_SRGB);

        //glViewport(0, 0, buffer.width, buffer.height);        // Useful if rendering to separate framebuffers (every eye)
        not_rl_ClearScreenBuffers();             // Clear current framebuffer(s)
        
        not_rl_vrStereoRender = not_rl_true;
    }
}



// End Oculus drawing process (and desktop mirror)
void not_rl_EndVrDrawing(void)
{
//return; // hack
    if (not_rl_vrSimulatorReady)
    {
        not_rl_vrStereoRender = not_rl_false;         // Disable stereo render
        
	glUseProgram(not_rl_vrConfig.distortionShader.id);
	checkGlError("pa0\n");
        not_rl_DisableRenderTexture();       // Unbind current framebuffer

//???
        not_rl_ClearScreenBuffers();         // Clear current framebuffer
	
	glUseProgram(not_rl_vrConfig.distortionShader.id);
	checkGlError("extra1\n");

        // Set viewport to default framebuffer size (screen size)
        glViewport(0, 0, not_rl_screenWidth, not_rl_screenHeight);
  checkGlError("p4\n");
        not_rl_currentMatrixMode = NOT_RL_PROJECTION;
        // Let rlgl reconfigure internal matrices
        not_rl_projection = not_rl_MatrixIdentity();

	not_rl_Matrix o;
        o = not_rl_MatrixOrtho(0.0, not_rl_screenWidth, not_rl_screenHeight, 0.0, 0.0, 1.0); // Recalculate internal projection matrix
        not_rl_projection = not_rl_MatrixMultiply(not_rl_projection,o);

	not_rl_currentMatrixMode = NOT_RL_MODELVIEW;
        not_rl_modelview = not_rl_MatrixIdentity();                                       // Reset internal modelview matrix

        // Draw RenderTexture (stereoFbos) using distortion shader

	glUseProgram(not_rl_vrConfig.distortionShader.id);
	checkGlError("pa0\n");
        not_rl_EnableTexture(not_rl_vrConfig.stereoFbo.texture.id);
checkGlError("p7\n");
//	glEnableVertexAttribArray(a_position_location);
//	checkGlError("glEnableVertexAttribArray");
//	glEnableVertexAttribArray(a_texture_coordinates_location);
//	checkGlError("glEnableVertexAttribArray");
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	checkGlError("glBindBuffer");
//	glClear( GL_DEPTH_BUFFER_BIT);  
//	checkGlError("glClear");
	
	
	
	
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//      checkGlError("glTexParameter");
//      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//      checkGlError("glTexParameteri");

//      GLenum type=GL_RGBA;

//      glTexImage2D(GL_TEXTURE_2D, 0, type, image->width, image->height, 0, type, GL_UNSIGNED_BYTE, (const void *)(ima$
//      logit("typ5 %d width %d height %d",type,image->width,image->height);
//      checkGlError("glTexImage2D");
//      glGenerateMipmap(GL_TEXTURE_2D);
//      checkGlError("glGenerateMipmap");

//      glBindTexture(GL_TEXTURE_2D, 0);
//      checkGlError("glGenTextures");
//      logit("texture made\n");
	
        not_rl_Matrix save_this_matrix;
        save_this_matrix = not_rl_modelview;
            not_rl_Begin(NOT_RL_QUADS);
                not_rl_Color4ub(255, 255, 255, 255);
                not_rl_Normal3f(0.0f, 0.0f, 1.0f);

                // Bottom-left corner for texture and quad
                not_rl_TexCoord2f(0.0f, 1.0f);
                not_rl_Vertex2f(0.0f, 0.0f);

                // Bottom-right corner for texture and quad
                not_rl_TexCoord2f(0.0f, 0.0f);
                not_rl_Vertex2f(0.0f, not_rl_vrConfig.stereoFbo.texture.height);

                // Top-right corner for texture and quad
                not_rl_TexCoord2f(1.0f, 0.0f);
                not_rl_Vertex2f(not_rl_vrConfig.stereoFbo.texture.width, not_rl_vrConfig.stereoFbo.texture.height);

                // Top-left corner for texture and quad
                not_rl_TexCoord2f(1.0f, 1.0f);
                not_rl_Vertex2f(not_rl_vrConfig.stereoFbo.texture.width, 0.0f);
            not_rl_End();

	
        not_rl_modelview = save_this_matrix; 

        not_rl_DisableTexture();
checkGlError("p8\n");

        // Update and draw render texture fbo with distortion to backbuffer
        not_rl_UpdateBuffersDefault();
checkGlError("p9\n");
        not_rl_DrawBuffersDefault();
checkGlError("p10\n");
        
        // Reset viewport and default projection-modelview matrices
        glViewport(0, 0, not_rl_screenWidth, not_rl_screenHeight);
checkGlError("p11aa\n");
        not_rl_projection = not_rl_MatrixOrtho(0.0, not_rl_screenWidth, not_rl_screenHeight, 0.0, 0.0, 1.0);
        not_rl_modelview = not_rl_MatrixIdentity();
        
checkGlError("p11\n");
        glDisable(GL_DEPTH_TEST);
    }
checkGlError("p12\n");
}







// Returns a random value between min and max (both included) -- from raylib
int not_rl_GetRandomValue(int min, int max)
{
    if (min > max)
    {
        int tmp = max;
        max = min;
        min = tmp;
    }

    return (rand()%(abs(max-min)+1) + min);
}




// first person camera mode from camera.h - lots of funk to use keys to move and scroll and shit
        
// Select camera mode (multiple camera modes available)
void not_rl_SetCameraMode(not_rl_Camera camera, int mode)
{   
    // TODO: not_rl_cameratargetdistance and not_rl_cameraAngle should be
    // calculated using camera parameters on UpdateCamera()
        
    not_rl_Vector3 v1 = camera.position;
    not_rl_Vector3 v2 = camera.target;
    
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;
        
    not_rl_cameratargetdistance = sqrtf(dx*dx + dy*dy + dz*dz);
        
    not_rl_Vector2 distance = { 0.0f, 0.0f };
    distance.x = sqrtf(dx*dx + dz*dz);
    distance.y = sqrtf(dx*dx + dy*dy);

    // Camera angle calculation
    not_rl_cameraAngle.x = asinf(fabsf(dx)/distance.x);  // Camera angle in plane XZ (0 aligned with Z, move positive CCW)
    not_rl_cameraAngle.y = -asinf(fabsf(dy)/distance.y); // Camera angle in plane XY (0 aligned with X, move positive CW)

    // NOTE: Just testing what not_rl_cameraAngle means
    //not_rl_cameraAngle.x = 0.0f*DEG2RAD;       // Camera angle in plane XZ (0 aligned with Z, move positive CCW)
    //not_rl_cameraAngle.y = -60.0f*DEG2RAD;     // Camera angle in plane XY (0 aligned with X, move positive CW)

    not_rl_playerEyesPosition = camera.position.y;


    cameraMode = mode;

}


void not_rl_ClearBackgroundForReals(not_rl_Color c)
{
glUseProgram(letters_opengles_program_handle());
checkGlError("not_rl_clearreals");

float r,g,b,a;
r = ((float)(c.r))*255.;
g = ((float)(c.g))*255.;
b = ((float)(c.b))*255.;
a = ((float)(c.a))*255.;
glClearColor(r,g,b,a);
checkGlError("not_rl_clearreals2");
	glUseProgram(not_rl_vrConfig.distortionShader.id);
	checkGlError("extra4\n");
}


extern void tell_apple_to_present_render_buffer(int value);
static int applehackfactor=-60;
// Load a texture to be used for rendering (fbo with color and depth attachments)
not_rl_RenderTexture2D not_rl_LoadRenderTexture(int width, int height)
{
    not_rl_RenderTexture2D target;
 
    target.id = 0;

    target.texture.id = 0;
    target.texture.width = width;
    target.texture.height = height;
    target.texture.format = NOT_RL_UNCOMPRESSED_R8G8B8A8;
    target.texture.mipmaps = 1;

    target.depth.id = 0;
    target.depth.width = width;
    target.depth.height = height;
    target.depth.format = 19;       //DEPTH_COMPONENT_24BIT
    target.depth.mipmaps = 1;
 
    logit("rendet text 2d width %d height %d\n",width,height);



    // Create the texture that will serve as the color attachment for the framebuffer
    glGenTextures(1, &target.texture.id);
    checkGlError("c0");
    glBindTexture(GL_TEXTURE_2D, target.texture.id);
    checkGlError("c1");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    checkGlError("c2");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGlError("c3");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    checkGlError("c4");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    checkGlError("c5");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    checkGlError("c6");
    glBindTexture(GL_TEXTURE_2D, 0);
    checkGlError("c7");

    // Create the renderbuffer that will serve as the depth attachment for the framebuffer.
    glGenRenderbuffers(1, &target.depth.id);
    checkGlError("e0");
 
    glBindRenderbuffer(GL_RENDERBUFFER, target.depth.id);
    checkGlError("e1");
    #ifdef __APPLE__
//    tell_apple_context_about_render_buffer();
    checkGlError("e2");
     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    checkGlError("e3");
        
    #else
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    #endif
       // GL_DEPTH_COMPONENT24 not supported on Android
    
    checkGlError("a10ss");
    // Create the framebuffer object
    glGenFramebuffers(1, &target.id);
    checkGlError("a10");
    logit("bind to %d framebuffer\n",target.id);
    glBindFramebuffer(GL_FRAMEBUFFER, target.id);
    checkGlError("a1");
 
    // Attach color texture and depth renderbuffer to FBO

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target.texture.id, 0);
    checkGlError("a2");

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target.depth.id);
    checkGlError("a3");
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    checkGlError("a5"); 
    
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        logit( "Framebuffer object could not be created...");
 
        switch (status)
        {
            case GL_FRAMEBUFFER_UNSUPPORTED: logit( "Framebuffer is unsupported"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: logit( "Framebuffer incomplete attachment"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: logit( "Framebuffer incomplete dimensions"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: logit( "Framebuffer incomplete missing attachment"); break;
            default: break;
        }
    
        glDeleteTextures(1, &target.texture.id);

        glDeleteRenderbuffers(1, &target.depth.id);
//elif defined(USE_DEPTH_TEXTURE)
        glDeleteTextures(1, &target.depth.id);
//endif
        glDeleteFramebuffers(1, &target.id);
        target.id=0;
        target.depth.id=0;
        target.texture.id=0;
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

    }
    else logit( "[FBO ID %d] Framebuffer object created successfully", target.id);
#ifdef __APPLE__
logit("gogo\n");
    glBindFramebuffer(GL_FRAMEBUFFER, applehackfactor/60);  //super hack just a guess
    if (checkGlError("a5")) {
      if ((applehackfactor % 60) == 0) {
        logit("%d did not work. Will try next number...\n",applehackfactor/60);
	}
      applehackfactor++;
      }
    // there is a way to find it
#else
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    checkGlError("a5"); 
#endif

    return target;



}





/*----------------------------------------------------------------------
Matrix stuff 
----------------------------------------------------*/
// Returns two matrix multiplication
// NOTE: When multiplying matrices... the order matters!
not_rl_Matrix not_rl_MatrixMultiply(not_rl_Matrix left, not_rl_Matrix right)
{
    not_rl_Matrix result;

    result.m0 = left.m0*right.m0 + left.m1*right.m4 + left.m2*right.m8 + left.m3*right.m12;
    result.m1 = left.m0*right.m1 + left.m1*right.m5 + left.m2*right.m9 + left.m3*right.m13;
    result.m2 = left.m0*right.m2 + left.m1*right.m6 + left.m2*right.m10 + left.m3*right.m14;
    result.m3 = left.m0*right.m3 + left.m1*right.m7 + left.m2*right.m11 + left.m3*right.m15;
    result.m4 = left.m4*right.m0 + left.m5*right.m4 + left.m6*right.m8 + left.m7*right.m12;
    result.m5 = left.m4*right.m1 + left.m5*right.m5 + left.m6*right.m9 + left.m7*right.m13;
    result.m6 = left.m4*right.m2 + left.m5*right.m6 + left.m6*right.m10 + left.m7*right.m14;
    result.m7 = left.m4*right.m3 + left.m5*right.m7 + left.m6*right.m11 + left.m7*right.m15;
    result.m8 = left.m8*right.m0 + left.m9*right.m4 + left.m10*right.m8 + left.m11*right.m12;
    result.m9 = left.m8*right.m1 + left.m9*right.m5 + left.m10*right.m9 + left.m11*right.m13;
    result.m10 = left.m8*right.m2 + left.m9*right.m6 + left.m10*right.m10 + left.m11*right.m14;
    result.m11 = left.m8*right.m3 + left.m9*right.m7 + left.m10*right.m11 + left.m11*right.m15;
    result.m12 = left.m12*right.m0 + left.m13*right.m4 + left.m14*right.m8 + left.m15*right.m12;
    result.m13 = left.m12*right.m1 + left.m13*right.m5 + left.m14*right.m9 + left.m15*right.m13;
    result.m14 = left.m12*right.m2 + left.m13*right.m6 + left.m14*right.m10 + left.m15*right.m14;
    result.m15 = left.m12*right.m3 + left.m13*right.m7 + left.m14*right.m11 + left.m15*right.m15;

    return result;
}


// Returns translation matrix
not_rl_Matrix not_rl_MatrixTranslate(float x, float y, float z)
{
    not_rl_Matrix result = { 1.0f, 0.0f, 0.0f, x,
                      0.0f, 1.0f, 0.0f, y,
                      0.0f, 0.0f, 1.0f, z,
                      0.0f, 0.0f, 0.0f, 1.0f };

    return result;
}

// Returns perspective projection matrix
not_rl_Matrix not_rl_MatrixFrustum(double left, double right, double bottom, double top, double near, double far)
{
    not_rl_Matrix result;

    float rl = (right - left);   
    float tb = (top - bottom);
    float fn = (far - near);
        
    result.m0 = (near*2.0f)/rl;
    result.m1 = 0.0f;
    result.m2 = 0.0f;
    result.m3 = 0.0f;
        
    result.m4 = 0.0f;
    result.m5 = (near*2.0f)/tb;
    result.m6 = 0.0f;
    result.m7 = 0.0f;
        
    result.m8 = (right + left)/rl;   
    result.m9 = (top + bottom)/tb;
    result.m10 = -(far + near)/fn;
    result.m11 = -1.0f;
 
    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = -(far*near*2.0f)/fn;
    result.m15 = 0.0f;
    
    return result;    
}




// Returns perspective projection matrix
// NOTE: Angle should be provided in radians
not_rl_Matrix not_rl_MatrixPerspective(double fovy, double aspect, double near, double far)
{
    double top = near*tan(fovy*0.5);
    double right = top*aspect;

    return not_rl_MatrixFrustum(-right, right, -top, top, near, far);
}



not_rl_Matrix not_rl_MatrixIdentity(void) {
// Returns identity matrix
    not_rl_Matrix result = { 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,  
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f };
    
    return result;
}

// Returns orthographic projection matrix
not_rl_Matrix not_rl_MatrixOrtho(double left, double right, double bottom, double top, double near, double far)
{
    not_rl_Matrix result;
    
    float rl = (right - left);
    float tb = (top - bottom);
    float fn = (far - near);

    result.m0 = 2.0f/rl;
    result.m1 = 0.0f;
    result.m2 = 0.0f;
    result.m3 = 0.0f;
    result.m4 = 0.0f;
    result.m5 = 2.0f/tb;
    result.m6 = 0.0f;
    result.m7 = 0.0f;
    result.m8 = 0.0f;
    result.m9 = 0.0f;
    result.m10 = -2.0f/fn;
    result.m11 = 0.0f;
    result.m12 = -(left + right)/rl;
    result.m13 = -(top + bottom)/tb;
    result.m14 = -(far + near)/fn;
    result.m15 = 1.0f;

    return result;  
}
 



// Returns float array of matrix data
float *not_rl_MatrixToFloat(not_rl_Matrix mat)
{       
    static float buffer[16];
            
    buffer[0] = mat.m0;
    buffer[1] = mat.m1;
    buffer[2] = mat.m2;
    buffer[3] = mat.m3;
    buffer[4] = mat.m4;
    buffer[5] = mat.m5;
    buffer[6] = mat.m6;
    buffer[7] = mat.m7;
    buffer[8] = mat.m8;
    buffer[9] = mat.m9;
    buffer[10] = mat.m10;
    buffer[11] = mat.m11;
    buffer[12] = mat.m12;
    buffer[13] = mat.m13;
    buffer[14] = mat.m14;
    buffer[15] = mat.m15;

    return buffer;
}



// Enable rendering to texture (fbo)
void not_rl_EnableRenderTexture(unsigned int id)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    if (checkGlError("r0")) {
      logit("error on render texture %d\n",id);
      }
    
    
    //glDisable(GL_CULL_FACE);    // Allow double side drawing for texture flipping
    //glCullFace(GL_FRONT);
}
 
// Disable rendering to texture
void not_rl_DisableRenderTexture(void)
{
#ifdef __APPLE__
//logit("gogo2 %d\n",applehackfactor/60);
    glBindFramebuffer(GL_FRAMEBUFFER, 2);  //super hack just a guess
//    glBindFramebuffer(GL_FRAMEBUFFER, applehackfactor/60);  //super hack just a guess
    if (checkGlError("a5")) {
      if ((applehackfactor % 60) == 0) {
        logit("%d did not work. Will try next number...\n",applehackfactor/60);
	}
      }
      applehackfactor++;
      applehackfactor = (applehackfactor % 600);
    // there is a way to find it
#else
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    checkGlError("a5"); 
#endif
 
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
}
    



// Clear used screen buffers (color and depth)
void not_rl_ClearScreenBuffers(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear used buffers: Color and Depth (Depth is used for 3D)
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);     // Stencil buffer not used...
}




// Set a custom modelview matrix (replaces internal modelview matrix)
void not_rl_SetMatrixModelview(not_rl_Matrix view)
{
    not_rl_modelview = view;
}
    
// Set a custom projection matrix (replaces internal projection matrix)
void not_rl_SetMatrixProjection(not_rl_Matrix proj)
{
    not_rl_projection = proj;
}
















// Set shader uniform value (float)
void not_rl_SetShaderValue(not_rl_Shader shader, int uniformLoc, const float *value, int size)
{
    glUseProgram(shader.id);
        
    if (size == 1) glUniform1fv(uniformLoc, 1, value);          // Shader uniform type: float
    else if (size == 2) glUniform2fv(uniformLoc, 1, value);     // Shader uniform type: vec2
    else if (size == 3) glUniform3fv(uniformLoc, 1, value);     // Shader uniform type: vec3
    else if (size == 4) glUniform4fv(uniformLoc, 1, value);     // Shader uniform type: vec4
    else logit( "Shader value float array size not supported");
    
    //glUseProgram(0);      // Avoid reseting current shader program, in case other uniforms are set
}
                                                               
// Set shader uniform value (int)
void not_rl_SetShaderValuei(not_rl_Shader shader, int uniformLoc, const int *value, int size)
{
    glUseProgram(shader.id);
    
    if (size == 1) glUniform1iv(uniformLoc, 1, value);          // Shader uniform type: int
    else if (size == 2) glUniform2iv(uniformLoc, 1, value);     // Shader uniform type: ivec2
    else if (size == 3) glUniform3iv(uniformLoc, 1, value);     // Shader uniform type: ivec3
    else if (size == 4) glUniform4iv(uniformLoc, 1, value);     // Shader uniform type: ivec4
    else logit( "Shader value int array size not supported");
    
    //glUseProgram(0);
}
     
// Set shader uniform value (matrix 4x4)
void not_rl_SetShaderValueMatrix(not_rl_Shader shader, int uniformLoc, not_rl_Matrix mat)
{
    glUseProgram(shader.id);

    glUniformMatrix4fv(uniformLoc, 1, not_rl_false, not_rl_MatrixToFloat(mat));
         
    //glUseProgram(0);
}
    



// Compile custom shader and return shader id
static unsigned int not_rl_CompileShader(const char *shaderStr, int type)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderStr, NULL);
    
    GLint success = 0;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        
    if (success != GL_TRUE)
    {
        logit( "[SHDR ID %d] Failed to compile shader...", shader);
        int maxLength = 0;  
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

#if defined(_MSC_VER)
        char *log = malloc(maxLength);
#else
        char log[maxLength];
#endif
        glGetShaderInfoLog(shader, maxLength, &length, log);

        logit( "%s", log);

#if defined(_MSC_VER)
        free(log);
#endif
    } 
    else logit( "[SHDR ID %d] Shader compiled successfully", shader);
    checkGlError("sasda");
    return shader;
}



int not_rl_GetShaderLocation(not_rl_Shader shader, const char *uniformName)
{
    int location = -1;
    location = glGetUniformLocation(shader.id, uniformName);

    if (location == -1) {logit( "[SHDR ID %d] Shader uniform [%s] COULD NOT BE FOUND", shader.id, uniformName);}
    else {logit( "[SHDR ID %d] Shader uniform [%s] set at location: %d", shader.id, uniformName, location);}
    return location;
}


// Load custom shader strings and return program id
static unsigned int not_rl_LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId)  
{
    unsigned int program = 0;


    GLint success = 0;
    program = glCreateProgram();
    
    glAttachShader(program, vShaderId);
    glAttachShader(program, fShaderId);

    // NOTE: Default attribute shader locations must be binded before linking
    glBindAttribLocation(program, 0, NOT_RL_DEFAULT_ATTRIB_POSITION_NAME);
    glBindAttribLocation(program, 1, NOT_RL_DEFAULT_ATTRIB_TEXCOORD_NAME);
    glBindAttribLocation(program, 2, NOT_RL_DEFAULT_ATTRIB_NORMAL_NAME);
    glBindAttribLocation(program, 3, NOT_RL_DEFAULT_ATTRIB_COLOR_NAME);
    glBindAttribLocation(program, 4, NOT_RL_DEFAULT_ATTRIB_TANGENT_NAME);
    glBindAttribLocation(program, 5, NOT_RL_DEFAULT_ATTRIB_TEXCOORD2_NAME);

    // NOTE: If some attrib name is no found on the shader, it locations becomes -1

    glLinkProgram(program);
 
    // NOTE: All uniform variables are intitialised to 0 when a program links

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {   
        logit( "[SHDR ID %d] Failed to link shader program...", program);

        int maxLength = 0; 
        int length;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
     
#ifdef _MSC_VER
        char *log = malloc(maxLength);
#else
        char log[maxLength];
#endif
        glGetProgramInfoLog(program, maxLength, &length, log);
 
        logit( "%s", log);

#ifdef _MSC_VER
        free(log);
#endif
        glDeleteProgram(program);

        program = 0;

       program = 0;
    }
    else logit( "[SHDR ID %d] Shader program loaded successfully", program);
    return program;
}




// Get location handlers to for shader attributes and uniforms
// NOTE: If any location is not found, loc point becomes -1
static void not_rl_SetShaderDefaultLocations(not_rl_Shader *shader)
{
    // NOTE: Default shader attrib locations have been fixed before linking:
    //          vertex position location    = 0
    //          vertex texcoord location    = 1
    //          vertex normal location      = 2
    //          vertex color location       = 3
    //          vertex tangent location     = 4
    //          vertex texcoord2 location   = 5

    // Get handles to GLSL input attibute locations
    shader->locs[NOT_RL_LOC_VERTEX_POSITION] = glGetAttribLocation(shader->id, NOT_RL_DEFAULT_ATTRIB_POSITION_NAME);
    shader->locs[NOT_RL_LOC_VERTEX_TEXCOORD01] = glGetAttribLocation(shader->id, NOT_RL_DEFAULT_ATTRIB_TEXCOORD_NAME);
    shader->locs[NOT_RL_LOC_VERTEX_TEXCOORD02] = glGetAttribLocation(shader->id, NOT_RL_DEFAULT_ATTRIB_TEXCOORD2_NAME);
    shader->locs[NOT_RL_LOC_VERTEX_NORMAL] = glGetAttribLocation(shader->id, NOT_RL_DEFAULT_ATTRIB_NORMAL_NAME);
    shader->locs[NOT_RL_LOC_VERTEX_TANGENT] = glGetAttribLocation(shader->id, NOT_RL_DEFAULT_ATTRIB_TANGENT_NAME);
    shader->locs[NOT_RL_LOC_VERTEX_COLOR] = glGetAttribLocation(shader->id, NOT_RL_DEFAULT_ATTRIB_COLOR_NAME);
                        logit("vvvvvvvvvvvv2 %d\n", shader->locs[NOT_RL_LOC_VERTEX_COLOR]);
checkGlError("777color4");
    
    // Get handles to GLSL uniform locations (vertex shader)
    shader->locs[NOT_RL_LOC_MATRIX_MVP]  = glGetUniformLocation(shader->id, "mvp");
    shader->locs[NOT_RL_LOC_MATRIX_PROJECTION]  = glGetUniformLocation(shader->id, "projection");
    shader->locs[NOT_RL_LOC_MATRIX_VIEW]  = glGetUniformLocation(shader->id, "view");
    
    // Get handles to GLSL uniform locations (fragment shader)
    shader->locs[NOT_RL_LOC_COLOR_DIFFUSE] = glGetUniformLocation(shader->id, "colDiffuse");
    shader->locs[NOT_RL_LOC_MAP_DIFFUSE] = glGetUniformLocation(shader->id, "texture0");
    shader->locs[NOT_RL_LOC_MAP_SPECULAR] = glGetUniformLocation(shader->id, "texture1");
    shader->locs[NOT_RL_LOC_MAP_NORMAL] = glGetUniformLocation(shader->id, "texture2");
}




// Enable texture usage
void not_rl_EnableTexture(unsigned int id)
{
   if (not_rl_draws[not_rl_drawsCounter - 1].textureId != id)
    {
        if (not_rl_draws[not_rl_drawsCounter - 1].vertexCount > 0) 
	  not_rl_drawsCounter++; 
     
        if (not_rl_drawsCounter >= NOT_RL_MAX_DRAWS_BY_TEXTURE)
        {
            not_rl_glDraw();
            not_rl_drawsCounter = 1;
        }
    
        not_rl_draws[not_rl_drawsCounter - 1].textureId = id;
        not_rl_draws[not_rl_drawsCounter - 1].vertexCount = 0;
    }

}
 
// Disable texture usage
void not_rl_DisableTexture(void)
{
}





// Load default internal buffers (lines, triangles, quads)
// just quads
static void not_rl_LoadBuffersDefault(void)
{
    // [CPU] Allocate and initialize float array buffers to store vertex data (lines, triangles, quads)
    //--------------------------------------------------------------------------------------------

    // Quads - Initialize arrays (vertex position, texcoord, color data and indexes)
    not_rl_quads.vertices = (float *)malloc(sizeof(float)*3*4*NOT_RL_MAX_QUADS_BATCH);        // 3 float by vertex, 4 vertex by quad
    not_rl_quads.texcoords = (float *)malloc(sizeof(float)*2*4*NOT_RL_MAX_QUADS_BATCH);       // 2 float by texcoord, 4 texcoord by quad
    not_rl_quads.colors = (unsigned char *)malloc(sizeof(unsigned char)*4*4*NOT_RL_MAX_QUADS_BATCH);  // 4 float by color, 4 colors by quad
    not_rl_quads.indices = (unsigned short *)malloc(sizeof(unsigned short)*6*NOT_RL_MAX_QUADS_BATCH);  // 6 int by quad (indices)

    for (int i = 0; i < (3*4*NOT_RL_MAX_QUADS_BATCH); i++) not_rl_quads.vertices[i] = 0.0f;
    for (int i = 0; i < (2*4*NOT_RL_MAX_QUADS_BATCH); i++) not_rl_quads.texcoords[i] = 0.0f;
    for (int i = 0; i < (4*4*NOT_RL_MAX_QUADS_BATCH); i++) not_rl_quads.colors[i] = 0;

    int k = 0;

    // Indices can be initialized right now
    for (int i = 0; i < (6*NOT_RL_MAX_QUADS_BATCH); i+=6)
    {
        not_rl_quads.indices[i] = 4*k;
        not_rl_quads.indices[i+1] = 4*k+1;
        not_rl_quads.indices[i+2] = 4*k+2;
        not_rl_quads.indices[i+3] = 4*k;
        not_rl_quads.indices[i+4] = 4*k+2;
        not_rl_quads.indices[i+5] = 4*k+3;

        k++;
    }

    not_rl_quads.vCounter = 0;
    not_rl_quads.tcCounter = 0;
    not_rl_quads.cCounter = 0;

    logit( "[CPU] Default buffers initialized successfully quads)");
    //--------------------------------------------------------------------------------------------

    // [GPU] Upload vertex data and initialize VAOs/VBOs (lines, triangles, quads)
    // NOTE: Default buffers are linked to use currentShader (defaultShader)
    //--------------------------------------------------------------------------------------------




    // Quads - Vertex buffers binding and attributes enable
    // Vertex position buffer (shader-location = 0)
    glGenBuffers(1, &not_rl_quads.vboId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, not_rl_quads.vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*4*NOT_RL_MAX_QUADS_BATCH, not_rl_quads.vertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_POSITION]);
    glVertexAttribPointer(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);

    // Vertex texcoord buffer (shader-location = 1)
    glGenBuffers(1, &not_rl_quads.vboId[1]);
    glBindBuffer(GL_ARRAY_BUFFER, not_rl_quads.vboId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*4*NOT_RL_MAX_QUADS_BATCH, not_rl_quads.texcoords, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_TEXCOORD01]);
    glVertexAttribPointer(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_TEXCOORD01], 2, GL_FLOAT, 0, 0, 0);

    // Vertex color buffer (shader-location = 3)
    glGenBuffers(1, &not_rl_quads.vboId[2]);
    glBindBuffer(GL_ARRAY_BUFFER, not_rl_quads.vboId[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned char)*4*4*NOT_RL_MAX_QUADS_BATCH, not_rl_quads.colors, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_COLOR]);
		checkGlError("777color8");
    
    glVertexAttribPointer(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
		checkGlError("777color9");

    // Fill index buffer
    glGenBuffers(1, &not_rl_quads.vboId[3]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, not_rl_quads.vboId[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short)*6*NOT_RL_MAX_QUADS_BATCH, not_rl_quads.indices, GL_STATIC_DRAW);

 logit( 
       "[VBO ID %d][VBO ID %d][VBO ID %d][VBO ID %d] Default buffers VBOs initialized successfully (quads)", not_rl_quads.vboId[0], not_rl_quads.vboId[1], not_rl_quads.vboId[2], not_rl_quads.vboId[3]);

    //--------------------------------------------------------------------------------------------
}







//----------------------------------------------------------------------------------
// Module Functions Definition - rlgl Functions
//----------------------------------------------------------------------------------

// Initialize rlgl: OpenGL extensions, default buffers/shaders/textures, OpenGL states
void not_rl_glInit(int width, int height)
{
logit("g1\n");
    // Check OpenGL information and capabilities
    //------------------------------------------------------------------------------

    // Print current OpenGL and GLSL version
        logit( "GPU: Vendor:   %s", glGetString(GL_VENDOR));
    logit( "GPU: Renderer: %s", glGetString(GL_RENDERER));
    logit( "GPU: Version:  %s", glGetString(GL_VERSION));
    logit( "GPU: GLSL:     %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // NOTE: We can get a bunch of extra information about GPU capabilities (glGet*)
    //int maxTexSize;
    //glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    //logit( "GL_MAX_TEXTURE_SIZE: %d", maxTexSize);

    //GL_MAX_TEXTURE_IMAGE_UNITS
    //GL_MAX_VIEWPORT_DIMS

    //int numAuxBuffers;
    //glGetIntegerv(GL_AUX_BUFFERS, &numAuxBuffers);
    //logit( "GL_AUX_BUFFERS: %d", numAuxBuffers);

    //GLint numComp = 0;
    //GLint format[32] = { 0 };
    //glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numComp);
    //glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, format);
    //for (int i = 0; i < numComp; i++) logit( "Supported compressed format: 0x%x", format[i]);

    // NOTE: We don't need that much data on screen... right now...

    // Get supported extensions list
    GLint numExt = 0;

    char *extensions = (char *)glGetString(GL_EXTENSIONS);  // One big const string

    // NOTE: We have to duplicate string because glGetString() returns a const value
    // If not duplicated, it fails in some systems (Raspberry Pi)
    // Equivalent to function: char *strdup(const char *str)
    char *extensionsDup;
    size_t len = strlen(extensions) + 1;
    void *newstr = malloc(len);
    if (newstr == NULL) extensionsDup = NULL;
    extensionsDup = (char *)memcpy(newstr, extensions, len);

    // NOTE: String could be splitted using strtok() function (string.h)
    // NOTE: strtok() modifies the received string, it can not be const

    char *extList[512];     // Allocate 512 strings pointers (2 KB)

    extList[numExt] = strtok(extensionsDup, " ");

    while (extList[numExt] != NULL)
    {
        numExt++;
        extList[numExt] = strtok(NULL, " ");
    }

    free(extensionsDup);    // Duplicated string must be deallocated

    numExt -= 1;


//    logit( "Number of supported extensions: %d", numExt);

    // Show supported extensions
    //for (int i = 0; i < numExt; i++)  logit( "Supported extension: %s", extList[i]);

    // Check required extensions
    for (int i = 0; i < numExt; i++)
    {

        // Check VAO support
        // NOTE: Only check on OpenGL ES, OpenGL 3.3 has VAO support as core feature
        if (strcmp(extList[i], (const char *)"GL_OES_vertex_array_object") == 0)
        {
            not_rl_vaoSupported = not_rl_false;

            glGenVertexArrays = NULL;
            glBindVertexArray = NULL;
            glDeleteVertexArrays = NULL;


//#else	    
//            glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)glXGetProcAddressARB("glGenVertexArraysOES");
//            glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)glXGetProcAddressARB("glBindVertexArrayOES");
//            glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC)glXGetProcAddressARB("glDeleteVertexArraysOES");
//#endif	    
            //glIsVertexArray = (PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress("glIsVertexArrayOES");     // NOTE: Fails in WebGL, omitted
        }

        // Check NPOT textures support
        // NOTE: Only check on OpenGL ES, OpenGL 3.3 has NPOT textures full support as core feature
        if (strcmp(extList[i], (const char *)"GL_OES_texture_npot") == 0) not_rl_texNPOTSupported = not_rl_true;
        
        // Check texture float support
        if (strcmp(extList[i], (const char *)"GL_OES_texture_float") == 0) not_rl_texFloatSupported = not_rl_true;


        // DDS texture compression support
        if ((strcmp(extList[i], (const char *)"GL_EXT_texture_compression_s3tc") == 0) ||
            (strcmp(extList[i], (const char *)"GL_WEBGL_compressed_texture_s3tc") == 0) ||
            (strcmp(extList[i], (const char *)"GL_WEBKIT_WEBGL_compressed_texture_s3tc") == 0)) not_rl_texCompDXTSupported = not_rl_true;

        // ETC1 texture compression support
        if ((strcmp(extList[i], (const char *)"GL_OES_compressed_ETC1_RGB8_texture") == 0) ||
            (strcmp(extList[i], (const char *)"GL_WEBGL_compressed_texture_etc1") == 0)) not_rl_texCompETC1Supported = not_rl_true;

        // ETC2/EAC texture compression support
        if (strcmp(extList[i], (const char *)"GL_ARB_ES3_compatibility") == 0) not_rl_texCompETC2Supported = not_rl_true;

        // PVR texture compression support
        if (strcmp(extList[i], (const char *)"GL_IMG_texture_compression_pvrtc") == 0) not_rl_texCompPVRTSupported = not_rl_true;

        // ASTC texture compression support
        if (strcmp(extList[i], (const char *)"GL_KHR_texture_compression_astc_hdr") == 0) not_rl_texCompASTCSupported = not_rl_true;

        // Anisotropic texture filter support
        if (strcmp(extList[i], (const char *)"GL_EXT_texture_filter_anisotropic") == 0)
        {
            not_rl_texAnisotropicFilterSupported = not_rl_true;
            glGetFloatv(0x84FF, &not_rl_maxAnisotropicLevel);   // GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
        }

        // Clamp mirror wrap mode supported
        if (strcmp(extList[i], (const char *)"GL_EXT_texture_mirror_clamp") == 0) not_rl_texClampMirrorSupported = not_rl_true;
        
        // Debug marker support
        if(strcmp(extList[i], (const char *)"GL_EXT_debug_marker") == 0) not_rl_debugMarkerSupported = not_rl_true;
        checkGlError("something_to_do_with_ext");
    }

#if defined(_MSC_VER)
    //free(extList);
#endif

    if (not_rl_vaoSupported) {logit( "[EXTENSION] VAO extension detected, VAO functions initialized successfully");}
    else {logit( "[EXTENSION] VAO extension not found, VAO usage not supported");}

    if (not_rl_texNPOTSupported) {logit( "[EXTENSION] NPOT textures extension detected, full NPOT textures supported");}
    else {logit( "[EXTENSION] NPOT textures extension not found, limited NPOT support (no-mipmaps, no-repeat)");}

    if (not_rl_texCompDXTSupported) {logit( "[EXTENSION] DXT compressed textures supported");}
    if (not_rl_texCompETC1Supported) {logit( "[EXTENSION] ETC1 compressed textures supported");}
    if (not_rl_texCompETC2Supported) {logit( "[EXTENSION] ETC2/EAC compressed textures supported");}
    if (not_rl_texCompPVRTSupported) {logit( "[EXTENSION] PVRT compressed textures supported");}
    if (not_rl_texCompASTCSupported) {logit( "[EXTENSION] ASTC compressed textures supported");}

    if (not_rl_texAnisotropicFilterSupported) logit( 
"[EXTENSION] Anisotropic textures filtering supported (max: %.0fX)", not_rl_maxAnisotropicLevel);
    if (not_rl_texClampMirrorSupported) logit( "[EXTENSION] Clamp mirror wrap texture mode supported");

    if (not_rl_debugMarkerSupported) logit( "[EXTENSION] Debug Marker supported");

    // Initialize buffers, default shaders and default textures
    //----------------------------------------------------------


    // Init default vertex arrays buffers (lines, triangles, not_rl_quads)
    not_rl_LoadBuffersDefault();

    // Init temp vertex buffer, used when transformation required (translate, rotate, scale)
// dont use it    tempBuffer = (not_rl_Vector3 *)malloc(sizeof(Vector3)*TEMP_VERTEX_BUFFER_SIZE);

//    for (int i = 0; i < TEMP_VERTEX_BUFFER_SIZE; i++) tempBuffer[i] = not_rl_Vector3Zero();


    // Init draw calls tracking system
    not_rl_draws = (not_rl_DrawCall *)malloc(sizeof(not_rl_DrawCall)*NOT_RL_MAX_DRAWS_BY_TEXTURE);

    for (int i = 0; i < NOT_RL_MAX_DRAWS_BY_TEXTURE; i++)
    {
        not_rl_draws[i].textureId = 0;
        not_rl_draws[i].vertexCount = 0;
    }

    not_rl_drawsCounter = 1;
//    not_rl_draws[not_rl_drawsCounter - 1].textureId = not_rl_whiteTexture;
// dont make whitetexture cause dont use
    not_rl_draws[not_rl_drawsCounter - 1].textureId = 0;
//    not_rl_currentDrawMode = NOT_RL_QUADS;     // Set default draw mode

    // Init internal matrix stack (emulating OpenGL 1.1)
    // dot use this eithert
//    for (int i = 0; i < MATRIX_STACK_SIZE; i++) stack[i] = MatrixIdentity();

    // Init internal projection and modelview matrices
    not_rl_projection = not_rl_MatrixIdentity();
    not_rl_modelview = not_rl_MatrixIdentity();
//?    not_rl_currentMatrix = &not_rl_modelview;


    // Initialize OpenGL default states
    //----------------------------------------------------------
    // Init state: Depth test
    glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
    glDisable(GL_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

    // Init state: Blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
    glEnable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)


        checkGlError("jdkldsfjlk");

    // Store screen size into global variables
    not_rl_screenWidth = width;
    not_rl_screenHeight = height;

    logit( "OpenGL default states initialized successfully");
}





// Initialize drawing mode (how to organize vertex)
void not_rl_Begin(int mode)
{
    // Draw mode can only be RL_LINES, RL_TRIANGLES and RL_QUADS
//    not_rl_currentDrawMode = mode;
}

// Finish vertex providing
void not_rl_End(void)
{
        {
            // Make sure colors count match vertex count
            if (not_rl_quads.vCounter != not_rl_quads.cCounter)
            {
                int addColors = not_rl_quads.vCounter - not_rl_quads.cCounter;

                for (int i = 0; i < addColors; i++)
                {
                    not_rl_quads.colors[4*not_rl_quads.cCounter] = 
			not_rl_quads.colors[4*not_rl_quads.cCounter - 4];
                    not_rl_quads.colors[4*not_rl_quads.cCounter + 1] = 
			not_rl_quads.colors[4*not_rl_quads.cCounter - 3];
                    not_rl_quads.colors[4*not_rl_quads.cCounter + 2] = 
			not_rl_quads.colors[4*not_rl_quads.cCounter - 2];
                    not_rl_quads.colors[4*not_rl_quads.cCounter + 3] = 
			not_rl_quads.colors[4*not_rl_quads.cCounter - 1];
    
                    not_rl_quads.cCounter++;
                }
            }
        
            // Make sure texcoords count match vertex count
            if (not_rl_quads.vCounter != not_rl_quads.tcCounter)
            {
                int addTexCoords = not_rl_quads.vCounter - not_rl_quads.tcCounter;

                for (int i = 0; i < addTexCoords; i++)
                {
                    not_rl_quads.texcoords[2*not_rl_quads.tcCounter] = 0.0f;
                    not_rl_quads.texcoords[2*not_rl_quads.tcCounter + 1] = 0.0f;

                    not_rl_quads.tcCounter++;
                }
            }
 
            // TODO: Make sure normals count match vertex count... if normals s$

        }
    
    // NOTE: Depth increment is dependant on not_rl_Ortho(): z-near and z-far values,
    // as well as depth buffer bit-depth (16bit or 24bit or 32bit)
    // Correct increment formula would be: depthInc = (zfar - znear)/pow(2, bit$
    not_rl_currentDepth += (1.0f/20000.0f);

    // TODO: Verify internal buffers limits
    // NOTE: Before launching draw, verify no matrix are left in the stack!
    // NOTE: Probably a lines/triangles margin should be left, not_rl_End could be c$
    // after an undetermined number of triangles buffered (check shapes::DrawPo$
    if ((not_rl_quads.vCounter/4 >= NOT_RL_MAX_QUADS_BATCH - 2)) not_rl_glDraw();
    //^^^ not drawing here
}



// Define one vertex (position)
void not_rl_Vertex3f(float x, float y, float z)
{
            {
                // Verify that NOT_RL_MAX_QUADS_BATCH limit not reached
                if (not_rl_quads.vCounter/4 < NOT_RL_MAX_QUADS_BATCH)
                {
                    not_rl_quads.vertices[3*not_rl_quads.vCounter] = x;
                    not_rl_quads.vertices[3*not_rl_quads.vCounter + 1] = y;
                    not_rl_quads.vertices[3*not_rl_quads.vCounter + 2] = z; 
 
                    not_rl_quads.vCounter++;

                    not_rl_draws[not_rl_drawsCounter - 1].vertexCount++;
                }
                else {logit( "MAX_not_rl_quads_BATCH overflow");}
     
            } 
}

                
// Define one vertex (texture coordinate)
// NOTE: Texture coordinates are limited to QUADS only
void not_rl_TexCoord2f(float x, float y)   
{
        not_rl_quads.texcoords[2*not_rl_quads.tcCounter] = x;
        not_rl_quads.texcoords[2*not_rl_quads.tcCounter + 1] = y;
        
        not_rl_quads.tcCounter++;
}           
 
// Define one vertex (normal)
// NOTE: Normals limited to TRIANGLES only ?
void not_rl_Normal3f(float x, float y, float z)
{                
    // TODO: Normals usage...
}
    
// Define one vertex (color)
void not_rl_Color4ub(not_rl_byte x, not_rl_byte y, not_rl_byte z, not_rl_byte w)
{
        {
            not_rl_quads.colors[4*not_rl_quads.cCounter] = x;
            not_rl_quads.colors[4*not_rl_quads.cCounter + 1] = y;
            not_rl_quads.colors[4*not_rl_quads.cCounter + 2] = z;
            not_rl_quads.colors[4*not_rl_quads.cCounter + 3] = w;
                
            not_rl_quads.cCounter++;

        }
}
                 
// Define one vertex (color)
void not_rl_Color4f(float r, float g, float b, float a)
{
    not_rl_Color4ub((not_rl_byte)(r*255), (not_rl_byte)(g*255), (not_rl_byte)(b*255), (not_rl_byte)(a*255));
}
                 
// Define one vertex (color)
void not_rl_Color3f(float x, float y, float z)
{
    not_rl_Color4ub((not_rl_byte)(x*255), (not_rl_byte)(y*255), (not_rl_byte)(z*255), 255);
}
                






// Define one vertex (position)
void not_rl_Vertex2f(float x, float y) 
{
    not_rl_Vertex3f(x, y, not_rl_currentDepth);
}




// Drawing batches: triangles, quads, lines
void not_rl_glDraw()
{
    // NOTE: In a future version, models could be stored in a stack...
    //for (int i = 0; i < modelsCount; i++) rlDrawMesh(models[i]->mesh, models[i]->material, models[i]->transform);

    // NOTE: Default buffers upload and draw
    not_rl_UpdateBuffersDefault();
    not_rl_DrawBuffersDefault();       // NOTE: Stereo rendering is checked inside
}








// Draw default internal buffers vertex data
// NOTE: We draw in this order: lines, triangles, quads
static void not_rl_DrawBuffersDefault(void)
{
//logit("not_rl_DrawBuffersDefault\n");
    not_rl_Matrix matProjection = not_rl_projection;
    not_rl_Matrix matModelView = not_rl_modelview;
    
    int eyesCount = 1;
    if (not_rl_vrStereoRender) eyesCount = 2;
    checkGlError("q1");
    for (int eye = 0; eye < eyesCount; eye++)
    {
        if (eyesCount == 2) not_rl_SetStereoView(eye, matProjection, matModelView);

        // Set current shader and upload current MVP matrix
        if ((not_rl_quads.vCounter > 0))
        {
            glUseProgram(not_rl_vrConfig.distortionShader.id);

            // Create modelview-projection matrix
            not_rl_Matrix matMVP = not_rl_MatrixMultiply(not_rl_modelview, not_rl_projection);

            glUniformMatrix4fv(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_MATRIX_MVP], 1, not_rl_false, 
	        not_rl_MatrixToFloat(matMVP));
            glUniform4f(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_COLOR_DIFFUSE], 1.0f, 1.0f, 1.0f, 1.0f);
            glUniform1i(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_MAP_DIFFUSE], 0);

            // NOTE: Additional map textures not considered for default buffers drawing
        }





    checkGlError("q4");
        // Draw quads buffers
        if (not_rl_quads.vCounter > 0)
        {
            int quadsCount = 0;
            int numIndicesToProcess = 0;
            int indicesOffset = 0;

            if (not_rl_vaoSupported)
            {
                glBindVertexArray(not_rl_quads.vaoId);
    checkGlError("q4a1");
            }
            else
            {
                // Bind vertex attrib: position (shader-location = 0)
                glBindBuffer(GL_ARRAY_BUFFER, not_rl_quads.vboId[0]);
                glVertexAttribPointer(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);
                glEnableVertexAttribArray(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_POSITION]);
    checkGlError("q4a4");

                // Bind vertex attrib: texcoord (shader-location = 1)
                glBindBuffer(GL_ARRAY_BUFFER, not_rl_quads.vboId[1]);
                glVertexAttribPointer(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_TEXCOORD01], 2, GL_FLOAT, 0, 0, 0);
                glEnableVertexAttribArray(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_TEXCOORD01]);
    checkGlError("q4a3");

                // Bind vertex attrib: color (shader-location = 3)
                glBindBuffer(GL_ARRAY_BUFFER, not_rl_quads.vboId[2]);
    checkGlError("q4a2a1");
                
		if (not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_COLOR] != -1) { /*Hib*/
                  glVertexAttribPointer(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_COLOR], 
		     4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
                  glEnableVertexAttribArray(not_rl_vrConfig.distortionShader.locs[NOT_RL_LOC_VERTEX_COLOR]);
    checkGlError("q4a2a");
		  }


                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, not_rl_quads.vboId[3]);
    checkGlError("q4a2");
            }

//            logit( "Draws required per frame: %d", not_rl_drawsCounter);

    checkGlError("q5");
            for (int i = 0; i < not_rl_drawsCounter; i++)
            {
                quadsCount = not_rl_draws[i].vertexCount/4;
                numIndicesToProcess = quadsCount*6;  // Get number of Quads*6 index by Quad

//                logit( "Quads to render: %d - Vertex Count: %d", quadsCount, not_rl_draws[i].vertexCount);

                glActiveTexture(GL_TEXTURE0);
    checkGlError("q5x");
                glBindTexture(GL_TEXTURE_2D, not_rl_draws[i].textureId);
    checkGlError("q5w");

                // NOTE: The final parameter tells the GPU the offset in bytes from the start of the index buffer to the location of the first index to process
                glDrawElements(GL_TRIANGLES, numIndicesToProcess, GL_UNSIGNED_SHORT, (GLvoid *)(sizeof(GLushort)*indicesOffset));
    checkGlError("q5y");
                GLenum err;
                if ((err = glGetError()) != GL_NO_ERROR) logit( "OpenGL error: %d", (int)err);    //GL_INVALID_ENUM!

                indicesOffset += not_rl_draws[i].vertexCount/4*6;
            }

    checkGlError("q6");
            if (!not_rl_vaoSupported)
            {
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            glBindTexture(GL_TEXTURE_2D, 0);    // Unbind textures
        }

        if (not_rl_vaoSupported) glBindVertexArray(0); // Unbind VAO

        glUseProgram(0);    // Unbind shader program
    }
    checkGlError("q7");

    // Reset not_rl_draws counter
    not_rl_drawsCounter = 1;
//    not_rl_draws[0].textureId = whiteTexture;
    not_rl_draws[0].textureId = 0; // not making whitetexture
    not_rl_draws[0].vertexCount = 0;

    // Reset vertex counters for next frame
    not_rl_quads.vCounter = 0;
    not_rl_quads.tcCounter = 0;
    not_rl_quads.cCounter = 0;

    // Reset depth for next draw
    not_rl_currentDepth = -1.0f;

    // Restore projection/modelview matrices
    not_rl_projection = matProjection;
    not_rl_modelview = matModelView;
    checkGlError("q8");
}




// Initialize display device and framebuffer
// NOTE: width and height represent the screen (framebuffer) desired size, not actual display size
// If width or height are 0, default display size will be used for framebuffer size
// NOTE: returns false in case graphic device could not be created
static not_rl_bool not_rl_InitGraphicsDevice(int width, int height)
{
    not_rl_screenWidth = width;        // User desired width
    not_rl_screenHeight = height;      // User desired height

    // NOTE: Framebuffer (render area - renderWidth, renderHeight) could include black bars...
    // ...in top-down or left-right to match display aspect ratio (no weird scalings)

    // Downscale matrix is required in case desired screen area is bigger than display area
    not_rl_downscaleView = not_rl_MatrixIdentity();



#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit())
    {
        TraceLog(LOG_WARNING, "Failed to initialize GLFW");
        return false;
    }

    // NOTE: Getting video modes is not implemented in emscripten GLFW3 version
#if defined(PLATFORM_DESKTOP)
    // Find monitor resolution
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (!monitor)
    {
        TraceLog(LOG_WARNING, "Failed to get monitor");
        return false;
    }
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    not_rl_displayWidth = mode->width;
    not_rl_displayHeight = mode->height;

    // Screen size security check
    if (not_rl_screenWidth <= 0) not_rl_screenWidth = not_rl_displayWidth;
    if (not_rl_screenHeight <= 0) not_rl_screenHeight = not_rl_displayHeight;
#endif  // defined(PLATFORM_DESKTOP)



#if defined(PLATFORM_WEB)
    not_rl_displayWidth = not_rl_screenWidth;
    not_rl_displayHeight = not_rl_screenHeight;
#endif  // defined(PLATFORM_WEB)

    glfwDefaultWindowHints();                       // Set default windows hints

    // Check some Window creation flags
    if (configFlags & FLAG_WINDOW_RESIZABLE) glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);   // Resizable window
    else glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);  // Avoid window being resizable

    if (configFlags & FLAG_WINDOW_DECORATED) glfwWindowHint(GLFW_DECORATED, GL_TRUE);   // Border and buttons on Window
    
    if (configFlags & FLAG_WINDOW_TRANSPARENT)
    {
        // TODO: Enable transparent window (not ready yet on GLFW 3.2)
    }

    if (configFlags & FLAG_MSAA_4X_HINT)
    {
        glfwWindowHint(GLFW_SAMPLES, 4);            // Enables multisampling x4 (MSAA), default is 0
        logit( "Trying to enable MSAA x4");
    }
    
    //glfwWindowHint(GLFW_RED_BITS, 8);             // Framebuffer red color component bits
    //glfwWindowHint(GLFW_DEPTH_BITS, 16);          // Depthbuffer bits (24 by default)
    //glfwWindowHint(GLFW_REFRESH_RATE, 0);         // Refresh rate for fullscreen window
    //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);    // Default OpenGL API to use. Alternative: GLFW_OPENGL_ES_API
    //glfwWindowHint(GLFW_AUX_BUFFERS, 0);          // Number of auxiliar buffers

    // NOTE: When asking for an OpenGL context version, most drivers provide highest supported version
    // with forward compatibility to older OpenGL versions.
    // For example, if using OpenGL 1.1, driver can provide a 4.3 context forward compatible.

    // Check selection OpenGL version
//        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);        // Choose OpenGL major version (just hint)
//        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);        // Choose OpenGL minor version (just hint)
//        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Profiles Hint: Only 3.3 and above!
                                                                       // Other values: GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_COMPAT_PROFILE
#if defined(__APPLEOSX__)
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // OSX Requires
#else
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE); // Fordward Compatibility Hint: Only 3.3 and above!
#endif
        //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    }

    if (fullscreen)
    {
        // Obtain recommended displayWidth/displayHeight from a valid videomode for the monitor
        int count;
        const GLFWvidmode *modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);

        // Get closest videomode to desired screenWidth/screenHeight
        for (int i = 0; i < count; i++)
        {
            if (modes[i].width >= screenWidth)
            {
                if (modes[i].height >= screenHeight)
                {
                    displayWidth = modes[i].width;
                    displayHeight = modes[i].height;
                    break;
                }
            }
        }

        TraceLog(LOG_WARNING, "Closest fullscreen videomode: %d x %d", displayWidth, displayHeight);

        // NOTE: ISSUE: Closest videomode could not match monitor aspect-ratio, for example,
        // for a desired screen size of 800x450 (16:9), closest supported videomode is 800x600 (4:3),
        // framebuffer is rendered correctly but once displayed on a 16:9 monitor, it gets stretched
        // by the sides to fit all monitor space...

        // At this point we need to manage render size vs screen size
        // NOTE: This function uses and modifies global module variables:
        //       screenWidth/screenHeight - renderWidth/renderHeight - downscaleView
        SetupFramebufferSize(displayWidth, displayHeight);

        window = glfwCreateWindow(displayWidth, displayHeight, windowTitle, glfwGetPrimaryMonitor(), NULL);

        // NOTE: Full-screen change, not working properly...
        //glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, screenWidth, screenHeight, GLFW_DONT_CARE);
    }
    else
    {
        // No-fullscreen window creation
        window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);

        if (window)
        {
#if defined(PLATFORM_DESKTOP)
            // Center window on screen
            int windowPosX = displayWidth/2 - screenWidth/2;
            int windowPosY = displayHeight/2 - screenHeight/2;

            if (windowPosX < 0) windowPosX = 0;
            if (windowPosY < 0) windowPosY = 0;

            glfwSetWindowPos(window, windowPosX, windowPosY);
#endif
            renderWidth = screenWidth;
            renderHeight = screenHeight;
        }
    }

    if (!window)
    {
        glfwTerminate();
        TraceLog(LOG_WARNING, "GLFW Failed to initialize Window");
        return false;
    }
    else
    {
        logit( "Display device initialized successfully");
#if defined(PLATFORM_DESKTOP)
        logit( "Display size: %d x %d", displayWidth, displayHeight);
#endif
        logit( "Render size: %d x %d", renderWidth, renderHeight);
        logit( "Screen size: %d x %d", screenWidth, screenHeight);
        logit( "Viewport offsets: %d, %d", renderOffsetX, renderOffsetY);
    }

    glfwSetWindowSizeCallback(window, WindowSizeCallback);      // NOTE: Resizing not allowed by default!
    glfwSetCursorEnterCallback(window, CursorEnterCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, MouseCursorPosCallback);   // Track mouse position changes
    glfwSetCharCallback(window, CharCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetWindowIconifyCallback(window, WindowIconifyCallback);
#if defined(PLATFORM_DESKTOP)
    glfwSetDropCallback(window, WindowDropCallback);
#endif

    glfwMakeContextCurrent(window);
    
    // Try to disable GPU V-Sync by default, set framerate using SetTargetFPS()
    // NOTE: V-Sync can be enabled by graphic driver configuration
    glfwSwapInterval(0);                

#if defined(PLATFORM_DESKTOP)
    // Load OpenGL 3.3 extensions
    // NOTE: GLFW loader function is passed as parameter
    rlLoadExtensions(glfwGetProcAddress);
#endif

    // Try to enable GPU V-Sync, so frames are limited to screen refresh rate (60Hz -> 60 FPS)
    // NOTE: V-Sync can be enabled by graphic driver configuration
    if (configFlags & FLAG_VSYNC_HINT)
    {
        glfwSwapInterval(1);
        logit( "Trying to enable VSYNC");
    }
#endif // defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)













#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI) || defined(PLATFORM_UWP)
    fullscreen = true;

    // Screen size security check
    if (screenWidth <= 0) screenWidth = displayWidth;
    if (screenHeight <= 0) screenHeight = displayHeight;

#if defined(PLATFORM_RPI)
    bcm_host_init();

    DISPMANX_ELEMENT_HANDLE_T dispmanElement;
    DISPMANX_DISPLAY_HANDLE_T dispmanDisplay;
    DISPMANX_UPDATE_HANDLE_T dispmanUpdate;

    VC_RECT_T dstRect;
    VC_RECT_T srcRect;
#endif

    EGLint samples = 0;
    EGLint sampleBuffer = 0;
    if (configFlags & FLAG_MSAA_4X_HINT)
    {
        samples = 4;
        sampleBuffer = 1;
        logit( "Trying to enable MSAA x4");
    }

    const EGLint framebufferAttribs[] =
    {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,     // Type of context support -> Required on RPI?
        //EGL_SURFACE_TYPE, EGL_WINDOW_BIT,          // Don't use it on Android!
        EGL_RED_SIZE, 8,            // RED color bit depth (alternative: 5)
        EGL_GREEN_SIZE, 8,          // GREEN color bit depth (alternative: 6)
        EGL_BLUE_SIZE, 8,           // BLUE color bit depth (alternative: 5)
        //EGL_ALPHA_SIZE, 8,        // ALPHA bit depth (required for transparent framebuffer)
        //EGL_TRANSPARENT_TYPE, EGL_NONE, // Request transparent framebuffer (EGL_TRANSPARENT_RGB does not work on RPI)
        EGL_DEPTH_SIZE, 16,         // Depth buffer size (Required to use Depth testing!)
        //EGL_STENCIL_SIZE, 8,      // Stencil buffer size
        EGL_SAMPLE_BUFFERS, sampleBuffer,    // Activate MSAA
        EGL_SAMPLES, samples,       // 4x Antialiasing if activated (Free on MALI GPUs)
        EGL_NONE
    };

    const EGLint contextAttribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

#if defined(PLATFORM_UWP)
    const EGLint surfaceAttributes[] =
    {
        // EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER is part of the same optimization as EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER (see above).
        // If you have compilation issues with it then please update your Visual Studio templates.
        EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_NONE
    };

    const EGLint defaultDisplayAttributes[] =
    {
        // These are the default display attributes, used to request ANGLE's D3D11 renderer.
        // eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

        // EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance benefits on mobile devices.
        // Its syntax is subject to change, though. Please update your Visual Studio templates if you experience compilation issues with it.
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        
        // EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call 
        // the IDXGIDevice3::Trim method on behalf of the application when it gets suspended. 
        // Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification requirement.
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };
    
    const EGLint fl9_3DisplayAttributes[] =
    {
        // These can be used to request ANGLE's D3D11 renderer, with D3D11 Feature Level 9_3.
        // These attributes are used if the call to eglInitialize fails with the default display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };

    const EGLint warpDisplayAttributes[] =
    {
        // These attributes can be used to request D3D11 WARP.
        // They are used if eglInitialize fails with both the default display attributes and the 9_3 display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE,
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };
    
    EGLConfig config = NULL;
#ifndef PLATFORM_SILENT_RADIANCE
    // eglGetPlatformDisplayEXT is an alternative to eglGetDisplay. It allows us to pass in display attributes, used to configure D3D11.
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)(eglGetProcAddress("eglGetPlatformDisplayEXT"));
#else
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = NULL;
#endif
    if (!eglGetPlatformDisplayEXT)
    {
        TraceLog(LOG_WARNING, "Failed to get function eglGetPlatformDisplayEXT");
        return false;
    }

    //
    // To initialize the display, we make three sets of calls to eglGetPlatformDisplayEXT and eglInitialize, with varying 
    // parameters passed to eglGetPlatformDisplayEXT:
    // 1) The first calls uses "defaultDisplayAttributes" as a parameter. This corresponds to D3D11 Feature Level 10_0+.
    // 2) If eglInitialize fails for step 1 (e.g. because 10_0+ isn't supported by the default GPU), then we try again 
    //    using "fl9_3DisplayAttributes". This corresponds to D3D11 Feature Level 9_3.
    // 3) If eglInitialize fails for step 2 (e.g. because 9_3+ isn't supported by the default GPU), then we try again 
    //    using "warpDisplayAttributes".  This corresponds to D3D11 Feature Level 11_0 on WARP, a D3D11 software rasterizer.
    //
    
    // This tries to initialize EGL to D3D11 Feature Level 10_0+. See above comment for details.
    display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
    if (display == EGL_NO_DISPLAY)
    {
        TraceLog(LOG_WARNING, "Failed to initialize EGL display");
        return false;
    }
        
    if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
    {
        // This tries to initialize EGL to D3D11 Feature Level 9_3, if 10_0+ is unavailable (e.g. on some mobile devices).
        display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, fl9_3DisplayAttributes);
        if (display == EGL_NO_DISPLAY)
        {
            TraceLog(LOG_WARNING, "Failed to initialize EGL display");
            return false;
        }

        if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
        {
            // This initializes EGL to D3D11 Feature Level 11_0 on WARP, if 9_3+ is unavailable on the default GPU.
            display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, warpDisplayAttributes);
            if (display == EGL_NO_DISPLAY) 
            {
                TraceLog(LOG_WARNING, "Failed to initialize EGL display");
                return false;
            }

            if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
            {
                // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
                TraceLog(LOG_WARNING, "Failed to initialize EGL");
                return false;
            }
        }
    }

	//SetupFramebufferSize(displayWidth, displayHeight);

    EGLint numConfigs = 0;
    if ((eglChooseConfig(display, framebufferAttribs, &config, 1, &numConfigs) == EGL_FALSE) || (numConfigs == 0))
    {
        TraceLog(LOG_WARNING, "Failed to choose first EGLConfig");
        return false;
    }

    // Create a PropertySet and initialize with the EGLNativeWindowType.
    //PropertySet^ surfaceCreationProperties = ref new PropertySet();
    //surfaceCreationProperties->Insert(ref new String(EGLNativeWindowTypeProperty), window);     // CoreWindow^ window

    // You can configure the surface to render at a lower resolution and be scaled up to 
    // the full window size. The scaling is often free on mobile hardware.
    //
    // One way to configure the SwapChainPanel is to specify precisely which resolution it should render at.
    // Size customRenderSurfaceSize = Size(800, 600);
    // surfaceCreationProperties->Insert(ref new String(EGLRenderSurfaceSizeProperty), PropertyValue::CreateSize(customRenderSurfaceSize));
    //
    // Another way is to tell the SwapChainPanel to render at a certain scale factor compared to its size.
    // e.g. if the SwapChainPanel is 1920x1280 then setting a factor of 0.5f will make the app render at 960x640
    // float customResolutionScale = 0.5f;
    // surfaceCreationProperties->Insert(ref new String(EGLRenderResolutionScaleProperty), PropertyValue::CreateSingle(customResolutionScale));

    
    // eglCreateWindowSurface() requires a EGLNativeWindowType parameter, 
    // In Windows platform: typedef HWND EGLNativeWindowType;
    
    
    // Property: EGLNativeWindowTypeProperty
    // Type: IInspectable
    // Description: Set this property to specify the window type to use for creating a surface.
    //              If this property is missing, surface creation will fail.
    //
    //const wchar_t EGLNativeWindowTypeProperty[] = L"EGLNativeWindowTypeProperty";
    
    //https://stackoverflow.com/questions/46550182/how-to-create-eglsurface-using-c-winrt-and-angle
    
    //surface = eglCreateWindowSurface(display, config, reinterpret_cast<IInspectable*>(surfaceCreationProperties), surfaceAttributes);
    surface = eglCreateWindowSurface(display, config, uwpWindow, surfaceAttributes);
    if (surface == EGL_NO_SURFACE)
    {
        TraceLog(LOG_WARNING, "Failed to create EGL fullscreen surface");
        return false;
    }

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT)
    {
        TraceLog(LOG_WARNING, "Failed to create EGL context");
        return false;
    }

	// Get EGL display window size 
	eglQuerySurface(display, surface, EGL_WIDTH, &screenWidth);
	eglQuerySurface(display, surface, EGL_HEIGHT, &screenHeight);
    

#else   // not PLATFORM_ANDROID, PLATFORM_RPI, or PLATFORM_UWP





















    EGLint numConfigs;

#ifndef PLATFORM_SILENT_RADIANCE
// display is not set and all this egl initialize is not run
    // Get an EGL display connection
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) 
    {
        TraceLog(LOG_WARNING, "Failed to initialize EGL display");
        return false;
    }

    // Initialize the EGL display connection
    if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
    {
        // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
        TraceLog(LOG_WARNING, "Failed to initialize EGL");
        return false;
    }

    // Get an appropriate EGL framebuffer configuration
    eglChooseConfig(display, framebufferAttribs, &config, 1, &numConfigs);

    // Set rendering API
    eglBindAPI(EGL_OPENGL_ES_API);

    // Create an EGL rendering context
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT)
    {
        TraceLog(LOG_WARNING, "Failed to create EGL context");
        return false;
    }
#endif  // not silent radiance
    
#endif // not PLATFORM_ANDROID, PLATFORM_RPI, or PLATFORM_UWP


    // Create an EGL window surface
    //---------------------------------------------------------------------------------
#if defined(PLATFORM_ANDROID)
    EGLint displayFormat;

    displayWidth = ANativeWindow_getWidth(app->window);
    displayHeight = ANativeWindow_getHeight(app->window);

    // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is guaranteed to be accepted by ANativeWindow_setBuffersGeometry()
    // As soon as we picked a EGLConfig, we can safely reconfigure the ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &displayFormat);

    // At this point we need to manage render size vs screen size
    // NOTE: This function use and modify global module variables: screenWidth/screenHeight and renderWidth/renderHeight and downscaleView
    SetupFramebufferSize(displayWidth, displayHeight);

    ANativeWindow_setBuffersGeometry(app->window, renderWidth, renderHeight, displayFormat);
    //ANativeWindow_setBuffersGeometry(app->window, 0, 0, displayFormat);       // Force use of native display size

    surface = eglCreateWindowSurface(display, config, app->window, NULL);
#endif  // defined(PLATFORM_ANDROID)

#if defined(PLATFORM_RPI)
    graphics_get_display_size(0, &displayWidth, &displayHeight);

    // At this point we need to manage render size vs screen size
    // NOTE: This function use and modify global module variables: screenWidth/screenHeight and renderWidth/renderHeight and downscaleView
    SetupFramebufferSize(displayWidth, displayHeight);

    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.width = displayWidth;
    dstRect.height = displayHeight;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.width = renderWidth << 16;
    srcRect.height = renderHeight << 16;

    // NOTE: RPI dispmanx windowing system takes care of srcRec scaling to dstRec by hardware (no cost)
    // Take care that renderWidth/renderHeight fit on displayWidth/displayHeight aspect ratio

    VC_DISPMANX_ALPHA_T alpha;
    alpha.flags = DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS;
    alpha.opacity = 255;    // Set transparency level for framebuffer, requires EGLAttrib: EGL_TRANSPARENT_TYPE
    alpha.mask = 0;

    dispmanDisplay = vc_dispmanx_display_open(0);   // LCD
    dispmanUpdate = vc_dispmanx_update_start(0);

    dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay, 0/*layer*/, &dstRect, 0/*src*/,
                                            &srcRect, DISPMANX_PROTECTION_NONE, &alpha, 0/*clamp*/, DISPMANX_NO_ROTATE);

    nativeWindow.element = dispmanElement;
    nativeWindow.width = renderWidth;
    nativeWindow.height = renderHeight;
    vc_dispmanx_update_submit_sync(dispmanUpdate);

    surface = eglCreateWindowSurface(display, config, &nativeWindow, NULL);
    //---------------------------------------------------------------------------------
#endif  // defined(PLATFORM_RPI)
    // There must be at least one frame displayed before the buffers are swapped
    //eglSwapInterval(display, 1);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
    {
        TraceLog(LOG_WARNING, "Unable to attach EGL rendering context to EGL surface");
        return false;
    }
    else
    {
        // Grab the width and height of the surface
        //eglQuerySurface(display, surface, EGL_WIDTH, &renderWidth);
        //eglQuerySurface(display, surface, EGL_HEIGHT, &renderHeight);

        logit( "Display device initialized successfully");
        logit( "Display size: %d x %d", displayWidth, displayHeight);
        logit( "Render size: %d x %d", renderWidth, renderHeight);
        logit( "Screen size: %d x %d", screenWidth, screenHeight);
        logit( "Viewport offsets: %d, %d", renderOffsetX, renderOffsetY);
    }
#endif // defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI)

	not_rl_renderWidth = not_rl_screenWidth;
	not_rl_renderHeight = not_rl_screenHeight;
	
    // Initialize OpenGL context (states and resources)
    // NOTE: screenWidth and screenHeight not used, just stored as globals
    not_rl_glInit(not_rl_screenWidth, not_rl_screenHeight);


    // Setup default viewport
    not_rl_SetupViewport();

    // Initialize internal projection and modelview matrices
    // NOTE: Default to orthographic projection mode with top-left corner at (0,0)
    not_rl_projection = not_rl_MatrixIdentity();
    not_rl_Matrix o;
    o = not_rl_MatrixOrtho(0.0,not_rl_renderWidth - not_rl_renderOffsetX, 
           not_rl_renderHeight - not_rl_renderOffsetY, 0, 0.0f, 1.0f);
    not_rl_projection = not_rl_MatrixMultiply(not_rl_projection,o);
    
    
    not_rl_currentMatrixMode = NOT_RL_MODELVIEW;
    not_rl_modelview = not_rl_MatrixIdentity();  


#ifndef PLATFORM_SILENT_RADIANCE
//    ClearBackground(RAYWHITE);      // Default background color for raylib games :P
#endif

    not_rl_windowReady = not_rl_true;             // IMPORTANT!
#if defined(PLATFORM_ANDROID)
    not_rl_windowReady = not_rl_true;             // IMPORTANT!
#endif
    return not_rl_true;
}





// Set viewport parameters
static void not_rl_SetupViewport(void)
{
#if defined(__APPLEOSX__)
    // Get framebuffer size of current window
    // NOTE: Required to handle HighDPI display correctly on OSX because framebuffer
    // is automatically reasized to adapt to new DPI.
    // When OS does that, it can be detected using GLFW3 callback: glfwSetFramebufferSizeCallback()
    int fbWidth, fbHeight;
   glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    checkGlError("glfwGetFramebufferSize");
    logit("apple vp %d,%d  ,%d, %d\n",not_rl_renderOffsetX/2, not_rl_renderOffsetY/2,  fbWidth -
          not_rl_renderOffsetX, fbHeight - not_rl_renderOffsetY);
    glViewport(not_rl_renderOffsetX/2, not_rl_renderOffsetY/2,  fbWidth - 
      not_rl_renderOffsetX, fbHeight - not_rl_renderOffsetY);
#else
    // Initialize screen viewport (area of the screen that you will actually draw to)
    // NOTE: Viewport must be recalculated if screen is resized
    glViewport(not_rl_renderOffsetX/2, not_rl_renderOffsetY/2, 
       not_rl_renderWidth - not_rl_renderOffsetX, 
       not_rl_renderHeight - not_rl_renderOffsetY);
#endif
}

/* end of not_rl.c */





#ifdef neuveasadsdsad
	int n=0;int tc=0;
	texcoord[tc++]=0.f;
	texcoord[tc++]=1.f;
	
	tvertices[n++]=0.f;
	tvertices[n++]=0.f;

		
	texcoord[tc++]=0.f;
	texcoord[tc++]=0.f;
	
	tvertices[n++]=0.f;
	tvertices[n++]=not_rl_vrConfig.stereoFbo.texture.height;
	
	
	
			
	texcoord[tc++]=1.f;
	texcoord[tc++]=1.f;
	
	tvertices[n++]=not_rl_vrConfig.stereoFbo.texture.width;
	tvertices[n++]=0.f;
	

	texcoord[tc++]=0.f;
	texcoord[tc++]=0.f;
	
	tvertices[n++]=0.f;
	tvertices[n++]=not_rl_vrConfig.stereoFbo.texture.height;
	
	
	texcoord[tc++]=1.f;
	texcoord[tc++]=0.f;
	
	tvertices[n++]=not_rl_vrConfig.stereoFbo.texture.width;
	tvertices[n++]=not_rl_vrConfig.stereoFbo.texture.height;
	
??	

	glEnableVertexAttribArray(gvPositionHandle);
	checkGlError("glEnableVertexAttribArraymt");
	glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, verticies);
	checkGlError("glVertexAttribPointer");
	
	glDrawArrays(GL_TRIANGLES,0,n>>1);
	checkGlError("glDrawArraysmt");
		
			
	glDisableVertexAttribArray(gvPositionHandle);
	checkGlError("glDrawElementsPmt");
					
	
#endif	
	
	
