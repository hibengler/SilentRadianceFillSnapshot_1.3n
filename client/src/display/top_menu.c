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






/*$Revision: 1.59 $*/
/* top level menu - like above the main menu yo! */
#ifdef ANDROID
#include <jni.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "glue.h"
#include "packet_summary.h"
#include "display_main.h"
#include "display_set.h"
#include "helper.h"
#include "default.h"
#include "purple_string.h"
#include "justin2.h"
#include "network_cube.h"
#include "backward_worm.h"
#include "preferences.h"
#include "main_menu.h"
#include "top_menu.h"


extern int heavy_metal_flag;
extern int hib_flag;

static int uchoose_button_id; // set early on to the uchoose id
char FunctionUChoose[500];
// you choose url, loaded from preferences




#define NUMBER_OF_TOP_BUTTONS 100


static float top_bubble_scale = 1.f; // gets smaller with more bubble s
static float top_bubble_radius = 1.f; // gets smaller with more bubble s



int station_has_free_songs=0;
static struct circle_top_button top_buttons[NUMBER_OF_TOP_BUTTONS];
static int top_button_click_count[NUMBER_OF_TOP_BUTTONS];
static int top_button_count=0;

static glMatrix modelMatrix;
static glMatrix effectMatrix; /* used to do effects like zoom near/zoom away */
static glMatrix viewMatrix;
static glMatrix projectionMatrix;
static glMatrix MVMatrix;
static glMatrix MVPMatrix;

static int effect_mode; /* 0 - initial. 1 - approaching, 2 - usable, 3 - disappearing */
static struct timeval effect_start_time;
static struct timeval effect_end_time;
static float effect_relative_position; /* [0,1) */
static GLfloat effect_translate[3];


static int do_effect = 0; /* if on, the effectMatrix will be added after the model matrix to move the 2d "frame" away from view */
static int colorHandle;


static struct menu_cursor default_xmc = {0,0.,0.,0.,0.,0.};
static struct menu_cursor xmc = {0,0.,0.,0.,0.,0.};
static struct menu_cursor *mc = &xmc;




static void render_network_button(struct circle_top_button *pbutton);
static void enable_network_button(struct circle_top_button *pbutton);
static void disable_network_button(struct circle_top_button *pbutton);
static void render_uchoose_button(struct circle_top_button *pbutton);
static void enable_uchoose_button(struct circle_top_button *pbutton);
static void disable_uchoose_button(struct circle_top_button *pbutton);
static void render_secret666_button(struct circle_top_button *pbutton);
static void enable_secret666_button(struct circle_top_button *pbutton);
static void disable_secret666_button(struct circle_top_button *pbutton);
static void render_secret721_button(struct circle_top_button *pbutton);
static void enable_secret721_button(struct circle_top_button *pbutton);
static void disable_secret721_button(struct circle_top_button *pbutton);
static void render_heavy_metal_button(struct circle_top_button *pbutton);
static void enable_heavy_metal_button(struct circle_top_button *pbutton);
static void disable_heavy_metal_button(struct circle_top_button *pbutton);
static void enable_dubstep_channel(struct circle_top_button *pbutton);
static void disable_dubstep_channel(struct circle_top_button *pbutton);
static void render_dubstep_channel(struct circle_top_button *pbutton);

static void enable_psytrance(struct circle_top_button *pbutton);
static void disable_psytrance(struct circle_top_button *pbutton);
static void render_psytrance(struct circle_top_button *pbutton);

static void enable_reggaeton(struct circle_top_button *pbutton);
static void disable_reggaeton(struct circle_top_button *pbutton);
static void render_reggaeton(struct circle_top_button *pbutton);
static void enable_reggaeton2(struct circle_top_button *pbutton);
static void disable_reggaeton2(struct circle_top_button *pbutton);
static void render_reggaeton2(struct circle_top_button *pbutton);

static void enable_reggaeton3(struct circle_top_button *pbutton);
static void disable_reggaeton3(struct circle_top_button *pbutton);
static void render_reggaeton3(struct circle_top_button *pbutton);

static void enable_merengue(struct circle_top_button *pbutton);
static void disable_merengue(struct circle_top_button *pbutton);
static void render_merengue(struct circle_top_button *pbutton);


static void enable_gospel(struct circle_top_button *pbutton);
static void disable_gospel(struct circle_top_button *pbutton);
static void render_gospel(struct circle_top_button *pbutton);

static void enable_rnb(struct circle_top_button *pbutton);
static void disable_rnb(struct circle_top_button *pbutton);
static void render_rnb(struct circle_top_button *pbutton);

static void enable_country(struct circle_top_button *pbutton);
static void disable_country(struct circle_top_button *pbutton);
static void render_country(struct circle_top_button *pbutton);

static void enable_grateful_dead(struct circle_top_button *pbutton);
static void disable_grateful_dead(struct circle_top_button *pbutton);
static void render_grateful_dead(struct circle_top_button *pbutton);
// WORLD_INTERNATIONAL off
char *default_es_function_flags="000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
char *default_en_function_flags="010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

// WORLD_INTERNATIONAL on
//#define WORLD_SR000_BASE_URL "https://sr000.silentradiance.com/download.cgi/"
char *default_function_uchoose=WORLD_SR000_BASE_URL;
 
static void compute_top_button_positions();
#define S66_6 9
#define S72_1 10

void invoke_order_721() {
top_buttons[S72_1].enabled = !(top_buttons[S72_1].enabled);
compute_top_button_positions();  
}


void invoke_order_66_6() {
top_buttons[S66_6].enabled = !(top_buttons[S66_6].enabled);
compute_top_button_positions();  
}

void clear_top_button_click_count() {
int i;
for (i=0;i<top_button_count;i++) {
  top_button_click_count[i]=0;
  }
}




static void set_top_bubble_scale(int number_buttons) {
#ifdef we_are_ready_to_Scale
float old_scale = top_bubble_scale;

//logit("%d buttons\n",number_buttons);
if (number_buttons <=7) {
  top_bubble_scale = 1.f;
  }
else if (number_buttons ==8) {
  top_bubble_scale = 0.95;
  }
else if (number_buttons ==9) {
  top_bubble_scale = 0.9;
  }
else if (number_buttons ==10) {
  top_bubble_scale = 0.84;
  }
else if (number_buttons==11) {
  top_bubble_scale = 0.68;
  }
else if (number_buttons==12) {
  top_bubble_scale = 0.60;
  }
else if (number_buttons==13) {
  top_bubble_scale = 0.52;
  }

else if (number_buttons >=14) {
  top_bubble_scale = 0.44;
  }
top_bubble_scale = top_bubble_scale*0.87f;

//logit("%f scale\n",top_bubble_scale);  
if (old_scale != top_bubble_scale) {
//  float factor = top_bubble_scale / old_scale;
// ^^^ good if buttons have relative size some day.
  int i;  
  for (i=0;i<NUMBER_OF_TOP_BUTTONS;i++) {
//    top_buttons[i].size*=factor;
    top_buttons[i].size = 0.5f*top_bubble_scale;
    }
  }
#endif  
top_bubble_scale = 1.f;
}

void check_top_button_click_count() {
int test666=0;
int test7=0;
int test2=0;
int test1=0;
int i;
for (i=0;i<top_button_count;i++) {
  if (top_button_click_count[i]==7) {
    test7++;
    }
  if (top_button_click_count[i]==2) {
    test2++;
    }
  if (top_button_click_count[i]==1) {
    test1++;
    }
  if ((test7==1)&&(test2==1)&&(test1==1)) {
    invoke_order_721();
    clear_top_button_click_count();
    break;
    }
  if (top_button_click_count[i]==6) {
    test666++;
    }
  if (test666==3) {
    invoke_order_66_6();
    clear_top_button_click_count();
    break;
    } 
  }
//   fprintf(stderr,"721 %d%d%d   666 %d%d%d\n",test7,test2,test1,test666,test666,test666);
}

void set_function_flags_based_on_string(char *button_flag_string) 
{
//logit("button flags %s",button_flag_string);
int i;
for (i=0;i<NUMBER_OF_TOP_BUTTONS;i++) {
  if (button_flag_string[i]) {
    if (button_flag_string[i]=='0') {
      top_buttons[i].flag=0;
      }
    else {
      top_buttons[i].flag=1;
      top_buttons[i].enabled=1; // save current station if found like 666 or 721
      }
    }
  else {
    break;
    }
  }
for (;i<NUMBER_OF_TOP_BUTTONS;i++) { /* cleanup in case the flags are short */
  top_buttons[i].flag=0;
  }    
}



void set_function_flags_to_preferences() 
{
//#define WORLD_FunctionUChoose "FunctionUChoose"
char *uchoose = getPreferenceOrDefault(&main_preferences,
  WORLD_FunctionUChoose,default_function_uchoose);
uchoose=default_function_uchoose; // for now
strncpy(FunctionUChoose,uchoose,256);
FunctionUChoose[255]='\0';

//#define WORLD_FunctionFlags "FunctionFlags"
char *def_function_flags;
if (strcmp(WORLD_LANGUAGE_CODE,"es")==0) {
  def_function_flags = default_es_function_flags;
  }
else {
  def_function_flags = default_en_function_flags;
  }
char *function_flags = getPreferenceOrDefault(&main_preferences,
  WORLD_FunctionFlags,def_function_flags);

set_function_flags_based_on_string(function_flags);
}


static void init_top_menu_structure();

/* this is called early - we might init some strucutre before the graphics here */
char *top_menu_url() {
init_top_menu_structure(); // ok if call more than once
//set_function_flags_to_preferences(); -- called by init

int i;
for (i=0;i<top_button_count;i++) {
  if (top_buttons[i].flag) {
    if (i==0) { // wifi broadcast 
      return NULL;
      }
    else if (i==uchoose_button_id) {
      return(FunctionUChoose);
      }
    else {
      return(top_buttons[i].url);
      }
    }
  } // for each button
return(NULL);
}



void set_function_flag_preferences() {
int i;
i=0;
char function_flag_Buffer[2000];
/* render background */
for (i=0;i<NUMBER_OF_TOP_BUTTONS;i++) {
  if (top_buttons[i].flag) {
    function_flag_Buffer[i]='1';
    }
  else {
    function_flag_Buffer[i]='0';
    }
  }
function_flag_Buffer[i]='\0';
//#define WORLD_FunctionFlags "FunctionFlags"
setPreference(&main_preferences,WORLD_FunctionFlags,function_flag_Buffer);
//#define WORLD_FunctionUChoose "FunctionUChoose"
setPreference(&main_preferences,WORLD_FunctionUChoose,FunctionUChoose);
}




