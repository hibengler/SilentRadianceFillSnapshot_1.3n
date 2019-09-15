/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Library Public License   
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Library Public License for more details.

You should have received a copy of the GNU Library Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/








/*$Revision: 1.54 $*/





#ifdef ANDROID
#include <jni.h>
#endif





#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glue.h"
#include "helper.h"
#include "packet_summary.h"
#include "display_set.h"
#include "display_main.h"
#include "main_menu.h"
#include "default.h"
#include "impact.h"
//#include "zcool-xiaowei.h"

#define FTGLES2 1
#include <FTGLES2/ftgles2.h>
#include <FTGLES2/ftgles2Glue.h>


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

GLuint mColorHandle;

static glMatrix myMatrix_one;
static int mMVPMatrixHandle;

static GLuint ftgles2_shaderProgram = 0;
static GLuint ftgles2_cameraUniform= -1;
static GLuint ftgles2_ucolor;
static GLfloat ftgles2_cameraMatrix[16];
static FTGLfont *ftgles2_button_font=NULL;
static FTGLfont *ftgles2_font=NULL;
static FTGLfont *ftgles2_background_font=NULL;
static glMatrix ftgles2_output_matrix; // output matrix for rendering fonts
static glMatrix ftgles2_button_output_matrix; // output matrix for rendering fonts in button

//"precision mediump float;\n"
// WORLD_INTERNATIONAL off

static const GLchar * ftgles2_vertex_shader_text =
"precision mediump float;\n"
"attribute vec4 ft_position;\n"
"attribute vec4 ft_color;\n"
"attribute vec2 ft_texCoord;\n"
"varying vec4 ft_colorVarying;\n"
"varying vec2 ft_texture_coordinate;\n"
"uniform vec4 ft_ucolor;\n"
" \n"
"uniform mat4 ft_camera;\n"
" \n"
"void main()\n"
"{\n"
"        gl_Position = (ft_camera * ft_position);\n"
"        ft_colorVarying = ft_ucolor;\n"
"        ft_texture_coordinate = ft_texCoord;\n"
"}\n";


//"precision mediump float;\n"

static const GLchar*  ftgles2_fragment_shader_text =
"precision mediump float;\n"
"varying vec4 ft_colorVarying;\n"
"varying vec2 ft_texture_coordinate;\n"
"uniform sampler2D ft_color_sampler;\n"
" \n"
"void main()\n"
"{\n"
"vec4 fc;\n"
"float factor;\n"
"fc = texture2D(ft_color_sampler, ft_texture_coordinate);\n"
"factor = fc.a;\n"
"fc.r = ft_colorVarying.r * factor;\n"
"fc.g = ft_colorVarying.g * factor;\n"
"fc.b = ft_colorVarying.b * factor;\n"
"fc.a = factor;\n"
"   gl_FragColor =  fc;\n"
"}\n";

// WORLD_INTERNATIONAL on




enum {
    RENDER_ATTRIB_VERTEX,
    RENDER_ATTRIB_COLOR,
    RENDER_NUM_ATTRIBUTES
};






static void aglOrtho(float *mOut, float left, float right, float bottom, float top, float zNear, float zFar)
{
    memset(mOut, 0,sizeof(float) * 16);
    
    if (right != left)
    {
        mOut[ 0] = 2 / (right - left);
        mOut[ 1] = 0;
        mOut[ 2] = 0;
        mOut[ 3] = - ((right + left) / (right - left));
    }
    
    if (top != bottom)
    {
        mOut[ 4] = 0;
        mOut[ 5] = 2 / (top - bottom);
        mOut[ 6] = 0;
        mOut[ 7] = - ((top + bottom) / (top - bottom));
    }
	
    if (zFar != zNear)
    {
        mOut[ 8] = 0;
        mOut[ 9] = 0;
        mOut[10] = -2 / (zFar - zNear);
        mOut[11] = - ((zFar + zNear) / (zFar - zNear));
    }
	
	mOut[12] = 0;
	mOut[13] = 0;
	mOut[14] = 0;
	mOut[15] = 1;
}

