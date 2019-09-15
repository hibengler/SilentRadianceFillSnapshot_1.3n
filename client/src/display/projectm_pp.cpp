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







/*$Revision: 1.8 $*/

#ifdef ANDROID
#include <jni.h>
#endif




/* borrowed from porjectM_SDL_main.cpp alot */
#include "projectmSR.hpp"

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



/* all for pipeline */
#include "dlldefs.h"  
#include "event.h"
#include "fatal.h"

class PipelineContext;
#include "PCM.hpp"
#include "Renderer/Pipeline.hpp"
class Pipeline;
#include "Common.hpp"
#include <memory>


static lcontext *lc;




#define TEST_ALL_PRESETS    0
#define USE_GLES	    1
//#define OGL_DEBUG      1
#define UNLOCK_FPS 0


#if OGL_DEBUG
static void DebugLog(GLenum source,
               GLenum type,
               GLuint id,
               GLenum severity,  
               GLsizei length,  
               const GLchar* message,
               const void* userParam) {

    /*if (type != GL_DEBUG_TYPE_OTHER)*/
        {
        std::cerr << " -- \n" << "Type: " <<
           type << "; Source: " <<
           source <<"; ID: " << id << "; Severity: " <<
           severity << "\n" << message << "\n";
       }
 }
#endif
 



static GLuint gProgram;


#define NUM_FROM_CURRENT 200
#define NUM_TOTAL 401
float freqs[NUM_TOTAL*6];
float db_levels[NUM_TOTAL*6];
static char average_note_string[50];

static int number_of_samples_to_look_at;
static int from_frame;
static int to_frame;





static GLuint mvpLoc;
static GLuint positionLoc;
static GLuint colorLoc;
static glMatrix mprojectionMat1;
static glMatrix *projectionMat1=&mprojectionMat1;
//static glMatrix mprojectionMat2;
//static glMatrix *projectionMat2=&mprojectionMat2;
static glMatrix mmodelviewMat1;
static glMatrix *modelviewMat1=&mmodelviewMat1;
static glMatrix mmvpMat1;
static glMatrix *mvpMat1=&mmvpMat1;

//static glMatrix *appliedModelTranformMatrix;
static GLuint matrixAttrLoc;
 
static GLint surface_width, surface_height;


//projection
static GLfloat near = -20.0f;
static GLfloat far  = 20.0f;
static GLfloat yFOV  = 75.0f;



// from projetm - these were in main - but now statics
static int width;
static int height;
static projectMSR *app= NULL;
static float heightWidthRatio=1.f;

