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

/* default handles the background color (when we animated lightning) 
Also the splash screen and pictutres being loaded.


Note - there are three types of splashes:  Splash girl for android and apple: shows a girl
that is a random choice of 42 pictures.

loaded -pictures - another deal that loads .bpg pictures through the interface

and special pictures - some visualizers play a picture to tell a story - such as larry_harvey_hater_robots.c
special pictures borrows from the splash girl logic, whereas the loaded pictures is an entirely different deal.

*/





/*$Revision: 1.74 $*/

#ifdef ANDROID
#include <jni.h>

#endif




#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <zlib.h>
#include <time.h>
#include "glue.h"
#include "packet_summary.h"
#include "display_main.h"
#include "main_menu.h"
#include "helper.h"
#include "png.h"
#include "default.h"
#include "letters_render_opengles2.h"

#ifdef COLLECT_BPG_IMAGES
#include "libbpg.h"
#endif

extern int heavy_metal_flag;
extern int hib_flag;

//#define THREAD_BPG
// no work
#ifdef THREAD_BPG
#include <pthread.h>
#endif
//??? vvv
// rawimagedata is for the bpg images that come across
typedef struct {
        const int width;
        const int height;
        const int size;
        const GLenum gl_color_format;
        const void* data;
} RawImageData;



//dataHandle is for the png files(the splash girl)
typedef struct {
        const png_byte* data;
        const png_size_t size;
} DataHandle;

typedef struct {
        const DataHandle data;
        png_size_t offset;
} ReadDataHandle;

typedef struct {
        const png_uint_32 width;
        const png_uint_32 height;
        const int color_type;
} PngInfo;



extern FileData get_file_data(const char* path);




/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv Splash girl/load image/special  vvvvvvvvvvvvvvvvvvvvvvvvvv*/


/* Returns the decoded image data, or aborts if there's an error during decoding. */
RawImageData get_raw_image_data_from_png(const void* png_data, const int png_data_size);
void release_raw_image_data(const RawImageData* data);

RawImageData get_raw_image_data_from_bpg(const void* bpg_data, const int bpg_data_size);

int splash_frame_count=0; //used by splash girl and silent radiance text intro
static lcontext *lc = NULL;
static        GLuint texture_object_id;

float aspect_ratio=1.33333333333f; /* changes when we have images */


// position X, Y, texture S, T
float default_active_splash_rect[16] = {  /* gets changed based on position, is read elsewhere (like larry harvey hater robot) */
    1.0f, 1.0f, 0.0f, 0.0f,
   1.0f,  -1.0f, 0.0f, 1.0f,
   -1.0f, 1.0f, 1.0f, 0.0f,
   -1.0f,  -1.0f, 1.0f, 1.0f};
 
static  float default_rect[] = { /* the default rectangle, */
    1.0f, 1.0f, 0.0f, 0.0f,
   1.0f,  -1.0f, 0.0f, 1.0f,
   -1.0f, 1.0f, 1.0f, 0.0f,
   -1.0f,  -1.0f, 1.0f, 1.0f};

/*
1.0f, 1.0f, 0.0f, 1.0f,
   1.0f,  -1.0f, 0.0f, 0.0f,
   -1.0f, 1.0f, 0.0f, 1.0f,
   -1.0f,  -1.0f, 1.0f, 0.0f};
 static  float default_active_splash_rect[] = {
     -1.0f, 1.0f, 	0.0f, 0.0f,
    -1.0f,  -1.0f, 	0.0f, 1.0f,
    1.0f, 1.0f, 	1.0f, 0.0f,
    1.0f,  -1.0f,	 1.0f, 1.0f};

 static  float default_rect[] = {
     -1.0f, 1.0f, 	0.0f, 0.0f,
    -1.0f,  -1.0f, 	0.0f, 1.0f,
    1.0f, 1.0f, 	1.0f, 0.0f,
    1.0f,  -1.0f,	 1.0f, 1.0f};
*/





static void draw_a_quad(float x1,float y1,float x2,float y2,float x3,float y3, float x4,float y4,
  float red,float green,float blue,float alpha) {
int n=0;
float verticies[12];
glUseProgram(letters_opengles_program_handle());
checkGlError("glUsePsrogram1L");
glLineWidth(1.f);

glUniform4f(letters_opengles_color_handle(),red,green,blue,1.0f);
checkGlError("glUnifosrm4fwrL");

verticies[n++]=x1;
verticies[n++]=y1;
verticies[n++]=x2;
verticies[n++]=y2;
verticies[n++]=x4;
verticies[n++]=y4;
verticies[n++]=x4;
verticies[n++]=y4;
verticies[n++]=x2;
verticies[n++]=y2;
verticies[n++]=x3;
verticies[n++]=y3;

glEnableVertexAttribArray(letters_opengles_position_handle());
checkGlError("glEnabssleVertexAttribArrayL");
glVertexAttribPointer(letters_opengles_position_handle(), 2, GL_FLOAT, GL_FALSE, 0, verticies);
checkGlError("glVertexassaAttribPointer");

glDrawArrays(GL_TRIANGLES,0,n>>1);
checkGlError("glDrawArraysL");


glDisableVertexAttribArray(letters_opengles_position_handle());
checkGlError("glDrawElementsdPL");
}
  
  

GLuint load_texture(
                    const GLsizei width, const GLsizei height,
                     GLenum type, const GLvoid* pixels) {
        glGenTextures(1, &texture_object_id);
	checkGlError("glGenTextures");
        assert(texture_object_id != 0);

        glBindTexture(GL_TEXTURE_2D, texture_object_id);
	checkGlError("glBindTexture");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	checkGlError("glTexParameter");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkGlError("glTexParameteri");
#ifndef ANDROID
	type=GL_RGBA;
#endif	
        glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, pixels);
//	logit("typ5 %d width %d height %d",type,width,height);
	checkGlError("glTexImage2D");
        glGenerateMipmap(GL_TEXTURE_2D);
	checkGlError("glGenerateMipmap");

        glBindTexture(GL_TEXTURE_2D, 0);
	checkGlError("glGenTextures");
        return texture_object_id;
}


/* used by the special event deals such as larry harvey hater robot, and the splash girl */
GLuint load_png_asset_into_texture(const char* relative_path) {
if (relative_path==NULL) return -1;

        const FileData png_file = get_asset_data(relative_path);
    const RawImageData raw_image_data = get_raw_image_data_from_png(png_file.data, (int)(png_file.data_length));
    if (raw_image_data.data == NULL) return -1;
    const GLuint texture_object_id = load_texture(
                raw_image_data.width, raw_image_data.height, raw_image_data.gl_color_format, raw_image_data.data);
        release_raw_image_data(&raw_image_data);
        release_asset_data(&png_file);
        return texture_object_id;
}