void init_ftgles2_once(int w, int h,int graphics_type) {
//logit("init_ftgles2_once %d,%d  %d\n",w,h,graphics_type);

ftgles2_shaderProgram = sr_create_program(ftgles2_vertex_shader_text,
    ftgles2_fragment_shader_text);
gerr("a0000");

// logit("s2");
   


glBindAttribLocation(ftgles2_shaderProgram, RENDER_ATTRIB_VERTEX, "ft_position");
gerr("j");
glBindAttribLocation(ftgles2_shaderProgram, RENDER_ATTRIB_COLOR, "ft_color");
gerr("k");
glBindAttribLocation(ftgles2_shaderProgram, 2, "ft_texCoord");
gerr("k");


//logit("s3");
ftgles2_cameraUniform = glGetUniformLocation(ftgles2_shaderProgram, "ft_camera");
gerr("m");
ftgles2_ucolor = glGetUniformLocation(ftgles2_shaderProgram, "ft_ucolor");
gerr("mjkl;");



glUseProgram(ftgles2_shaderProgram);
   gerr("gluse\n");

#define FONT_TTF bin2c_impact_ttf,IMPACT_TTF_SIZE
//#define FONT_TTF bin2c_ZCOOLXiaoWei_Regular_ttf,ZCOOL_TTF_SIZE

glUseProgram(0);
   gerr("gluse\n");

//logit("s4");
//ftgles2_font = ftglCreateTextureFontData(FONT_TTF);
ftgles2_font = ftglCreatePolygonFontData(FONT_TTF);
ftglSetFontFaceSize(ftgles2_font, 24,24);
gerr("p");

ftgles2_background_font = 
  ftglCreatePolygonFontData(FONT_TTF);
ftglSetFontFaceSize(ftgles2_background_font, 27,27);
gerr("q");

ftgles2_button_font
 = ftglCreatePolygonFontData(FONT_TTF);
ftglSetFontFaceSize(ftgles2_button_font, 34,34);
// might need to be 72
gerr("r");

//logit("init_ftgles2_init done %d,%d  %d\n",w,h,graphics_type);

glUseProgram(one_color_program);
   gerr("gluse\n");
}




void init_ftgles2(int w, int h,int graphics_type) {
//logit("init_ftgles2 %d,%d  %d\n",w,h,graphics_type);
//    loadIdentity(ftgles2_prefix_matrix);
    int smaller = w;
    if (smaller>h) smaller=h;
//    scaleMatrix(ftgles2_prefix_matrix,1.f/((float)(smaller)),1.f/((float)(smaller)),1.f((float)(smaller)));
//    scaleMatrix(ftgles2_prefix_matrix,1.f/((float)(smaller)),1.f/((float)(smaller)),1.f((float)(smaller)));
//    if ((w<500)||(h<500)) {
//       w = (w*3)/2;
//       h = (h*3)/2;
//       }
glUseProgram(ftgles2_shaderProgram);
gerr("yui");

//logit("init_ftgles2 %f,%f  to %f,%f, with %f,%f\n", 
//   (float)(-w),(float)(w),(float)(-h),(float)(h), -10000.0f, 10000.0f);

    aglOrtho(ftgles2_cameraMatrix,   (float)(-w),(float)(w),(float)(-h),(float)(h), -10000.0f, 10000.0f);


//    logit("done init_ftgles2 %d,%d, %d\n",w,h,graphics_type);
    
glUseProgram(0);
gerr("yui");
   
}





int visual_voice_setupGraphics(int w, int h,int graphics_type) {
//logit("Visual voice setupGraphics %d,%d,%d  with ren_before %d\n",w,h,graphics_type,ran_setup_graphics_once_before);
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
 
//myMatrix = origin_matrix;
myMatrix = projectionMatrix;
// was projection_matrix
#ifdef __APPLE__
      if (screen_orientation==2) {
        rotationMatrix(&myMatrix,90.f,0.f,0.f,1.f);
	}
      else if (screen_orientation==3) {
        rotationMatrix(&myMatrix,180.f,0.f,0.f,1.f);
	}
      else if (screen_orientation==4) {
        rotationMatrix(&myMatrix,270.f,0.f,0.f,1.f);
	}
#endif

 
if (!ran_setup_graphics_once_before) {
		  
  // letters init
  lc = linit_context(); // we will get our own letter context
    
  // vvv this is done in main_menu.c
  //letters_opengles_setgraphics(one_color_program,colorHandle,gvPositionHandle,mPositionHandle); // needs to be set up once, with a single color program
  mColorHandle = glGetUniformLocation(one_color_program, "v_Color");
      
  init_ftgles2_once(w,h,graphics_type);
  }
gerr("glup4aaaaa");

    
glUseProgram(ftgles2_shaderProgram); 
gerr("glup44p");
  
init_ftgles2(w,h,graphics_type);
gerr("cp1");

glUseProgram(ftgles2_shaderProgram); 
gerr("glup44p");
//logit("ftgles2_cameraUniform %d, ftgles2_cameraMatrix %lx\n",ftgles2_cameraUniform,(long)(ftgles2_cameraMatrix));


glUniformMatrix4fv(ftgles2_cameraUniform, 
   1, GL_FALSE, ftgles2_cameraMatrix);      
gerr("cp2");
//logit("done with visual_voice_setupGraphics\n");
glUseProgram(one_color_program);
return 1;
}