static GLuint one_color_program;
static GLuint full_color_program;
static GLuint gvPositionHandle;
static GLuint mPositionHandle ;

static float width_in_pixels;
static float height_in_pixels;
static float width_in_inches;
static float height_in_inches;
static float height_to_inches;
static float rotation_in_radians;
static float relative_width;
static float relative_height;
static float width_to_inches;
static int circle_factor;
static lcontext *lc;



static void set_drawer() {
float g=    ((float)(rand()%456363))/456363.f;


effect_translate[0] = cos(g*2.0*3.1415927);
effect_translate[1] = sin(g*2.0*3.1415927)*2.f;
effect_translate[2] = 1.f;
}





static    GLfloat thisAngle = ((180.+72.+72.)*3.14159)/180.; // so the last point os on the bottom, for heavy metal
static    GLfloat angle=(72.0*3.14159)/180.;                 // note - converted to faster helper sin and cosing
static    GLfloat radius = 0.80f;

static void compute_top_button_positions() {

// count number of enabled itemss
float count=0.f;
int i;
for (i=0;i<NUMBER_OF_TOP_BUTTONS;i++) {
  if (!(top_buttons[i].render)) break; // get out early
  if (top_buttons[i].enabled) {
    count++;
    }
  }
if (count != 0.) {
  angle = (3.14159*2.0)/count;
  }

set_top_bubble_scale(count);

//radius = 1.0f -  (0.3f * top_bubble_radius);  /* radius 0,5 * factor - 0.25 but we are at 0.30 */
// this is for variable top_bubble_Radius - which is moot - set to 1

float anAngle = thisAngle;
  
// compute x and y
for (i=0;i<NUMBER_OF_TOP_BUTTONS;i++) {
  if (top_buttons[i].enabled) {
    float x,y;
    x =top_buttons[i].source_center_x= radius * hcos(anAngle);
    y =top_buttons[i].source_center_y= radius * hsin(anAngle);
#ifdef __APPLE__
      if (screen_layout==0) { /* portrait */
        if ((screen_orientation==1)||(screen_orientation==2)) {
	  top_buttons[i].center_x = x;
	  top_buttons[i].center_y = y ;
	  }
	else { /* upside wrong */
	  top_buttons[i].center_x = -x;
	  top_buttons[i].center_y = (-y);
	  }
	}
      else { /* landscape */
        if ((screen_orientation==1)||(screen_orientation==2)) {
	  top_buttons[i].center_x = (-y);
	  top_buttons[i].center_y = x;
	  }
        else {
	  top_buttons[i].center_x = y;
	  top_buttons[i].center_y = -x;
	  }
	} /* landscape */

#else
      if (screen_layout==0) { /* portrait */
        if ((screen_orientation==1)||(screen_orientation==2)) {
	  top_buttons[i].center_x = x;
	  top_buttons[i].center_y = y;
	  }
	else { /* upside wrong */
	  top_buttons[i].center_x = -x;
	  top_buttons[i].center_y = (-y);
	  }
	}
      else { /* landscape */
        if ((screen_orientation==1)||(screen_orientation==2)) {
	  top_buttons[i].center_x = (-y);
	  top_buttons[i].center_y = x;
	  }
        else {
	  top_buttons[i].center_x = y;
	  top_buttons[i].center_y = -x;
	  }
	} /* landscape */
#endif  
    anAngle += angle;
    }
//  if (!(top_buttons[i].render)) break; // get out early
  }

}




static int did_init_top_menu_structure=0;
static void init_top_menu_structure() {
if (did_init_top_menu_structure) return;
did_init_top_menu_structure=1;

    /* Set up the buttons */
    int i=0;
    

    GLfloat *fc;
    GLfloat *bc;
    
    

    /* 0 - choose network button */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=2;
    top_buttons[i].render=render_network_button;
    top_buttons[i].enable=enable_network_button;
    top_buttons[i].disable=disable_network_button;
    top_buttons[i].flag=0;
    top_buttons[i].url=NULL;
    i++;
	    
	    
    /* 1 - DubStep Button */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_dubstep_channel;
    top_buttons[i].enable=enable_dubstep_channel;
    top_buttons[i].disable=disable_dubstep_channel;
    top_buttons[i].flag=0;
//#define WORLD_DUBSTEP_BASE_URL "https://ds.silentradiance.com/download.cgi/"
    top_buttons[i].url=WORLD_DUBSTEP_BASE_URL;
    i++;

#ifdef CC_SOME_DAY    
    /* creative Commons  button */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_creative_commons;
    top_buttons[i].enable=enable_creative_commons;
    top_buttons[i].disable=disable_creative_commons;
    top_buttons[i].flag=0;
//#define WORLD_CREATIVE_COMMONS_BASE_URL "https://cc.silentradiance.com/download.cgi/"
    top_buttons[i].url=WORLD_CREATIVE_COMMONS_BASE_URL;
    i++;
#endif

    /* 2 - Psytrance button - YEAH! */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_psytrance;
    top_buttons[i].enable=enable_psytrance;
    top_buttons[i].disable=disable_psytrance;
    top_buttons[i].flag=0;
//#define WORLD_PSYTRANCE_BASE_URL "https://pt.silentradiance.com/download.cgi/"
    top_buttons[i].url=WORLD_PSYTRANCE_BASE_URL;
    i++;
    

    /* 3 - Reggaeton button - YEAH! */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_reggaeton;
    top_buttons[i].enable=enable_reggaeton;
    top_buttons[i].disable=disable_reggaeton;
    top_buttons[i].flag=0;
//#define WORLD_REGGAETON_BASE_URL "https://reggaeton.silentradiance.com/download.cgi/"
    top_buttons[i].url=WORLD_REGGAETON_BASE_URL;
    i++;

    /* 4 - Reggaeton2 button - YEAH! */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_reggaeton2;
    top_buttons[i].enable=enable_reggaeton2;
    top_buttons[i].disable=disable_reggaeton2;
    top_buttons[i].flag=0;
    top_buttons[i].url="https://reggaeton2.silentradiance.com/download.cgi/";
    i++;

    /* 5 - Reggaeton3 button - YEAH! */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_reggaeton3;
    top_buttons[i].enable=enable_reggaeton3;
    top_buttons[i].disable=disable_reggaeton3;
    top_buttons[i].flag=0;
    top_buttons[i].url="https://reggaeton3.silentradiance.com/download.cgi/";
    i++;

    /* 6 - grateful_dead button - YEAH! */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_grateful_dead;
    top_buttons[i].enable=enable_grateful_dead;
    top_buttons[i].disable=disable_grateful_dead;
    top_buttons[i].flag=0;
//#define WORLD_GRATEFUL_DEAD_BASE_URL "https://gd.silentradiance.com/download.cgi/"
    top_buttons[i].url=WORLD_GRATEFUL_DEAD_BASE_URL;
    i++;
    

    /* 7 - rnb button - YEAH! */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_rnb;
    top_buttons[i].enable=enable_rnb;
    top_buttons[i].disable=disable_rnb;
    top_buttons[i].flag=0;
    top_buttons[i].url="https://rnb.silentradiance.com/download.cgi/";
    i++;

    /* 8 - country - YEAH! */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_country;
    top_buttons[i].enable=enable_country;
    top_buttons[i].disable=disable_country;
    top_buttons[i].flag=0;
    top_buttons[i].url="https://country.silentradiance.com/download.cgi/";
    i++;

    /* 9 - Merengue button - YEAH! */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_merengue;
    top_buttons[i].enable=enable_merengue;
    top_buttons[i].disable=disable_merengue;
    top_buttons[i].flag=0;
    top_buttons[i].url="https://merengue.silentradiance.com/download.cgi/";
    i++;

    /* 10 - gospel button - YEAH! */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=0;
    top_buttons[i].render=render_gospel;
    top_buttons[i].enable=enable_gospel;
    top_buttons[i].disable=disable_gospel;
    top_buttons[i].flag=0;
    top_buttons[i].url="https://gospel.silentradiance.com/download.cgi/";
    i++;
    
    /* 8 - secret 666 menu button */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=0;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.5f;
    bc[1]=0.f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.3f;
    fc[1]=0.2f;
    fc[2]=0.0f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=2;
    top_buttons[i].render=render_secret666_button;
    top_buttons[i].enable=enable_secret666_button;
    top_buttons[i].disable=disable_secret666_button;
    top_buttons[i].flag=0;
//#define WORLD_HEAVY_METAL_BASE_URL "https://hm.silentradiance.com/download.cgi/"
    top_buttons[i].url=WORLD_HEAVY_METAL_BASE_URL;
    uchoose_button_id = i;
    i++;

    
	

    /* 9 - secret 721 menu button */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=0;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.f;
    bc[2]=1.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.8f;
    fc[1]=0.7f;
    fc[2]=0.0f;
    fc[3]=1.f;
    top_buttons[i].has_free_songs=2;
    top_buttons[i].render=render_secret721_button;
    top_buttons[i].enable=enable_secret721_button;
    top_buttons[i].disable=disable_secret721_button;
    top_buttons[i].flag=0;
//#define WORLD_SR002_BASE_URL "https://sr002.silentradiance.com/download.cgi/"
    top_buttons[i].url=WORLD_SR002_BASE_URL;
    uchoose_button_id = i;
    i++;
	        
		
    /* 10 - u choose network button --- has to be last */
    top_buttons[i].size=0.5f*top_bubble_scale;
    top_buttons[i].enabled=1;
    bc=top_buttons[i].background_color; /* dark green */
    bc[0]=0.f;
    bc[1]=0.10f;
    bc[2]=0.f;
    bc[3]=1.f;
    fc=top_buttons[i].foreground_color;
    fc[0]=0.f;
    fc[1]=1.f;
    fc[2]=1.f;
    fc[3]=1.f;
    top_buttons[i].render=render_uchoose_button;
    top_buttons[i].enable=enable_uchoose_button;
    top_buttons[i].disable=disable_uchoose_button;
    top_buttons[i].flag=0;
    top_buttons[i].url=NULL;
    uchoose_button_id = i;
    i++;

        
    
    set_function_flags_to_preferences();    
    
    
    
    compute_top_button_positions();
    
    top_button_count = i;

}