/* for osx
FileData get_asset_data(const char* relative_path) {
        assert(relative_path != NULL);
        return get_file_data(relative_path);
}

void release_asset_data(const FileData* file_data) {
        assert(file_data != NULL);
        release_file_data(file_data);
}
*/









static void read_png_data_callback(
        png_structp png_ptr, png_byte* png_data, png_size_t read_length);
static PngInfo read_and_update_info(const png_structp png_ptr, const png_infop info_ptr);
static DataHandle read_entire_png_image(
        const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height);
static GLenum get_gl_color_format(const int png_color_format);




RawImageData get_raw_image_data_from_png(const void* png_data, const int png_data_size) {
        if ( (png_data == NULL) || (!( png_data_size > 8)) ||
	    (!(png_check_sig((void*)png_data, 8))) ) {
	         return (RawImageData) {
            16, 
            16,
            0,
            0,
            NULL
            };
          }

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        assert(png_ptr != NULL);

        png_infop info_ptr = png_create_info_struct(png_ptr);
        assert(info_ptr != NULL);

        ReadDataHandle png_data_handle = (ReadDataHandle) {{png_data, png_data_size}, 0};
        png_set_read_fn(png_ptr, &png_data_handle, read_png_data_callback);

        if (setjmp(png_jmpbuf(png_ptr))) {
//                logit("Error reading PNG file!");
        }

        const PngInfo png_info = read_and_update_info(png_ptr, info_ptr);
        const DataHandle raw_image = read_entire_png_image(png_ptr, info_ptr, png_info.height);

        png_read_end(png_ptr, info_ptr);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

        return (RawImageData) {
        png_info.width,
        png_info.height,
        (int)(raw_image.size),
        get_gl_color_format(png_info.color_type),
        raw_image.data};
}

void release_raw_image_data(const RawImageData* data) {
        assert(data != NULL); 
        free((void*)data->data);
}

static void read_png_data_callback(png_structp png_ptr, png_byte* raw_data, png_size_t read_length) {
        ReadDataHandle* handle = png_get_io_ptr(png_ptr);
        const png_byte* png_src = handle->data.data + handle->offset;

        memcpy(raw_data, png_src, read_length);
        handle->offset += read_length;
}

static PngInfo read_and_update_info(const png_structp png_ptr, const png_infop info_ptr) {
        png_uint_32 width, height;
        int bit_depth, color_type; 

        png_read_info(png_ptr, info_ptr);
        png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

        // Convert transparency to full alpha
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
                png_set_tRNS_to_alpha(png_ptr);

        // Convert grayscale, if needed.
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
                png_set_expand_gray_1_2_4_to_8(png_ptr);

        // Convert paletted images, if needed.
        if (color_type == PNG_COLOR_TYPE_PALETTE)
                png_set_palette_to_rgb(png_ptr);

        // Add alpha channel, if there is none (rationale: GL_RGBA is faster than GL_RGB on many GPUs)
        if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_RGB)
           png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

        // Ensure 8-bit packing
        if (bit_depth < 8)
           png_set_packing(png_ptr);
        else if (bit_depth == 16)
                png_set_scale_16(png_ptr);

        png_read_update_info(png_ptr, info_ptr);

        // Read the new color type after updates have been made.
        color_type = png_get_color_type(png_ptr, info_ptr);
        
        return (PngInfo) {width, height, color_type};
}
        
static DataHandle read_entire_png_image(const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height) {
        const png_size_t row_size = png_get_rowbytes(png_ptr, info_ptr);
        const int data_length = (int)(row_size * height);
        assert(row_size > 0); 
        
        png_byte* raw_image = malloc(data_length);
        assert(raw_image != NULL);

        png_byte* row_ptrs[height];
        
        png_uint_32 i;
        for (i = 0; i < height; i++) {
                row_ptrs[i] = raw_image + i * row_size;
        }

        png_read_image(png_ptr, &row_ptrs[0]);

        return (DataHandle) {raw_image, data_length};
}
        
static GLenum get_gl_color_format(const int png_color_format) {
        assert(png_color_format == PNG_COLOR_TYPE_GRAY
            || png_color_format == PNG_COLOR_TYPE_RGB_ALPHA
            || png_color_format == PNG_COLOR_TYPE_GRAY_ALPHA);
                
        switch (png_color_format) {
                case PNG_COLOR_TYPE_GRAY:
                        return GL_LUMINANCE;
                case PNG_COLOR_TYPE_RGB_ALPHA:
                        return GL_RGBA;
                case PNG_COLOR_TYPE_GRAY_ALPHA:
                        return GL_LUMINANCE_ALPHA;
        }

        return 0;
}
           










static GLuint texture=-1;
static GLuint splash_girl_buffer=-1;
static GLuint program;
static volatile int resize_splash_girl_buffer_flag=0;

static GLint a_position_location;
static GLint a_texture_coordinates_location;
static GLint u_texture_unit_location;



// WORLD_INTERNATIONAL off

static char *gVertexShader =
"attribute vec4 a_Position;\n"
"attribute vec2 a_TextureCoordinates;\n"
"varying vec2 v_TextureCoordinates;\n"
"\n"
"void main()\n"
"{\n"
"    v_TextureCoordinates = a_TextureCoordinates;\n"
"    gl_Position = a_Position;\n"
"}\n";


static char * gFragmentShader =
"precision mediump float;\n"
"uniform sampler2D u_TextureUnit;\n"
"uniform vec4 v_alpha;\n"
"varying vec2 v_TextureCoordinates;\n"
"void main()\n"
"{\n"
"gl_FragColor = texture2D(u_TextureUnit, v_TextureCoordinates)*v_alpha;}\n";

// WORLD_INTERNATIONAL on




static GLuint createProgram() {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, gVertexShader);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, gFragmentShader);
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

#define BUFFER_OFFSET(i) ((void*)(i))
GLuint create_vbo(const GLsizeiptr size, const GLvoid* data, const GLenum usage,GLuint *old_buffer) {
GLuint vbo_object;
    assert(data != NULL);
    if (old_buffer) {
      if (*old_buffer) {
        glDeleteBuffers(1,old_buffer);
        *old_buffer = 0;
	}
      }
        glGenBuffers(1, &vbo_object);
	checkGlError("glGenBuffers");
	
        assert(vbo_object != 0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_object);
	checkGlError("glBindBuffer");
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	checkGlError("glBufferData");
        glBindBuffer(GL_ARRAY_BUFFER, 0);
	checkGlError("glBindBuffers");

    if (old_buffer) {
      *old_buffer = vbo_object;
      }
      
      return vbo_object;
}


GLuint resize_vbo(const GLsizeiptr size, const GLvoid* data, const GLenum usage,GLuint *old_buffer) {
    if (!old_buffer) return 0;
    if (!data) return *old_buffer;
        glUseProgram(program);
        checkGlError("glUseProgram");
        glBindBuffer(GL_ARRAY_BUFFER, *old_buffer);
	checkGlError("glBindBuffer");
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	checkGlError("glBufferData");
        glBindBuffer(GL_ARRAY_BUFFER, 0);
	checkGlError("glBindBuffers");
        return *old_buffer;
}





