void visual_voice_renderBackground(int screen,glMatrix *eye) {
}





/*ftgles2 uses ftgles2 fonts */
static void double_ftgles2_out_c1(fl scale,fl x,fl y,fl z,fl width,
     char *string,float r1,float g1, float b1, float al1,float r2,float g2, float b2, float al2) {
if (!(ftgles2_font && ftgles2_shaderProgram)) return;
if (ftgles2_cameraUniform==-1) return;

float x1 = x;
float y1 = y;
//if (width<0.f) width=1.f;
      
glMatrix m1;

GLfloat *c=ftgles2DirectAccessToFakeCurrentColor();



loadIdentity(&m1);
translateMatrix(&m1,x1-(width*(1.5/(24.*15.))),y1,0.03f);
scaleMatrix(&m1,0.005f,0.005f,0.005f);
multMatrix(&ftgles2_output_matrix,&m1,&myMatrix);

glUseProgram(ftgles2_shaderProgram); 
gerr("glup44p");

glUniformMatrix4fv(ftgles2_cameraUniform, 1, GL_FALSE, (float *)(&ftgles2_output_matrix));
gerr("ftgles023223");

c[0]=r2;c[1]=g2;c[2]=b2;c[3]=al2;
glUniform4f(ftgles2_ucolor,r2,g2,b2,al2);
gerr("ftgles02");
//logit("qqq_out_c1 scale %f	%f,%f,%f  width %f,   rgb %f,%f,%f,%f  -> %f,%f,%f,%f string %s\n",scale,x,y,z,width,
//    r1,g1,b1,al1,r2,g2,b2,al2,string);

      glUseProgram(one_color_program);
      gerr("r12");
//ooo      
      glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&ftgles2_output_matrix));
      gerr("r10");
      glUniform4f(mColorHandle,r2,g2,b2,al2);
      gerr("ftgles02");
      

ftglRenderFont(ftgles2_background_font,string,FTGL_RENDER_FRONT);
gerr("ftglesbb");
            


}


static void double_ftgles2_out_c2(fl scale,fl x,fl y,fl z,fl width,
     char *string,float r1,float g1, float b1, float al1,float r2,float g2, float b2, float al2) {
if (!(ftgles2_font && ftgles2_shaderProgram)) return;
if (ftgles2_cameraUniform==-1) return;


float x1 = x;
float y1 = y;
//if (width<0.f) width=1.f;
      
glMatrix m1;

GLfloat *c=ftgles2DirectAccessToFakeCurrentColor();



loadIdentity(&m1);
translateMatrix(&m1,x1,y1,-0.03f);
scaleMatrix(&m1,0.005f,0.005f,0.005f);
multMatrix(&ftgles2_output_matrix,&m1,&myMatrix);
glUseProgram(ftgles2_shaderProgram); 
gerr("glup44p");

glUniformMatrix4fv(ftgles2_cameraUniform, 1, GL_FALSE, (float *)(&ftgles2_output_matrix));
gerr("ftgles023223aa");

c[0]=r1;c[1]=g1;c[2]=b1;c[3]=al1;
glUniform4f(ftgles2_ucolor,r1,g1,b1,al1);
gerr("ftglesb");
//logit("qqq_out_c2 scale %f	%f,%f,%f  width %f,   rgb %f,%f,%f,%f  <- %f,%f,%f,%f  string %s\n",scale,x,y,z,width,
//    r1,g1,b1,al1,r2,g2,b2,al2,string);

gerr("ftgles02");

glUniform4f(ftgles2_ucolor,r1,g1,b1,al1);
gerr("ftgles02ddd");

glUseProgram(one_color_program);
gerr("r12");
//ooo      
      glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&ftgles2_output_matrix));
      gerr("r10");
      glUniform4f(mColorHandle,r1,g1,b1,al1);
      gerr("ftgles02");