int top_menu_setupGraphics(int w, int h,int screen_mode) {
    loadIdentity(&modelMatrix);  
    rotate_apple_only(screen_orientation,&modelMatrix);
    loadIdentity(&effectMatrix);  
    loadIdentity(&viewMatrix);  
    loadIdentity(&projectionMatrix);  
    pass_menu_setup_info(
 	&width_in_pixels,
 	&height_in_pixels,
 	&width_in_inches,
 	&height_in_inches,
 	&height_to_inches,
 	&rotation_in_radians,
 	&relative_width,
 	&relative_height,
 	&width_to_inches,
 	&circle_factor,
 	&one_color_program,
 	&full_color_program,
 	&gvPositionHandle,
  	&projectionMatrix,
  	&mPositionHandle
 );
 
 

  if (!ran_setup_graphics_once_before    ) {
    clear_top_button_click_count(); 


    // letters init
    lc = linit_context(); // we will get our own letter context
    
    // vvv this is done in main_menu.c
    //letters_opengles_setgraphics(one_color_program,colorHandle,gvPositionHandle,mPositionHandle); // needs to be set up once, with a single color program
      
    
    colorHandle = glGetUniformLocation(one_color_program, "v_Color");
    
    do_effect = 0;
    effect_mode = 0;
    set_drawer();
    
    init_top_menu_structure();    
    
    
    mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");
    
    
    
    } // if first run
    
    /* convert these coordinates to cartesian coordinates where we are from -1,1 to -1.3 to 1.3 or -1.7 to 1.7  - a rectangle with aspect ratio fixed */
    
    compute_top_button_positions();    
    return 1;
}


void top_menu_renderBackground(int screen,glMatrix *eye) {
    /* To start out determine the color.
     * dead - black
     * nothing - dark red
     * warming - middle yellow
     * warning - orange
     * good - green
     * */
    glClearColor(0.0f, 0.0f, 0.f, 1.0f);
    checkGlError("glClearColor");

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
}


void top_menu_step() {
thisAngle = thisAngle + 0.0002;
compute_top_button_positions();
}

void top_menu_renderFrame(int screen,glMatrix *eye) {    
}



static void set_matrix() {
     GLint mMVPMatrixHandle ;
    mMVPMatrixHandle = glGetUniformLocation(one_color_program, "u_MVPMatrix");

    glMatrix *xmatrix;
    glMatrix stageMatrix;
    if (do_effect) {
      multMatrix(&stageMatrix,&modelMatrix,&effectMatrix);
      xmatrix = &stageMatrix;
      }
    else {
      xmatrix = &modelMatrix;
      }
    multMatrix(&MVMatrix,xmatrix,&viewMatrix);
    multMatrix(&MVPMatrix,&MVMatrix,&projectionMatrix);
    // Apply the projection and view transformation
    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&MVPMatrix));
}

static void set_matrix_translate(GLfloat centerx,GLfloat centery) {
    loadIdentity(&modelMatrix);
    translateMatrix(&modelMatrix,centerx,centery,0.f);
set_matrix();
}

static void GLDrawCircle(int segments,GLfloat circleSize,GLfloat centerx,GLfloat centery, int filled){
GLfloat vertices[7000];
    vertices[0] = 0;
    vertices[1] = 0;
    vertices[2] = 0;
    vertices[3] = circleSize;
    vertices[4] = 0;
    vertices[5] = 0;
if (segments>363) segments=363;
float difference = 3.1415927*2.0/((float)segments);
    int i;
    for(i =1; i <segments; i++){
        vertices[(i * 3)+ 3] = (float) (circleSize * cosf(difference * (float)i ));
        vertices[(i * 3)+ 4] = (float) (circleSize * sinf(difference * (float)i ));
        vertices[(i * 3)+ 5] = 0;
    }
    vertices[(i * 3)+ 3] = circleSize;
    vertices[(i * 3)+ 4] = 0.f;
    vertices[(i * 3)+ 5] = 0;



    glUseProgram(one_color_program);

    GLint mPositionHandle ;
    // get handle to vertex shader's vPosition member
     mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");

    // Enable a handle to the triangle vertices
    glEnableVertexAttribArray(mPositionHandle);

    // Prepare the triangle coordinate data
    glVertexAttribPointer(mPositionHandle, 3,
                                 GL_FLOAT, GL_FALSE,12
                                 ,vertices);
				 
    set_matrix_translate(centerx,centery);

    // Draw the triangle
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments+2);

    // Disable vertex array
    glDisableVertexAttribArray(mPositionHandle);


}



#ifdef SOMEOTHERDAY

static void GLDrawArc(int segments,GLfloat circleSize,GLfloat centerx,GLfloat centery, GLfloat startDegrees,GLfloat lengthDegrees){
GLfloat vertices[7000];
if (segments>363) segments=363;
float startRadians = startDegrees*3.1415927f/180.f;
float difference = lengthDegrees*3.1415927f/180.f/((float)segments);
    int i;
    for(i =0; i <=segments; i++){
        vertices[(i * 3)+ 0] = (float) (circleSize * cosf(startRadians+difference * (float)i ));
        vertices[(i * 3)+ 1] = (float) (circleSize * sinf(startRadians+difference * (float)i ));
        vertices[(i * 3)+ 2] = 0;
    }

    glUseProgram(one_color_program);

    GLint mPositionHandle ;
    // get handle to vertex shader's vPosition member
     mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");

    // Enable a handle to the triangle vertices
    glEnableVertexAttribArray(mPositionHandle);

    // Prepare the triangle coordinate data
    glVertexAttribPointer(mPositionHandle, 3,
                                 GL_FLOAT, GL_FALSE,12
                                 ,vertices);
    set_matrix_translate(centerx,centery);


    // Draw the triangle
    glDrawArrays(GL_LINE_STRIP, 0, segments+1);

    // Disable vertex array
    glDisableVertexAttribArray(mPositionHandle);


}
#endif



static void GLDrawLine(GLfloat x1,GLfloat y1,GLfloat x2,GLfloat y2) {
GLfloat vertices[70];
vertices[0] = x1;
vertices[1] = y1;
vertices[2] = 0.f;
vertices[3] = x2;
vertices[4] = y2;
vertices[5] = 0;
glUseProgram(one_color_program);
GLint mPositionHandle ;
// get handle to vertex shader's vPosition member
mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");

// Enable a handle to the triangle vertices
glEnableVertexAttribArray(mPositionHandle);

// Prepare the triangle coordinate data
glVertexAttribPointer(mPositionHandle, 3,
                                 GL_FLOAT, GL_FALSE,0
                                 ,vertices);
// Draw the triangle
glDrawArrays(GL_LINES, 0, 2);
// Disable vertex array
glDisableVertexAttribArray(mPositionHandle);
}



/* by specifying a width, we convert a line to a rectangle  - the rectangle is w wide with the line points in the center */
static void find_line_to_rectangle_by_width(GLfloat x1p, GLfloat y1p, 
                                   GLfloat x2p, GLfloat y2p, 
				   GLfloat w, 
				   GLfloat *px1a, GLfloat *py1a, 
				   GLfloat *px2a, GLfloat *py2a, 
				   GLfloat *px1b, 
				   GLfloat *py1b, 
				   GLfloat *px2b, GLfloat *py2b) {
float x1 = (x1p<x2p?x1p:x2p);
float x2 = (x1p<x2p?x2p:x1p);
float y1 = (x1p<x2p?y1p:y2p);
float y2 = (x1p<x2p?y2p:y1p);

// handle no slope 
if ((x2-x1)==0.f) {
  float w_over_2 = w*0.5;
  *px1a = x1-w_over_2;
  *py1a = y1;
  *px2a = x2-w_over_2;
  *py2a = y2;
  
  *px1b = x1+w_over_2;
  *py1b = y1;
  *px2b = x2+w_over_2;
  *py2b = y2;
  }
// handle slope of zero
else if ((y2-y1)==0.f) {
  float w_over_2 = w*0.5;
  *px1a = x1;
  *py1a = y1-w_over_2;
  *px2a = x2;
  *py2a = y2-w_over_2;
  
  *px1b = x1;
  *py1b = y1+w_over_2;
  *px2b = x2;
  *py2b = y2+w_over_2;
  }
else {
  float perpindicular_slope = (x1-x2)/(y2-y1); // - the inverse
  float c =  w*0.5;
  float xdist = c/sqrtf(perpindicular_slope*perpindicular_slope + 1.f);
  float ydist = xdist * perpindicular_slope;
  *px1a = x1 + xdist;
  *py1a = y1 + ydist;
  *px2a = x2 + xdist;
  *py2a = y2 + ydist;
  
  *px1b = x1 - xdist;
  *py1b = y1 - ydist;
  *px2b = x2 - xdist;
  *py2b = y2 - ydist;
  }
}




/* Essentially draws a thick line from endpoint to endpoint 
by converting the line into a rectangle, possibly at an angle.
A circle of diameter w can alternately be added at the endpoint to smooth out the work
This could be used by font work 
This is a 2d thick line
*/
static void GLDrawThickLine(GLfloat x1,GLfloat y1,GLfloat x2,GLfloat y2,GLfloat w) {
/*    x1a          x2a  */
/*    w/2  */
/*    x1     ->    x2 */
/*    w/2  */
/*    x1b          x2b */
/* find the end points of the line */
float x1a,y1a,x2a,y2a,x1b,y1b,x2b,y2b;
find_line_to_rectangle_by_width(x1,y1,x2,y2,w,&x1a,&y1a,&x2a,&y2a,&x1b,&y1b,&x2b,&y2b);
GLfloat vertices[] = { x1a,y1a,0.f,x1b,y1b,0.f,x2b,y2b,0.f,
                       x2b,y2b,0.f,x2a,y2a,0.f,x1a,y1a,0.f};
glUseProgram(one_color_program);
GLint mPositionHandle ;
// get handle to vertex shader's vPosition member
mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");

// Enable a handle to the triangle vertices
glEnableVertexAttribArray(mPositionHandle);

// Prepare the triangle coordinate data
glVertexAttribPointer(mPositionHandle, 3,
                                 GL_FLOAT, GL_FALSE,12
                                 ,vertices);
// Draw the triangle
glDrawArrays(GL_TRIANGLES, 0, 6);
// Disable vertex array
glDisableVertexAttribArray(mPositionHandle);
}





#ifdef SOMEOTHERDAY

