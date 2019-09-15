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


/* button flag
   0 - lightning	display set N/A
   1 - purple_string                3
   2 - justin2                      1
   3 - network_cube                 4
   4 - backward_worm                5
   5 - words                        6
   7 - visual voice                 8
   8 - larry_harvey_hater_robots    9
   9 - mathias1 10
   11 - staff
   
   9 - ivonne1 11
   10 - luis1 12
   11 - dennis 13
The button flag decides if the button is selected or not. the display set defines the order.
*/

// WORLD_INTERNATIONAL off

#ifndef LINUX_CLIENT
/*                           01234567890*/
 char    *demo_button_flags="1000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
 char *default_en_button_flags="1000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
 char *default_es_button_flags="0101010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
#else
/*                              01234567890*/
 char *default_en_button_flags="1000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
 char *default_es_button_flags="0101010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
 char    *demo_button_flags="1001111001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
#endif

// WORLD_INTERNATIONAL on



/*$Revision: 1.104 $*/
/* main menu - with the graphics boxes */

#ifdef ANDROID
#include <jni.h>
#endif

#define CIRCLE_WIDTH 0.35f
#define SMILE_X 0.0f
#define SMILE_Y 0.666666f

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
#include "main_menu.h"


#include "letters_render_opengles2.h"
#include "preferences.h"



struct lightning_info {
GLfloat start_x;
GLfloat end_x;
int count;
};


#define NUMBER_LIGHTNING_BOLTS 5
struct lightning_info bolts[NUMBER_LIGHTNING_BOLTS];

struct menu_square_button buttons[100];
int button_count=0;



static glMatrix modelMatrix;
static glMatrix effectMatrix; /* used to do effects like zoom near/zoom away */
static glMatrix viewMatrix;
static glMatrix projectionMatrix;
static glMatrix MVMatrix;
static glMatrix MVPMatrix; // effective matrix for the menu - might be spinning in space.

glMatrix origin_matrix;  // matrix tha makes x from -1 to 1, y from -1 to 1.  X or y might be bigger depending on orientation

int menu_effect_mode; /* 0 - initial. 1 - approaching, 2 - usable, 3 - disappearing */
static struct timeval effect_start_time;
static struct timeval effect_end_time;
static float effect_relative_position; /* [0,1) */
static GLfloat effect_translate[3];
static lcontext *lc;

static int do_effect = 0; /* if on, the effectMatrix will be added after the model matrix to move the 2d "frame" away from view */
static int colorHandle;



static struct menu_cursor default_xmc = {0,0.,0.,0.,0.,0.};

static struct menu_cursor xmc = {0,0.,0.,0.,0.,0.};
static struct menu_cursor *mc = &xmc;

// WORLD_INTERNATIONAL off

static char *gVertexShaderOneColor =
"uniform mat4 u_MVPMatrix;      \n"		// A constant representing the combined model/view/projection matrix.
			
		   "attribute vec4 vPosition;     \n"		// Per-vertex position information we will pass in.
		   "void main()                    \n"		// The entry point for our vertex shader.
		   "{                              \n"
		   "   gl_PointSize = 1.;         \n"
		   "   gl_Position = u_MVPMatrix   \n" 	// gl_Position is a special variable used to store the final position.
		   "               * vPosition;   \n"     // Multiply the vertex by the matrix to get the final point in 			                                            			 
		   "}                              \n";    // normalized screen coordinates.

static char * gFragmentShaderOneColor =
"precision mediump float;\n"
	"uniform vec4 v_Color;\n"
        "void main() {\n"
        "  gl_FragColor = v_Color;\n"
        "}\n";
// WORLD_INTERNATIONAL on

extern void enable_generic(struct menu_square_button *pbutton);
extern void enable_generic_back(struct menu_square_button *pbutton);
extern void disable_generic(struct menu_square_button *pbutton);
void standard_step_button(struct menu_square_button *pbutton);
void standard_render_button(struct menu_square_button *pbutton,int screen);


extern void enable_generic_diamond(struct menu_square_button *pbutton);
extern void disable_generic_diamond(struct menu_square_button *pbutton);
void standard_step_diamond(struct menu_square_button *pbutton);
void standard_render_diamond(struct menu_square_button *pbutton,int screen);

void disable_main_menu_cursor() {
*mc = default_xmc;
}

glMatrix *menu_MVP_matrix() {
return &MVPMatrix;
}


static GLuint createOneColorProgram() {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, gVertexShaderOneColor);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, gFragmentShaderOneColor);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glBindAttribLocation(program, 0, "vPosition");
//        glBindAttribLocation(program, 0, "a_Color");

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
//                    logit("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}





// WORLD_INTERNATIONAL off

static char *gVertexShaderFull =
"uniform mat4 u_MVPMatrix;      \n"		// A constant representing the combined model/view/projection matrix.
			
		   "attribute vec4 a_Position;     \n"		// Per-vertex position information we will pass in.
		   "attribute vec4 a_Color;        \n"		// Per-vertex color information we will pass in.			  
		  
		   "varying vec4 v_Color;          \n"		// This will be passed into the fragment shader.
		  
		   "void main()                    \n"		// The entry point for our vertex shader.
		   "{                              \n"
		   "   v_Color = a_Color;          \n"		// Pass the color through to the fragment shader. 
		   "   gl_PointSize = 1.;         \n"		  											// It will be interpolated across the triangle.
		   "   gl_Position = u_MVPMatrix   \n" 	// gl_Position is a special variable used to store the final position.
		   "               * a_Position;   \n"     // Multiply the vertex by the matrix to get the final point in 			                                            			 
		   "}                              \n";    // normalized screen coordinates.


static char * gFragmentShaderFull =
"precision mediump float;\n"
	"varying vec4 v_Color;\n"
        "void main() {\n"
        "  gl_FragColor = v_Color;\n"
        "}\n";

// WORLD_INTERNATIONAL on

static GLuint createFullColorProgram() {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, gVertexShaderFull);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, gFragmentShaderFull);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glBindAttribLocation(program, 0, "a_Position");
        glBindAttribLocation(program, 1, "a_Color");

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
//                    logit("Could not link full program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}







static float width_in_pixels;
static float height_in_pixels;
static float width_in_inches;
static float height_in_inches;
static float height_to_inches;
static float rotation_in_radians;
static float relative_width;
static float relative_height;
static float width_to_inches;

static GLuint one_color_program;
static GLuint full_color_program;
static GLuint gvPositionHandle;
static GLint mPositionHandle ;



void init_lightning_bolt(struct lightning_info *bolt) {
float factor=2.f/456363.f;
bolt->start_x = 0.85* (( ((float)(rand()%456363))*factor)-1.f) ;
bolt->end_x = bolt->start_x +  0.15* (( ((float)(rand()%456363))*factor)-1.f) ;
bolt->count = (rand()%28)+3;
if ((rand()%50)<35) {
  bolt->count= -bolt->count;
  }
}






static void set_drawer() {
float g=    ((float)(rand()%456363))/456363.f;

effect_translate[0] = cos(g*2.0*3.1415927);
effect_translate[1] = sin(g*2.0*3.1415927)*2.f;
effect_translate[2] = 1.f;
}




#define NUMBER_OF_BUTTON_FLAGS 100


int button_flags[NUMBER_OF_BUTTON_FLAGS];
int diamond_flags[NUMBER_OF_BUTTON_FLAGS];
       
       
       