extern "C" {
int projectm_init(int w,int h,int drawtypw) {
    // letters init
    lc = linit_context(); // we will get our own letter context

if (app) {
  delete app;
  app = NULL;
  }
// there was a bunch of stuff to read from configuration file path and could 
// have stuff there - but I wornt try that yet - resources are tricky on 
// the phones.   Hib
    
// Settings
projectM::Settings settings;

  {
    if (make_demo_mode==0) {
        settings.windowWidth = w;
        settings.windowHeight = h;
        settings.meshX = 200;
        settings.meshY = settings.meshX * heightWidthRatio;
// width*0.66 is too much
//        settings.meshX = (w*2)/3; // was 200
//       settings.meshY = (h*2)/3;
        settings.fps   = 60;
        settings.smoothPresetDuration = 3; // seconds
        settings.presetDuration = 17; // seconds
        settings.beatSensitivity = 0.8;
        settings.aspectCorrection = 1;
        settings.shuffleEnabled = 1;
        settings.softCutRatingsEnabled = 1; // ???
        // get path to our app, use CWD for presets/fonts/etc
//        std::string base_path = SDL_GetBasePath();
        std::string base_path = "/usr/local/share/projectM/";
        settings.presetURL = base_path + "presets";
//        settings.presetURL = base_path + "presets/presets_shader_test";
        settings.menuFontURL = base_path + "fonts/Vera.ttf";
        settings.titleFontURL = base_path + "fonts/Vera.ttf";
	}
    else if (make_demo_mode==2) {
        settings.windowWidth = w;
        settings.windowHeight = h;
        settings.meshX = 1920;
        settings.meshY = 1080;
        settings.fps   = 60;
        settings.smoothPresetDuration = 1; // seconds
        settings.presetDuration = 5; // seconds
        settings.beatSensitivity = 0.8;
        settings.aspectCorrection = 1;
        settings.shuffleEnabled = 1;
        settings.softCutRatingsEnabled = 1; // ???
        // get path to our app, use CWD for presets/fonts/etc
//        std::string base_path = SDL_GetBasePath();
        std::string base_path = "/usr/local/share/projectM/";
        settings.presetURL = base_path + "presets";
//        settings.presetURL = base_path + "presets/presets_shader_test";
        settings.menuFontURL = base_path + "fonts/Vera.ttf";
        settings.titleFontURL = base_path + "fonts/Vera.ttf";
	}
    else { //make_Demo_mode=1
        settings.windowWidth = w;
        settings.windowHeight = h;
        settings.meshX = 480;
//        settings.meshY = settings.meshX * heightWidthRatio;
        settings.meshY = 480;
        settings.fps   = 60;
        settings.smoothPresetDuration = 1; // seconds
        settings.presetDuration = 5; // seconds
        settings.beatSensitivity = 0.8;
        settings.aspectCorrection = 1;
        settings.shuffleEnabled = 1;
        settings.softCutRatingsEnabled = 1; // ???
        // get path to our app, use CWD for presets/fonts/etc
//        std::string base_path = SDL_GetBasePath();
        std::string base_path = "/usr/local/share/projectM/";
        settings.presetURL = base_path + "presets";
//        settings.presetURL = base_path + "presets/presets_shader_test";
        settings.menuFontURL = base_path + "fonts/Vera.ttf";
        settings.titleFontURL = base_path + "fonts/Vera.ttf";
	}
     
       // init with settings
       app = new projectMSR(settings, 0);
       gerr("aprojectm app new");
       }
//    app->init(win, &glCtx);
      app->init();
    gerr("projectm app init");
    

#if TEST_ALL_PRESETS
    uint buildErrors = 0;
    for(unsigned int i = 0; i < app->getPlaylistSize(); i++) {
        std::cout << i << "\t" << app->getPresetName(i) << std::endl;
        app->selectPreset(i);
        if (app->getErrorLoadingCurrentPreset()) {
            buildErrors++;
        }
    }

    if (app->getPlaylistSize()) {
        fprintf(stdout, "Preset loading errors: %d/%d [%d%%]\n", buildErrors, app->getPlaylistSize(), (buildErrors*100) / app->getPlaylistSize());
    }

    delete app;

    return;
#endif
  
  //////////////////////////"permanent transforms"///////////////////

  scaleMatrix(modelviewMat1,1.0f,1.0f,1.0f);
  //translateMatrix( modelviewMat1,-0.0f,-0.4f,0.0f); //pretty good with current matrix setup
  translateMatrix( modelviewMat1,-0.f,-0.4f,0.0f); 
  rotationMatrix( modelviewMat1, 40.0f, 1.0f, 0.2f, 0.0f);
}
} //extern c


extern "C" {
int projectm_setupGraphics(int w, int h,int graphics_type) {
width=w;
height=h;
if (ran_setup_graphics_once_before==0)  {
  projectm_init(w,h,graphics_type);
  gerr("projectm_init");
  }
else {
  if (app) {
    /* could resize if we knew how. */
    app->resize(w,h);
    gerr("projectm_resize");
    }
  }
//{{{ main Model View matrix setup

    //Model View matrix setup
    //loads blank matrix
    loadIdentity( modelviewMat1 );

//}}} end modelView matrix setup
    
loadIdentity( projectionMat1 );
 
GLfloat aspect = (float)surface_width/(float)surface_height;
GLfloat height = tanf( yFOV / 360.0f * M_PI ) * near;
GLfloat width = height * aspect;
 

frustumMatrix( projectionMat1, -width, width, -height, height, near,
               far );
if ( surface_width > surface_height ){
    scaleMatrix( projectionMat1, (float)surface_height/(float)surface_width,
                 1.0f, 1.0f );
} else {
    scaleMatrix( projectionMat1, 1.0f,
                 (float)surface_width/(float)surface_height, 1.0f );
}

    return 1;
}
} // extern c

