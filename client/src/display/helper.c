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






#include "helper.h"
#include "glue.h"
#include "packet_summary.h"
#include <sys/time.h>

static int helper_initted=0;

float helper_cos[4096];

void helper_init() {
if (helper_initted) return;
int i;
//helper_cos_factor = 3.14159*2.0/4096.;
for (i=0;i<4096;i++) {
  helper_cos[i] = (cosf( ((float)(i))*HELPER_COS_FACTOR1));
  }
//helper_cos_factor = 4096.f/(3.14159*2.0);
helper_initted=1;
}







#define DEG2RAD_FACTOR 0.01745329251994327777f
// ^^^ M_PI/180
float deg2rad(float deg){
	return deg * DEG2RAD_FACTOR;
}
			
			
			
			
   


float max(float a, float b){
	if(a>=b) return a;
	else return b;
}

float min(float a, float b){
	if(a<b) return a;
	else return b;
}


float threeway_max(float a, float b, float c) {
    return max(a, max(b, c));
}

float threeway_min(float a, float b, float c) {
    return min(a, min(b, c));
}

GLfloat clamp(GLfloat x, GLfloat min, GLfloat max){
    if (x < min) {
        x = min;
    }
    else if (x > max) {
        x = max;
    }
    return x;
}


GLfloat smootherStep(GLfloat edge0, GLfloat edge1, GLfloat x) {//not just smoothing x, multiplying by the range as well
	GLfloat result;
    //logit("---------smootherStep input::.. edge0 %f,  edge1 %f, x  %f", edge0,edge1,x);
    //x = clamp((x - edge0)/(edge1 - edge0), 0.0, 1.0);//this is the problem line// // // // // // get the f'ing arithmetic right// // // //
    // Evaluate polynomial
    x = x*x*x*(x*(x*6 - 15) + 10);
    result = x*(edge1-edge0)+edge0; /* if x is .5, edge0 is 50 edge1 is 100, then the new x is 75 */
    //logit("x after smooth---------%f, edge0::.. %f, edge1::..  %f, result %f", x, edge0, edge1,result);
    return result;
}   

GLfloat weightedAverageDecel(GLfloat currentValue, GLfloat targetValue, GLfloat slowDownFactor){
	currentValue = ((currentValue * (slowDownFactor - 1)) + targetValue) / slowDownFactor; 
	return currentValue;
}

void rgbToHsv(float r, float g, float b, float hsv[]) {
float rd = r;
    float gd=g;
    float bd = b;
float max = threeway_max(rd, gd, bd), min = threeway_min(rd, gd, bd);
float h, s, v = max;

float d = max - min;
s = max == 0.f ? 0.f : d / max;

if (max == min) {
h = 0.f; // achromatic
} else {
  if (max == rd) {
  h = (gd - bd) / d + (gd < bd ? 6.f : 0);
} else if (max == gd) {
  h = (bd - rd) / d + 2.f;
} else if (max == bd) {
  h = (rd - gd) / d + 4.f;
}
else { h=0.f;}
h /= 6.f;
}

hsv[0] = h;
hsv[1] = s;
hsv[2] = v;
}

float hue2rgb(float p, float q, float t){
    if(t < 0.f) t += 1.f;
    if(t > 1.f) t -= 1.f;
    if(t < 0.166666666666f) return p + (q - p) * 6.f * t;
    if(t < 0.5f) return q;
    if(t < 0.6666666666666f) return p + (q - p) * (0.6666666666f - t) * 6.f;
    return p;
}




void hslToRgb(float h, float s, float l,float rgb[]){
    float r, g, b;

    if(s == 0.f){
        r = g = b = l; // achromatic
    }else{

        float q = l < 0.5f ? l * (1.f + s) : l + s - l * s;
        float p = 2.f * l - q;
        r = hue2rgb(p, q, h + 0.33333333f);
        g = hue2rgb(p, q, h);
        //g = 0.5f;
        b = hue2rgb(p, q, h - 0.33333333f);
    }
    rgb[0]=r;
   rgb[1]=g;
    // rgb[1] = 0.2f;
    rgb[2]=b;
}