static float width_in_pixels;
static float height_in_pixels;
static float width_in_inches;
static float height_in_inches;
static float height_to_inches;
static float rotation_in_radians;
static float relative_width;
static float relative_height;
static float width_to_inches;
















/* This is used to scale our uploaded photos and splash screen.
The splash screen has an aspect ratio of 1.33 to 1  - taller than wide.
But then rotates
*/
static void resize_aspect_ratio(float picwidth,float picheight) {
if (picwidth==0.f) picwidth=picheight;
float new_aspect_ratio = picheight / picwidth;


aspect_ratio = new_aspect_ratio;
//logit("resize_aspect_ratio default orientation %d  aspect %f relative_height %f relative_width %f\n",
//  screen_orientation,aspect_ratio,relative_height,relative_width);

#ifdef __APPLE__

  /* scale it */
  int offset=0;
  if ((screen_orientation==3)||(screen_orientation==4)) {
    offset=2;
    }

  if ((screen_orientation==1)||(screen_orientation==3)) {
    int i;
    if (new_aspect_ratio*relative_height>1. ) {
      for (i=0;i<4;i++) {
        int j=i;
        if (offset) j=3-i;
        default_active_splash_rect[j*4+0] = -default_rect[i*4+0]*relative_height/new_aspect_ratio;
        default_active_splash_rect[j*4+1] = default_rect[i*4+1]; // untested
        }
      }
    else {
      for (i=0;i<4;i++) {
        int j=i;
        if (offset) j=3-i;
        default_active_splash_rect[j*4+0] = -default_rect[i*4+0];
        default_active_splash_rect[j*4+1] = (default_rect[i*4+1]/relative_height)*new_aspect_ratio; // tested
        }
      }      
    } /* if we are vertical */
  else { // horizontal
    if (new_aspect_ratio*relative_height>1. ) {
      int i;
      for (i=0;i<4;i++) {
        int j=i;
        if (offset) j=3-i;
        default_active_splash_rect[j*4+0] = default_rect[i*4+1];
        default_active_splash_rect[j*4+1] = default_rect[i*4+0]/(new_aspect_ratio)*relative_height;
        }
      }
    else {
      int i;
      for (i=0;i<4;i++) {
        int j=i;
        if (offset) j=3-i;
        default_active_splash_rect[j*4+0] = default_rect[i*4+1]*new_aspect_ratio*relative_height; // tested
        default_active_splash_rect[j*4+1] = default_rect[i*4+0];
        }
      }
    } // if horizontal


#else    
//logit("screen_orientation %d\n",screen_orientation);
    /* scale it */
    if ((screen_orientation==1)||(screen_orientation==3)) {
      if (new_aspect_ratio >relative_height) {
        int i;
        for (i=0;i<4;i++) {
          default_active_splash_rect[i*4+0] = -default_rect[i*4+0]*relative_height/new_aspect_ratio;
          default_active_splash_rect[i*4+1] = default_rect[i*4+1];
	  }
	}
      else {
        int i;
        for (i=0;i<4;i++) {
          default_active_splash_rect[i*4+0] = -default_rect[i*4+0];
          default_active_splash_rect[i*4+1] = (default_rect[i*4+1]/relative_height)*new_aspect_ratio;
//	  logit ("%f / %f * %f = %f",default_rect[i*4+1],relative_height,new_aspect_ratio,(default_rect[i*4+1]/relative_height)*new_aspect_ratio);
	  }
	}
      }
    else { // 2 or 4 oriantation
      if (1.f/new_aspect_ratio >relative_width) {
        int i;
        for (i=0;i<4;i++) {
          default_active_splash_rect[i*4+0] = -default_rect[i*4+0]*relative_width;
          default_active_splash_rect[i*4+1] = default_rect[i*4+1]/new_aspect_ratio; 
	  }
	}
      else {
        int i;
        for (i=0;i<4;i++) {
          default_active_splash_rect[i*4+0] = (-default_rect[i*4+0]/(relative_width))/new_aspect_ratio;
          default_active_splash_rect[i*4+1] = default_rect[i*4+1]; 
	  }
        }
      }

#endif
resize_splash_girl_buffer_flag=1;

}

static void handle_resize_splash_girl_buffer(void) {
if (resize_splash_girl_buffer_flag) {
  resize_splash_girl_buffer_flag=0;
//  logit("handle_resize_splash_girl_buffer resize_vbo default_active_splash_rect\n");
  if (make_demo_mode) return;
  if (splash_girl_buffer==-1) return;
  
  splash_girl_buffer = resize_vbo(
    sizeof(default_active_splash_rect), default_active_splash_rect, 
       GL_STATIC_DRAW,&splash_girl_buffer);
//  logit("resized handle_resize_splash_girl_buffer");
  }
}