ftglRenderFont(ftgles2_font,string,FTGL_RENDER_FRONT);
gerr("ftglesb");



}



void compute_visual_voices(volatile packet_measure_chunk *mc) {
//logit("mc %lx b %d\n",(long long)(mc),mc->number_line_buffers);
for (int i=0;i<mc->number_line_buffers;i++) {
  volatile visual_voice_line *vvl = mc->visual_voice_lines+i;
  int j;

  /* step 1- figure out the length of the symbols */
  for (j=0;j<vvl->number_of_symbols;j++) {
    if (vvl->on_this_line[j]) {

      float bounds[6];
//    logit("symbol (%s)",vvl->symbols[j]);
      ftglGetFontBBox(ftgles2_font, ((visual_voice_line *)(vvl))->symbols[j],(int)(strlen(  ((visual_voice_line *)(vvl))->symbols[j] )),bounds);
//    logit("%f,%f,%f  -> %f,%f,%f\n",bounds[0],
//     bounds[1],bounds[2],bounds[3],bounds[4],bounds[5]);
      float llx=bounds[0]/15.;
      float lly=bounds[1]/15.;
      float urx=bounds[3]/15.;
      float ury=bounds[4]/15.;
      vvl->x_ul[j] = -llx;
      vvl->y_ul[j] = -lly;
      vvl->width[j] = urx-llx;
      vvl->height[j] = ury-lly;
      }
    }
  }
  
/* step 2 - Set the ul and ur relative to the other symbols - including symbols on other lines */

for (int i=0;i<mc->number_line_buffers;i++) {
  visual_voice_line *vvl = (visual_voice_line *)(mc->visual_voice_lines+i);

  float startx=0.f;
//  float starty=0.f;
  for (int j=0;j<vvl->number_of_symbols;j++) {
    float xoffset=0.f;
    if (vvl->space_prefix[j]) {
      xoffset += 1.0f;
      }
    else if (j) {
      xoffset += 0.f;
      }    
    
    float max_x = -1000000.f;
    float max_y = -1000000.f;
    for (int jj=0;jj<mc->number_line_buffers;jj++) {
      float local_startx = 0.f;
      float local_starty = 0.f;
      if (mc->visual_voice_lines[jj].space_prefix[j]) {
        local_startx += 1.0f;
        }
      else if (j) {
        local_startx += 0.f;
        }
      if (mc->visual_voice_lines[jj].on_this_line[j]) {
        if ((local_startx+mc->visual_voice_lines[jj].width[j]) > max_x) max_x = (local_startx+mc->visual_voice_lines[jj].width[j]);
        if ((local_starty+mc->visual_voice_lines[jj].height[j]) > max_y) max_y = (local_starty+mc->visual_voice_lines[jj].height[j]);
        }
      }
    if (max_x == -1000000.f) max_x = startx;
    vvl->x[j] = startx+xoffset;
    vvl->y[j] = 0.f;
    startx = startx + max_x;
//    if (i) logit("line %d symbols %s %f\n",i,vvl->symbols[j],vvl->x[j]);
    
    } // for each symbol
  vvl->computed = 2;
//  if (i) logit("line %d symbols %d\n",i,vvl->number_of_symbols);
  } // for each line
  
 
/* junk to change height
float maxx=0.f;  
for (j=0;j<vvl->number_of_symbols;j++) {
  if(val->x_ul[j])
  */  
    
}







void visual_voice_step() {
int s=packet_measurechunkhead;
while (s != packet_measurechunktail) {
  volatile packet_measure_chunk *mc = packet_measure_chunks+s;
  int i;
/*  for (i=0;i<mc->number_line_buffers;i++) {
    volatile visual_voice_line *vvl = mc->visual_voice_lines+i;
    if (vvl->computed==0) {
      build_visual_voice_lines(mc);
      break;
      }
    }*/
  for (i=0;i<mc->number_line_buffers;i++) {
    volatile visual_voice_line *vvl = mc->visual_voice_lines+i;
    if (vvl->computed==1) {
      compute_visual_voices(mc);
      break;
      }
    }
  s = (s + 1) % MEASURE_CHUNK_COUNT;
  }  
}















