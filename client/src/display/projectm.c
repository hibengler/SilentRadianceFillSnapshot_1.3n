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

This one links up with libprojectm
*/







/*$Revision: 1.4 $*/

#ifdef ANDROID
#include <jni.h>
#endif





#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glue.h"
#include "default.h"
#include "packet_summary.h"
#include "display_set.h"
#include "display_main.h"
#include "helper.h"
#include "letters_render_opengles2.h"
#include "projectm.h"

const display_set projectm_display_set = {
  .name = "Projectm",
  .setupGraphics = projectm_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .step = projectm_step,
  .renderBackground = projectm_renderBackground,
  .renderMain  = projectm_renderFrame,
  .renderForeground = projectm_renderForeground,
  .renderButton = projectm_renderButton,
  .stepButton = projectm_stepButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = NULL
  };

/*
$Log: projectm.c,v $
Revision 1.4  2019/05/03 20:43:58  hib
Cleaned up alot and hopefully the impact font as well

Revision 1.3  2019/04/28 07:19:38  hib
wip

Revision 1.2  2019/04/28 04:11:16  hib
wip - changing display_set to separate init, glsetup, and screen changed

Revision 1.1  2019/04/23 22:39:20  hib
Added projectm and resetup staff - but staff doesnt work yet
Might need more milkdrop files - our firsr C++ visualizer

Revision 1.1  2019/04/23 20:06:40  hib
adding projectm


*/

