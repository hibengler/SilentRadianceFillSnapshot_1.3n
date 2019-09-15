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


/*$Revision: 1.2 $*/

#ifndef DASOUND24_SRIKANT1_H
#define DASOUND24_SRIKANT1_H

#include "glue.h"
#include "helper.h"





extern int srikant1_setupGraphics(int w, int h,int screen_mode);
// setup graphics is calles on initialization, and when the screen changes.
// w - width
// height
// screen mode - 0 is normal, 1 is virtual reality left eye, 1 is vr right eye, 3 is for below upside down.
// why below upside down?  because dept can be computed by the eye with reflection - see the time traveler video
// game.


extern void srikant1_step(void);
// This is where the computation goes to figure out the next frame.  The computation is separate from rendering
// BECAUSE rendering might be called twice in the case of vistual reality.

extern void srikant1_renderMain(int screen,glMatrix *additionalProjection);
// screen mode - 0 is normal, 1 is virtual reality left eye, 1 is vr right eye, 3 is for below upside down.
// why below upside down?  because dept can be computed by the eye with reflection - see the time traveler video
// game.
// additional progection will be used for different left,right eye camera views.
// This does the rendering of the frame. I is called twice for vr mode.


void srikant1_stepButton(void);
// the square button is also renered, but separately when the main menu is up.
// This allows calculation for the changes.


void srikant1_renderButton(int program,int mColorHandle,int menu_position_handle,float *,int);
// draw the button graphics
//

extern const display_set srikant1_display_set;
// this has all the calls to the above functions.

#endif