//   inconsistent pattern with rgbToHSV (like this approach better) but it's basically working for now http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
rgb hsv2rgb(double H, double S, double V)
{
    rgb RGB;
    double /* H = HSV.h, S = HSV.s, V = HSV.v, */
            P, Q, T,
            fract;
     H *= 360;//adapt for 0-1 hue input
    (H == 360.)?(H = 0.):(H /= 60.);
    fract = H - floor(H);

    P = V*(1. - S);
    Q = V*(1. - S*fract);
    T = V*(1. - S*(1. - fract));

    if      (0. <= H && H < 1.)
        RGB = (rgb){.r = V, .g = T, .b = P};
    else if (1. <= H && H < 2.)
        RGB = (rgb){.r = Q, .g = V, .b = P};
    else if (2. <= H && H < 3.)
        RGB = (rgb){.r = P, .g = V, .b = T};
    else if (3. <= H && H < 4.)
        RGB = (rgb){.r = P, .g = Q, .b = V};
    else if (4. <= H && H < 5.)
        RGB = (rgb){.r = T, .g = P, .b = V};
    else if (5. <= H && H < 6.)
        RGB = (rgb){.r = V, .g = P, .b = Q};
    else
        RGB = (rgb){.r = 0., .g = 0., .b = 0.};

    return RGB;
}






// WORLD_INTERNATIONAL off
const char *gStandard2dVertexShader =
"attribute vec4 vPosition;\n"
        "void main() {\n"
        "  gl_Position = vPosition;\n"
        "}\n";



/* draw all things with one color 
a_Color - attribute
vPosition - position of vector
*/
const char *gStandard2dVertexOneColorShader = 
"attribute vec4 vPosition;\n"
"uniform vec4 a_Color;\n"
        "varying vec4 v_Color;\n"
        "void main() {\n"
        "  v_Color = a_Color; \n"
        "  gl_Position = vPosition;\n"
        "}\n";

const char *gStandard2dFragmentOneColorShader = 
"precision mediump float;       \n"     // Set the default precision to medium. We don't need as high of a
                                            // precision in the fragment shader.
   "varying vec4 v_Color;          \n"     // This is the color from the vertex shader interpolated across the
                                            // triangle per fragment.
   "void main()                    \n"     // The entry point for our fragment shader.
   "{                              \n"
   "   gl_FragColor = v_Color;     \n"     // Pass the color directly through the pipeline.
   "}                              \n";



const char * gStandardPurpleFragmentShader =
"precision mediump float;\n"
        "void main() {\n"
        "  gl_FragColor = vec4(0.8, 0.0, 1.0, 1.0);\n"
        "}\n";




// justin2 - 43 with mvpmatrix, position, and a varying color 
#ifdef _WIN32
const char gStandard3dVertexVaryingColorShader[] =
        "precision mediump float;"
        "uniform mat4 u_mvpMat;"
        "attribute vec4 a_position;"
        "attribute vec4 a_color;"
        "varying vec4 v_color;"
        "void main()"
        "{"
        "gl_Position = u_mvpMat * a_position;"
        "v_color = a_color;"
        "}";
const char gStandard3dFragmentVaryingColorShader[] =
        "precision mediump float;"
        "varying vec4 v_color;"
        "void main()"
        "{"
        "    gl_FragColor = v_color;"
        "}";
#else
const char gStandard3dVertexVaryingColorShader[] =
        "precision mediump float;"
        "uniform mat4 u_mvpMat;"
        "attribute vec4 a_position;"
        "attribute vec4 a_color;"
        "varying lowp vec4 v_color;"
        "void main()"
        "{"
        "gl_Position = u_mvpMat * a_position;"
        "v_color = a_color;"
        "}";
const char gStandard3dFragmentVaryingColorShader[] =
        "varying lowp vec4 v_color;"
        "void main()"
        "{"
        "    gl_FragColor = v_color;"
        "}";