#define VR_BOX 17
// box number for vr

void set_button_flags_based_on_string(char *button_flag_string) 
{
logit("button flags %s",button_flag_string);
int i;
for (i=0;i<NUMBER_OF_BUTTON_FLAGS;i++) {
  if (button_flag_string[i]) {
    if (button_flag_string[i]=='0') {
      button_flags[i]=0;
      }
    else {
      button_flags[i]=1;
      }
    }
  else {
    break;
    }
  }
for (;i<NUMBER_OF_BUTTON_FLAGS;i++) { /* cleanup in case the flags are short */
  button_flags[i]=0;
  }    
lightning_flag=button_flags[0];

/* diamond flags are auto off */
for (i=0;i<NUMBER_OF_BUTTON_FLAGS;i++) {
  diamond_flags[i]=0;
  }


//display_main_set_screen_drawmode(button_flags[VR_BOX]);
screen_drawmode = button_flags[VR_BOX];
    
}

extern int make_demo_mode;

void set_button_flags_to_preferences() 
{
char *def_button_flags;
if (strcmp(WORLD_LANGUAGE_CODE,"es")==0) {
  def_button_flags = default_es_button_flags;
  }
else {
  def_button_flags = default_en_button_flags;
  }  
//#define WORLD_ButtonFlags "ButtonFlags"
char *button_flags = getPreferenceOrDefault(&main_preferences,
   WORLD_ButtonFlags,make_demo_mode?demo_button_flags:def_button_flags);
set_button_flags_based_on_string(button_flags);
}



void set_button_flag_preferences() {
int i;
i=0;
char button_flag_buffer[2000];
lightning_flag=button_flags[0];
/* render background */
for (i=0;i<NUMBER_OF_BUTTON_FLAGS;i++) {
  if (button_flags[i]) {
    button_flag_buffer[i]='1';
    }
  else {
    button_flag_buffer[i]='0';
    }
  }
button_flag_buffer[i]='\0';
//#define WORLD_ButtonFlags "ButtonFlags"
setPreference(&main_preferences,WORLD_ButtonFlags,button_flag_buffer);
}


static void set_origin_matrix(void);

static int circle_factor=1;


static float dcolumns[] = {-0.80,-0.40,0.,0.40,0.80};
#define DNUMCOLUMNS 5
static float drows[] = {0.35,0.0,-0.35,-0.70};

static void add_button(int i,int display_set_id) {
GLfloat *fc;
GLfloat *bc;
buttons[i].shape=0;
buttons[i].source_center_x = dcolumns[i%DNUMCOLUMNS];
buttons[i].source_center_y = drows[i/DNUMCOLUMNS];
buttons[i].size=0.5f;
bc=buttons[i].background_color; /* dark blue */
bc[0]=0.f;
bc[1]=0.f;
bc[2]=0.35f;
bc[3]=1.f;
fc=buttons[i].foreground_color;
fc[0]=0.f;
fc[1]=1.f;
fc[2]=1.f;
fc[3]=1.f;
buttons[i].display_set=display_set_id;
buttons[i].pflag = button_flags+i;
buttons[i].button_step=standard_step_button;
buttons[i].button_render=standard_render_button;
buttons[i].enable=enable_generic;
buttons[i].disable=disable_generic;

}



static void add_diamond(int i,int display_set_id) {

GLfloat *fc;
GLfloat *bc;
buttons[i].shape=1;   // diamond
buttons[i].source_center_x = dcolumns[i%DNUMCOLUMNS];
buttons[i].source_center_y = drows[i/DNUMCOLUMNS]+ ((float)(i&1))*0.05-0.025;;
buttons[i].size=0.5f;
bc=buttons[i].background_color; /* dark blue */
bc[0]=0.f;
bc[1]=0.f;
bc[2]=0.35f;
bc[3]=1.f;
fc=buttons[i].foreground_color;
fc[0]=0.f;
fc[1]=1.f;
fc[2]=1.f;
fc[3]=1.f;
buttons[i].display_set=display_set_id; // just for the button to work I think
buttons[i].pflag = diamond_flags+i;
buttons[i].button_step=standard_step_diamond;
buttons[i].button_render=standard_render_diamond;
buttons[i].enable=enable_generic_diamond;
buttons[i].disable=disable_generic_diamond;
}



void render_lightning_button1(struct menu_square_button *pbutton,int screen);
void step_lightning_button(struct menu_square_button *pbutton);
void render_vr_button1(struct menu_square_button *pbutton,int screen);
void step_vr_button(struct menu_square_button *pbutton);
void enable_vr_button(struct menu_square_button *pbutton); 
void disable_vr_button(struct menu_square_button *pbutton); 