int default_setupGraphics(int w, int h,int graphics_mode) {
    /* adjust projection for scrren dpi */
    /* adjust projection for scrren dpi */
    
    width_in_pixels=w;
    height_in_pixels=h;

    width_in_inches = ((float)w)/xdpi; if (width_in_inches <=0.0f) width_in_inches = 1.0f;
    height_in_inches = ((float)h)/ydpi; if (height_in_inches <=0.0f) height_in_inches = 1.0f;
    
//    logit("default_setupgraphics  w %d h %d xdpi %f ydpi %f width %f height %f\n",w,h,xdpi,ydpi,width_in_inches,height_in_inches);
    /* adjust to be portrait */
    rotation_in_radians = 0.;
    
    
    
#ifdef __APPLE__
      
      /* width is to be from -1 to +1 */
      relative_width = 1.0;
      relative_height = height_in_inches / width_in_inches;
      width_to_inches = 1.0/width_in_inches;
      height_to_inches = 1.0/height_in_inches;

//      scaleMatrix(&projectionMatrix,1.0,width_in_inches/height_in_inches,1.0);



    if (width_in_inches >= height_in_inches) {
      float t;
      
      /* width is to be from -1 to +1 */
      relative_width = width_in_inches / height_in_inches;
      relative_height = 1.0;
      width_to_inches = 1.0/width_in_inches;
      height_to_inches = 1.0/height_in_inches;
 //     scaleMatrix(&projectionMatrix,height_in_inches/width_in_inches,1.0,1.0);
      }
    else {
      /* width is to be from -1 to +1 */
      relative_width = 1.0;
      relative_height = height_in_inches / width_in_inches;
      width_to_inches = 1.0/width_in_inches;
      height_to_inches = 1.0/height_in_inches;
//      scaleMatrix(&projectionMatrix,1.0,width_in_inches/height_in_inches,1.0);
      }

    /* scale it */
    int offset=0;
    if ((screen_orientation==4)||(screen_orientation==3)) {
      offset=2;
      }

{
      if ((screen_orientation==1)||(screen_orientation==3)) {
        int i;
        for (i=0;i<4;i++) {
          int j=i;
	  if (offset) j=3-i;
          default_active_splash_rect[j*4+0] = default_rect[i*4+0];
          default_active_splash_rect[j*4+1] = default_rect[i*4+1]*aspect_ratio/relative_height;
  	  }
        }
      else {
        int i;
        for (i=0;i<4;i++) {
          int j=i;
  	  if (offset) j=3-i;
          default_active_splash_rect[j*4+0] = default_rect[i*4+1];
          default_active_splash_rect[j*4+1] = default_rect[i*4+0]/aspect_ratio/relative_height;
	  }
        }
      }

#else    
//logit("qqq setup sidth height %f,%f to something aspect ratio %f\n",width_in_inches,height_in_inches,
//   aspect_ratio);
   
   
    if (width_in_inches >= height_in_inches) {
      
      /* width is to be from -1 to +1 */
      relative_width = width_in_inches / height_in_inches;
      relative_height = 1.0;
      width_to_inches = 1.0/width_in_inches;
      height_to_inches = 1.0/height_in_inches;
 //     scaleMatrix(&projectionMatrix,height_in_inches/width_in_inches,1.0,1.0);
      }
    else {
      /* width is to be from -1 to +1 */
      relative_width = 1.0;
      relative_height = height_in_inches / width_in_inches;
      width_to_inches = 1.0/width_in_inches;
      height_to_inches = 1.0/height_in_inches;
//      scaleMatrix(&projectionMatrix,1.0,width_in_inches/height_in_inches,1.0);
      }

{
       /* scale it */
  if (screen_layout==1) { // landscape      
      if ((screen_orientation==1)||(screen_orientation==3)) {
        int i;
        for (i=0;i<4;i++) {
          default_active_splash_rect[i*4+0] = default_rect[i*4+0]/relative_width/aspect_ratio;
          default_active_splash_rect[i*4+1] = default_rect[i*4+1];
	      }
        }
      else {
        int i;
        for (i=0;i<4;i++) {
          default_active_splash_rect[i*4+1] = default_rect[i*4+1];
          default_active_splash_rect[i*4+0] = default_rect[i*4+0]*aspect_ratio/relative_height;
	      }

        }
      }
    else { // portrait mode
      if ((screen_orientation==1)||(screen_orientation==3)) {
        int i;
        for (i=0;i<4;i++) {
          default_active_splash_rect[i*4+0] = default_rect[i*4+0];
          default_active_splash_rect[i*4+1] = default_rect[i*4+1]*aspect_ratio/relative_height;
	      }
        }
      else {
        int i;
        for (i=0;i<4;i++) {
          default_active_splash_rect[i*4+1] = default_rect[i*4+1];
          default_active_splash_rect[i*4+0] = default_rect[i*4+0]/relative_width/aspect_ratio;
	      }

        }
      }
  }
#endif

if (!ran_setup_graphics_once_before    ) {
  lc = linit_context();
  
#ifdef SHOW_SPLASH
  if (make_demo_mode) return 0; // dont splash the name of the app

  char filename[500];
  srand((int)(time(NULL)));
/* WORLD_INTROS_COUNT is 210 for Reggaeton 69, 45 regular, and 1 for lele */

  //load random 
//#define WORLD_intros__pct_3_dot_3d_dot_png "intros_%3.3d.png"
#ifndef WORLD_intros__count
#define WORLD_intros__count 45
#endif  
  sprintf(filename,WORLD_intros__pct_3_dot_3d_dot_png,rand()%WORLD_intros__count);
  //sprintf(filename,"intros_%3.3d.png",rand()%1);//locked to 1
  texture = load_png_asset_into_texture(filename);
  if (!(texture==-1)) return 0; // dont splash the name of the app
  
#endif
/* we will use the vbo (splash_girl_buffer) and program over and over */
//  logit("create vbo %f %f %f %f\n",default_active_splash_rect[0],default_active_splash_rect[4],default_active_splash_rect[8],default_active_splash_rect[12]);
//  logit("       and  %f %f %f %f\n",default_active_splash_rect[1],default_active_splash_rect[5],default_active_splash_rect[9],default_active_splash_rect[13]);
  splash_girl_buffer = create_vbo(sizeof(default_active_splash_rect), 
      default_active_splash_rect, GL_STATIC_DRAW,NULL);
  program = createProgram();
  if (!program) {
      logit("Could not create texture program.");
      return 0;
    }

  a_position_location = glGetAttribLocation(program, "a_Position");
  a_texture_coordinates_location = glGetAttribLocation(program, "a_TextureCoordinates");
  u_texture_unit_location = glGetUniformLocation(program, "u_TextureUnit");
  }
//logit("o2a");
{  
//  if (splash_frame_count<SPLASH_TOTAL_TIME) {
//  logit("resize vbo %f %f %f %f\n",default_active_splash_rect[0],default_active_splash_rect[4],default_active_splash_rect[8],default_active_splash_rect[12]);
//  logit("       and  %f %f %f %f\n",default_active_splash_rect[1],default_active_splash_rect[5],default_active_splash_rect[9],default_active_splash_rect[13]);
  resize_splash_girl_buffer_flag=1;
  }
//logit("o3");
    

return 1;
}



static float start_phase=0.f;



void draw_a_splash(GLfloat alpha,GLuint the_texture,GLuint the_buffer) {

if (make_demo_mode) return;
if (the_texture==-1) return;
if (the_buffer==-1) return;

glUseProgram(program);   
checkGlError("glUseProgram");
glActiveTexture(GL_TEXTURE0);
checkGlError("glActiveTexture");
glBindTexture(GL_TEXTURE_2D, texture);
checkGlError("glBindTexture");
glUniform1i(u_texture_unit_location, 0);
checkGlError("glUniform1i");


GLuint alphaHandle;
alphaHandle = glGetUniformLocation(program, "v_alpha");
glUniform4f(alphaHandle,alpha,alpha,alpha,alpha);

glBindBuffer(GL_ARRAY_BUFFER, splash_girl_buffer);
checkGlError("glBindsplash_girl_buffer");
glVertexAttribPointer(a_position_location, 2, GL_FLOAT, GL_FALSE, 
   4 * sizeof(GL_FLOAT), BUFFER_OFFSET(0));
checkGlError("glVertexAttribPointer");
glVertexAttribPointer(a_texture_coordinates_location, 2, GL_FLOAT, 
  GL_FALSE, 4 * sizeof(GL_FLOAT), BUFFER_OFFSET(2 * sizeof(GL_FLOAT)));
checkGlError("glVertexAttribPointer");
glEnableVertexAttribArray(a_position_location);
checkGlError("glEnableVertexAttribArray");
glEnableVertexAttribArray(a_texture_coordinates_location);
checkGlError("glEnableVertexAttribArray");
glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
checkGlError("glDrawArrays");
 
glBindBuffer(GL_ARRAY_BUFFER, 0);
checkGlError("glBindBuffer");
glClear( GL_DEPTH_BUFFER_BIT);
checkGlError("glClear");

  
  
}