#endif
// WORLD_INTERNATIONAL on



/* the distortion shader is used for distorting frames into fiseye frames for various VR systems such as the oculus rift 
This was extrapolated and mimiced from raylib - but hard wired for OpenGL ES 2.0 
At first we put shims into raylib to deal with it's VR system, but by hard wiring it, perhaps we can get the apple system to work?
*/
// WORLD_INTERNATIONAL off

/* pulled from shader_distortion.h raylib */
const char gStandardDistortionVShader[] =
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

const char gStandardDistortionFShader[] = 
"#version 100                       \n"
"precision mediump float;           \n"     // precision required for OpenGL ES2 (WebGL)
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
// WORLD_INTERNATIONAL on


// from android samples
/* return current time in milliseconds */
static double now_ms(void) {

    struct timeval res;
    sr_gettimeofday(&res,NULL);
    return 1000.0 * ((double)res.tv_sec) + ((double) res.tv_usec) * 0.001;

}


void multMatrix(glMatrix *result, glMatrix *srcA, glMatrix *srcB)
{
    glMatrix    tmp;
    int         i;
 
    for (i=0; i<4; i++)
    {
        tmp.mat[i][0] = (srcA->mat[i][0] * srcB->mat[0][0]) +
                        (srcA->mat[i][1] * srcB->mat[1][0]) +
                        (srcA->mat[i][2] * srcB->mat[2][0]) +
                        (srcA->mat[i][3] * srcB->mat[3][0]) ;
 
        tmp.mat[i][1] = (srcA->mat[i][0] * srcB->mat[0][1]) +
                        (srcA->mat[i][1] * srcB->mat[1][1]) +
                        (srcA->mat[i][2] * srcB->mat[2][1]) +
                        (srcA->mat[i][3] * srcB->mat[3][1]) ;
 
        tmp.mat[i][2] = (srcA->mat[i][0] * srcB->mat[0][2]) +
                        (srcA->mat[i][1] * srcB->mat[1][2]) +
                        (srcA->mat[i][2] * srcB->mat[2][2]) +
                        (srcA->mat[i][3] * srcB->mat[3][2]) ;
 
        tmp.mat[i][3] = (srcA->mat[i][0] * srcB->mat[0][3]) +
                        (srcA->mat[i][1] * srcB->mat[1][3]) +
                        (srcA->mat[i][2] * srcB->mat[2][3]) +
                        (srcA->mat[i][3] * srcB->mat[3][3]) ;
    }
    memcpy(result, &tmp, sizeof(glMatrix));
}



void loadIdentity(glMatrix *result)
{
    memset(result, 0x0, sizeof(glMatrix));
    result->mat[0][0] = 1.0f;
    result->mat[1][1] = 1.0f;
    result->mat[2][2] = 1.0f;
    result->mat[3][3] = 1.0f;
}

void translateMatrix(glMatrix *result, GLfloat x,GLfloat y,GLfloat z) {
	glMatrix transmat;
    //memset(transmat, 0x0, sizeof(glMatrix));
    loadIdentity(&transmat);

     transmat.mat[3][0] = x;
     transmat.mat[3][1] = y;
     transmat.mat[3][2] = z;
     transmat.mat[3][3] = 1.0f;   
/*
      transmat.mat[0][3] = x;
     transmat.mat[1][3] = y;
     transmat.mat[2][3] = z;
     transmat.mat[3][3] = 1.0f; 
*/

    multMatrix( result, &transmat, result );
}

void translateMatrix2(glMatrix *result, GLfloat x,GLfloat y,GLfloat z) {
	glMatrix transmat;
    //memset(transmat, 0x0, sizeof(glMatrix));
    loadIdentity(&transmat);
/*
     transmat.mat[3][0] = x;
     transmat.mat[3][1] = y;
     transmat.mat[3][2] = z;
     transmat.mat[3][3] = 1.0f;   
*/
      transmat.mat[0][3] = x;
     transmat.mat[1][3] = y;
     transmat.mat[2][3] = z;
     transmat.mat[3][3] = 1.0f; 


    multMatrix( result, &transmat, result );
}

   
     