void visual_voice_renderFrame(int screen,glMatrix *eye) {    
                                                          if (packet_summary.current_frame==-1) return;

// this section fixed androids 1.3n for silentradiance.
glUseProgram(one_color_program);
gerr("glUseProgram1");

// ppp
//glEnable(GL_BLEND);   // VERY important - took 5 days
//gerr("blendcol");

// Apply the projection and view transformation
glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&myMatrix_one));
gerr("glmatm4p");
 glLineWidth(1.0f);
gerr("glups44p");
							  
//logit("qqq_visual_voice_renderFrame screen %d\n",screen);
glUseProgram(ftgles2_shaderProgram); 
gerr("glup44p");

//glUniformMatrix4fv(ftgles2_cameraUniform, 1, GL_FALSE, (GLfloat *)(&myMatrix));

glDisable(GL_BLEND);   // VERY important - took 5 days
// was enable - needed to be there?
gerr("glup44sp");
glDisable(GL_DEPTH_TEST);

 glLineWidth(1.0f);
gerr("glups44p");

// Apply the projection and view transformation
//glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&myMatrix));
gerr("glmatm4p");
float scale=0.08;
float startx= -10.f; // constant offset
float starty= 4.f;

int s=packet_measurechunkhead;
while (s != packet_measurechunktail) {
  packet_measure_chunk *mc = (packet_measure_chunk *)(packet_measure_chunks+s);
  int try_to_remove_packet = 0;
  int iii;
  for (iii=0;iii<mc->number_line_buffers;iii++) {
    visual_voice_line *vvl = mc->visual_voice_lines+iii;
    if (vvl->computed!=2) {
//       if (iii) logit("line %d aaaak\n",iii);
      continue;
      }
    if (vvl->done_by>FRAMES_WHERE_DONE_VISUAL_BECOMES_GONE_VISUAL) {
//       if (iii) logit("line %d jukk\n",iii);
      vvl->number_of_symbols = 0;
      try_to_remove_packet = 1;
      continue;
      }
    else if (vvl->done_by) {
      float factor = (float)(vvl->done_by-1) * (float)(1.f/(float)(FRAMES_WHERE_DONE_VISUAL_BECOMES_GONE_VISUAL)) ;  
      if (factor>1.f) factor=1.f;
      starty += 1.66f * factor;  
      }
    int j;
    for (j=0;j<vvl->number_of_symbols;j++) {
      float factor = 1.0f;
      if (vvl->done_by) {
        factor = 1.0f - (  (float)(vvl->done_by-1) * 0.05 );
        if (factor<=0.f) {
          factor=0.f;
	  break;
	  }
	}

      if (!vvl->on_this_line[j]) {
////        if (iii) logit("line %d sym %d not on\n",iii,j);
        continue;
        }
	
	
      float r,g,b;	
      r=0.89;
      g=0.65;
      b=0.9;	
      if (mc->number_line_buffers>1) {
        if (iii==0) {
	  r=0.2;g=0.;b=0.3;
	  }
        else if (iii==1) {
	  r=0.3;g=0.;b=1.;
	  }
        else if (iii==2) {
	  r=0.8;g=0.7;b=0.0;
	  }
	}  
      if ((vvl->current_symbol==j) && (vvl->symbol_passed_by<32)) {
        double_ftgles2_out_c1(scale,scale*(startx+(vvl->x[j])),
           scale*starty,0.f,vvl->width[j],vvl->symbols[j],1.f,1.f,1.f,1.f,0.6f,0.6f,0.6f,1.f);
        }
      else {
        double_ftgles2_out_c1(scale,scale*(startx+(vvl->x[j])),
          scale*starty,0.f,vvl->width[j],vvl->symbols[j],r*factor,g*factor,b*factor,1.f,0.f,0.f,0.0f,1.f);
        }
      gerr("koutd");
      } // for each symbol
      
      
    for (j=0;j<vvl->number_of_symbols;j++) {
      float factor = 1.0f;
      if (vvl->done_by) {
        factor = 1.0f - (  (float)(vvl->done_by-1) * 0.05 );
        if (factor<=0.f) {
          factor=0.f;
	  break;
	  }
	}

      if (!vvl->on_this_line[j]) {
        continue;
        }
	
	
      float r,g,b;	
      r=0.89;
      g=0.75;
      b=1.;	
      if (mc->number_line_buffers>1) {
        if (iii==1) {
	  r=0.8;g=0.;b=0.2;
	  }
        else if (iii==2) {
	  r=0.2;g=0.0;b=0.9;
	  }
        else if (iii==3) {
	  r=0.8;g=0.6;b=0.0;
	  }
	}  
      if ((vvl->current_symbol==j) && (vvl->symbol_passed_by<32)) {
        double_ftgles2_out_c2(scale,scale*(startx+(vvl->x[j])),
           scale*starty,0.f,vvl->width[j],vvl->symbols[j],1.f,1.f,1.f,1.f,0.6f,0.6f,0.6f,1.f);
        }
      else {
        double_ftgles2_out_c2(scale,scale*(startx+(vvl->x[j])),
          scale*starty,0.f,vvl->width[j],vvl->symbols[j],r*factor,g*factor,b*factor,1.f,0.f,0.f,0.0f,1.f);
        }
      gerr("koutd");
      } // for each symbol
      
    starty -= 1.66f;
    } // for each line 
  if (s != packet_measurechunkhead) {
    try_to_remove_packet = 0;
    }
    
  s = (s + 1) % MEASURE_CHUNK_COUNT;
  if (try_to_remove_packet) {
    packet_measurechunkhead=s;
    }
  }  // for each mc