static void GLDrawThickArc(int segments,GLfloat circleSize,GLfloat centerx,GLfloat centery, GLfloat startDegrees,GLfloat lengthDegrees,GLfloat lineWidth,int drawStartCap,int drawEndCap){
GLfloat vertices[7000];
if (segments>363) segments=363;
float startRadians = startDegrees*3.1415927f/180.f;
float difference = lengthDegrees*3.1415927f/180.f/((float)segments);
float circleSizeInside = circleSize - (lineWidth*0.5);
float circleSizeOutside = circleSize + (lineWidth*0.5);
    int i;
    int ix6 = 0;
    for(i =0; i <=segments; i++){
        GLfloat cosx = cosf(startRadians+difference * (float)i );
	GLfloat sinx = sinf(startRadians+difference * (float)i );
	
        vertices[ix6++] = (float) (circleSizeInside * cosx);
        vertices[ix6++] = (float) (circleSizeInside * sinx);
        vertices[ix6++] = 0.f;	
        vertices[ix6++] = (float) (circleSizeOutside * cosx);
        vertices[ix6++] = (float) (circleSizeOutside * sinx);
        vertices[ix6++] = 0.f;
        }

    glUseProgram(one_color_program);

    GLint mPositionHandle ;
    // get handle to vertex shader's vPosition member
     mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");

    // Enable a handle to the triangle vertices
    glEnableVertexAttribArray(mPositionHandle);

    // Prepare the triangle coordinate data
    glVertexAttribPointer(mPositionHandle, 3,
                                 GL_FLOAT, GL_FALSE,segments+segments
                                 ,vertices);
    set_matrix_translate(centerx,centery);


    // Draw the triangle
    glDrawArrays(GL_TRIANGLE_STRIP, 0, segments+segments);

    // Disable vertex array
    glDisableVertexAttribArray(mPositionHandle);

}




static void GLDrawSquare(GLfloat squareSize,GLfloat centerx,GLfloat centery, int filled) {
GLfloat vertices[700];
 GLfloat square_root_of_2_over_4 = 0.70710678118654752440f;
 GLfloat point = square_root_of_2_over_4*squareSize*0.5;
    vertices[0] = -point;
    vertices[1] = point;
    vertices[2] = 0;
    vertices[3] = point;
    vertices[4] = point;
    vertices[5] = 0;
    vertices[6] = point;
    vertices[7] = -point;
    vertices[8] = 0;
    
    vertices[9] = point;
    vertices[10] = -point;
    vertices[11] = 0;
    vertices[12] = -point;
    vertices[13] = -point;
    vertices[14] = 0;
    vertices[15] = -point;
    vertices[16] = point;
    vertices[17] = 0;
    
    glUseProgram(one_color_program);

    GLint mPositionHandle ;
    // get handle to vertex shader's vPosition member
     mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");

    // Enable a handle to the triangle vertices
    glEnableVertexAttribArray(mPositionHandle);

    // Prepare the triangle coordinate data
    glVertexAttribPointer(mPositionHandle, 3,
                                 GL_FLOAT, GL_FALSE,12
                                 ,vertices);


    set_matrix_translate(centerx,centery);

    // Draw the triangle
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Disable vertex array
    glDisableVertexAttribArray(mPositionHandle);


}
#endif

					       
void lg_circle_notransform(int segments,fl circleSize,lp center);
#define BUTTON_INTERNAL_AREA_FACTOR 0.85f
#define BUTTON_SELECTED_AREA_FACTOR 0.93f

static void render_radio_button_start(struct circle_top_button *pbutton) {
glUseProgram(one_color_program);
checkGlError("glUseProgram");
    
loadIdentity(&modelMatrix);
rotate_apple_only(screen_orientation,&modelMatrix);

lg_set_scale_translate(top_bubble_scale,(top_bubble_scale-1.f)*pbutton->center_x,
                                        (top_bubble_scale-1.f)*pbutton->center_y,0.f);
    set_matrix_translate(0,0);
  
fl orad = pbutton->size*0.5f;
fl irad = orad * BUTTON_SELECTED_AREA_FACTOR;
glUniform4f(colorHandle,0.f,0.f,0.f,1.f); /* draw a transparent black background */
lg_wide_arc(40,(orad+irad)*0.5f,(lp){pbutton->center_x,pbutton->center_y},0.f,365.f,(orad-irad)*1.05,0,0);

if (pbutton->flag) {
  GLfloat *fc=pbutton->foreground_color;
  glUniform4f(colorHandle,fc[0],fc[1],fc[2],fc[3]); /* draw a transparent blue square */
  orad = irad;
  irad = pbutton->size*0.5f*BUTTON_INTERNAL_AREA_FACTOR;
  lg_wide_arc(40,(orad+irad)*0.5f,(lp){pbutton->center_x,pbutton->center_y},0.f,365.f,(orad-irad),0,0);  
  }

GLfloat *bc=pbutton->background_color;
glUniform4f(colorHandle,bc[0],bc[1],bc[2],1.f); /* draw a transparent blue square */
lg_circle(40,irad,(lp){pbutton->center_x,pbutton->center_y});

}


static void render_radio_button_finish(struct circle_top_button *pbutton) {
}


    





static void setup_for_render_button(struct circle_top_button *pbutton) {
GLfloat factor = (pbutton->size*BUTTON_INTERNAL_AREA_FACTOR*0.5f*0.70710678118654752440f);
loadIdentity(&modelMatrix);
scaleMatrix(&modelMatrix,factor,
                         factor,
			 factor);
translateMatrix(&modelMatrix,pbutton->center_x/factor,pbutton->center_y/factor,0.f);

    glUseProgram(one_color_program);

    // get handle to vertex shader's vPosition member
     mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");
     
    // Enable a handle to the triangle vertices
    //glEnableVertexAttribArray(mPositionHandle);
    set_matrix();
}


static float network_button_arc_positions[] = 
{45.,115.,
 45.,115.,
 45.,115.,
 45.,117.,
 45.,125.,
 45.,135.,
 42.,140.,
 39.,145.,
 36.,150.,
 33.,150.,
 30.,150., 
 30.,140., 
 30.,133., 
 30.,125., 
 30.,117., 
 30.,112., 
 30.,109., 
 30.,100., 
 30.,96., 
 30.,91., 
 30.,85.,
 30.,76.,
 30.,72.,
 30.,60.,
 30.,52.,
};

static int network_phase = 0; /* 0 to 60 */

// qqq
static void render_network_button(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);

glUniform4f(colorHandle, 1.f,1.f,0.f,1.f);
GLDrawCircle(5*circle_factor,0.022f*top_bubble_scale, pbutton->center_x-0.1288f,pbutton->center_y-0.03839232f,1);
set_matrix_translate(pbutton->center_x,pbutton->center_y);

GLDrawLine(-0.1288*top_bubble_scale,-0.0884*top_bubble_scale, -0.1288*top_bubble_scale,-0.0604*top_bubble_scale);
GLDrawLine(-0.1288,-0.0712, -0.1128,-0.0724);
GLDrawLine(-0.1128,-0.0724, -0.104,-0.0788);
GLDrawLine(-0.1288,-0.0884, -0.1368,-0.1164);
GLDrawLine(-0.1288,-0.0884, -0.1208,-0.1164);

// table
glUniform4f(colorHandle, 0.545f,0.271f,0.075f,1.0f); // saddle brown http://prideout.net/archive/colors.php
GLDrawThickLine(-0.1128,-0.0844, -0.0928,-0.0844,0.004);
GLDrawThickLine(-0.1088,-0.1164, -0.1088,-0.0844,0.004);
GLDrawThickLine(-0.0968,-0.1164, -0.0968,-0.0844,0.004);

glUniform4f(colorHandle, 0.54f,0.54f,0.54f,1.0f);
GLDrawThickLine(-0.1124,-0.0812, -0.0948,-0.0812,0.002);
GLDrawThickLine(-0.0948,-0.0812, -0.0896,-0.0628,0.002);

network_phase=(network_phase+1199)%1200;
lg_set_scale_translate(top_bubble_scale,(top_bubble_scale-1.f)*pbutton->center_x,
                                        (top_bubble_scale-1.f)*pbutton->center_y,0.f);

for (int i=0;i<25;i++) {
  fl adj = (1200-network_phase)%75 *0.012f *0.01333333333f;
  fl h = (i+network_phase+40)%60 * 0.01666666666666666666f;
  fl s = (i+network_phase+20)%80 * 0.7 * 0.01666666666666666666f + 0.3f;
  fl v = (i+network_phase+20)%100 * 0.7 * 0.01666666666666666666f + 0.15f;
  rgb rgb;
  rgb = hsv2rgb(h,s,v);
  glUniform4f(colorHandle, rgb.r,rgb.g,rgb.b,1.f);
  
  GLfloat start = network_button_arc_positions[i+i] +    (float)((rand()&131071)) * 0.00000762939453125000 ; // + 0 to 1 degree
  GLfloat len = network_button_arc_positions[i+i+1]  +    (float)((rand()&131071)) * 0.00000762939453125000 - start;
    
  lg_wide_arc(20,((GLfloat)i)*0.012f + adj + 0.008f,(lp){-0.0896,-0.0628},start,len,0.004f,0,0);
  }

render_radio_button_finish(pbutton);
}


static void hey_undo_and_unprep_network() {
// at the glue level, nothting to do
top_buttons[0].flag=0;
}

static void hey_prep_network() {
// at the glue level, nothting to do
top_buttons[0].flag=1;
switch_packet_source(NULL,init_packet_source_block_flag);
}

static void hey_prep_streaming() {
// nothing to do at glue level
}

static void hey_unprep_streaming() {
// nothing to do at glue level
}

static void hey_undo_streaming(struct circle_top_button *pbutton) {
// nothing to do at glue level
pbutton->flag=0;
}

static void hey_link_to_streaming(struct circle_top_button *pbutton) {
pbutton->flag = 1;
if (pbutton-top_buttons == 0) {
  switch_packet_source(NULL,init_packet_source_block_flag);
  }
if (pbutton-top_buttons == uchoose_button_id) {
  switch_packet_source(FunctionUChoose,init_packet_source_block_flag);
  }
else {
  switch_packet_source(pbutton->url,init_packet_source_block_flag);
  }
// something to do !
}