void scaleMatrix(glMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz)
{
    result->mat[0][0] *= sx;
    result->mat[0][1] *= sx;
    result->mat[0][2] *= sx;
    result->mat[0][3] *= sx;
 
    result->mat[1][0] *= sy;
    result->mat[1][1] *= sy;
    result->mat[1][2] *= sy;
    result->mat[1][3] *= sy;
 
    result->mat[2][0] *= sz;
    result->mat[2][1] *= sz;
    result->mat[2][2] *= sz;
    result->mat[2][3] *= sz;
}

void translatePORT_ATTPT_FROM_GLM_Matrix(glMatrix *result, GLfloat x, GLfloat y, GLfloat z)//DOESN'T work cause of type conversion errors
{
		//result->mat[3] = result->mat[0] * x + result->mat[1] * y + result->mat[2] * z + result->mat[3];
}


void polarExplode(glMatrix *result,GLfloat x, GLfloat y, GLfloat z)//attempted to port & convert translate from row major, from Matrices.cpp (matrix library c++)
{

    result->mat[0][0] += result->mat[3][0] * x;   
    result->mat[0][1] += result->mat[3][1] * x;   
    result->mat[0][2] += result->mat[3][2]* x;   
    result->mat[0][3] += result->mat[3][3]* x;
    
    result->mat[1][0] += result->mat[3][0] * y;   
    result->mat[1][1] += result->mat[3][1] * y;   
    result->mat[1][2] += result->mat[3][2]* y;   
    result->mat[1][3]+= result->mat[3][3]* y;
    
    result->mat[2][0] += result->mat[3][0] * z;   
    result->mat[2][1] += result->mat[3][1] * z;   
    result->mat[2][2]+= result->mat[3][2]* z;   
    result->mat[2][3]+= result->mat[3][3]* z;

    
}


void rotationMatrix(glMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat sinAngle, cosAngle;
    GLfloat mag = sqrtf(x * x + y * y + z * z);
 
    sinAngle = sin ( angle * M_PI / 180.0f );
    cosAngle = cos ( angle * M_PI / 180.0f );
    if ( mag > 0.0f )
    {
        GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
        GLfloat oneMinusCos;
        glMatrix rotMat;
 
        x /= mag;
        y /= mag;
        z /= mag;
 
        xx = x * x;
        yy = y * y;
        zz = z * z;
        xy = x * y;
        yz = y * z;
        zx = z * x;
        xs = x * sinAngle;
        ys = y * sinAngle;
        zs = z * sinAngle;
        oneMinusCos = 1.0f - cosAngle;
 
        rotMat.mat[0][0] = (oneMinusCos * xx) + cosAngle;
        rotMat.mat[0][1] = (oneMinusCos * xy) - zs;
        rotMat.mat[0][2] = (oneMinusCos * zx) + ys;
        rotMat.mat[0][3] = 0.0F;
 
        rotMat.mat[1][0] = (oneMinusCos * xy) + zs;
        rotMat.mat[1][1] = (oneMinusCos * yy) + cosAngle;
        rotMat.mat[1][2] = (oneMinusCos * yz) - xs;
        rotMat.mat[1][3] = 0.0F;
 
        rotMat.mat[2][0] = (oneMinusCos * zx) - ys;
        rotMat.mat[2][1] = (oneMinusCos * yz) + xs;
        rotMat.mat[2][2] = (oneMinusCos * zz) + cosAngle;
        rotMat.mat[2][3] = 0.0F;
 
        rotMat.mat[3][0] = 0.0F;
        rotMat.mat[3][1] = 0.0F;
        rotMat.mat[3][2] = 0.0F;
        rotMat.mat[3][3] = 1.0F;
 
        multMatrix( result, &rotMat, result );
    }
}



