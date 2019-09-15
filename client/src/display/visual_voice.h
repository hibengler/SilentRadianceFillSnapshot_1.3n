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


/*$Revision: 1.3 $*/

#ifndef DASOUND24_visual_vocie_H
#define DASOUND24_visual_vocie_H

#include "glue.h"
#include "helper.h"





extern int visual_vocie_setupGraphics(int w, int h,int);

extern void visual_vocie_renderBackground(int);
extern void visual_vocie_renderFrame(int);
extern void visual_vocie_renderForeground(int);
extern void visual_vocie_touchEvent(int action,float x,float y);
void visual_voice_renderButton(int program,int mColorHandle,int menu_position_handle,float *,int);

extern const display_set visual_vocie_display_set;



#endif