#ifdef SHOW_SPLASH    
static void draw_splash(GLfloat alpha) {
draw_a_splash(alpha,texture,splash_girl_buffer);
}


static void draw_nova() {
if (!letters_opengles_program_handle()) return;
/* draw a nova */
{
  float ph = start_phase;
  int blades=13;
  float add_ph = 3.1415937*2.0/((float)blades);
  float add_ph_over_3 = add_ph * 0.3333333333f;
  float add_ph_over_2 = add_ph * 0.5f;
  float add_ph_2_over_3 = add_ph * 0.66666666666f;
  
  
  int i;
  for (i=0;i<blades;i++) {
    float ph2 = ph+add_ph_over_3;
    
    float ph3 = ph+add_ph_over_2;
    float ph4 = ph+add_ph_2_over_3;
    float cosph = hcos(ph);
    float sinph = hsin(ph);
    float cosph2 = hcos(ph2);
    float sinph2 = hsin(ph2);
    float cosph3 = hcos(ph3);
    float sinph3 = hsin(ph3);
    float cosph4 = hcos(ph4);
    float sinph4 = hsin(ph4);
    
    float x1 = 0.3f*cosph;
    float y1 = 0.3f*sinph;
    float x2 = 2.0f*cosph;
    float y2 = 2.0f*sinph;
    float x3 = 2.0f*cosph2;
    float y3 = 2.0f*sinph2;
    float x4 = 0.3f*cosph2;
    float y4 = 0.3f*sinph2;
    
    
    float x5 = 0.3f*cosph3;
    float y5 = 0.3f*sinph3;
    float x6 = 2.0f*cosph3;
    float y6 = 2.0f*sinph3;
    float x7 = 2.0f*cosph4;
    float y7 = 2.0f*sinph4;
    float x8 = 0.3f*cosph4;
    float y8 = 0.3f*sinph4;
    
#ifdef WORLD_default_nova_colorset_1
    draw_a_quad(x1,y1,x2,y2,x3,y3,x4,y4,WORLD_default_nova_colorset_1); /* normally yellow */
    draw_a_quad(x5,y5,x6,y6,x7,y7,x8,y8,WORLD_default_nova_colorset_2); /* blue/slight purple */
#else
    draw_a_quad(x1,y1,x2,y2,x3,y3,x4,y4,0.9f,0.9f,0.0f,1.f); /* yellow */
    draw_a_quad(x5,y5,x6,y6,x7,y7,x8,y8,0.2f,0.f,1.f,1.f); /* blue/slight purple */
#endif
    ph += add_ph;
    }
  start_phase += 0.051;
  }
}

#endif

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Splash stuff ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/




























#ifdef COLLECT_BPG_IMAGES
static void draw_bpg_image(struct bpg_image *image,GLfloat alpha) {
glUseProgram(program);   
checkGlError("glUseProgram");
glActiveTexture(GL_TEXTURE0);
checkGlError("glActiveTexture");

glBindTexture(GL_TEXTURE_2D, image->texture_object_id);
checkGlError("glBindTexture");
glUniform1i(u_texture_unit_location, 0);
checkGlError("glUniform1i");

GLuint alphaHandle;
alphaHandle = glGetUniformLocation(program, "v_alpha");
glUniform4f(alphaHandle,alpha,alpha,alpha,alpha);
#ifdef needtofix
glBindBuffer(GL_ARRAY_BUFFER, bufferdhd<hd);
checkGlError("glBindBuffer");
glVertexAttribPointer(a_position_location, 2, GL_FLOAT, GL_FALSE, 
   4 * sizeof(GL_FLOAT), BUFFER_OFFSET(0));
checkGlError("glVertexAttribPointer");
glVertexAttribPointer(a_texture_coordinates_location, 2, GL_FLOAT, 
  GL_FALSE, 4 * sizeof(GL_FLOAT), BUFFER_OFFSET(2 * sizeof(GL_FLOAT)));
checkGlError("glVertexAttribPointer");
glEnableVertexAttribArray(a_position_location);
checkGlError("glEnableVertexAttribArray");
glEnableVertexAttribArray(a_texture_coordinates_location);
checkGlError("glEnableVertexAttribArray");
glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
checkGlError("glDrawArrays");
#endif 
glBindBuffer(GL_ARRAY_BUFFER, 0);
checkGlError("glBindBuffer");
glClear( GL_DEPTH_BUFFER_BIT);
checkGlError("glClear");
}
#endif



void *(*thread_bpg_to_raw(void *im)) {
struct bpg_image *image = (struct bpg_image *)im;
      RawImageData r = get_raw_image_data_from_bpg(image->image,image->file_size);
      image->width = r.width;
      image->height = r.height;
      image->raw_image_size = r.size;
      image->gl_color_format = r.gl_color_format;
      image->raw_image = (unsigned char *)r.data;
      image->rawthread = 0;
//      logit("raw_image_made thread\n");
return NULL;
}









































void default_renderBackground(int screen,glMatrix *extra) {
    /* To start out determine the color.
     * dead - black
     * nothing - dark red
     * warming - middle yellow
     * warning - orange
     * good - green
     * */
    glEnable(GL_BLEND);
    struct packet *p=NULL;
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
            p = packet_summary.packets+index;
            if (p->has_beat) {
//	        has_beat=1;
		break;
		}
	    }
/*	if ((has_beat)&&lightning_flag) { */
        if (1==0) {
            glClearColor(1.0f, 1.0f, 1.0f,1.f); /* white */
            }
        else  {
	  if (packet_summary.command_show_ip_address_stuff.show_mode) {
	    if (packet_summary.command_show_ip_address_stuff.ourcolor==0) {
	      glClearColor(1.0f,0.0f,0.0f,1.0f); /* red */
	      } /* red is 0 - bit is determined based on the original color */
            else { /* cyan is 1 */
	      glClearColor(0.0f,1.0f,1.0f,1.0f);
	      }     
	    } /* if we are doing an override */
	  else {
            if (heavy_metal_flag) {
	      float r,g,b;
	      r = packet_summary.commanded_background_color[0];
	      g = packet_summary.commanded_background_color[1];
	      b = packet_summary.commanded_background_color[2];
	      // redden across the board.
	      r = r + 0.2;
	      if (r>1.0) r=1.0;
	      
	      // green cant be bigger than red
	      if (g>r) g=r;
	      
	      // much less  blue
	      b=b*0.2;
  	      glClearColor(r,g,b,packet_summary.commanded_background_color[3]); /* so we can set the colore */		
	      }
            else if (hib_flag) {
	      float r,g,b;
	      r = packet_summary.commanded_background_color[0];
	      g = packet_summary.commanded_background_color[1];
	      b = packet_summary.commanded_background_color[2];
	      // purple across the board.
	      r = (r + 0.3)*0.76923f;
	      if (r>1.0f) r=1.0f;
	      // purple across the board.
	      b = (b + 0.3)*0.76923f;
	      if (b>1.0f) b=1.0f;
	      
	      // douse green
	      g = g * 0.8f;
	      
	      // if r>b, bring up b
	      if (r>b) {b =b+((b-r)*0.5);}
	      
	      
	      // green cant be bigger than red
	      if (g>r) g=r;
	      // green cant be bigger than blue
	      if (g>r) g=r;
	      
	      glClearColor(r,g,b,packet_summary.commanded_background_color[3]); /* so we can set the colore */		
	      }
	    else {   
	      glClearColor(packet_summary.commanded_background_color[0],
                             packet_summary.commanded_background_color[1],
                             packet_summary.commanded_background_color[2],
                             packet_summary.commanded_background_color[3]); /* so we can set the colore */		
	      }
	    }
        }
    }
    checkGlError("glClearColoraa");

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
#ifdef SHOW_SPLASH    
handle_resize_splash_girl_buffer();