static void enable_network_button(struct circle_top_button *pbutton) {
if (pbutton->flag) { // could do extra things for radio stations
  //goto_choose_channel vvv
#ifdef ANDROID  
  pick_wifi_network_flag=1;
#endif  
  return;
  }

{
  int i;
  for (i=1;i<top_button_count;i++) {
    if (top_buttons[i].flag) {
      hey_undo_streaming(top_buttons+i);
      }
    }
  }
hey_unprep_streaming();
hey_prep_network();
station_has_free_songs = 2;
//set_displayset_preferences();
}

static void disable_network_button(struct circle_top_button *pbutton) {
}





//  
static void render_uchoose_button(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
glUniform4f(colorHandle, 1.f,1.f,1.f,1.0f);
set_matrix_translate(pbutton->center_x,pbutton->center_y);

lg_set_scale_translate(top_bubble_scale,(top_bubble_scale-1.f)*pbutton->center_x,
                                        (top_bubble_scale-1.f)*pbutton->center_y,0.f);
lc->needed_points->line_width = 0.019f;

fl cr=0.08f;
lp c =  {0.0f,-0.07f};

lg_wide_arc(5*circle_factor,cr, c,180.f,180.f,0.06f,1,1);

lg_wide_line((lp){-0.080f,-0.07f},
            (lp){-0.080f,0.14f},0.06f);
lg_wide_line((lp){0.080f,-0.07f},
           (lp){0.080f,0.14f},0.06f);
lg_circle(3*circle_factor,0.06f*0.5f,(lp){0.080f,0.14f});
lg_circle(3*circle_factor,0.06f*0.5f,(lp){-0.080f,0.14f});
glUniform4f(colorHandle, 1.f,0.f,1.f,1.0f);

lc->needed_points->line_width = 0.04f;

render_radio_button_finish(pbutton);
}


static void enable_generic_button(struct circle_top_button *pbutton) {
int i;

if (pbutton->flag) { // could do extra things for radio stations
  hey_undo_streaming(pbutton);
  hey_link_to_streaming(pbutton);
  return;
  }
if (top_buttons[0].flag) {
  hey_undo_and_unprep_network();
  hey_prep_streaming();
  }
else {
  int i;
  for (i=1;i<top_button_count;i++) {
    if (top_buttons[i].flag) {
      if (top_buttons[i].disable) top_buttons[i].disable(top_buttons+i);
//      hey_undo_streaming(top_buttons+i);
      }
    }
  }
hey_link_to_streaming(pbutton);
station_has_free_songs = pbutton->has_free_songs;
//set_displayset_preferences();
}

static void disable_generic_button(struct circle_top_button *pbutton) {
if (pbutton->flag) { // could do extra things for radio stations
  hey_undo_streaming(pbutton);
  }        
    
}



static void enable_uchoose_button(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
}

static void disable_uchoose_button(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}



static void enable_secret666_button(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
heavy_metal_flag=1;
}

static void disable_secret666_button(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
heavy_metal_flag=0;
}

static void enable_secret721_button(struct circle_top_button *pbutton) {
hib_flag=1;
enable_generic_button(pbutton);
}

static void disable_secret721_button(struct circle_top_button *pbutton) {
hib_flag=0;
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}

static int hfull=0;
static int heavy_metal_state = 0;

static float heavy_xy[] = {
0.f,1.f,
0.95105652,0.30901699,
0.58778525,-0.80901699,
-0.58778525,-0.80901699,
-0.95105652,0.30901699,
};
//  
static void render_heavy_metal_button(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);

lg_set_scale_translate(top_bubble_scale,(top_bubble_scale-1.f)*pbutton->center_x,
                                        (top_bubble_scale-1.f)*pbutton->center_y,0.f);
fl rad=1.4f;


// outside circle around star
//glUniform4f(colorHandle, 0.0f,0.f,0.f,1.f);
//lg_circle(7*circle_factor,rad+0.04f,(lp){0.0f,0.0f});
glUniform4f(colorHandle, 0.25f,0.f,0.f,1.f);
lg_circle(7*circle_factor,rad,(lp){0.0f,0.0f});



// star
glUniform4f(colorHandle, 0.f,0.f,0.f,1.f);
fl oldx2=heavy_xy[6];
fl oldy2=heavy_xy[7];
fl oldx=heavy_xy[8];
fl oldy=heavy_xy[9];
for (int i=0;i<10;) {
  fl x,y;
  x=heavy_xy[i++];
  y=heavy_xy[i++];
  lg_wide_line((lp){oldx2*rad,oldy2*rad},
            (lp){x*rad,y*rad},0.01f);
  if ((hfull<4)&&((heavy_metal_state>>1) == (i>>1)-1)) {
    glUniform4f(colorHandle, 0.4f,0.4f,0.f,1.f);
    GLfloat mx,my;
    mx = (oldx2*rad + x*rad) * 0.5;
    my = (oldy2*rad + y*rad) * 0.5;
    if (heavy_metal_state&1) {
      lg_wide_line((lp){mx,my},
              (lp){x*rad,y*rad},0.017f);
      }
    else {
      lg_wide_line((lp){oldx2*rad,oldy2*rad},
              (lp){mx,my},0.017f);
      }
    glUniform4f(colorHandle, 0.f,0.f,0.f,1.f);
    }
    
  oldx2=oldx;
  oldy2=oldy;
  oldx=x;
  oldy=y;
  }
   
hfull = (hfull+1)&7;
if (!hfull) {
  heavy_metal_state = (heavy_metal_state+1)%10;
  }

// bottom of hand fill
glUniform4f(colorHandle, 0.9f,0.7f,0.4f,1.f);
lg_circle(6*circle_factor,0.666f,(lp){0.0f,-0.333f});

// left and right finger fill
lg_wide_line((lp){0.5f,-0.333f},(lp){0.5f,1.f-0.166f},0.333333f);
lg_circle(4*circle_factor,0.333333f*0.5f,(lp){0.5f,1.f-0.166f});

lg_wide_line((lp){-0.5f,-0.333f},(lp){-0.5f,1.f}       ,0.33333f);
lg_circle(4*circle_factor,0.333333f*0.5f,(lp){-0.5f,1.f});

// middle fingers fill
lg_wide_line((lp){0.1666666f,-0.333f},(lp){0.1666666f,0.5f},0.333333f);
lg_circle(4*circle_factor,0.333333f*0.5f,(lp){0.1666666f,0.5f});

lg_wide_line((lp){-0.1666666f,-0.333f},(lp){-0.16666f,0.5f}       ,0.33333f);
lg_circle(4*circle_factor,0.333333f*0.5f,(lp){-0.1666666f,0.5f});



// bottom arc
glUniform4f(colorHandle, 0.05f,0.f,0.f,1.f);
lg_wide_arc(5*circle_factor,0.666f, (lp){0.f,-0.333f},180.f,180.f,0.05f,1,1);


// left finger
lg_wide_line((lp){0.666f,-0.333f},(lp){0.666f,1.f-0.16666f},0.05f);
lg_wide_line((lp){0.33333f,-0.16666666f},(lp){0.33333f,1.f-0.16666f},0.05f);
lg_wide_arc(4*circle_factor,0.33333333f*0.5f, (lp){0.5f,1.f-0.1666666f},0.f,180.f,0.05f,1,1);


// right finger
lg_wide_line((lp){-0.666f,-0.333f},(lp){-0.666f,1.f},0.05f);
lg_wide_line((lp){-0.33333f,0.f},(lp){-0.33333f,1.f},0.05f);
lg_wide_arc(4*circle_factor,0.33333333f*0.5f, (lp){-0.5f,1.f},0.f,180.f,0.05f,1,1);

// middle fingers
lg_wide_line((lp){0.0f,-0.166666666666f},(lp){0.f,0.5f},0.05f);
lg_wide_arc(4*circle_factor,0.33333333f*0.5f, (lp){-0.1666666f,0.5f},0.f,180.f,0.05f,1,1);
lg_wide_arc(4*circle_factor,0.33333333f*0.5f, (lp){0.1666666f,0.5f},0.f,180.f,0.05f,1,1);

lg_wide_arc(4*circle_factor,0.33333333f*0.5f, (lp){0.1666666f,-0.166666666f},180.f,180.f,0.05f,1,1);


// thumb
lg_wide_arc(4*circle_factor,0.33333333f*0.5f, (lp){-0.5f,-0.166666666f},90.f,90.f,0.05f,1,1);
lg_wide_line((lp){-0.5f,0.f},(lp){-0.16666f,0.f},0.05f);
lg_wide_arc(4*circle_factor,0.33333333f*0.5f, (lp){-0.1666666666f,-0.166666666f},270.f,180.f,0.05f,1,1);
lg_wide_line((lp){-0.3333333333f,-0.3333333f},(lp){-0.16666f,-0.3333333333f},0.05f);

render_radio_button_finish(pbutton);
}










static void enable_heavy_metal_button(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
heavy_metal_flag=1;
}




static void disable_heavy_metal_button(struct circle_top_button *pbutton) {
heavy_metal_flag=0;
disable_generic_button(pbutton);
}

static void enable_dubstep_channel(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
}

static void disable_dubstep_channel(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
}










static int dubstep_indicator = 0;

static float u3down1(int r,int a) {
float ind=   ((float)(r%a))  / ((float)(a));
if (ind<0.75f) {
  return ind*1.3333333f;
  }
else {
  return 1.f - ((ind-0.75f)*(-4.f));
  }
}

static float u1down1(int r,int a) {
float ind=   ((float)(r%a))  / ((float)(a));
if (ind<0.5f) {
  return ind*2.f;
  }
else {
  return 1.f - ((ind-0.5f)*(2.f));
  }
}

static void render_dubstep_channel(struct circle_top_button *pbutton)  {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);

dubstep_indicator = (dubstep_indicator+3)%19200;


glLineWidth(1.f);
lg_set_scale_translate(top_bubble_scale,(top_bubble_scale-1.f)*pbutton->center_x,
                                        (top_bubble_scale-1.f)*pbutton->center_y,0.f);
float width = (u1down1(dubstep_indicator,80) * 0.016f) + 0.05f;
float brightness = (u3down1(dubstep_indicator,160) * 0.4f) + 0.6f;
float g = (u1down1(dubstep_indicator,120) * 0.6f) + 0.4f;
float b = 1.0f - g;
glUniform4f(colorHandle, 0.2f*brightness,g*brightness,b*brightness,1.0f);

lg_wide_arc(5*circle_factor,1.*0.080f,(lp) {-0.11f,0.f},270.f,180.f,width,1,1);
lg_wide_line((lp) {-0.11f,-0.080f},(lp){-0.11f,0.080f},width);