glDisable(GL_BLEND);   // VERY important - took 5 days
// was enable - needed to be there?
gerr("glup44sp");
glDisable(GL_DEPTH_TEST);

//logit("done visual_voice_renderFrame screen %d  program %d\n",screen,ftgles2_shaderProgram);
}		    


				    
								    
												    
																    


    




static int button_state=0;
static int button_timer=10;

void visual_voice_stepButton() {
button_timer = (button_timer+1)%17;
if (button_timer==0) {
  button_state = (button_state+1)%3;
  }
}


static void ftgles2_button_render(int program,int mColorHandle,int mPositionhandle,float x,float y,
   float scale,float r,float g,float b,float a,char *text) {
if (ftgles2_cameraUniform==-1) return;
//logit("qqq_visual_voice_buttonrender %f,%f,  %f   %f,%f,%f,%f   %s\n",x,y,scale,r,g,b,a,text);
 glUseProgram(ftgles2_shaderProgram);  
 gerr("yui");
 GLfloat *c=ftgles2DirectAccessToFakeCurrentColor();
 c[0]=r;c[1]=g;c[2]=b;c[3]=a;
// logit("cb %f,%f,%f,%f text %s\n",r,g,b,a,text);
      glUniform4f(ftgles2_ucolor,r,g,b,a);
      gerr("ftgles02");
 
     
      float x1 = x;
      float y1 = y;
      
      glMatrix m1;
      loadIdentity(&m1);
      glMatrix *menu_MVP_matrix();
      translateMatrix(&m1,x1,y1,+0.03f);
      scaleMatrix(&m1,0.02f,0.02f,0.02f);
      multMatrix((glMatrix *)(&ftgles2_button_output_matrix),&m1,menu_MVP_matrix());

      
       
      

   glUseProgram(program);  
   gerr("yui");
      glUniformMatrix4fv(mPositionhandle, 1, GL_FALSE, (GLfloat *)(&ftgles2_button_output_matrix));
      gerr("r10");
      glUniform4f(mColorHandle,r,g,b,a);
      gerr("ftgles02");
	
      ftglRenderFont(ftgles2_button_font,text,FTGL_RENDER_FRONT);      
      gerr("r10");
/* Maybe for cleanup ...
      glUseProgram(ftgles2_shaderProgram);
      gerr("r12");
      glUniformMatrix4fv(ftgles2_cameraUniform, 1, GL_FALSE, (GLfloat *)(&ftgles2_output_matrix));
       gerr("r10");
*/
}