extern "C" {
void projectm_renderBackground(int screen,glMatrix *eye) {
}
}

extern "C" {
void projectm_stepButton(void) {
}
}

static Pipeline our_pipeline;
static Pipeline *alt_pipeline;

extern "C" {

void projectm_step(void) {
if (packet_summary.current_frame==-1) return;
/* kinda derived from backward worm */
int i;
int q;


//        app->addFakePCM();
app->addPCMFromPacketSummary();
// need to get the last 512 samples in


#ifdef junkfromstaffnotready  
q = 0;
back_distance = packet_summary.current_frame-packet_summary.no_gap_frame;
if (back_distance<0) back_distance += PACKET_SUMMARY_SIZE;
if (back_distance>NUM_FROM_CURRENT) back_distance = NUM_FROM_CURRENT;

from_frame=packet_summary.current_frame-back_distance;
if (from_frame <0) from_frame += PACKET_SUMMARY_SIZE;


forward_distance = packet_summary.end_frame-packet_summary.current_frame;
if (forward_distance<0) forward_distance += PACKET_SUMMARY_SIZE;

to_frame = (packet_summary.current_frame+forward_distance);
number_of_samples_to_look_at=to_frame-from_frame;
	

i = from_frame;
q=0;

float psum=0.f;
int pcount=0;
for (;i<to_frame;i++) { // and q<number_of_samples_to_look_at
/*  frames[q]=i; //need? maybe the future*/
  int index = (packet_summary.start_index + i - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
  if (index<0) index += PACKET_SUMMARY_SIZE;
  struct packet *p = packet_summary.packets+index;
  
//  logit("q %d	i %d	ind %d	end %d	h %d",q,i,index,packet_summary.end_frame,p->has_statistics);
  if (p->has_statistics) {

    float pitch=p->pitch;
    if (pitch<1.) {
      goto carry;
      }
 /* first constant is 1/log(2)*12 because ln(x)/ln(2) = log2(x) */
           /* second constant makes 440 hertz the basis instead of 256 hz */
    float plog = logf(pitch)*17.31234049066756088849f + 45.37631656229591524908;  /* first constant is 1/log(2) because ln(x)/ln(2) = log2(x) */

    psum += plog;
    pcount ++;

    freqs[q]=plog;
    db_levels[q] = p->db_level; //maybe do colors as per time and wheel it!
/*
piches are now in the range of 0 (13.75hz) to 132 (28160 - high A)
*/
    }
  else {

carry: 
    if (q==0) {
      freqs[q]=77.; /* indicates grey */
      db_levels[q] = p->db_level;
      }
    else {
      int qm1=q-1;
      freqs[q]=freqs[qm1];
      psum += freqs[q];
      pcount ++;
      }
     
    }
  q++;
  } // for 

/* figure out if we are namaste */
namaste_432=0;
avg_note=-1;
if (pcount) {
  float avgf440 = psum / (float)(pcount);
  float avgf432 = avgf440 -0.31766653633429271652f; // namaste freq 
  
  int avg440 = (int)avgf440;  
  int avg432 = (int)avgf432;
  float var440 = avgf440 - (float)avg440;
  float var432 = avgf432 - (float)avg432;
  if (var432 <var440) {
    namaste_432=1;
    avg_note = avg432;
    }  
  else {avg_note=avg440;}
  } 


/* make current average shown */
if (avg_note!= -1) {
  int abcdef =   avg_note % 12;
  int octave=    avg_note/12;
  if (octave>12) octave=12;
//#define WORLD_display_projectm_ ""
  char *namastate=WORLD_display_projectm_;
  if (namaste_432) {
//#define WORLD__432_Namaste " 432 Namaste"
    namastate=WORLD__432_Namaste;
    }
//#define WORLD_display_projectm__pct_s_pct_s_pct_s "%s%s%s"
  sprintf(average_note_string,WORLD_display_projectm__pct_s_pct_s_pct_s,avg_note_names[abcdef],
      avg_octave_names[octave],namastate);
        
  }

#endif  
// ^^^ junk


alt_pipeline = app->renderFrameOnlyPass1(&our_pipeline);
}




void projectm_renderFrame(int screen,glMatrix *eye) {    
if (packet_summary.current_frame==-1) return;
    if (app->done) return;
    if (packet_summary.current_frame==-1) return;
/*
int p =       letters_opengles_program_handle();
int clrhandle=letters_opengles_color_handle();
*/
/*??? might need help for apple */  
app->renderFrameOnlyPass2(alt_pipeline,
                    base_viewport_x1,base_viewport_y1,
                                          base_viewport_eye);

if (base_viewport_eye==1) {
  app->renderFrameEndOnSeparatePasses(alt_pipeline);
  }
}
}




extern "C" {
void projectm_renderButton(int program,int mColorHandle,int menu_position_handle,glMatrix *MVPMatrix,int screen,glMatrix *eye) {
glUseProgram(program);
    glLineWidth(1.f);
    
    if (packet_summary.current_frame==-1) return;

    int try_forward=4;

    int to_frame = packet_summary.current_frame+try_forward;
    if (to_frame >packet_summary.end_frame) to_frame=packet_summary.end_frame;
    
    int from_frame=to_frame-9; 
    if (from_frame<packet_summary.no_gap_frame) from_frame=packet_summary.no_gap_frame;
    
    int f;
    int flag=0;
    GLfloat x,y;
    GLfloat vertices[723];
    x=0.f;y=0.f;
    
    glEnableVertexAttribArray(menu_position_handle);
    checkGlError("glEnableVertexAttribArray(men");
    
    
    glVertexAttribPointer(menu_position_handle, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    checkGlError("glVertexAttribPointer");
    
    
//    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, (GLfloat *)(&MVPMatrix));
    
    
    


    
    {
      glUseProgram(letters_opengles_program_handle());
      checkGlError("koutds");
      float scale = 0.5;
      float x=-0.9f;
      float y=0.15f;
      glUniform4f(letters_opengles_color_handle(), 1.0f,1.0f,0.7f,1.0f);
      lc->needed_points->line_width=0.128f;

      letters_out(lc,scale,x,y,0.f,"PMD");
      }
    
}
}


extern "C" {
void projectm_renderForeground(int screen,glMatrix *eye) {
}
}

extern "C" {
void projectm_touchEvent(int action,float x,float y) {
if (action==1) {
  push_display_set(2);
  }
}
}


/* this is in projectm.c cause initialization is c99 and c++ is c89
But here it is for reference
const display_set projectm_display_set = {
  .name = "Projectm",
  .setupGraphics = NULL,
  .init = projectm_init,
  .setupGL = projectm_setupGL,
  .changeScreen = projectm_changeScreen,
  .step = projectm_step,
  .renderBackground = projectm_renderBackground,
  .renderMain  = projectm_renderFrame,
  .renderForeground = projectm_renderForeground,
  .renderButton = projectm_renderButton,
  .touchEvent = default_touchEvent,
  .keyEvent = default_keyEvent,
  .finishStreaming = NULL
  };
*/

/*
$Log: projectm_pp.cpp,v $
Revision 1.8  2019/08/25 05:07:08  hib
Finished linux client for leleradiance. Tag is lele

Revision 1.7  2019/06/07 02:45:04  hib
wip prime

Revision 1.6  2019/05/20 12:45:47  hib
helped idenify an error - with project m
makde gerr which ios a better way to trach wtf

Revision 1.5  2019/05/03 20:43:58  hib
Cleaned up alot and hopefully the impact font as well

Revision 1.4  2019/04/28 12:47:56  hib
Ready to try again at android site.

Revision 1.3  2019/04/28 07:19:38  hib
wip

Revision 1.2  2019/04/24 00:29:34  hib
Milkdrop has the kinks out of it.  Now for staff
Want to put the display set list up

Revision 1.1  2019/04/23 22:47:11  hib
wip

Revision 1.1  2019/04/23 20:06:40  hib
adding projectm


*/