lg_wide_arc(5*circle_factor,1.*0.04f,(lp) {0.09f,0.04f},90.f,180.f,width,1,1);
lg_wide_line((lp) {0.09f,0.08f},(lp){0.13f,0.08f},width);

lg_wide_arc(5*circle_factor,1.*0.04f,(lp) {0.09f,-0.04f},270.f,180.f,width,1,1);
lg_wide_line((lp) {0.05f,-0.08f},(lp){0.09f,-0.08f},width);


render_radio_button_finish(pbutton);
}





static int render_state=0;




static void enable_psytrance(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}

static void disable_psytrance(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}









static void render_psytrance(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.8f,0.1f,0.6f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.0f,0.0f,0.7f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


glUniform4f(colorHandle, 0.96f,0.4f,0.9f,1.0f);
GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);

glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);

glUniform4f(colorHandle, 1.0f,1.0f,0.0f,1.0f);
lc->needed_points->line_width=0.145f;
float scale = 0.18;
float x=-0.50;
float y=-0.26;
//#define WORLD_display_top_menu_Psy "Psy"
letters_out(lc,scale,x*scale,y*scale,0.f,WORLD_display_top_menu_Psy) ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}









static void enable_reggaeton(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}

static void disable_reggaeton(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}









static void render_reggaeton(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.0f,0.6f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);

glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);

glUniform4f(colorHandle, 0.3f,0.96f,0.96f,1.0f);
lc->needed_points->line_width=0.145f;
float scale = 0.15;
float x=-0.60;
float y=-0.36;
//#define WORLD_display_top_menu_Rge "Rge"
letters_out(lc,scale,x*scale,y*scale,0.f,WORLD_display_top_menu_Rge) ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}








static void enable_reggaeton2(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}

static void disable_reggaeton2(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}









static void render_reggaeton2(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.0f,0.6f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);

glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);

glUniform4f(colorHandle, 0.3f,0.96f,0.96f,1.0f);
lc->needed_points->line_width=0.145f;
float scale = 0.15;
float x=-0.60;
float y=-0.36;

letters_out(lc,scale,x*scale,y*scale,0.f,"Rg2") ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}


static void enable_reggaeton3(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}

static void disable_reggaeton3(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}









static void render_reggaeton3(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.0f,0.6f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);

glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);

glUniform4f(colorHandle, 0.3f,0.96f,0.96f,1.0f);
lc->needed_points->line_width=0.145f;
float scale = 0.15;
float x=-0.60;
float y=-0.36;

letters_out(lc,scale,x*scale,y*scale,0.f,"Rg3") ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}





static void enable_merengue(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}

static void disable_merengue(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}









static void render_merengue(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.0f,0.6f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);

glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);

glUniform4f(colorHandle, 0.3f,0.96f,0.96f,1.0f);
lc->needed_points->line_width=0.145f;
float scale = 0.15;
float x=-0.60;
float y=-0.36;

letters_out(lc,scale,x*scale,y*scale,0.f,"Mge") ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}



static void enable_gospel(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
//set_displayset_preferences();
}

static void disable_gospel(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//set_displayset_preferences();
}









static void render_gospel(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.0f,0.6f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);

glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);

lg_wide_line((lp){0.f,-0.08f},
             (lp){0.f,0.14f},0.07f);
lg_wide_line((lp){-0.05f,0.08f},
             (lp){0.05f,0.08f},0.07f);
	     

glUniform4f(colorHandle, 0.3f,0.96f,0.96f,1.0f);
lc->needed_points->line_width=0.145f;
float scale = 0.15;
float x=-0.60;
float y=-0.36;

letters_out(lc,scale,x*scale,y*scale,0.f,"Gsp") ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}




static void enable_rnb(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}

static void disable_rnb(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}









static void render_rnb(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.0f,0.3f,0.5f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);

glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);

glUniform4f(colorHandle, 0.3f,0.96f,0.96f,1.0f);
lc->needed_points->line_width=0.145f;
float scale = 0.15;
float x=-0.60;
float y=-0.36;

letters_out(lc,scale,x*scale,y*scale,0.f,"R&B") ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}







static void enable_country(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}

static void disable_country(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}









static void render_country(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.3f,0.4f,0.3f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);

glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);

glUniform4f(colorHandle, 0.3f,0.96f,0.96f,1.0f);
lc->needed_points->line_width=0.145f;
float scale = 0.08;
float x=-1.93;
float y=-0.26;

letters_out(lc,scale,x*scale,y*scale,0.f,"Country") ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}

static void enable_grateful_dead(struct circle_top_button *pbutton) {
enable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}

static void disable_grateful_dead(struct circle_top_button *pbutton) {
disable_generic_button(pbutton);
//insert_display_set(4);
//set_displayset_preferences();
}









static void render_grateful_dead(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.0f,0.6f,0.0f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.6f,0.0f,0.7f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);
GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);



glUniform4f(colorHandle, 0.26f,0.83f,0.48f,1.0f);
lc->needed_points->line_width=WORLD_display_top_menu_Dead_line_width; //0.100f;
float scale = WORLD_display_top_menu_Dead_scale; //0.12;
float x=WORLD_display_top_menu_Dead_x; //-1.20;
float y=WORLD_display_top_menu_Dead_y; //-0.30;
//#define WORLD_display_top_menu_Dead "Dead"
letters_out(lc,scale,x*scale,y*scale,0.f,WORLD_display_top_menu_Dead) ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}



static void render_secret666_button(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.8f,0.1f,0.6f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.0f,0.0f,0.7f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


glUniform4f(colorHandle, 0.96f,0.4f,0.9f,1.0f);
GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);

glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);

glUniform4f(colorHandle, 1.0f,0.0f,0.0f,1.0f);
lc->needed_points->line_width=0.145f;
float scale = 0.14;
float x=-0.70;
float y=-0.36;
//#define WORLD_display_top_menu_666 "666"
letters_out(lc,scale,x*scale,y*scale,0.f,WORLD_display_top_menu_666) ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}



static void render_secret721_button(struct circle_top_button *pbutton) {
render_radio_button_start(pbutton);
setup_for_render_button(pbutton);
set_matrix_translate(pbutton->center_x,pbutton->center_y);
render_state = (render_state + 14) %360;

glUniform4f(colorHandle, 0.8f,0.1f,0.6f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);
glUniform4f(colorHandle, 0.0f,0.0f,0.7f,1.0f);
GLDrawCircle(10*circle_factor,0.2125f*0.90*top_bubble_scale,pbutton->center_x,pbutton->center_y,1);


glUniform4f(colorHandle, 0.96f,0.4f,0.9f,1.0f);
GLfloat phase = render_state;
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},45.f+phase,90.f,0.002,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},60.f+phase,60.f,0.004,0,0);
lg_wide_arc(3*circle_factor,0.2125f*0.96,(lp){0.f,0.f},75.f+phase,30.f,0.006,0,0);

glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.0f);

glUniform4f(colorHandle, 1.0f,0.0f,1.0f,1.0f);
lc->needed_points->line_width=0.145f;
float scale = 0.18;
float x=-0.50;
float y=-0.26;
//#define WORLD_display_top_menu_Hib "Hib"
letters_out(lc,scale,x*scale,y*scale,0.f,WORLD_display_top_menu_Hib) ;

lc->needed_points->line_width=0.04f;

glLineWidth(1.f);
 

render_radio_button_finish(pbutton);
}





void render_radio_top_buttons() {
int i;
for (i=0;i<top_button_count;i++) {

  struct circle_top_button *pbutton = top_buttons+i;
  if (!pbutton->enabled) continue;
  if (pbutton->render) {
    
    pbutton->render(pbutton);
    }
  // ??? debug event click
//  set_matrix_translate(pbutton->center_x,pbutton->center_y);
//  glUniform4f(colorHandle, 0.50f,0.50f,0.90f,1.0f);
//  lg_circle(20,pbutton->size*0.5,(lp){0.f,0.f});
  
  }
}





static void handle_effect_mode() {
if (effect_mode==0) {
  do_effect=1;
  effect_mode=1;
  effect_start_time = packet_summary.current_sample_time;
  effect_relative_position=0.;
  effect_end_time = add_milliseconds_to_time(packet_summary.current_sample_time,packet_summary.beat_interval_in_milliseconds);
//  logit("b %d,et %d-%d\n",packet_summary.beat_interval_in_milliseconds,(int)(effect_end_time.tv_sec),(int)(effect_end_time.tv_usec));
  }
if (effect_mode==1) {
  int difference = time_difference_in_milliseconds(effect_end_time,packet_summary.current_sample_time);
  if (difference<=0) {
    effect_mode = 2;
    mc->enabled=0;
    do_effect=0;
    }
  int difference2 = time_difference_in_milliseconds(effect_end_time,effect_start_time);
  effect_relative_position = 1.0f-(float)( ((double)(difference2-difference))/((double)difference2) );
//  logit("%d %d %f %f,%f,%f\n",difference,difference2,effect_relative_position,
//     effect_translate[0]*effect_relative_position, effect_translate[1]*effect_relative_position, effect_translate[2]*effect_relative_position);
  float x,y,z;
  x = effect_translate[0]*effect_relative_position;
  y = effect_translate[1]*effect_relative_position;
  z = expf(logf(1.0)+ (logf(1.0f/15.0f)-logf(1.0f))*(effect_relative_position));
  loadIdentity(&effectMatrix);
  loadIdentity(&effectMatrix);
  translateMatrix(&effectMatrix,x,y,0.f);
  scaleMatrix(&effectMatrix,z,z,1.f);
  }
else if (effect_mode==3) {
  effect_mode=4;
  do_effect=1;
  set_drawer();  
  effect_start_time = packet_summary.current_sample_time;
  effect_relative_position=0.;
  effect_end_time = add_milliseconds_to_time(packet_summary.current_sample_time,packet_summary.beat_interval_in_milliseconds);  
  }
if (effect_mode==4) {
  int difference = time_difference_in_milliseconds(effect_end_time,packet_summary.current_sample_time);
  if (difference<=0) {
    effect_mode = 0;
    do_effect=0;
    pop_display_set();
    return;
    }
  int difference2 = time_difference_in_milliseconds(effect_end_time,effect_start_time);
  effect_relative_position = (float)( ((double)(difference2-difference))/((double)difference2) );
  float x,y,z;
  x = effect_translate[0]*effect_relative_position;
  y = effect_translate[1]*effect_relative_position;
  z = expf(logf(1.0)+ (logf(1.0f/15.0f)-logf(1.0f))*(effect_relative_position));
//  logit("%d %d %f %f,%f,%f\n",difference,difference2,effect_relative_position,
//     effect_translate[0]*effect_relative_position, effect_translate[1]*effect_relative_position, z);
  loadIdentity(&effectMatrix);
  translateMatrix(&effectMatrix,x,y,0.f);
  scaleMatrix(&effectMatrix,z,z,1.f);
  }

}