void visual_voice_renderButton(int program,int mColorHandle,int menu_position_handle,
  glMatrix *MVPMatrix,int screen,glMatrix *eye) {
if (!(ftgles2_button_font && ftgles2_shaderProgram)) {return;}


glUseProgram(ftgles2_shaderProgram);
gerr("yui");
glDisable(GL_BLEND);   // VERY important - took 5 days
gerr("glup44sp");
glDisable(GL_DEPTH_TEST);
gerr("glup44sp");
      gerr("b4g");
if (menu_effect_mode !=2) return;
float scale = WORLD_display_visual_voice__Sing_scale; // 0.55
//float x=-0.82f;
//float y=0.15f;

 glUseProgram(program);
 gerr("yui");
 
{

  /* dull the words id there is no visual voice currently */
  float yeah=1.f;
  if (packet_measurechunkhead==packet_measurechunktail) {
    yeah = yeah * 0.75f;
    }
  /* three states:
    0  <nothing>

    1	Sing
	
    2	Sing
	Along
	
    the color gets brighter if singalong music is detected.
  */
     
  if (button_state>0) {
//#define WORLD_display_visual_voice__Sing " Sing"
//#define WORLD_display_visual_voice__Sing_x -0.80f
//#define WORLD_display_visual_voice__Sing_y 0.15f
//#define WORLD_display_visual_voice_Along_x -0.80f
//#define WORLD_display_visual_voice_Along_y -0.70f

    ftgles2_button_render(program,mColorHandle,menu_position_handle,WORLD_display_visual_voice__Sing_x,
         WORLD_display_visual_voice__Sing_y,scale,yeah,yeah,yeah*0.8f,1.f,WORLD_display_visual_voice__Sing);
    }
  if (button_state>1) {
//#define WORLD_display_visual_voice_Along "Along"
    ftgles2_button_render(program,mColorHandle,menu_position_handle,WORLD_display_visual_voice_Along_x,
       WORLD_display_visual_voice_Along_y,
       scale,yeah,yeah,yeah*0.8f,1.f,WORLD_display_visual_voice_Along);
    }
  }
glEnable(GL_BLEND);   // VERY important - took 5 days
  glUseProgram(0);
  gerr("r1");
return;
}


void visual_voice_renderForeground() {
}



void visual_voice_touchEvent(int action,float x,float y) {
}

// WORLD_INTERNATIONAL off
const display_set visual_voice_display_set = {
  .name = "visual_voice",
  .setupGraphics = visual_voice_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .step = visual_voice_step,
  .renderBackground = visual_voice_renderBackground,
  .renderMain  = visual_voice_renderFrame,
  .renderForeground = visual_voice_renderForeground,
  .stepButton = visual_voice_stepButton,
  .renderButton = visual_voice_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = NULL
  };
// WORLD_INTERNATIONAL on

