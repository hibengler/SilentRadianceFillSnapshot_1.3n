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






#ifndef SR_HELPER_G
#define SR_HELPER_G 1

#include "glue.h"

#ifdef ANDROID
#define GL_GLEXT_PROTOTYPES 1
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#ifdef __APPLE__
#include <OpenGLES/ES2/glext.h>
#endif

#ifdef LINUX_CLIENT
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#ifdef _WIN32
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <string.h>



#ifndef M_PI
#define M_PI  3.14159265358979323846264338327950288
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    double r;       //  [0, 1]
    double g;       //  [0, 1]
    double b;       //  [0, 1]
} rgb;

typedef struct {
    double h;       //  [0, 1]
    double s;       //  [0, 1]
    double v;       //  [0, 1]
} hsv;

typedef struct{
    GLfloat mat[4][4];
    } glMatrix;


#include "display_set.h"
// letters.h - the new font stuff
#include "letters.h"




struct display_helper {
  GLuint program;
  GLuint vertexID;
  GLuint colorID;
  
   GLuint mvpLoc;
   GLuint positionLoc;
   GLuint colorLoc;
      glMatrix *projectionMat;
      glMatrix *modelviewMat;
      glMatrix *mvpMat;
      
       GLint surface_width, surface_height;
       lcontext *lc;
  };

/*
static GLuint program;
static GLuint vertexID;
static GLuint colorID;
 
static GLuint mvpLoc;
static GLuint positionLoc;
static GLuint colorLoc;
static glMatrix *projectionMat;
static glMatrix *modelviewMat;
static glMatrix *mvpMat;
 
static GLint surface_width, surface_height;
*/

#define HELPER_COS_FACTOR1 0.00153397949218750000f
/*  3.14159*2.0/4096.; - used to build the cosine table */
#define HELPER_COS_FACTOR 651.89919754009912178228f
/*  4096.f/(3.14159*2.0 - used to convert radians into 4096 possibile answers */

#define hcos(a) helper_cos[ ((int)(HELPER_COS_FACTOR*(a)))&4095 ]
#define hsin(a) helper_cos[ (1024-(int)(HELPER_COS_FACTOR*(a)))&4095 ]
extern float helper_cos[4096];
     
extern void helper_init(void);
extern void translateMatrix(glMatrix *result, GLfloat x,GLfloat y,GLfloat z);
extern void translateMatrix2(glMatrix *result, GLfloat x,GLfloat y,GLfloat z);
extern void multMatrix(glMatrix *result, glMatrix *srcA, glMatrix *srcB);
extern void frustumMatrix(glMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);
extern void rotationMatrix(glMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
extern void scaleMatrix(glMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz);
extern void loadIdentity(glMatrix *result);





extern const char *gStandard2dVertexShader;
extern const char *gStandardPurpleFragmentShader;

extern const char *gStandard2dVertexOneColorShader;
extern const char *gStandard2dFragmentOneColorShader;
extern const char gStandardDistortionVShader[];
extern const char gStandardDistortionFShader[];
extern const char gStandard3dVertexVaryingColorShader[];
extern const char gStandard3dFragmentVaryingColorShader[];
extern char gStandardColorPhaseByGradient[2000]; // dynamic based on pixel_width, set once assuming screen size kinda consistent


extern GLuint sr_create_program(const char* pVertexSource, const char* pFragmentSource);


/* Draw ellipse 2d */
extern void GLDrawEllipse (int segments, 
					GLfloat width, GLfloat height, GLfloat centerx,
					GLfloat centery, int filled);

/* Draw circle 2d */

extern void GLDrawString(char *string,float size,float x, float y);



extern GLuint loadShader(GLenum shaderType, const char* pSource);

#ifdef NDEBUG
#define checkGlError(dfdff) (0)
#define gerr(dsdssd) 
extern int checkGlErrorForReal(const char* op);
#else
extern int checkGlError(const char* op);
//#define gerr(a) checkGlError(__FILE__ ":" __LINE__ ":" a)
#define gerr(a) checkGlError3(__FILE__,__LINE__,a)
#define checkGlErrorForReal(dfdff) checkGlError(dfddf)
#endif

extern int checkGlError3(const char *filename,int lineno,const char* op);

extern void printGLString(const char *name, GLenum s);
extern float deg2rad(float);
extern rgb hsv2rgb(double H, double S, double V);
extern void rgbToHsv(float r, float g, float b, float hsv[]);
extern void hslToRgb(float h, float s, float l, float rgb[]);
extern float hue2rgb(float p, float q, float t);

extern int arraySlotCountf(GLfloat array1[],GLuint itemElementSize);
extern void normalizeXYZ(GLfloat *vector);
extern void crossProduct(GLfloat *result,GLfloat *a,GLfloat *b);
extern void ViewLookAt(glMatrix *view,GLfloat *eye,GLfloat *at,GLfloat *up);
extern void transformVector(GLfloat *xyzt,GLfloat *xyz,glMatrix *mat);
extern GLfloat smootherStep(GLfloat edge0, GLfloat edge1, GLfloat x);
extern GLfloat clamp(GLfloat x, GLfloat min, GLfloat max);
extern GLfloat weightedAverageDecel(GLfloat currentValue, GLfloat targetValue, GLfloat slowDownFactor);
extern void rotate_apple_only(int screen_orientation,glMatrix *m);
extern void rotate_generic(int screen_orientation,glMatrix *m);


#ifdef __cplusplus
}
#endif

#endif


