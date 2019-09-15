/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler
Idea for this visualizer by Mathias Werowiks.  Almost got him to code it too.


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


/*$Revision: 1.1 $*/

#ifndef DASOUND24_mathias1_H
#define DASOUND24_mathias1_H

#include "glue.h"
#include "helper.h"





extern int mathias1_setupGraphics(int w, int h,int screen_mode);

extern void mathias1_renderBackground(int screen);
extern void mathias1_renderFrame(int screen);
extern void mathias1_step(void);
extern void mathias1_renderForeground(int screen);
extern void mathias1_touchEvent(int action,float x,float y);
void mathias1_renderButton(int program,int mColorHandle,int menu_position_handle,float *,int);

extern const display_set mathias1_display_set;

#endif

