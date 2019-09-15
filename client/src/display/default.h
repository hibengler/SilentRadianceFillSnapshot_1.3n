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






/*$Revision: 1.13 $*/

#ifndef DASOUND24_DEFAULT_H
#define DASOUND24_DEFAULT_H

#include "glue.h"
#include "helper.h"



extern int default_setupGraphics(int w, int h,int graphics_mode);
extern void default_step(void);
extern void default_renderBackground(int screen,glMatrix *extra);
extern void default_renderFrame(int screen,glMatrix *additionalProjection);
extern void default_renderForeground(int screen,glMatrix *additionalProjection);
extern void default_touchEvent(int action,float x,float y,glMatrix *extra);
extern int default_keyEvent(int action,float x,float y,glMatrix *extra);
void default_renderButton(int program,
int mColorHandle,int menu_position_handle,
int mMVPMatrixHandle,GLfloat *MVPMatrix,int screen,glMatrix *extra);

extern const display_set default_display_set;

extern float default_active_splash_rect[16];
extern GLuint create_vbo(const GLsizeiptr size, const GLvoid* data, const GLenum usage,GLuint *old_buffer);
extern GLuint resize_vbo(const GLsizeiptr size, const GLvoid* data, const GLenum usage,GLuint *old_buffer);
extern GLuint load_png_asset_into_texture(const char* relative_path);
extern void draw_a_splash(GLfloat alpha,GLuint the_texture,GLuint the_buffer);

#endif