static void render_cursor() {
if (mc->enabled) {
  float x = mc->x;
  float y = mc->y;
  float d = 0.06;
  x = x - d*cos(mc->phase*17.) + (d*0.8)*cos(mc->phase*13.) + ((d*0.3)*cos(mc->phase*61.));
  y = y - d*sin(mc->phase*17.) + (d*0.8)*sin(mc->phase*13.) + ((d*0.3)*sin(mc->phase*61.));
  mc->phase -= 6.28/365.*0.67;
  glUseProgram(one_color_program);
  checkGlError("glUseProgramsss");
  glUniform4f(colorHandle, 1.0f,0.0f,0.0f,1.f);
  GLDrawCircle(6*circle_factor,0.3*0.25*top_bubble_scale,x,y,1);
  glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.f);
  GLDrawCircle(6*circle_factor,0.3*0.20*top_bubble_scale,x,y,1);
  glUniform4f(colorHandle, 1.0f,0.0f,0.0f,1.f);
  GLDrawCircle(5*circle_factor,0.3*0.15*top_bubble_scale,x,y,1);
  glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.f);
  GLDrawCircle(5*circle_factor,0.3*0.10*top_bubble_scale,x,y,1);
  glUniform4f(colorHandle, 1.0f,0.0f,0.f,1.f);
  GLDrawCircle(4*circle_factor,0.3*0.05*top_bubble_scale,x,y,1);  
  }
}





void top_menu_renderForeground(int screen,glMatrix *eye) {
handle_effect_mode();
if (effect_mode ==0) return;
glDisable(GL_DEPTH_TEST);
checkGlError("disbleDepthTest");
glClear( GL_DEPTH_BUFFER_BIT);
checkGlError("glClear");

//draw_channel_button();    



render_radio_top_buttons();
render_cursor();  
    

}



int pick_wifi_network_flag=0;



void top_menu_touchEvent(int action,float x,float y,glMatrix *eye) {
/*event is in pixels from the upper right */
float ourx,oury;
  ourx = (x-width_in_pixels*0.5)/width_in_pixels*2.0;
  oury = (height_in_pixels*0.5-y)/height_in_pixels*2.0;
#ifdef __APPLE__kjisaooi      
if (screen_layout==0) {
  if ((screen_orientation==1)||(screen_orientation==2)) {
    oury = oury;
    }
  else {
    oury = -oury;
    ourx = -ourx;
    }
  }
else {
  if ((screen_orientation==1)||(screen_orientation==2)) {
    float t=ourx;
    ourx = -oury;
    oury = t;
    }
  else {
    float t=ourx;
    ourx = oury;
    oury = -t;
    }
  }
#else
if (screen_layout==0) {
  oury = oury*relative_height;
  }
else {
  ourx = ourx*relative_width;
  }
#endif
/* first try the buttons */
/* first try the top_buttons */

if (action==10001) { // unselected somethin
  check_top_button_click_count();
  }

//logit("event %d, %f,%f    -> %f,%f",action,x,y,ourx,oury);
//logit("width %f height %f\n",width_in_pixels,height_in_pixels);
if (action==1) {
  int i;
  for (i=0;i<top_button_count;i++) {
    float fromx,fromy/*,tox,toy*/;
    float size_squared;
    if (!(top_buttons[i].enabled)) continue;
    size_squared = top_buttons[i].size*top_buttons[i].size*0.25;
    fromx = top_buttons[i].center_x;
    fromy = top_buttons[i].center_y;
  //    logit("	compare to %d	%f,%f  size %f\n",i,fromx,fromy,sqrtf(size_squared));

    float distance_squared = (ourx-fromx)*(ourx-fromx)+(oury-fromy)*(oury-fromy);
   // logit("	distance to %f,%f is %f\n",ourx,oury,sqrtf(distance_squared));
    if (distance_squared <=size_squared) {
    //  logit("	ENABLE!!!!!!!!\n");
      if (top_buttons[i].enable) top_buttons[i].enable(top_buttons+i);
      top_button_click_count[i]++;
      set_function_flag_preferences();
      return;
      }
    }
    
  /* dont forget our circle button - we want to indicate that the network settins should come up to pick a wifi */  
//  float dist;
  
//  dist = sqrtf((ourx-SMILE_X)*(ourx-SMILE_X)+(oury-SMILE_Y)*(oury-SMILE_Y));
//  if (dist<1.04f*0.5) {
//    pick_wifi_network_flag=1;
//    return;
//    }  
//  }
  }  


if (action==1) {
  int retime_flag=0;
  /* this is cool - put the menu away of they click twice. or bring back if being filed. */
  if (effect_mode==2) {
    effect_mode=3;
    }
  else if (effect_mode==4) {
    effect_mode=2;
    mc->enabled=0;
    do_effect=0;
//    effect_mode=1;
//    retime_flag=1;
    }
  else if (effect_mode==1) {
    effect_mode=0;
    do_effect=0;
    pop_display_set();    
//    effect_mode=4;
//    retime_flag=1;
    }
  if (retime_flag) { /* doesnt work right. Disabled. Some other day. Hib */
    int difference = time_difference_in_milliseconds(effect_end_time,packet_summary.current_sample_time);
    if (difference>0) {
      int difference2 = time_difference_in_milliseconds(effect_end_time,effect_start_time);
      effect_relative_position = (float)( ((double)(difference2-difference))/((double)difference2) );
      int difference3 = time_difference_in_milliseconds(packet_summary.current_sample_time,effect_start_time);
      /* difference3 is the time we have to tack on to the end time from the current time */
      effect_end_time = add_milliseconds_to_time(packet_summary.current_sample_time,difference3);
      if (effect_relative_position<1.) {
        difference2 = ((float)(difference3)) / (1.f-effect_relative_position);
        }
      effect_start_time = add_milliseconds_to_time(effect_end_time,difference2);
      }
    } /* if retiming */
  }
}










int top_menu_key_common() {
if (!(mc->enabled)) {
  *mc=default_xmc;
  mc->enabled=1;
  mc->phase = (rand() % 2386);
  }
else {
  mc->oldx = mc->x;
  mc->oldy = mc->y;
  }
int best=-1;
double best_distance=1000000000.;
int i; /* find our button */
for (i=0;i<top_button_count+1;i++) {
  double distance = (top_buttons[i].center_x-mc->x)*(top_buttons[i].center_x-mc->x) + (top_buttons[i].center_y-mc->y)* (top_buttons[i].center_y-mc->y);
  if (distance<best_distance) {
    best_distance=distance;
    best=i;
    }
  }
if (best==-1) best=0;
return best;
}



int top_menu_up() {
int best=top_menu_key_common();
double bestx = top_buttons[best].center_x;
double besty = top_buttons[best].center_y;
int i;
int t;
int best2 = -1;
for (t=0;t<2;t++) {
  double best_distance2=10000000000.;
  for (i=0;i<top_button_count+1;i++) {
    if ((i!=best) &&(top_buttons[i].center_y>besty)) {
      double distance = (top_buttons[i].center_x-bestx)*(top_buttons[i].center_x-bestx) + (top_buttons[i].center_y-besty)* (top_buttons[i].center_y-besty);
      if (distance<best_distance2) {
        best_distance2=distance;
        best2=i;
        }
      }
    } /* for each button */
  if (best2==-1) {
    besty = -1000000.;
    }
  else {
    break;
    }
  }
if (best2==-1) best2=0;
mc->x=top_buttons[best2].center_x;
mc->y=top_buttons[best2].center_y;
    return 0;
}



int top_menu_down() {
int best=top_menu_key_common();
double bestx = top_buttons[best].center_x;
double besty = top_buttons[best].center_y;
int i;
int t;
int best2 = -1;
for (t=0;t<2;t++) {
  double best_distance2=10000000000.;
  for (i=0;i<top_button_count+1;i++) {
    if ((i!=best) &&(top_buttons[i].center_y<besty)) {
      double distance = (top_buttons[i].center_x-bestx)*(top_buttons[i].center_x-bestx) + (top_buttons[i].center_y-besty)* (top_buttons[i].center_y-besty);
      if (distance<best_distance2) {
        best_distance2=distance;
        best2=i;
        }
      }
    } /* for each button */
  if (best2==-1) {
    besty = 100000.;
    }
  else {
    break;
    }
  }
if (best2==-1) best2=0;
mc->x=top_buttons[best2].center_x;
mc->y=top_buttons[best2].center_y;
    return 0;
}

int top_menu_right() {
int best=top_menu_key_common();
double bestx = top_buttons[best].center_x;
double besty = top_buttons[best].center_y;

int i;
int t;
int best2 = -1;
for (t=0;t<2;t++) {
  double best_distance2=10000000000.;
  for (i=0;i<top_button_count+1;i++) {
    if ((i!=best) &&(top_buttons[i].center_x>bestx)) {
      double distance = (top_buttons[i].center_x-bestx)*(top_buttons[i].center_x-bestx) + (top_buttons[i].center_y-besty)* (top_buttons[i].center_y-besty);
      if (distance<best_distance2) {
        best_distance2=distance;
        best2=i;
        }
      }
    } /* for each button */
  if (best2==-1) {
    bestx = -10000.;
    }
  else {
    break;
    }
  }
if (best2==-1) best2=0;
mc->x=top_buttons[best2].center_x;
mc->y=top_buttons[best2].center_y;  
    return 0;}



int top_menu_left() {
int best=top_menu_key_common();
double bestx = top_buttons[best].center_x;
double besty = top_buttons[best].center_y;

int i;
int t;
int best2 = -1;
for (t=0;t<2;t++) {
  double best_distance2=10000000000.;
  for (i=0;i<top_button_count+1;i++) {
    if ((i!=best) &&(top_buttons[i].center_x<bestx)) {
      double distance = (top_buttons[i].center_x-bestx)*(top_buttons[i].center_x-bestx) + (top_buttons[i].center_y-besty)* (top_buttons[i].center_y-besty);
      if (distance<best_distance2) {
        best_distance2=distance;
        best2=i;
        }
      }
    } /* for each button */
  if (best2==-1) {
    bestx = 10000.;
    }
  else {
    break;
    }
  }
if (best2==-1) best2=0;
mc->x=top_buttons[best2].center_x;
mc->y=top_buttons[best2].center_y;
return 0;
}