int main_menu_setupGraphics(int w, int h,int graphics_mode) {
      
    loadIdentity(&modelMatrix);  
    rotate_apple_only(screen_orientation,&modelMatrix);
    loadIdentity(&effectMatrix);  
    loadIdentity(&viewMatrix);  
    loadIdentity(&projectionMatrix);  
    /* adjust projection for scrren dpi */
    width_in_pixels=w;
    height_in_pixels=h;
    if ((width_in_pixels)>=3840||(height_in_pixels>=3840)) {
      circle_factor=8;
      }
    else if ((width_in_pixels)>=2560||(height_in_pixels>=2560)) {
      circle_factor=6;
      }
    else if ((width_in_pixels)>=1920||(height_in_pixels>=1920)) {
      circle_factor=5;
      }
    else if ((width_in_pixels)>=1280||(height_in_pixels>=1280)) {
      circle_factor=4;
      }
    else if ((width_in_pixels)>=800||(height_in_pixels>=800)) {
      circle_factor=3;
      }
    else {
      circle_factor=2;
      }
    width_in_inches = ((float)w)/xdpi; if (width_in_inches <=0.0f) width_in_inches = 1.0f;
    height_in_inches = ((float)h)/ydpi; if (height_in_inches <=0.0f) height_in_inches = 1.0f;
    
//    logit("w %d h %d xdpi %f ydpi %f width %f height %f\n",w,h,xdpi,ydpi,width_in_inches,height_in_inches);
    /* adjust to be portrait */
    rotation_in_radians = 0.;

#ifdef __APPLE__
      
      /* width is to be from -1 to +1 */
      relative_width = 1.0;
      relative_height = height_in_inches / width_in_inches;
      width_to_inches = 1.0/width_in_inches;
      height_to_inches = 1.0/height_in_inches;

      scaleMatrix(&projectionMatrix,1.0,width_in_inches/height_in_inches,1.0);





    
#else
    if (width_in_inches >= height_in_inches) {
      
      /* width is to be from -1 to +1 */
      relative_width = width_in_inches / height_in_inches;
      relative_height = 1.0;
      width_to_inches = 1.0/width_in_inches;
      height_to_inches = 1.0/height_in_inches;
      
      scaleMatrix(&projectionMatrix,height_in_inches/width_in_inches,1.0,1.0);
      }
    else {
      /* width is to be from -1 to +1 */
      relative_width = 1.0;
      relative_height = height_in_inches / width_in_inches;
      width_to_inches = 1.0/width_in_inches;
      height_to_inches = 1.0/height_in_inches;

      scaleMatrix(&projectionMatrix,1.0,width_in_inches/height_in_inches,1.0);
      }
/* non apple */  
#endif     
	
set_origin_matrix();    
    
if (!ran_setup_graphics_once_before    ) {
    one_color_program = createOneColorProgram();
    if (!one_color_program) {
//        logit("Could not create one color program.");
        return 0;
    }
/*    
    full_color_program = createFullColorProgram();
    if (!full_color_program) {
//        logit("Could not create full color program.");
        return 0;
    }
    */
    full_color_program = 0; 
    gvPositionHandle = glGetAttribLocation(one_color_program, "vPosition");
    checkGlError("glGetAttribLocation");
//    logit("glGetAttribLocation(\"vPosition\") = %d\n",
//         gvPositionHandle);

//    logit("viewport %d,%d",w,h);
    // get handle to fragment shader's v_Color member
    colorHandle = glGetUniformLocation(one_color_program, "v_Color");
    mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");
    // letters init
    lc = linit_context();

    letters_opengles_setgraphics(one_color_program,colorHandle,gvPositionHandle,mPositionHandle); // needs to be set up once, with a single color program
      //once amonst all the drivers
	 
    
    do_effect = 0;
    menu_effect_mode = 0;
    set_drawer();

    {
      int i;       
      /* initialize lightning bolts */
      for (i=0;i<NUMBER_LIGHTNING_BOLTS;i++) {
         init_lightning_bolt(bolts+i);
         }
       }
    
    }
        
  /* convert these coordinates to cartesian coordinates where we are from -1,1 to -1.3 to 1.3 or -1.7 to 1.7  - a rectangle with aspect ratio fixed */
  int i;
  for (i=0;i<=button_count;i++) {
      GLfloat x,y,size;
      x = buttons[i].source_center_x;
      y = buttons[i].source_center_y;
      size = buttons[i].size;
#ifdef __APPLE__      
      if (screen_layout==0) { /* portrait */
        if ((screen_orientation==1)||(screen_orientation==2)) {
	  buttons[i].center_x = x;
	  buttons[i].center_y = y * relative_height;
	  buttons[i].size = size;
	  }
	else { /* upside wrong */
	  buttons[i].center_x = -x;
	  buttons[i].center_y = (-y) * relative_height;
	  buttons[i].size = size;
	  }
	}
      else { /* landscape */
        if ((screen_orientation==1)||(screen_orientation==2)) {
	  buttons[i].center_x = (-y) * relative_height ;
	  buttons[i].center_y = x;
	  buttons[i].size = size;
	  }
        else {
	  buttons[i].center_x = y * relative_height;
	  buttons[i].center_y = -x;
	  buttons[i].size = size;
	  }
	} /* landscape */
      } /* for each variable */

#else
      if (screen_layout==0) { /* portrait */
        if ((screen_orientation==1)||(screen_orientation==2)) {
	  buttons[i].center_x = x;
	  buttons[i].center_y = y * relative_height;
	  buttons[i].size = size;
	  }
	else { /* upside wrong */
	  buttons[i].center_x = -x;
	  buttons[i].center_y = (-y) * relative_height;
	  buttons[i].size = size;
	  }
	}
      else { /* landscape */
        if ((screen_orientation==1)||(screen_orientation==2)) {
	  buttons[i].center_x = (-y) * relative_width;
	  buttons[i].center_y = x;
	  buttons[i].size = size;
	  }
        else {
	  buttons[i].center_x = y * relative_width;
	  buttons[i].center_y = -x;
	  buttons[i].size = size;
	  }
	} /* landscape */
      } /* for each variable */
#endif  

    return 1;
}


int vr_button_id=0;



void main_menu_renderBackground(int screen,glMatrix *extra) {
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





extern void window_shop(struct menu_square_button *pbutton);
extern void buy_it_now(struct menu_square_button *pbutton); 



/* this is done before we call setupGraphics and is done once I think */
void main_menu_init() {
    
    
    /* Set up the buttons */
    int i=0;
    int j=0;

    /* lightning button */
    add_button(i,-1);
    buttons[i].button_step=step_lightning_button;
    buttons[i].button_render=render_lightning_button1;
    buttons[i].enable=NULL;
    buttons[i].disable=NULL;
    i++;
    
    add_button(i++,3);       /* purple_string button */
    add_button(i++,1);       /* justin2 button */
    add_button(i++,4);       /* network_cube button */
    buttons[i-1].enable=enable_generic_back;

    add_button(i++,5);       /* backward worm button */
    add_button(i++,6);       /* words button */
    
    
    
    // display_set 7 is top_menu 
    j=8;    
    add_button(i++,j++);       /* Virtual voice */
    add_button(i++,j++);	     /* larry harvey robot haters */
    add_button(i++,j++);      /* mathias1 */
    buttons[i-1].enable=enable_generic_back;
   
#ifdef later
    add_button(i++,j++);      /* srikant1 */
#endif    
    
    add_button(i++,j++);      /* staff */

#ifdef LINUX_CLIENT
    add_button(i++,j++);      /* milkdrop/projectM */
    buttons[i-1].enable=enable_generic_back;
#endif
    
/* note - apple VR mode fails less epicly */    
/* android needs work as well */
#ifndef __APPLE__
#ifndef ANDROID
    add_button(i,-1);        /* VR button */
    buttons[i].button_step=step_vr_button;
    buttons[i].button_render=render_vr_button1;
    buttons[i].enable=enable_vr_button;
    buttons[i].disable=disable_vr_button;
    vr_button_id = i;
    i++;
#endif
#endif
    
    
//    add_button(i++,11);       /* ivonne1 */
//    add_button(i++,12);       /* luis1 */
//    add_button(i++,13);       /* dennis1 */
//    add_button(i++,11);       /* mathias1 */
    
    if (!i_am_a_tv) { // no browsing allowed on tv    
      add_diamond(i++,j++);       /* window shop */
      add_diamond(i++,j++);       /* buy it now */
      }

    set_button_flags_to_preferences();    
    
    button_count = i;
    
    buttons[i].source_center_x = SMILE_X; /* for the key characters - up down left right */
    buttons[i].source_center_y = SMILE_Y;
    buttons[i].size = CIRCLE_WIDTH;
    
} /* if never run before */




void main_menu_renderFrame(int screen,glMatrix *extra) {
}

static void set_origin_matrix() {
// no effect origin matrix
glMatrix mvmatrix;
multMatrix(&mvmatrix,&modelMatrix,&viewMatrix);
multMatrix(&origin_matrix,&mvmatrix,&projectionMatrix);
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
    rotate_apple_only(screen_orientation,&modelMatrix);
    translateMatrix(&modelMatrix,centerx,centery,0.f);
set_matrix();
}




void GLDrawCircle(int segments,GLfloat circleSize,GLfloat centerx,GLfloat centery, int filled){
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

/*
    int vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderCode);
    int fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

    one_color_program = glCreateProgram();             // create empty OpenGL ES Program
    glAttachShader(one_color_program, vertexShader);   // add the vertex shader to program
    glAttachShader(one_color_program, fragmentShader); // add the fragment shader to program
    glLinkProgram(one_color_program);  
*/

    glUseProgram(one_color_program);
checkGlError("fdkjfdjk");
    // get handle to vertex shader's vPosition member
     mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");
checkGlError("glU4334r4");
    // Enable a handle to the triangle vertices
    glEnableVertexAttribArray(mPositionHandle);
checkGlError("glwerere");
    // Prepare the triangle coordinate data
    glVertexAttribPointer(mPositionHandle, 3,
                                 GL_FLOAT, GL_FALSE,12
                                 ,vertices);
    checkGlError("glddsd");
				 
    set_matrix_translate(centerx,centery);


    // Draw the triangle
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments+2);

    // Disable vertex array
    glDisableVertexAttribArray(mPositionHandle);


}





