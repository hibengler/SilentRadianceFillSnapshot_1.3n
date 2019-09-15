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

//
//  display_main_osx.h
//  SilentRadiance
//
//  Created by hib on 6/29/16.
//  Copyright © 2016 Killer Cool Development, LLC. All rights reserved.
//

#ifndef display_main_linux_h
#define display_main_linux_h

#include <stdio.h>



void Renderer_Init(int width, int height);
void Renderer_Init_Demo(int width, int height);

int Renderer_Step();
int Renderer_Step_Demo();
void Renderer_Finish();
void Renderer_Finish_Demo();

extern int lightning_flag;

extern int save_width;
extern int save_height;
extern int screen_orientation; /* 1 top, 2 left, 3 bottom, 4 right */
extern int screen_layout;     /* 0 - portrait, 1 - landscape */
extern int screen_drawmode; /* 0 - normal, 1 - 3d */



extern void display_main_set_screen_drawmode(int newmode); /* used to turn on and off vr mode */



#endif /* display_main_linux_h */

