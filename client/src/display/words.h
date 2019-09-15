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


/*$Revision: 1.5 $*/

#ifndef DASOUND24_words_H
#define DASOUND24_words_H

#include "glue.h"
#include "helper.h"





extern int words_setupGraphics(int w, int h,int);

extern void words_renderBackground(int);
extern void words_renderFrame(int);
extern void words_renderForeground(int);
extern void words_touchEvent(int action,float x,float y);
void words_renderButton(int program,int mColorHandle,int menu_position_handle,float *,int);

extern const display_set words_display_set;



#endif