void frustumMatrix(glMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    glMatrix    frust;
 
    if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
         (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
            return;
 
    frust.mat[0][0] = 2.0f * nearZ / deltaX;
    frust.mat[0][1] = frust.mat[0][2] = frust.mat[0][3] = 0.0f;
 
    frust.mat[1][1] = 2.0f * nearZ / deltaY;
    frust.mat[1][0] = frust.mat[1][2] = frust.mat[1][3] = 0.0f;
 
    frust.mat[2][0] = (right + left) / deltaX;
    frust.mat[2][1] = (top + bottom) / deltaY;
    frust.mat[2][2] = -(nearZ + farZ) / deltaZ;
    frust.mat[2][3] = -1.0f;
 
    frust.mat[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust.mat[3][0] = frust.mat[3][1] = frust.mat[3][3] = 0.0f;
 
    multMatrix(result, &frust, result);
}


void normalizeXYZ(GLfloat *vector) {
GLfloat inverse_size;
inverse_size = sqrtf(vector[0]*vector[0]+vector[1]*vector[1]+vector[2]*vector[2]);
if (inverse_size ==0.f) return;
inverse_size = 1.f/inverse_size;
vector[0] *= inverse_size;
vector[1] *= inverse_size;
vector[2] *= inverse_size;
}

void crossProduct(GLfloat *result,GLfloat *a,GLfloat *b) {
result[0] = a[1]*b[2] - a[2]*b[1];
result[1] = a[2]*b[0] - a[0]*b[2];
result[2] = a[0]*b[1] - a[1]*b[0];
}

void ViewLookAt(glMatrix *view,GLfloat *eye,GLfloat *at,GLfloat *up) {
GLfloat forward[3];
GLfloat side[3];
forward[0]=at[0]-eye[0];
forward[1]=at[1]-eye[1];
forward[2]=at[2]-eye[2];
normalizeXYZ(forward);
crossProduct(side,forward,up);
normalizeXYZ(side);
crossProduct(up,side,forward);
view->mat[0][0] = side[0];
view->mat[0][1] = up[0];
view->mat[0][2] = -forward[0];
view->mat[0][3] = 0.f;

view->mat[1][0] = side[1];
view->mat[1][1] = up[1];
view->mat[1][2] = -forward[1];
view->mat[1][3] = 0.f;

view->mat[2][0] = side[2];
view->mat[2][1] = up[2];
view->mat[2][2] = -forward[2];
view->mat[2][3] = 0.f;

view->mat[3][0] = 0.f;
view->mat[3][1] = 0.f;
view->mat[3][2] = 0.f;
view->mat[3][3] = 1.f;
}

void transformVector(GLfloat *xyzt,GLfloat *xyz,glMatrix *mat) {
/*
xyzt[0] = xyz[0] * mat->mat[0][0] + xyz[1]*mat->mat[1][0] + xyz[2]*mat->mat[2][0] + mat->mat[3][0];
xyzt[1] = xyz[0] * mat->mat[0][1] + xyz[1]*mat->mat[1][1] + xyz[2]*mat->mat[2][1] + mat->mat[3][1];
xyzt[2] = xyz[0] * mat->mat[0][2] + xyz[1]*mat->mat[1][2] + xyz[2]*mat->mat[2][2] + mat->mat[3][2];
*/

/*
xyzt[0] = xyz[0] * mat->mat[0][0] + xyz[1]*mat->mat[0][1] + xyz[2]*mat->mat[0][2] + mat->mat[0][0];
xyzt[1] = xyz[0] * mat->mat[1][0] + xyz[1]*mat->mat[1][1] + xyz[2]*mat->mat[1][2] + mat->mat[1][1];
xyzt[2] = xyz[0] * mat->mat[2][0] + xyz[1]*mat->mat[2][1] + xyz[2]*mat->mat[2][2] + mat->mat[2][2];
*/

GLfloat *m = (GLfloat *)mat;
xyzt[0] = xyz[0]*m[0]+xyz[1]*m[4]+xyz[2]*m[8]+m[12];
xyzt[1] = xyz[0]*m[1]+xyz[1]*m[5]+xyz[2]*m[9]+m[13];
xyzt[2] = xyz[0]*m[2]+xyz[1]*m[6]+xyz[2]*m[10]+m[14];

}



/* 
int64_t getTimeNsec() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (int64_t) now.tv_sec*1000000000LL + now.tv_nsec;
}

 */




void GLDrawString(char *string,float size,float x, float y) 
{
/* ??? - use letter fonts or the modern font layout to do this 
This is still here - to allow perhaps my hextop tower defense to work
*/
}








GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    logit("Could not compile shader %d:\n%s\n",
                         shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}








void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    logit("GL %s = %s\n", name, v);
}

#ifdef NDEBUG
int checkGlErrorReal(const char* op) {
#else
int checkGlError(const char* op) {
#endif
int eflag=0;
    for (GLint error = glGetError(); error; error  = glGetError()) {
        eflag=1;
//#define WORLD_display_helper_ ""
        char *s = WORLD_display_helper_;
//#define WORLD_GL_INAVLID_ENUM "GL_INAVLID_ENUM"
	if  (error==GL_INVALID_ENUM) {s=WORLD_GL_INAVLID_ENUM;}
//#define WORLD_GL_INVALID_VALUE "GL_INVALID_VALUE"
	else if (error==GL_INVALID_VALUE) {s=WORLD_GL_INVALID_VALUE;}
//#define WORLD_GL_INVALID_OPERATION "GL_INVALID_OPERATION"
	else if (error==GL_INVALID_OPERATION) {s=WORLD_GL_INVALID_OPERATION;}
//#define WORLD_GL_INVALID_FRAMEBUFFER_OPERATION "GL_INVALID_FRAMEBUFFER_OPERATION"
	else if (error==GL_INVALID_FRAMEBUFFER_OPERATION) {s=WORLD_GL_INVALID_FRAMEBUFFER_OPERATION;}
//#define WORLD_GL_OUT_OF_MEMORY "GL_OUT_OF_MEMORY"
	else if (error==GL_OUT_OF_MEMORY) {s = WORLD_GL_OUT_OF_MEMORY;}
        logit("after %s() glError %s(0x%x)\n", op, s,error);
    }
return eflag;
}

int checkGlError3(const char *filename,int lineno,const char* op) {
char buf[4000];
sprintf(buf,"%s:%d: %s",filename,lineno,op);
return checkGlError(buf);
}

/* these rotate things only for apple - because the start and end coordinates are different I think */
void rotate_apple_only(int screen_orientation,glMatrix *m) {
#ifdef __APPLE__
      if (screen_orientation==2) {
        rotationMatrix(m,90.f,0.f,0.f,1.f);
        }
      else if (screen_orientation==3) {
        rotationMatrix(m,180.f,0.f,0.f,1.f);
        }
      else if (screen_orientation==4) {
        rotationMatrix(m,270.f,0.f,0.f,1.f);
        }
#endif
}


/* This rotates generically - which I think is useful for larry_harvey - why I am not 100% sure */
void rotate_generic(int screen_orientation,glMatrix *m) {
#ifndef __APPLE__
      if (screen_orientation==4) {
        rotationMatrix(m,90.f,0.f,0.f,1.f);
        }
      else if (screen_orientation==3) {
        rotationMatrix(m,180.f,0.f,0.f,1.f);
        }
      else if (screen_orientation==2) {
        rotationMatrix(m,270.f,0.f,0.f,1.f);
        }
#endif
}






GLuint sr_create_program(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }
     
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!fragmentShader) {
        return 0;
    }
     
    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, fragmentShader);
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
                    logit("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            glDeleteShader(fragmentShader);
            glDeleteShader(vertexShader);
            program = 0;
        }
    }

    // We don't need the shaders anymore - the program is enough
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    return program;
}