if (splash_frame_count<SPLASH_TOTAL_TIME) {
  if (splash_frame_count<SPLASH_CLEAR_TIME) {
     draw_nova();
     draw_splash(1.0f);
     }
  else {
     draw_splash( 1.0f -  (  ((float)splash_frame_count)-((float)SPLASH_CLEAR_TIME) )/((float)(SPLASH_TOTAL_TIME-SPLASH_CLEAR_TIME))   );
    }
  splash_frame_count++;
  }
#else
splash_frame_count = SPLASH_TOTAL_TIME+1;
#endif

#ifdef COLLECT_BPG_IMAGES
/* draw images from the queue */
if (splash_frame_count>=SPLASH_TOTAL_TIME) {
  /* process through the list */
  int i;
  
  /* timer. right now just replacemeny */
  int queue_length = bpg_image_queue_tail-bpg_image_queue_head;
  if (queue_length<0) queue_length += BPG_IMAGE_QUEUE_SIZE;
  if (queue_length >1) {
//    logit("skip\n");
    bpg_image_queue[bpg_image_queue_head] = 0;
    bpg_image_queue_head = (bpg_image_queue_head+1) % BPG_IMAGE_QUEUE_SIZE;
    }
  
    
  for (i=bpg_image_queue_head;i !=bpg_image_queue_tail; i = (i+1) % BPG_IMAGE_QUEUE_SIZE) {
//    logit("q23b %d ",bpg_image_queue[i]);
    struct bpg_image *image = bpg_images[bpg_image_queue[i]];
    if (!image) continue;
    if (image->raw_image == NULL) {
      if (image->rawthread) break;
//         logit("q24");
//       logit("special start: %2.2x%2.2x%2.2x%2.2x\n",image->image[0],image->image[1],image->image[2],image->image[3]);
       /* process the raw image for a good deed - and do more later frames */
      #ifdef THREAD_BPG
      pthread_create(&(image->rawthread),NULL,thread_bpg_to_raw,(void *) image);
      #else
      RawImageData r = get_raw_image_data_from_bpg(image->image,image->file_size);
      image->width = r.width;
      image->height = r.height;
      image->raw_image_size = r.size;
      image->gl_color_format = r.gl_color_format;
      image->raw_image = (unsigned char *)r.data;
//      logit("raw_image_made\n");
      #endif
      break; /* good for now */
      }
    if (image->texture_object_id ==0) {// -1?
//      logit("q25");
      /* note - we should use a vbo instead of mipmappint */
      glGenTextures(1, &image->texture_object_id);
      checkGlError("glGenTextures");
      assert(image->texture_object_id != 0);

      glBindTexture(GL_TEXTURE_2D, image->texture_object_id);
      checkGlError("glBindTexture");

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      checkGlError("glTexParameter");
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      checkGlError("glTexParameteri");

      GLenum type=GL_RGBA;

      glTexImage2D(GL_TEXTURE_2D, 0, type, image->width, image->height, 0, type, GL_UNSIGNED_BYTE, (const void *)(image->raw_image));
//      logit("typ5 %d width %d height %d",type,image->width,image->height);
      checkGlError("glTexImage2D");
      glGenerateMipmap(GL_TEXTURE_2D);
      checkGlError("glGenerateMipmap");

      glBindTexture(GL_TEXTURE_2D, 0);
      checkGlError("glGenTextures");
//      logit("texture made\n");
      break;
      } // if
    } // for  

  int image_id;
//  logit("head %d(%d) tail %d",bpg_image_queue_head, bpg_image_queue[bpg_image_queue_head],bpg_image_queue_tail);
  if (bpg_image_queue_head != bpg_image_queue_tail) {
    image_id = bpg_image_queue[bpg_image_queue_head];
    struct bpg_image *image = bpg_images[image_id];
    if (image) {
      logit("image %d original width %d original height %d\n",image_id,image->original_width,image->original_height);
      if ((image->original_width!=0) && (image->original_height !=0))  {
        resize_aspect_ratio((float)(image->original_width),(float)(image->original_height));
        }
      else {
        resize_aspect_ratio(1.f,1.f);
        }
      draw_bpg_image(image,1.f);
      } // if there is an image there
    }
  } // if we are not showing the intro
#endif

}










extern unsigned int letters_opengles_color_handle();
extern unsigned int letters_opengles_program_handle();

static void draw_sr_title(char *sr_title,int letter_number) {
unsigned int colorHandle = letters_opengles_color_handle();
unsigned int one_program_handle = letters_opengles_program_handle();

glUseProgram(one_program_handle);   
checkGlError("(useprogramdefault)");
fl scale = WORLD_SilentRadianceWithVersion_SCALE;
fl x,y;
if (screen_layout==1) {
  x = WORLD_SilentRadianceWithVersion_x; // -0.90f;
  y = 1.f +  (WORLD_SilentRadianceWithVersion_y); // - 0.25f;
  }
else {
  x = WORLD_SilentRadianceWithVersion_x; // -0.90f;
#ifdef __APPLE__
  y = relative_width + (WORLD_SilentRadianceWithVersion_y);
#else
  y = relative_height + (WORLD_SilentRadianceWithVersion_y);
#endif
  }
fl z=0.f;
int l=strlen(sr_title);
float r = lc->needed_points->line_width;

if (letter_number ==-1) {
  lc->needed_points->line_width= 0.11f;
  glUniform4f(colorHandle, 0.9f,0.4f,1.f,1.0f); // bright purple
  letters_out(lc,scale,x,y,z,sr_title);
  checkGlError("l1def");
  lc->needed_points->line_width = r;
  }
else if (letter_number >= l) {
  }
else {
  float factor = (l-letter_number-1) / (float)(l-1);
  lc->needed_points->line_width= 0.03f + 0.08f*factor;
  glUniform4f(colorHandle, factor*0.9f ,factor*0.4f,factor,1.0f); // darker
  letters_out(lc,scale,x,y,z,sr_title);
  checkGlError("l2def");
  }
#ifdef WORLD_A_SilentRadiance_app


if (letter_number < l)
{ // show its silentradiance.com
  scale = WORLD_A_SilentRadiance_app_SCALE;
  if (screen_layout==1) {
    x = WORLD_A_SilentRadiance_app_x;
    y = WORLD_A_SilentRadiance_app_y+1.f;
    }
  else {
#ifdef __APPLE__
    x = WORLD_A_SilentRadiance_app_x;
    y = relative_width + (WORLD_A_SilentRadiance_app_y);
#else
    x = WORLD_A_SilentRadiance_app_x;
    y = relative_height + (WORLD_A_SilentRadiance_app_y);
#endif
    }  
  
  lc->needed_points->line_width= 0.03f + 0.08f;
  glUniform4f(colorHandle, 1.f ,0.3f,0.8f,1.f); // bright redder purple
  letters_out(lc,scale,x,y,z,WORLD_A_SilentRadiance_app);
  checkGlError("l2def");
  }
#endif  
lc->needed_points->line_width = r;
}