void GLDrawArc(int segments,GLfloat circleSize,GLfloat centerx,GLfloat centery, GLfloat startDegrees,GLfloat lengthDegrees){
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




void GLDrawSquare(GLfloat squareSize,GLfloat centerx,GLfloat centery, int filled) {
GLfloat vertices[700];
 GLfloat square_root_of_2_over_4 = 0.70710678118654752440f*0.5;;
 GLfloat point = square_root_of_2_over_4*squareSize;
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





void GLDrawDiamond(GLfloat squareSize,GLfloat centerx,GLfloat centery, int filled) {

GLfloat vertices[700];
 GLfloat point = squareSize*0.5;
    vertices[0] = -point;
    vertices[1] = 0;
    vertices[2] = 0;
    
    vertices[3] = 0;
    vertices[4] = point;
    vertices[5] = 0;
    
    vertices[6] = 0;
    vertices[7] = -point;
    vertices[8] = 0;
    
    vertices[9] = point;
    vertices[10] = 0;
    vertices[11] = 0;
    
    vertices[12] = 0;
    vertices[13] = -point;
    vertices[14] = 0;
    
    vertices[15] = 0;
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

					       


/*channel_button is the smiley face */
static void draw_channel_button(int screen) {
glUseProgram(one_color_program);
checkGlError("glUseProgram");
    
loadIdentity(&modelMatrix);
rotate_apple_only(screen_orientation,&modelMatrix);
/* draw three circles in upper part of the screen, 
  center 0.75 from the top - three for the 3 channels
  width could be 1/2 an inch
 */
glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
GLDrawCircle(9*circle_factor,1.04f*CIRCLE_WIDTH,buttons[button_count].center_x,buttons[button_count].center_y,1);

if (packet_summary.state == PACKET_STATE_WARNING) { /* yellow hollow ball */
  glUniform4f(colorHandle, 1.0f,1.0f,0.0f,1.0f);
  GLDrawCircle(9*circle_factor,1.0f*CIRCLE_WIDTH,buttons[button_count].center_x,buttons[button_count].center_y,1);
  glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
  GLDrawCircle(9,((1.f-packet_summary.quality*0.92f)*CIRCLE_WIDTH),buttons[button_count].center_x,buttons[button_count].center_y,1);
    
  /* draw lesser smile */

  glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
  glLineWidth(12.f*width_to_inches);
  GLDrawArc(4*circle_factor,CIRCLE_WIDTH*0.90f,buttons[button_count].center_x,buttons[button_count].center_y+0.0120f*2.0,-60.f,-60.f);
  glLineWidth(1.f*width_to_inches);
  
  }
else if (packet_summary.state == PACKET_STATE_WARMING) {
  glUniform4f(colorHandle, 1.0f,(145.f/255.f),0.0f,1.0f);
  GLDrawCircle(9*circle_factor,1.0*CIRCLE_WIDTH,buttons[button_count].center_x,buttons[button_count].center_y,1);
  glUniform4f(colorHandle, 1.0f,(165.f/255.f),0.0f,1.0f);
  GLDrawCircle(9*circle_factor,((1.f-packet_summary.warming_quality*0.92f)*CIRCLE_WIDTH),buttons[button_count].center_x,buttons[button_count].center_y,1);
    
    
    /* draw lesser frown */
    
    glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
    glLineWidth(12.f*CIRCLE_WIDTH);
    GLDrawArc(4*circle_factor,CIRCLE_WIDTH*0.90f,buttons[button_count].center_x,buttons[button_count].center_y-0.1625f*2.0,60.f,60.f);
    glLineWidth(1.f*CIRCLE_WIDTH);

  }
else if (packet_summary.state == PACKET_STATE_GOOD) { /* White-blue and yellow hollow ball */
  glUniform4f(colorHandle, 0.9f,0.9f,0.0f,1.0f);
  GLDrawCircle(9*circle_factor,1.0*CIRCLE_WIDTH,buttons[button_count].center_x,buttons[button_count].center_y,1);
  glUniform4f(colorHandle, 1.0f,1.0f,0.0f,1.0f);
  GLDrawCircle(9*circle_factor,(packet_summary.quality)*CIRCLE_WIDTH,buttons[button_count].center_x,buttons[button_count].center_y,1);
  /* smile */
  glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
  glLineWidth(8.f);
  GLDrawArc(4*circle_factor,CIRCLE_WIDTH*0.77f,buttons[button_count].center_x,buttons[button_count].center_y+0.0120f*2.0,-30.f,-120.f);
  glLineWidth(1.f);
  
  }
else if (packet_summary.state == PACKET_STATE_NOTHING) { /* White-blue and yellow hollow ball */
  glUniform4f(colorHandle, 0.9f,0.f,0.0f,1.0f);
  GLDrawCircle(9*circle_factor,1.0*CIRCLE_WIDTH,buttons[button_count].center_x,buttons[button_count].center_y,1);
  
  /* smile , well, frown */
  glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
  glLineWidth(6.f);
  GLDrawArc(4*circle_factor,CIRCLE_WIDTH*0.77f,buttons[button_count].center_x,buttons[button_count].center_y -0.117f*2.0,45.f,90.f);
  glLineWidth(1.f);
  }
else if (packet_summary.state == PACKET_STATE_DEAD) { /* White-blue and yellow hollow ball */
  glUniform4f(colorHandle, 0.7f,0.f,0.0f,1.0f);
  GLDrawCircle(9*circle_factor,1.0*CIRCLE_WIDTH,buttons[button_count].center_x,buttons[button_count].center_y,1);
                 
  glUniform4f(colorHandle, 0.0f,0.0f,0.0f,1.0f);
  GLDrawCircle(9*circle_factor,(0.95f)*CIRCLE_WIDTH,buttons[button_count].center_x,buttons[button_count].center_y,1);
    
    
    /* smile , well, frown */
  glUniform4f(colorHandle, 0.7f,0.0f,0.0f,1.0f);
  glLineWidth(6.f);
  GLDrawArc(4*circle_factor,CIRCLE_WIDTH*0.77f,buttons[button_count].center_x,buttons[button_count].center_y -0.117f*2.0,40.f,100.f);
  glLineWidth(1.f);
    
  }

}





#define BUTTON_INTERNAL_AREA_FACTOR 0.85f
#define BUTTON_SELECTED_AREA_FACTOR 0.93f


static void render_square_button_start(struct menu_square_button *pbutton) {
  if (checkGlError("render_square_button_start()")) {
//    logit("oob (%d)",(int)(pbutton-buttons));
    }
    glUseProgram(one_color_program);
//glViewport(0,0,width_in_pixels,height_in_pixels);
display_set_gl_base_viewport();
checkGlError("glViewport_main_menu1");
    glUseProgram(one_color_program);
checkGlError("glUseProgram");
    
loadIdentity(&modelMatrix);
rotate_apple_only(screen_orientation,&modelMatrix);

glUniform4f(colorHandle,0.f,0.f,0.f,0.f); /* draw a transparent black square */
GLDrawSquare(pbutton->size,pbutton->center_x,pbutton->center_y,1);

if (*(pbutton->pflag)) {
  GLfloat *fc=pbutton->foreground_color;
  glUniform4f(colorHandle,fc[0],fc[1],fc[2],fc[3]); /* draw a transparent foreground square */
  GLDrawSquare(pbutton->size*BUTTON_SELECTED_AREA_FACTOR,pbutton->center_x,pbutton->center_y,1);
  
  GLfloat *bc=pbutton->background_color;
  glUniform4f(colorHandle,bc[0],bc[1],bc[2],bc[3]); /* draw a transparent background square */
  GLDrawSquare(pbutton->size*BUTTON_INTERNAL_AREA_FACTOR,pbutton->center_x,pbutton->center_y,1);
  
  }
else {
  GLfloat *bc=pbutton->background_color;
  glUniform4f(colorHandle,bc[0],bc[1],bc[2],bc[3]); /* draw a transparent background square */
  GLDrawSquare(pbutton->size*BUTTON_SELECTED_AREA_FACTOR,pbutton->center_x,pbutton->center_y,1);
  }
  
glUseProgram(one_color_program);
checkGlError("glUseProgram");

}








static void render_square_button_finish(struct menu_square_button *pbutton) {
}







#define DIAMOND_INTERNAL_AREA_FACTOR 0.85f
#define DIAMOND_SELECTED_AREA_FACTOR 0.93f


static void render_diamond_start(struct menu_square_button *pbutton) {
if (checkGlError("render_diamond_start()")) {
//  logit("oob (%d)",(int)(pbutton-buttons));
  }

glUseProgram(one_color_program);

display_set_gl_base_viewport();
checkGlError("glViewport_main_menu1");

glUseProgram(one_color_program);
checkGlError("glUseProgram");
    
loadIdentity(&modelMatrix);
rotate_apple_only(screen_orientation,&modelMatrix);

glUniform4f(colorHandle,0.f,0.f,0.f,0.f); /* draw a transparent black square */
GLDrawDiamond(pbutton->size,pbutton->center_x,pbutton->center_y,1);

if (*(pbutton->pflag)) {
  GLfloat *fc=pbutton->foreground_color;
  glUniform4f(colorHandle,fc[0],fc[1],fc[2],fc[3]); /* draw a transparent foreground square */
  GLDrawDiamond(pbutton->size*DIAMOND_SELECTED_AREA_FACTOR,pbutton->center_x,pbutton->center_y,1);
  
  GLfloat *bc=pbutton->background_color;
  glUniform4f(colorHandle,bc[0],bc[1],bc[2],bc[3]); /* draw a transparent background square */
  GLDrawDiamond(pbutton->size*DIAMOND_INTERNAL_AREA_FACTOR,pbutton->center_x,pbutton->center_y,1);
  
  }
else {
  GLfloat *bc=pbutton->background_color;
  glUniform4f(colorHandle,bc[0],bc[1],bc[2],bc[3]); /* draw a transparent background square */
  GLDrawDiamond(pbutton->size*DIAMOND_SELECTED_AREA_FACTOR,pbutton->center_x,pbutton->center_y,1);
  }
  
glUseProgram(one_color_program);
checkGlError("glUseProgram");

}








static void render_diamond_finish(struct menu_square_button *pbutton) {
}










static void render_shape_start(struct menu_square_button *pbutton) {
if (pbutton->shape==0) render_square_button_start(pbutton);
else render_diamond_start(pbutton);
}








static void render_shape_finish(struct menu_square_button *pbutton) {
if (pbutton->shape==0) render_square_button_finish(pbutton);
else render_diamond_finish(pbutton);
}








void step_lightning_button(struct menu_square_button *pbutton) {
int i;

for (i=0;i<NUMBER_LIGHTNING_BOLTS;i++) {
  struct lightning_info *li = bolts+i;
  if (li->count<0) {
    li->count++;
    if (li->count>0) {
      init_lightning_bolt(li);
      }
    }
  else {
    li->count--;
    if (li->count<0) {
      init_lightning_bolt(li);
      }
    }
  if (li->count<0) continue;
  }
}
    






void render_lightning_button(struct menu_square_button *pbutton,int screen) {
GLfloat vertices[700];
loadIdentity(&modelMatrix);
rotate_apple_only(screen_orientation,&modelMatrix);
translateMatrix(&modelMatrix,pbutton->center_x,pbutton->center_y,0.f);
scaleMatrix(&modelMatrix,(pbutton->size*BUTTON_INTERNAL_AREA_FACTOR*0.5f*0.70710678118654752440f),
                         (pbutton->size*BUTTON_INTERNAL_AREA_FACTOR*0.5f*0.70710678118654752440f),1.f);



	glLineWidth(2.0f);
      glUseProgram(one_color_program);
glUniform4f(colorHandle,1.0f,1.0f,0.9f,1.0f); /* draw a Bright yellow lightning bolt */

    GLint mPositionHandle ;
    // get handle to vertex shader's vPosition member
     mPositionHandle = glGetAttribLocation(one_color_program, "vPosition");

    // Enable a handle to the triangle vertices
    glEnableVertexAttribArray(mPositionHandle);

    // Prepare the triangle coordinate data
    glVertexAttribPointer(mPositionHandle, 3,
                                 GL_FLOAT, GL_FALSE,12
                                 ,vertices);
    set_matrix();


int i;

for (i=0;i<NUMBER_LIGHTNING_BOLTS;i++) {
  struct lightning_info *li = bolts+i;
  if (li->count<0) continue;
  int j;
  GLfloat x;
  GLfloat y;
  j=0;
  x=li->start_x;
  y=-1.f;
  while (1) {
    int m=j*3;
    if (li->start_x==li->end_x) x=li->start_x;
    else x = li->start_x + (li->end_x-li->start_x)*0.5 * (y+1.f);
    x = x +  (  ((float)(rand()&65535))*0.00001525878906250000f ) *0.24f -0.12f;
    vertices[m]=x;
    vertices[m+1]=y;
    vertices[m+2]=0.f;

    j++;    
    if (y>=1.f) break;
    y=y+  (  ((float)(rand()&65535))*0.00001525878906250000f ) *0.16f + 0.04f;
    if (y>1.f) y=1.f;
    }
  glDrawArrays(GL_LINE_STRIP,0,j);
  }
// Disable vertex array
glDisableVertexAttribArray(mPositionHandle);
	glLineWidth(1.0f);

}
    

    
void render_lightning_button1(struct menu_square_button *pbutton,int screen) {
	render_square_button_start(pbutton); 
	render_lightning_button(pbutton,screen);
	render_square_button_finish(pbutton);
}
	    

	    
static int render_button_state=3;		    
static int render_button_timer=11;

void step_vr_button(struct menu_square_button *pbutton) {
render_button_timer = (render_button_timer+1)%13;
if (render_button_timer==0) {
  render_button_state = (render_button_state+1)%3;
  }
}  





static void setup_for_button_render(struct menu_square_button *pbutton);









/* Virtual reality button - turns onand any click turns it off */

void render_vr_button1(struct menu_square_button *pbutton,int screen) {
render_shape_start(pbutton); 
setup_for_button_render(pbutton);

glUseProgram(one_color_program);
checkGlError("kout1ssd");
float scale = WORLD_main_menu_virtual_scale; // 0.53
float x= WORLD_main_menu_virtual_x; // -0.83f
float y= WORLD_main_menu_virtual_y; // 0.15f
if (render_button_state>0) { 
  glUniform4f(colorHandle, 0.8f,1.0f,1.0f,1.0f);
  lc->needed_points->line_width=0.128f;
//#define WORLD_display_main_menu_Virtual "Virtual"
  letters_out(lc,scale,x,y,0.f,WORLD_display_main_menu_Virtual);
  }
if (render_button_state>1) {
  glUniform4f(colorHandle, 0.8f,1.0f,1.0f,1.0f);
  lc->needed_points->line_width=0.128f;

//#define WORLD_main_menu_reality_scale 0.53
//#define WORLD_main_menu_reality_x x+0.2*scale
//#define WORLD_main_menu_reality_y y-scale-0.3
  
//#define WORLD_display_main_menu_Reality "Reality"
  letters_out(lc,WORLD_main_menu_reality_scale,WORLD_main_menu_reality_x,
        WORLD_main_menu_reality_y,0.f,WORLD_display_main_menu_Reality);
  }

render_shape_finish(pbutton);
}


#include <stdio.h>

void enable_vr_button(struct menu_square_button *pbutton) {
display_main_set_screen_drawmode(1);
disable_main_menu_cursor();
remove_display_set(2);
menu_effect_mode=0;
}



void disable_vr_button(struct menu_square_button *pbutton) {
display_main_set_screen_drawmode(0);
disable_main_menu_cursor();
}

			    	    





static GLint mPositionHandle;


static void setup_for_button_render(struct menu_square_button *pbutton) {
GLfloat factor = (pbutton->size*BUTTON_INTERNAL_AREA_FACTOR*0.5f*0.70710678118654752440f);
loadIdentity(&modelMatrix);
rotate_apple_only(screen_orientation,&modelMatrix);
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



/* turns out they are almost all the same, except lightning */
void enable_generic(struct menu_square_button *pbutton) {
insert_display_set(pbutton->display_set);
set_displayset_preferences();
}
void disable_generic(struct menu_square_button *pbutton) {
remove_display_set(pbutton->display_set);
set_displayset_preferences();
}
/* turns out they are almost all the same, except lightning */
void enable_generic_back(struct menu_square_button *pbutton) {
insert_display_set_back(pbutton->display_set);
set_displayset_preferences();
}

void enable_generic_diamond(struct menu_square_button *pbutton) {
display_set *ds = registered_display_sets+pbutton->display_set;
if (ds->clickedOnMenuItemEvent) {
  (*(ds->clickedOnMenuItemEvent))();
  }
}
void disable_generic_diamond(struct menu_square_button *pbutton) {
}

void step_square_buttons() {
int i;
for (i=0;i<button_count;i++) {
  struct menu_square_button *pbutton = buttons+i;
  if (pbutton->button_step) {
    pbutton->button_step(pbutton);
    }
  }
}

void render_square_buttons(int screen) {
int i;
for (i=0;i<button_count;i++) {
  struct menu_square_button *pbutton = buttons+i;
  if (pbutton->button_render) {
//    logit("button test %d:\n",i);
    glUseProgram(one_color_program);
    pbutton->button_render(pbutton,screen);
    }
  }
}





void standard_step_button(struct menu_square_button *pbutton) {
if (pbutton->display_set != -1) {
  display_set *ds = registered_display_sets+pbutton->display_set;
  if (ds->stepButton) {
    (*(ds->stepButton))();
    }
  }
}


void standard_render_button(struct menu_square_button *pbutton,int screen) {
render_square_button_start(pbutton);
setup_for_button_render(pbutton);
if (pbutton->display_set != -1) {
  display_set *ds = registered_display_sets+pbutton->display_set;
  if (ds->renderButton) {
    (*(ds->renderButton))(one_color_program,colorHandle,mPositionHandle,&MVPMatrix,screen,NULL);
    }
  }
render_square_button_finish(pbutton);
}





void standard_step_diamond(struct menu_square_button *pbutton) {
if (pbutton->display_set != -1) {
  display_set *ds = registered_display_sets+pbutton->display_set;
  if (ds->stepButton) {
    (*(ds->stepButton))();
    }
  }
}


void standard_render_diamond(struct menu_square_button *pbutton,int screen) {
render_diamond_start(pbutton);
setup_for_button_render(pbutton);
if (pbutton->display_set != -1) {
  display_set *ds = registered_display_sets+pbutton->display_set;
  if (ds->renderButton) {
    (*(ds->renderButton))(one_color_program,colorHandle,mPositionHandle,&MVPMatrix,screen,NULL);
    }
  }
render_square_button_finish(pbutton);
}




/* effect mode is the menu coming out from a file, and being refiled*/


static void handle_effect_mode() {
if (menu_effect_mode==0) {
  do_effect=1;
  menu_effect_mode=1;
  effect_start_time = packet_summary.current_sample_time;
  effect_relative_position=0.;
  effect_end_time = add_milliseconds_to_time(packet_summary.current_sample_time,packet_summary.beat_interval_in_milliseconds);
//  logit("b %d,et %d-%d\n",packet_summary.beat_interval_in_milliseconds,(int)(effect_end_time.tv_sec),(int)(effect_end_time.tv_usec));
  }
if (menu_effect_mode==1) {
  int difference = time_difference_in_milliseconds(effect_end_time,packet_summary.current_sample_time);
  if (difference<=0) {
    menu_effect_mode = 2;
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
else if (menu_effect_mode==3) {
  menu_effect_mode=4;
  do_effect=1;
  set_drawer();  
  effect_start_time = packet_summary.current_sample_time;
  effect_relative_position=0.;
  effect_end_time = add_milliseconds_to_time(packet_summary.current_sample_time,packet_summary.beat_interval_in_milliseconds);  
  }
if (menu_effect_mode==4) {
  int difference = time_difference_in_milliseconds(effect_end_time,packet_summary.current_sample_time);
  if (difference<=0) {
    menu_effect_mode = 0;
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





/* cursor is a target that doesn't aim well */

void step_cursor() {
if (mc->enabled) {
  mc->phase -= 6.28/365.*0.67;
  }
}



void render_cursor(int screen) {
if (mc->enabled) {
  float x = mc->x;
  float y = mc->y;
  float d = 0.06;
  x = x - d*cos(mc->phase*17.) + (d*0.8)*cos(mc->phase*13.) + ((d*0.3)*cos(mc->phase*61.));
  y = y - d*sin(mc->phase*17.) + (d*0.8)*sin(mc->phase*13.) + ((d*0.3)*sin(mc->phase*61.));
  
  glUseProgram(one_color_program);
  checkGlError("glUseProgramsss");
  glUniform4f(colorHandle, 1.0f,0.0f,0.0f,1.f);
  GLDrawCircle(6*circle_factor,0.3*0.25,x,y,1);
  glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.f);
  GLDrawCircle(6*circle_factor,0.3*0.20,x,y,1);
  glUniform4f(colorHandle, 1.0f,0.0f,0.0f,1.f);
  GLDrawCircle(5*circle_factor,0.3*0.15,x,y,1);
  glUniform4f(colorHandle, 1.0f,1.0f,1.0f,1.f);
  GLDrawCircle(5*circle_factor,0.3*0.10,x,y,1);
  glUniform4f(colorHandle, 1.0f,0.0f,0.f,1.f);
  GLDrawCircle(4*circle_factor,0.3*0.05,x,y,1);  
  }
}











void main_menu_step() {
handle_effect_mode();
if (menu_effect_mode ==0) return;
step_square_buttons();
step_cursor();
}


void main_menu_renderForeground(int screen,glMatrix *extra) {

if (menu_effect_mode ==0) return;
glDisable(GL_DEPTH_TEST);
checkGlError("disbleDepthTest");
glClear( GL_DEPTH_BUFFER_BIT);
checkGlError("glClear");

draw_channel_button(screen);    

render_square_buttons(screen);

//glViewport(0,0,width_in_pixels,height_in_pixels);
display_set_gl_base_viewport();
checkGlError("glViewport_main_menu2");

render_cursor(screen);
}



struct menu_square_button *selected_button=NULL;


void unselect_selected_button() {
/* handle diamonds*/
if (selected_button) {
  if (*(selected_button->pflag)) {
    *(selected_button->pflag) = 0;
    if (selected_button->disable) selected_button->disable(selected_button);
    selected_button=NULL;
    }
  }
}  




static int find_button(float ourx,float oury) {
int i;
{
  for (i=0;i<button_count;i++) {
    float fromx,fromy,tox,toy;
    float size_over_2;
    size_over_2 = buttons[i].size*0.70710678118654752440f*0.5f;
    fromx = buttons[i].center_x - size_over_2;
    fromy = buttons[i].center_y - size_over_2;
    tox = buttons[i].center_x + size_over_2;
    toy = buttons[i].center_y + size_over_2;
//    logit("  %d size %f -> %f   x %f in (%f,%f) y %f in (%f,%f)",i,buttons[i].size,size_over_2,
//       ourx,fromx,tox,oury,fromy,toy);
    if ((ourx>=fromx)&&(ourx<=tox)&&(oury>=fromy)&&(oury<=toy)) {
      return i;
      }
    }
    
  /* dont forget our circle button - we want to indicate that the network settins should come up to pick a wifi */  
  float dist;
  
  dist = sqrtf((ourx-buttons[button_count].center_x)*(ourx-buttons[button_count].center_x)+(oury-buttons[button_count].center_y)*(oury-buttons[button_count].center_y));
  if (dist<1.04f*0.5) {
    return(-1);
    }
    
  return i;
  } // block
}  




void main_menu_touchEvent(int action,float x,float y,glMatrix *extra) {
/*event is in pixels from the upper right */
float ourx,oury;
ourx = (x-width_in_pixels*0.5)*2./width_in_pixels;
oury = (height_in_pixels*0.5-y)*2./height_in_pixels;
//logit("coord %f,%f -> %f,%f w %f h %f  layout %d orientation %d rh %f rw %f\n",x,y,ourx,oury,width_in_pixels,height_in_pixels,screen_layout,screen_orientation,
//     relative_height,relative_width);
#ifdef __APPLE__      
if (screen_layout==0) {
  if ((screen_orientation==1)||(screen_orientation==2)) {
    oury = oury*relative_height;
    }
  else {
    oury = -oury*relative_height;
    ourx = -ourx;
    }
  }
else {
  if ((screen_orientation==1)||(screen_orientation==2)) {
    float t=ourx;
    ourx = -oury*relative_height;
    oury = t;
    }
  else {
    float t=ourx;
    ourx = oury*relative_height;
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


if (action==10001) {
  unselect_selected_button();
  }

//logit("event %d, %f,%f layout %d   -> %f,%f   rh %f rw %f",action,x,y,screen_layout,ourx,oury,relative_height,relative_width);
//logit("width %f height %f\n",width_in_pixels,height_in_pixels);
if (action==1) {
  int i;
  i = find_button(ourx,oury);
  if (i!=button_count) {
    if (i!= -1) { // not top menu
      *(buttons[i].pflag) = !(*(buttons[i].pflag));
      if (*(buttons[i].pflag)) {
        if (buttons[i].enable) buttons[i].enable(buttons+i);
	}
      else {
        if (buttons[i].disable) buttons[i].disable(buttons+i);
        }
      if ((buttons[i].shape==1)&& (*(buttons[i].pflag)) ) {
        selected_button=buttons+i;
	}
      set_button_flag_preferences();
      return;
      }
    else {
      /* play around with this for now. top menu is supposed to allow different functions */
      push_display_set(TOP_MENU_ID);
      return;
      }
    }  // if clicked something
    
  // here we clicked in space and shpould hide or move the menu
  int retime_flag;
//  disappear:
  retime_flag=0;
  /* this is cool - put the menu away of they click twice. or bring back if being filed. */
  if (menu_effect_mode==2) {
    menu_effect_mode=3;
    }
  else if (menu_effect_mode==4) {
    menu_effect_mode=2;
    mc->enabled=0;
    do_effect=0;
//    menu_effect_mode=1;
//    retime_flag=1;
    }
  else if (menu_effect_mode==1) {
    menu_effect_mode=0;
    do_effect=0;
//    pop_display_set();    
    remove_display_set(2);
//    menu_effect_mode=4;
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
  } // if action is 1
  
}


int main_menu_key_common() {
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
for (i=0;i<button_count+1;i++) {
  double distance = (buttons[i].center_x-mc->x)*(buttons[i].center_x-mc->x) + (buttons[i].center_y-mc->y)* (buttons[i].center_y-mc->y);
  if (distance<best_distance) {
    best_distance=distance;
    best=i;
    }
  }
if (best==-1) best=0;
return best;
}



int main_menu_up() {
int best=main_menu_key_common();
double bestx = buttons[best].center_x;
double besty = buttons[best].center_y;
int i;
int t;
int best2 = -1;
for (t=0;t<2;t++) {
  double best_distance2=10000000000.;
  for (i=0;i<button_count+1;i++) {
    if ((i!=best) &&(buttons[i].center_y>besty)) {
      double distance = (buttons[i].center_x-bestx)*(buttons[i].center_x-bestx) + (buttons[i].center_y-besty)* (buttons[i].center_y-besty);
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
mc->x=buttons[best2].center_x;
mc->y=buttons[best2].center_y;
    return 0;
}



int main_menu_down() {
int best=main_menu_key_common();
double bestx = buttons[best].center_x;
double besty = buttons[best].center_y;
int i;
int t;
int best2 = -1;
for (t=0;t<2;t++) {
  double best_distance2=10000000000.;
  for (i=0;i<button_count+1;i++) {
    if ((i!=best) &&(buttons[i].center_y<besty)) {
      double distance = (buttons[i].center_x-bestx)*(buttons[i].center_x-bestx) + (buttons[i].center_y-besty)* (buttons[i].center_y-besty);
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
mc->x=buttons[best2].center_x;
mc->y=buttons[best2].center_y;
    return 0;
}

int main_menu_right() {
int best=main_menu_key_common();
double bestx = buttons[best].center_x;
double besty = buttons[best].center_y;

int i;
int t;
int best2 = -1;
for (t=0;t<2;t++) {
  double best_distance2=10000000000.;
  for (i=0;i<button_count+1;i++) {
    if ((i!=best) &&(buttons[i].center_x>bestx)) {
      double distance = (buttons[i].center_x-bestx)*(buttons[i].center_x-bestx) + (buttons[i].center_y-besty)* (buttons[i].center_y-besty);
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
mc->x=buttons[best2].center_x;
mc->y=buttons[best2].center_y;  
    return 0;}



int main_menu_left() {
int best=main_menu_key_common();
double bestx = buttons[best].center_x;
double besty = buttons[best].center_y;

int i;
int t;
int best2 = -1;
for (t=0;t<2;t++) {
  double best_distance2=10000000000.;
  for (i=0;i<button_count+1;i++) {
    if ((i!=best) &&(buttons[i].center_x<bestx)) {
      double distance = (buttons[i].center_x-bestx)*(buttons[i].center_x-bestx) + (buttons[i].center_y-besty)* (buttons[i].center_y-besty);
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
mc->x=buttons[best2].center_x;
mc->y=buttons[best2].center_y;
return 0;
}



int main_menu_keyEvent(int action,float x,float y,glMatrix *eye) {
/*event is in pixels from the upper right */
if (action==10001) {
  unselect_selected_button();
  }


//logit("key event %d",action);

if ((menu_effect_mode==1)||(menu_effect_mode==3)||(menu_effect_mode==4)) { return(0);}

if (action==1) { /* like enter or something */
  if (!(mc->enabled)) { /* if nothing is toggled, make the target appear */
    int best2=main_menu_key_common();
    mc->x=buttons[best2].center_x;
    mc->y=buttons[best2].center_y;  
    mc->oldx=mc->x;
    mc->oldy=mc->y;
    mc->phase = (rand() % 2386);
    }
  else {
    int i= main_menu_key_common();
    
    if (i==button_count) {    
      push_display_set(TOP_MENU_ID);
      
      return 0;
      }  /* if clicked on the circle */
    
    *(buttons[i].pflag) = !(*(buttons[i].pflag));
    if (*(buttons[i].pflag)) {
      if (buttons[i].enable) buttons[i].enable(buttons+i);
      }
    else {
      if (buttons[i].enable) buttons[i].disable(buttons+i);
      }      
    if ((buttons[i].shape==1)&& (*(buttons[i].pflag)) ) {
      selected_button=buttons+i;
      }
    set_button_flag_preferences();
    return 0;
    } /* if we are toggling something */
  } /* if we hit return */
if (action==2) { /* up */
  main_menu_up();
  }
if (action==3) { /*down */
  main_menu_down();
  }
if (action==4) { /* left */
  main_menu_left();
  }
if (action==5) { /* right */
  main_menu_right();
  }
if (action==6) {
//  disappear:
  /* this is cool - put the menu away of they click twice. or bring back if being filed. */
  if (menu_effect_mode==2) {
    menu_effect_mode=3;
    }
  }
return 0;
}






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
 ) {
*pwidth_in_pixels=width_in_pixels;
*pheight_in_pixels=height_in_pixels;
*pwidth_in_inches=width_in_inches;
*pheight_in_inches=height_in_inches;
*pheight_to_inches=height_to_inches;
*protation_in_radians=rotation_in_radians;
*prelative_width=relative_width;
*prelative_height=relative_height;
*pwidth_to_inches=width_to_inches;
*pcircle_factor=circle_factor;
*pone_color_program=one_color_program;
*pfull_color_program=full_color_program;
*pgvPositionHandle=gvPositionHandle;
*pprojectionMatrix=projectionMatrix;
*pmPositionHandle=mPositionHandle;
return 1;
}


extern int vr_button_id; // used to turn off on any click.


/* called to force a disable vr - like from a mouse click or key click */
void menu_disable_vr_mode() {
//logit("disable vr mode\n");
disable_vr_button(NULL);
if (vr_button_id) {
  *(buttons[vr_button_id].pflag) = 0;
  }
set_button_flag_preferences();
}






const display_set main_menu_display_set = {
//#define WORLD_main_menu "main_menu"
  .name = WORLD_main_menu,
  .init = main_menu_init,
  .setupGraphics = main_menu_setupGraphics,
  .finish = NULL,
  .changeScreen = NULL,
  .step = main_menu_step,
  .renderBackground = main_menu_renderBackground,
  .renderMain  = main_menu_renderFrame,
  .renderForeground = main_menu_renderForeground,
  .stepButton = NULL,
  .renderButton = NULL,
  .touchEvent = main_menu_touchEvent,
  .keyEvent = main_menu_keyEvent,
  .finishStreaming = NULL,
  };

/*
$Log: main_menu.c,v $
Revision 1.104  2019/06/30 22:36:20  hib
Cleaned up purple_string by taking out its sign and using hsin from helper
Also amke srikant1.c as an example for our new contrubutor.

Revision 1.103  2019/05/28 08:15:56  hib
Added variant o - probably only do for reggaeton because have the images.
Turn off web browsing for tv

Revision 1.102  2019/05/17 00:39:01  hib
this would be the silentradiance 1/3l version

Revision 1.101  2019/05/16 23:17:52  hib
Visual voice works on android.

Revision 1.100  2019/05/16 11:27:38  hib
wip

Revision 1.99  2019/05/14 18:04:06  hib
wip

Revision 1.98  2019/05/04 19:13:39  hib
wip

Revision 1.97  2019/05/04 09:37:14  hib
I think it was that the menu was rendered when very small - and so
by not drawing the button until the menu is up, the problem should be fixed

Revision 1.96  2019/05/03 16:28:10  hib
wip. the impact font is goofed up again.

Revision 1.95  2019/05/03 12:55:04  hib
Cleaned up justin. Added milkdrop back to linux client

Revision 1.94  2019/05/03 12:26:57  hib
from non cvs

Revision 1.93  2019/04/28 04:11:16  hib
wip - changing display_set to separate init, glsetup, and screen changed

Revision 1.92  2019/04/24 00:29:34  hib
Milkdrop has the kinks out of it.  Now for staff
Want to put the display set list up

Revision 1.91  2019/04/23 22:39:20  hib
Added projectm and resetup staff - but staff doesnt work yet
Might need more milkdrop files - our firsr C++ visualizer

Revision 1.90  2019/04/15 18:01:10  hib
OK this is linux silentradiance and resplendor version 1.3 official.

Revision 1.89  2019/04/15 11:29:48  hib
OK. NOW this is the correct version for internationalization on linux!

Revision 1.88  2019/04/15 10:40:50  hib
WORLD_ macros have now set up silent radiance to be internationalized.
It is easier to implement a system where there are separate builds,
but I could handle a weird multi-languge on one build with more trickiness.

The world_internatiolization.h file sets it up
Currently just english

Revision 1.87  2019/04/15 08:13:56  hib
cleaning international

Revision 1.86  2019/04/15 05:27:38  hib
again polishing for the international push

Revision 1.85  2019/04/14 20:05:45  hib
started work on staff.c visualizer but it crashed - need to look at justins stuff.
Moved justins multi-color programs to helper and cleaned up the code.

Revision 1.84  2019/04/04 17:43:46  hib
trying archive build 2 of 1.3CVS: ----------------------------------------------------------------------

Revision 1.83  2019/04/04 13:07:07  hib

Going to try to push it to 1.3 for apple
next - profiling

Revision 1.82  2019/03/31 19:16:54  hib
nting on the apple
VS: ----------------------------------------------------------------------

Revision 1.81  2019/03/20 15:37:44  hib

works on ios

Revision 1.80  2019/03/19 02:22:48  hib
Diamonds now have button text

Revision 1.79  2019/03/17 12:58:25  hib
Updated copyrights

Revision 1.78  2019/03/17 02:06:07  hib
Added Mathias1 for colored squares in the background

Revision 1.77  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

*/