/*
$Log: visual_voice.c,v $
Revision 1.54  2019/05/27 19:35:15  hib
Fixed visual voice - finally.
Which means I fixed ios

Revision 1.53  2019/05/27 18:39:17  hib
xiwii

Revision 1.52  2019/05/27 12:36:11  hib
was working on visual voice - why does it fuck up where the 1.3n does not?

Revision 1.51  2019/05/20 12:45:47  hib
helped idenify an error - with project m
makde gerr which ios a better way to trach wtf

Revision 1.50  2019/05/20 05:07:28  hib
wip

Revision 1.49  2019/05/17 00:39:01  hib
this would be the silentradiance 1/3l version

Revision 1.48  2019/05/16 23:17:52  hib
Visual voice works on android.

Revision 1.47  2019/05/16 20:51:13  hib
Its getting there, The colors are all reight,  but the fonts are always huge on the buttons.
I think I have to set the matrix twice for the buttons.

Revision 1.46  2019/05/16 14:54:22  hib
think i fixed the fonts

Revision 1.45  2019/05/14 18:53:15  hib
wip - separate demos, languages

Revision 1.44  2019/05/14 18:04:06  hib
wip

Revision 1.43  2019/05/04 19:13:39  hib
wip

Revision 1.42  2019/05/04 12:48:18  hib
WIP

Revision 1.41  2019/05/04 09:37:14  hib
I think it was that the menu was rendered when very small - and so
by not drawing the button until the menu is up, the problem should be fixed

Revision 1.40  2019/05/04 08:49:29  hib
back to linux mode

Revision 1.39  2019/05/04 01:10:23  hib
Got it working.  Polygons - slow - but it works!

Revision 1.38  2019/05/04 00:23:57  hib
wip but visual voice looks like crap

Revision 1.37  2019/05/03 22:31:57  hib
wip

Revision 1.36  2019/05/03 20:43:58  hib
Cleaned up alot and hopefully the impact font as well

Revision 1.35  2019/05/03 19:26:15  hib
what is the problem visual voice?

Revision 1.34  2019/05/03 16:28:10  hib
wip. the impact font is goofed up again.

Revision 1.33  2019/04/28 04:11:16  hib
wip - changing display_set to separate init, glsetup, and screen changed

Revision 1.32  2019/04/15 22:09:03  hib
Well, the android es port works. however, it is not showing the impact font correctly - and revision 1.3 DID
Also, popups - like low battery will still cause the blank screen problem
So, I should fix it.

Revision 1.31  2019/04/15 18:01:11  hib
OK this is linux silentradiance and resplendor version 1.3 official.

Revision 1.30  2019/04/15 11:29:48  hib
OK. NOW this is the correct version for internationalization on linux!

Revision 1.29  2019/04/15 10:40:50  hib
WORLD_ macros have now set up silent radiance to be internationalized.
It is easier to implement a system where there are separate builds,
but I could handle a weird multi-languge on one build with more trickiness.

The world_internatiolization.h file sets it up
Currently just english

Revision 1.28  2019/04/15 08:13:56  hib
cleaning international

Revision 1.27  2019/04/15 05:27:38  hib
again polishing for the international push

Revision 1.26  2019/03/20 15:37:44  hib

works on ios

Revision 1.25  2019/03/17 12:58:25  hib
Updated copyrights

Revision 1.24  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

Revision 1.23  2018/09/28 23:08:37  hib
Publish 1.2 time

Revision 1.22  2018/09/28 17:46:46  hib
Version 1.2 - after apple. Linting warnings and errors.
Also - trying to fix perspective of larryharvey in android

Revision 1.21  2018/09/19 00:09:28  hib
Fixed the problem
Didnt show up in linux, because memory was zeroed.

Revision 1.19  2018/09/18 03:25:05  hib
I think I got the singalong locked in.  Now try on the apple.

Revision 1.18  2018/09/18 02:38:19  hib
Much better - still need to rotate, and multi color for multi line

Revision 1.17  2018/09/18 00:19:31  hib
Rewrote the visual voice stuff - more stable - but it doesnt work yet.
Also, it      handle     lines      are     the        segment
         can         the lines that     on      same

Revision 1.16  2018/09/17 19:09:43  hib
did some cleanup for visual voice - still needs to rotate with apple, and still not displaying correctly.

Revision 1.15  2018/09/03 06:18:20  hib
Adding impact font files

Revision 1.14  2018/08/25 19:26:02  hib
ok

Revision 1.13  2018/08/25 10:53:31  hib
made it do more lines - still goofs up.

Revision 1.12  2018/08/25 02:14:51  hib
Almost working but need the spacing to be correct.

Revision 1.11  2018/08/23 20:20:36  hib
Well, the test kicked it in.  I think its because the mateix was not set for this program.  We can do that.

Revision 1.10  2018/08/22 21:58:51  hib
work in progress

Revision 1.9  2018/08/21 01:34:28  hib
wip

Revision 1.8  2018/08/20 13:51:50  hib
wip

Revision 1.7  2018/08/02 22:48:32  hib
fixed problem with visual voice font size too big

Revision 1.6  2018/07/30 18:54:58  hib
better but need work on visual voice. performance better on larry harvey

Revision 1.5  2018/07/30 14:47:50  hib
larry robots much better

Revision 1.4  2018/07/26 10:29:03  hib
nailed it.

Revision 1.3  2018/07/26 05:05:52  hib
visual_voice compiles but doesnt bring it over yet

Revision 1.2  2018/07/26 01:26:18  hib
Set up better for V1.2

Revision 1.1  2018/07/22 13:46:40  hib
Got rid of raylib example - we still sota use it for 3d - and that will be gutted
Also added visual_voice - as a copy of word for now.


*/

