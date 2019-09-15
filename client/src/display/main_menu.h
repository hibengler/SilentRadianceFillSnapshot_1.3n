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






/*$Revision: 1.22 $*/

#ifndef DASOUND24_MAIN_MENU_H
#define DASOUND24_MAIN_MENU_H
#include "glue.h"
#include "helper.h"
#include "preferences.h"

struct menu_square_button {
GLfloat center_x;
GLfloat center_y;
GLfloat size;
GLfloat source_center_x;
GLfloat source_center_y;
int display_set;
GLfloat background_color[4];
GLfloat foreground_color[4];
int *pflag;
int shape; // 0 - square, 1 - diamond - diamonds are instant buttons
void (*button_step)(struct menu_square_button *pbutton);
void (*button_render)(struct menu_square_button *pbutton,int screen);
void (*enable)(struct menu_square_button *pbutton);
void (*disable)(struct menu_square_button *pbutton);
};


extern struct preferences *preferences_hash_table;

extern void main_menu_touchEvent(int action,float x,float y,glMatrix *extra);
extern int main_menu_keyEvent(int action,float,float,glMatrix *);

extern const display_set main_menu_display_set;
extern int menu_effect_mode; /* 0 - initial. 1 - approaching, 2 - usable, 3 - disappearing */

extern int pass_menu_setup_info(
 float *pwidth_in_pixels,
 float *pheight_in_pixels,
 float *pwidth_in_inches,
 float *pheight_in_inches,
 float *pheight_to_inches,
 float *protation_in_radians,
 float *prelative_width,
 float *prelative_height,
 float *pwidth_to_inches,
 int *pcircle_factor,
 GLuint *pone_color_program,
 GLuint *pfull_color_program,
 GLuint *pgvPositionHandle,
  glMatrix *pprojectionMatrix,
  GLuint *pmPositionHandle
 );


struct menu_cursor {
int enabled;
double x; /* in cartesian */
double y; /* in cartesian */
double oldx;
double oldy;
double phase;
};

void disable_main_menu_cursor(void); /* called when vr mode changes */
extern glMatrix origin_matrix; // useful in many places


extern void rotate_buttons(void);



 
#endif

