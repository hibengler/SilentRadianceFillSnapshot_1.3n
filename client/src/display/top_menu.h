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






/*$Revision: 1.11 $*/

#ifndef DASOUND24_TOP_MENU_H
#define DASOUND24_TOP_MENU_H

#include "glue.h"
#include "helper.h"
#include "preferences.h"



struct circle_top_button {
int enabled;
GLfloat center_x;
GLfloat center_y;
GLfloat size;
GLfloat source_center_x;
GLfloat source_center_y;
GLfloat background_color[4];
GLfloat foreground_color[4];
int flag;
int has_free_songs;
char *url; // can be custom - just for uchoose
void (*render)(struct circle_top_button *pbutton);
void (*enable)(struct circle_top_button *pbutton);
void (*disable)(struct circle_top_button *pbutton);
};


extern struct preferences *preferences_hash_table;

extern int top_menu_setupGraphics(int w, int h,int screen_mode);

extern void top_menu_renderBackground(int screen,glMatrix *eye);
extern void top_menu_renderFrame(int screen,glMatrix *eye);
extern void top_menu_renderForeground(int screen,glMatrix *eye);
extern void top_menu_touchEvent(int action,float x,float y,glMatrix *eye);
extern int top_menu_keyEvent(int action,float x,float y,glMatrix *eye);

#endif