void default_renderFrame(int screen,glMatrix *additionalProjection) {    
}


void default_step() {    
}

//#define WORLD_Silent_Radiance_With_Version_Splash_Title "Silent Radiance 1.?"
static char *sr_title = WORLD_Silent_Radiance_With_Version_Splash_Title;


void default_renderForeground(int screen,glMatrix *additionalProjection) {
unsigned int one_program_handle = letters_opengles_program_handle();
glUseProgram(one_program_handle);   
if (checkGlError("(useprogramdefaulta)")) {
//  logit("ph %d",one_program_handle);
  }
int mMVPMatrixHandle = glGetUniformLocation(one_program_handle, "u_MVPMatrix");
glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&origin_matrix));
    
if (make_demo_mode) return; // dont splash the name of the app
if (splash_frame_count<SPLASH_TOTAL_TIME) {
  if (splash_frame_count<SPLASH_SR_BRIGHTEN_LETTER_TIME) {
     draw_sr_title(sr_title,-1);
     }
  else {
    int letter_number;
#ifdef ANDROID
	letter_number = (splash_frame_count-SPLASH_SR_BRIGHTEN_LETTER_TIME)>>2;
#else
#ifdef __APPLE__
	letter_number = (splash_frame_count-SPLASH_SR_BRIGHTEN_LETTER_TIME)>>2;
#else
	letter_number = (splash_frame_count-SPLASH_SR_BRIGHTEN_LETTER_TIME)>>1;
#endif	
#endif 
     draw_sr_title(sr_title,letter_number);
     } /* if it is time to flash */
  } /* if the flash is over */
checkGlError("title");
}




















#ifdef COLLECT_BPG_IMAGES
/* vvvvvvvvvvvvvvvvvvvv bvg interface */

RawImageData get_raw_image_data_from_bpg(const void* bpg_data, const int bpg_data_size) {
    BPGDecoderContext *img;
    
    img = bpg_decoder_open();
    if (bpg_decoder_decode(img, bpg_data, bpg_data_size) < 0) {
//        logit( "Could not decode image\n");
        exit(1);
    }
    
    BPGImageInfo img_info_s, *img_info = &img_info_s;
    int w, h, y;
    bpg_decoder_get_info(img, img_info);
    
    
    w = img_info->width;
    h = img_info->height;
    int data_length = w*h*4;
    unsigned char *raw_image = malloc(data_length);
    
    bpg_decoder_start(img, BPG_OUTPUT_FORMAT_RGBA32);
    for (y = 0; y < h; y++) {
        bpg_decoder_get_line(img, raw_image+(w*4*y));
	}
    bpg_decoder_close(img);
        
        return (RawImageData) {
          w,
          h,
        (int)(data_length),
        GL_RGBA,
        raw_image};
}

/* ^^^^^^^^^^^^ bpg interface ^^^^^^^^^^^^ */

#endif // COLLECT_BPG_IMAGES


extern int last_draw_mode;

extern void disable_vr_button(void *);

extern void menu_disable_vr_mode();

int back_to_non_vr_mod_on_any_event(int action) { // well, not the hold and release events
//logit("back_to_non_vr action %d\n",action);
if ((action<1000)&&(action!=7)&&(action!=8)) {
  if (last_draw_mode==1) {
    menu_disable_vr_mode();
    return 0;
    }
  }
return 1;
}

void default_touchEvent(int action,float x,float y,glMatrix *extra) {
if (back_to_non_vr_mod_on_any_event(action)) {
  if (action==1) {
    push_display_set(2);
    }
  }
}

int default_keyEvent(int action,float x,float y,glMatrix *extra) {
if (back_to_non_vr_mod_on_any_event(action)) {
  if (action==1) {
    push_display_set(2);
    }
  if (action==6) {
    /* should exit */
    return -1;
    }
  return 0;
  }
return 0;
}








const display_set default_display_set = {
//#define WORLD_display_default_default "default"
  .name = WORLD_display_default_default,
  .setupGraphics = default_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .step = default_step,
  .renderBackground = default_renderBackground,
  .renderMain  = default_renderFrame,
  .renderForeground = default_renderForeground,
  .renderButton = NULL,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = NULL
  };

