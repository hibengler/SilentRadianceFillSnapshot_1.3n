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






#ifndef SR_DISPLAY_SET

#define SR_DISPLAY_SET 1

#include "glue.h"
#include "helper.h"

#define TOP_MENU_ID 7
/* set this to 9 if we have 8 options instead of 6 */
#define LARRY_HARVEY_DISPLAY_SET 9

/* set the splash time for android/apple */
#ifdef ANDROID
#define SPLASH_SR_BRIGHTEN_LETTER_TIME 80
#define SPLASH_CLEAR_TIME 80
#define SPLASH_TOTAL_TIME 400
#else
#ifdef __APPLE__
#define SPLASH_SR_BRIGHTEN_LETTER_TIME 80
#define SPLASH_CLEAR_TIME 120
#define SPLASH_TOTAL_TIME 800
#else
#define SPLASH_SR_BRIGHTEN_LETTER_TIME 40
#define SPLASH_CLEAR_TIME 60
#define SPLASH_TOTAL_TIME 500
#endif
#endif
/* APPLE usually did 30 FPS, but I switched it. But if we go back to 30 , set total time to 150 above. */
// good for vr porting to projectm
extern int base_viewport_x1;
extern int base_viewport_y1;
extern int base_viewport_w;
extern int base_viewport_h;
extern int base_viewport_eye;

extern int menu_up;

typedef struct display_set {
  char name[40];
  int (*setupGraphics)(int w, int h,int graphics_mode); /* called once(or more) to init grphics code, programs, etc */
  void (*init)(void); /* called once */
  int (*changeScreen)(int w, int h,int graphics_mode); /* called to change the screen size */
  void (*finish)(void); /* called once */
  void (*step)(void);
  void (*renderBackground)(int screen,glMatrix *additionalProjection); /* only done by first renderer on the bottom. screen is 0 for whole screen, 1 for left eye, 2 for right eye, 3 for below upside down */
  void (*renderMain)(int screen,glMatrix *additionalProjection);       /* done for every renderer in the render loop */
  void (*renderForeground)(int screen,glMatrix *additionalProjection); /* only done by the last renderer on the top */
  void (*stepButton)(void);
  void (*renderButton)(int program,int mColorHandle,int menu_position_handle,glMatrix *MVPMatrix,int screen,glMatrix *additionalProjection); /* done within a menu context */
  void (*touchEvent)(int level,float x,float y,glMatrix *additionalProjection); /* Called when we receive a touch event */
  int (*keyEvent)(int action,float x,float y,glMatrix *additionalProjection); /* Called when a key is pressed - like dpad key */
  void (*finishStreaming)(void); /* Called when streaming stopped */
  void (*clickedOnMenuItemEvent)(void); /* for diamonds */
  } display_set;

  
extern int registered_display_set_count;
extern struct display_set registered_display_sets[40];
extern char *default_display_set_order;

extern int display_stack_pointer;
extern int display_set_stack[40];


extern int  ran_setup_graphics_once_before; /* this flag handles the fact that we do not have an init fucntion.       
   If ran_setup_graphics_once_before is set, we can skip some of the setup - like program creation, and just do the
      width/height specific alterations. This is fairly new, and visualizers before 9/25/2016 probably need to    
         have their init code redone slightly. */
extern int ran_once_before;  /* this flag handles the fact that we called stuff before once.
But this does not mean we can skip the graphics initialization.
ran_setup_graphics_once_before is for that.
This is useful for justin's code that sets initial variables, and cannot be called again.
*/

	 	 
extern int last_screen_orientation;

extern int display_set_init(void);
extern int display_set_surface_changed(int,int,int,int,int);
extern void display_set_render(void);
extern void display_set_finish(void);

extern void display_set_touchEvent(int action,float x,float y);
extern int display_set_keyEvent(int action); /* returns -1 if we should quit */

extern void push_display_set(int ds);
extern int pop_display_set(void);
extern int top_display_set(void);


extern void insert_display_set(int set_id);
extern void insert_display_set_back(int set_id);
extern void remove_display_set(int set_id);


extern void set_displayset_preferences(void);
extern void display_set_stream_is_closing(void); // resets for new stream

extern void display_set_gl_base_viewport(void);
// viewport is different in vr mode, so we call this instead


extern int last_draw_mode; // 1 if vr, 0 otherwise


extern void set_resplash_only_show_frame(int frames);
/* to splash non pictures as part of play - like larry harveys display set */

#endif