int top_menu_keyEvent(int action,float x,float y,glMatrix *eye) {
/*event is in pixels from the upper right */

//logit("key event %d",action);

if ((effect_mode==1)||(effect_mode==3)||(effect_mode==4)) { return(0);}
if (action==10001) { // unselected somethin
  check_top_button_click_count();
  }
if (action==1) { /* like enter or something */
  if (!(mc->enabled)) { /* if nothing is toggled, make the target appear */
    int best2=top_menu_key_common();
    mc->x=top_buttons[best2].center_x;
    mc->y=top_buttons[best2].center_y;  
    mc->oldx=mc->x;
    mc->oldy=mc->y;
    mc->phase = (rand() % 2386);
    }
  else {
    int i= top_menu_key_common();
    
//    if (i==top_button_count) {    
//      push_display_set(TOP_MENU_ID);
      
      /* old school -pick a network. only works on android - well get it back on the top menu */
//      pick_wifi_network_flag=1;
    
//      goto disappear;
//      return 0;
//      }  /* if clicked on the circle */
    
    if (top_buttons[i].enable) top_buttons[i].enable(top_buttons+i);
    top_button_click_count[i]++;

    set_function_flag_preferences();
    return 0;
    } /* if we are toggling something */
  } /* if we hit return */
if (action==2) { /* up */
  top_menu_up();
  }
if (action==3) { /*down */
  top_menu_down();
  }
if (action==4) { /* left */
  top_menu_left();
  }
if (action==5) { /* right */
  top_menu_right();
  }
if (action==6) {
//  disappear:
  /* this is cool - put the menu away of they click twice. or bring back if being filed. */
  if (effect_mode==2) {
    effect_mode=3;
    }
  }
return 0;
}



#ifdef sdsd
int top_menu_keyEvent(int action) {
/*event is in pixels from the upper right */
if (action==6) {
  int retime_flag=0;
  /* this is cool - put the menu away of they click twice. or bring back if being filed. */
  if (effect_mode==2) {
    effect_mode=3;
    }
  else if (effect_mode==4) {
    effect_mode=2;
    do_effect=0;
//    effect_mode=1;
//    retime_flag=1;
    }
  else if (effect_mode==1) {
    effect_mode=0;
    do_effect=0;
    pop_display_set();    
//    effect_mode=4;
//    retime_flag=1;
    }
  if (retime_flag) { /* doesnt work right. Disabled. Some other day. Hib */
    int difference = time_difference_in_milliseconds(effect_end_time,packet_summary.current_sample_time);
    if (difference>0) {
      int difference2 = time_difference_in_milliseconds(effect_end_time,effect_start_time);
      effect_relative_position = (float)( ((double)(difference2-difference))/((double)difference2) );
      int difference3 = time_difference_in_milliseconds(packet_summary.current_sample_time,effect_start_time);
      /* difference3 is the time we have to tack on to the end time from the current time */
      effect_end_time = add_milliseconds_to_time(packet_summary.current_sample_time,difference3);
      if (effect_relative_position<1.) {
        difference2 = ((float)(difference3)) / (1.f-effect_relative_position);
        }
      effect_start_time = add_milliseconds_to_time(effect_end_time,difference2);
      }
    } /* if retiming */
  }
//logit("top action %d",action);
return(0);
}
#endif





const display_set top_menu_display_set = {
//#define WORLD_display_top_menu_top_menu "top_menu"
  .name = WORLD_display_top_menu_top_menu,
  .setupGraphics = top_menu_setupGraphics,
  .init = NULL,
  .finish = NULL,
  .changeScreen = NULL,
  .step = top_menu_step,
  .renderBackground = top_menu_renderBackground,
  .renderMain  = top_menu_renderFrame,
  .renderForeground = top_menu_renderForeground,
  .renderButton = NULL,
  .touchEvent = top_menu_touchEvent,
  .keyEvent = top_menu_keyEvent,
  .finishStreaming = NULL
  };

/*
$Log: top_menu.c,v $
Revision 1.59  2019/09/05 22:05:31  hib
Added gospel, merengue

Revision 1.58  2019/07/01 00:05:41  hib
minor - added hib_flag visual mode.

Revision 1.57  2019/06/30 23:34:54  hib
Addes heavy metal mode, finally.

Revision 1.56  2019/05/26 18:11:15  hib
Added country

Revision 1.55  2019/05/21 02:35:57  hib
wip

Revision 1.54  2019/05/16 11:27:38  hib
wip

Revision 1.53  2019/05/15 03:56:12  hib
Looking good

Revision 1.52  2019/05/03 16:28:10  hib
wip. the impact font is goofed up again.

Revision 1.51  2019/04/28 04:11:16  hib
wip - changing display_set to separate init, glsetup, and screen changed

Revision 1.50  2019/04/26 23:12:51  hib
-* change the logo to the correct prime records logo.


-* show cover big when buy now is pressed.
-* Cheap.reggaeton - add reggaeton2
-* Cheap.reggaeton - add reggaeton3

Revision 1.49  2019/04/15 18:01:11  hib
OK this is linux silentradiance and resplendor version 1.3 official.

Revision 1.48  2019/04/15 11:29:48  hib
OK. NOW this is the correct version for internationalization on linux!

Revision 1.47  2019/04/15 10:40:50  hib
WORLD_ macros have now set up silent radiance to be internationalized.
It is easier to implement a system where there are separate builds,
but I could handle a weird multi-languge on one build with more trickiness.

The world_internatiolization.h file sets it up
Currently just english

Revision 1.46  2019/04/15 06:09:25  hib
internationalization wip

Revision 1.45  2019/03/30 20:38:00  hib
Set up the top_menu fo the different channels in 1.3

Revision 1.44  2019/03/20 15:37:44  hib

works on ios

Revision 1.43  2019/03/19 02:50:55  hib
made get/buy buttons better - $$$ comes up only once in a while.

Revision 1.42  2019/03/17 12:58:25  hib
Updated copyrights

Revision 1.41  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

Revision 1.40  2018/12/24 09:00:52  hib
Added 666 and 721

Revision 1.39  2018/09/28 18:57:56  hib
SR 1.2 apple done <sr1.2 android
Fixed the scale of the hater bots - android has a strange rotate this time, instead of apple. Go figure.

Revision 1.38  2018/09/28 17:46:46  hib
Version 1.2 - after apple. Linting warnings and errors.
Also - trying to fix perspective of larryharvey in android

Revision 1.37  2018/08/22 21:58:51  hib
work in progress

Revision 1.36  2018/07/19 18:14:39  hib
wip

Revision 1.35  2018/02/22 21:57:55  hib
Help modularize the display visualizers much more - for display_set - need to do for the main menu too
Also - added step/draw initial logic, to make it more likely that we can render split screen for goggles.

Revision 1.34  2018/01/23 10:13:11  hib
This is the 1.1 release version of Silent Radiance for the android

Revision 1.33  2018/01/21 01:44:28  hib
menus are better but not perfect for apple.CVS: ----------------------------------------------------------------------

Revision 1.32  2018/01/20 22:10:07  hib
added 2018

Revision 1.31  2018/01/19 10:00:36  hib
deblocked psytrance and cc extra arcs.

Revision 1.30  2018/01/18 10:15:55  hib
This is good to go into prerelese now.

Revision 1.29  2018/01/17 17:19:00  hib
got psy

Revision 1.28  2018/01/17 13:46:46  hib
Added button crontrol to top menu. Yeah!

Revision 1.27  2018/01/09 10:46:35  hib
Got top menu tpo work on android! Real tough!

Revision 1.26  2017/12/30 16:26:44  hib
Ready to test on iphone

Revision 1.25  2017/12/28 21:10:46  hib
cleaned up hevy metal with star. fixed event selection

Revision 1.24  2017/12/24 11:27:55  hib
wip

Revision 1.23  2017/12/22 21:37:48  hib
Got the top_menu working

Revision 1.21  2017/12/22 04:27:55  hib
closer. working on heavy metal

Revision 1.20  2017/12/22 02:27:11  hib
Close but need heavy metal

Revision 1.19  2017/12/21 20:46:52  hib
looking better

Revision 1.18  2017/12/14 20:24:06  hib
fonts are working yeah!

Revision 1.17  2017/12/12 21:07:32  hib
Starting to get a wide line and wide arc to make letters

Revision 1.16  2017/12/12 05:27:56  hib
wip - got cruddy DS, cruddy CC, need Cruddy U, and then the network

Revision 1.15  2017/12/12 03:27:52  hib
wip

Revision 1.14  2017/12/04 06:13:33  jason
apply changes

Revision 1.13  2017/12/02 04:08:19  hib
getting there

Revision 1.12  2017/12/01 12:48:27  hib
Did a bit of a revamp for the graphics - cleaning out glViewport, added scissors that should clip the buttons on cheap phones.
Better set up to handle 3d, and the "third eye" idea.

Revision 1.11  2017/12/01 04:45:48  hib
Made all the graphical apps check to see if ran before so we don't recreate all the programs, etc.
just before we did it, images was close to working on android.
Thats our main focus right now.

Revision 1.10  2017/10/05 21:58:38  hib
work in progress

Revision 1.9  2017/07/05 12:40:10  hib
Added copyleft

Revision 1.8  2017/04/06 11:41:16  hib
clean up linux client ifdef

Revision 1.7  2017/04/06 11:10:31  hib
Works and switches orientation now. Whew!

Revision 1.6  2017/04/06 09:37:20  hib
Making multiple direction. Fix the tv icons. Clean up the tv

Revision 1.5  2017/04/05 19:17:02  hib
Got the keys to work, though their positions are off and the enter key doesn't work yet.
Now just got to fix the fucking problemw with translation from the fake world to the real world of points.

Revision 1.4  2017/04/05 06:33:33  hib
wip

Revision 1.3  2017/04/04 09:08:37  hib
Got it to run on linux as a client! yo!

Revision 1.2  2016/12/22 04:08:06  hib
removed the extra graphics from the menu, but it crashes

Revision 1.1  2016/12/20 23:45:38  hib
consolidating


*/