/*
$Log: default.c,v $
Revision 1.74  2019/08/25 05:07:08  hib
Finished linux client for leleradiance. Tag is lele

Revision 1.73  2019/07/01 00:05:41  hib
minor - added hib_flag visual mode.

Revision 1.72  2019/06/30 23:34:54  hib
Addes heavy metal mode, finally.

Revision 1.71  2019/05/20 05:08:06  hib
wip

Revision 1.70  2019/05/19 01:52:14  hib
made variable nova colors

Revision 1.69  2019/05/19 01:17:22  hib
wip

Revision 1.68  2019/05/17 00:39:00  hib
this would be the silentradiance 1/3l version

Revision 1.67  2019/05/15 22:27:25  hib
Ready for reggaeton

Revision 1.66  2019/05/15 22:02:16  hib
*** empty log message ***

Revision 1.65  2019/05/15 03:56:12  hib
Looking good

Revision 1.64  2019/05/15 03:42:33  hib
Got the last thing done before making the resplandor relase cycle: nova
Also, did alot of cleanup of demo -makeing a second demo progra,
Also - got the system to (sortof) make multiple languages
Also - enabled the splash_screen temporarily for the linux program - to test the nova background
um - anyways next is to remove the logs and build on android
and later on - make a psuedo file system to load all the .png's in linux,
and later on - use that also to get the projectm files loaded in.

Revision 1.63  2019/05/15 01:16:05  hib
Added a second title on resplandor so we know it is silent radiance

Revision 1.62  2019/05/03 16:28:10  hib
wip. the impact font is goofed up again.

Revision 1.61  2019/05/03 12:26:57  hib
from non cvs

Revision 1.60  2019/04/28 04:11:16  hib
wip - changing display_set to separate init, glsetup, and screen changed

Revision 1.59  2019/04/15 18:01:10  hib
OK this is linux silentradiance and resplendor version 1.3 official.

Revision 1.58  2019/04/15 11:29:48  hib
OK. NOW this is the correct version for internationalization on linux!

Revision 1.57  2019/04/15 10:40:50  hib
WORLD_ macros have now set up silent radiance to be internationalized.
It is easier to implement a system where there are separate builds,
but I could handle a weird multi-languge on one build with more trickiness.

The world_internatiolization.h file sets it up
Currently just english

Revision 1.56  2019/04/15 06:09:25  hib
internationalization wip

Revision 1.55  2019/04/03 14:22:15  hib
clean up logit

Revision 1.54  2019/03/21 21:26:24  hib
started work on android stuff - needs new glue funcs for 1.3
going to work on not_rl and make it vr

Revision 1.53  2019/03/17 12:58:24  hib
Updated copyrights

Revision 1.52  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

Revision 1.51  2018/12/25 17:09:52  hib
Made sound_to_file use the current to point tot he current + next

Revision 1.50  2018/12/24 09:00:52  hib
Added 666 and 721

Revision 1.49  2018/12/22 15:39:11  hib
Well, silentradiance still works.  the demo maker - got it to dump some files upside down.
Switched sr_demo to run the file reader in the background, and the sound in the foreground-  switching abck and forth from sound_to_file.c and sound_alsa.c - well sound_alsa.c usually works but doesn t close the app. and the other one freezes.
Also the video stopped being outputted.
Need to take a break from it.

Revision 1.48  2018/09/28 23:08:37  hib
Publish 1.2 time

Revision 1.47  2018/07/29 20:44:08  hib
wip

Revision 1.46  2018/07/26 01:26:18  hib
Set up better for V1.2

Revision 1.45  2018/07/26 00:10:51  hib
ran on the ipad. no 3d., no hater robots

Revision 1.44  2018/07/25 22:57:36  hib
wip

Revision 1.43  2018/07/25 22:48:59  hib
wip

Revision 1.42  2018/07/08 18:00:57  hib
First attempt at scene 3

Revision 1.41  2018/05/03 19:08:33  hib
Work in progress

Revision 1.40  2018/02/27 04:30:21  hib
Cleaned up the vr mode alot

Revision 1.39  2018/02/22 21:57:55  hib
Help modularize the display visualizers much more - for display_set - need to do for the main menu too
Also - added step/draw initial logic, to make it more likely that we can render split screen for goggles.

Revision 1.38  2018/01/23 10:13:11  hib
This is the 1.1 release version of Silent Radiance for the android

Revision 1.37  2018/01/22 20:32:28  hib
1.1a work for android

Revision 1.36  2018/01/20 22:13:38  hib
Gor rid of fprintfs

Revision 1.35  2018/01/20 22:10:07  hib
added 2018

Revision 1.34  2018/01/16 06:28:13  hib
Got the Silent Radiance word splash to show up pretty well.
Got the words better than before.
fixed baloon head in main menu I think.

Revision 1.33  2018/01/15 23:29:22  hib
minor adjustment

Revision 1.32  2018/01/15 23:27:55  hib
Got the silent radiance title to appear and fade and thin.

Revision 1.31  2018/01/15 15:25:30  hib
stuff to make the logo appear

Revision 1.30  2018/01/09 10:46:35  hib
Got top menu tpo work on android! Real tough!

Revision 1.29  2017/12/02 04:08:19  hib
getting there

Revision 1.28  2017/12/01 17:33:12  hib
t things working again on apple - smooth

Revision 1.27  2017/12/01 16:57:35  hib
image pretty much done - sometimes doesn't show up

Revision 1.26  2017/12/01 12:48:27  hib
Did a bit of a revamp for the graphics - cleaning out glViewport, added scissors that should clip the buttons on cheap phones.
Better set up to handle 3d, and the "third eye" idea.

Revision 1.25  2017/12/01 04:45:48  hib
Made all the graphical apps check to see if ran before so we don't recreate all the programs, etc.
just before we did it, images was close to working on android.
Thats our main focus right now.

Revision 1.24  2017/11/15 14:26:19  hib
doesnt display pictures right - not yet.

Revision 1.23  2017/11/15 11:57:23  hib
The image display is working pretty good on ios.  now for the other platforms.

Revision 1.22  2017/11/15 09:35:50  hib
working on default.c pictures...

Revision 1.21  2017/11/15 06:16:08  hib
Got libbpg in

Revision 1.20  2017/11/08 11:48:46  hib
wip

Revision 1.19  2017/11/08 06:21:51  hib
got it to compile with bpg

Revision 1.18  2017/11/08 04:45:11  hib
wip - we are getting closer

Revision 1.17  2017/11/02 03:19:03  hib
Took out dept render stuff in justin2.c - caused win to blow up and looks like it isnt needed.

Revision 1.16  2017/11/02 02:52:12  hib
minor

Revision 1.15  2017/10/29 10:06:11  hib
got an image through!

Revision 1.14  2017/10/06 13:52:22  hib
linting this program.

Revision 1.13  2017/10/06 00:12:10  hib
I got the angle of the ipad to make a difference in the menu and the splash screen.

Revision 1.12  2017/07/05 12:40:10  hib
Added copyleft

Revision 1.11  2017/07/04 21:04:09  hib
Getting closer to production

Revision 1.10  2017/04/06 20:10:39  hib
fixed the splash screen

Revision 1.9  2017/04/06 11:41:16  hib
clean up linux client ifdef

Revision 1.8  2017/04/06 11:10:31  hib
Works and switches orientation now. Whew!

Revision 1.7  2017/04/06 09:37:20  hib
Making multiple direction. Fix the tv icons. Clean up the tv

Revision 1.6  2017/04/05 19:17:02  hib
Got the keys to work, though their positions are off and the enter key doesn't work yet.
Now just got to fix the fucking problemw with translation from the fake world to the real world of points.

Revision 1.5  2017/04/05 08:19:41  hib
Getting closer - but should probably enable the keys on linux client. Easier tot est thean android

Revision 1.4  2017/04/05 06:33:33  hib
wip

Revision 1.3  2017/04/04 09:08:37  hib
Got it to run on linux as a client! yo!

Revision 1.2  2017/01/19 11:50:39  justin
Got the color shift to go on beat, other misc changes

Revision 1.1  2016/12/20 20:07:48  hib
Should be enough to build this

Revision 1.5  2016/07/02 00:54:14  hib
Got the justid display to work as well.

Revision 1.3  2016/06/30 03:42:18  hib
made display better

Revision 1.1  2016/06/30 00:03:56  hib
display stuff to go over to apple

*/
