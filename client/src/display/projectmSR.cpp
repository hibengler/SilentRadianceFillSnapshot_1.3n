//
// projectmSR.cpp
//
//  pmSDL.cpp of  SDLprojectM
//  Created by Mischa Spiegelmock on 2017-09-18.
//
// heavily modified by Hibbard Engler for Silent Radiance

#include "projectmSR.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Renderer/ShaderEngine.hpp"
extern "C" {
#include "glue.h"
#include "helper.h"
#include "packet_summary.h"


#ifdef ANDROID
#define glGenVertexArrays(a,b) glGenVertexArraysOES(a,b)
#define glBindVertexArray(a) glBindVertexArrayOES(a,b)
#endif

}

#ifdef seethistounderstand
void projectMSR::audioInputCallbackF32(void *userdata, unsigned char *stream, int len) {
    projectMSR *app = (projectMSR *) userdata;
    //    printf("LEN: %i\n", len);
    // stream is (i think) samples*channels floats (native byte order) of len BYTES
    app->pcm()->addPCMfloat((float *)stream, len/sizeof(float));
}

void projectMSR::audioInputCallbackS16(void *userdata, unsigned char *stream, int len) {
    //    printf("LEN: %i\n", len);
    projectMSR *app = (projectMSR *) userdata;
    short pcm16[2][512];

    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < app->audioChannelsCount; j++) {
            pcm16[j][i] = stream[i+j];
        }
    }
    app->pcm()->addPCM16(pcm16);
}
#endif


int projectMSR::openAudioInput() {

    // requested format
//    want.freq = 48000;
//    want.format = AUDIO_F32;  // float
//    want.channels = 2;
//    want.samples = 512;
//    want.callback = projectMSR::audioInputCallbackF32;
//    want.userdata = this;


    audioChannelsCount = 2;
    audioSampleRate = 48000;
    audioSampleCount = 240;
//    audioFormat = 0; // have.format; from SDL?
//    audioInputDevice = 0;
    return 1;
}

void projectMSR::beginAudioCapture() {
    // allocate a buffer to store PCM data for feeding in
//    SDL_PauseAudioDevice(audioDeviceID, false);
}

void projectMSR::endAudioCapture() {
//    SDL_PauseAudioDevice(audioDeviceID, true);
}

void projectMSR::maximize() {
//    resize(dm.w, dm.h);
}

void projectMSR::toggleFullScreen() {
}


void projectMSR::addFakePCM() {
    int i;
    short pcm_data[2][512];
    /** Produce some fake PCM data to stuff into projectM */
    for ( i = 0 ; i < 512 ; i++ ) {
        if ( i % 2 == 0 ) {
            pcm_data[0][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
            pcm_data[1][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
        } else {
            pcm_data[0][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
            pcm_data[1][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
        }
        if ( i % 2 == 1 ) {
            pcm_data[0][i] = -pcm_data[0][i];
            pcm_data[1][i] = -pcm_data[1][i];
        }
    }

    /** Add the waveform data */
    pcm()->addPCM16(pcm_data); 
}

void projectMSR::addPCMFromPacketSummary() {
    short pcm_data[2][512];
    memset((void *)pcm_data,0,sizeof(pcm_data));
    
    if (packet_summary.current_frame==-1) {
      /** Add the waveform data */
      pcm()->addPCM16(pcm_data); 
      return;
    }


    int try_forward=4;
    int to_frame = packet_summary.current_frame+try_forward;
    if (to_frame >packet_summary.end_frame) to_frame=packet_summary.end_frame;
    
    int from_frame=to_frame-9; 
    if (from_frame<packet_summary.no_gap_frame) from_frame=packet_summary.no_gap_frame;
    
    int f;
    int n=0;
    int number_of_frames = to_frame-from_frame+1;
    if (number_of_frames>10) {
        number_of_frames=0;
        from_frame=to_frame+1;
        }
    int j=0;
    for (f=from_frame;f<=to_frame;f++) {
      int index = (packet_summary.start_index + f - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
      if (index<0) index += PACKET_SUMMARY_SIZE;
      struct packet *p = packet_summary.packets+index;
      int i;
      if (p->has_music_samples) {
        int max=512-j;
	if (max>240) max=240;
	if (max<=0) break; // quit if we are done.
	max=max+max;
	for (i=0;i<max;i+=2) {
	  pcm_data[0][j]=p->sample[i];
	  pcm_data[1][j]=p->sample[i+1];
	  j++;
          } /* for each frame we want to draw */
	}
      else {
        fprintf(stderr,"miss\n");
        break; // quit if we are missing samples
	}
      }
    pcm()->addPCM16(pcm_data); 

}

void projectMSR::resize(unsigned int width_, unsigned int height_) {
// probably need the resetGL in the setupGraphics app
    width = width_;
    height = height_;
    projectM_resetGL(width, height);
}


void projectMSR::renderFrame() {
//    glClearColor( 0.0, 0.0, 0.0, 0.0 );
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projectM::renderFrame();

    if (renderToTexture) {
        renderTexture();
    }

//    SDL_GL_SwapWindow(win);
}

projectMSR::projectMSR(Settings settings, int flags) : projectM(settings, flags) {
    width = getWindowWidth();
    height = getWindowHeight();
    done = 0;
    isFullScreen = false;
}

projectMSR::projectMSR(std::string config_file, int flags) : projectM(config_file, flags) {
    width = getWindowWidth();
    height = getWindowHeight();
    done = 0;
    isFullScreen = false;
}

void projectMSR::init(const bool _renderToTexture) {
    projectM_resetGL(width, height);
    gerr("projectm_init_resetgl");
    // are we rendering to a texture?
    renderToTexture = _renderToTexture;
    if (renderToTexture) {
        programID = ShaderEngine::CompileShaderProgram(ShaderEngine::v2f_c4f_t2f_vert, ShaderEngine::v2f_c4f_t2f_frag, "v2f_c4f_t2f");
        textureID = projectM::initRenderToTexture();

        float points[16] = {
            -0.8, -0.8,
            0.0,    0.0,

            -0.8, 0.8,
            0.0,   1.0,

            0.8, -0.8,
            1.0,    0.0,

            0.8, 0.8,
            1.0,    1.0,
        };

        gerr("projectm_compshp");
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        gerr("projectm_glbindb");
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, points, GL_DYNAMIC_DRAW);
        gerr("projectm_bufferd");

        glGenVertexArrays(1, &m_vao);
	gerr("projectm_init_glgenvert");
        glBindVertexArray(m_vao);
	gerr("projectm_init_gbindvert");

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0); // Positions
	gerr("projectm_vap");

        glDisableVertexAttribArray(1);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2)); // Textures
	gerr("projectm_vap2");

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
	gerr("projectm_bb3");
    }
}


std::string projectMSR::getActivePresetName()
{
    unsigned int index = 0;
    if (selectedPresetIndex(index)) {
        return getPresetName(index);
    }
    return std::string();
}


void projectMSR::renderTexture() {
    static int frame = 0;
    frame++;

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, getWindowWidth(), getWindowHeight());

    glUseProgram(programID);

    glUniform1i(glGetUniformLocation(programID, "texture_sampler"), 0);

    glm::mat4 mat_proj = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 2.0f, 10.0f);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 mat_model = glm::mat4(1.0f);
    mat_model = glm::translate(mat_model, glm::vec3(cos(frame*0.023),
                                                    cos(frame*0.017),
                                                    -5+sin(frame*0.022)*2));
    mat_model = glm::rotate(mat_model, glm::radians((float) sin(frame*0.0043)*360),
                            glm::vec3(sin(frame*0.0017),
                                      sin(frame *0.0032),
                                      1));

    glm::mat4 mat_transf = glm::mat4(1.0f);
    mat_transf = mat_proj * mat_model;

    glUniformMatrix4fv(glGetUniformLocation(programID, "vertex_transformation"), 1, GL_FALSE, glm::value_ptr(mat_transf));

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

    glBindVertexArray(m_vao);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
}

void projectMSR::presetSwitchedEvent(bool isHardCut, size_t index) const {
    std::string presetName = getPresetName(index);
    fprintf(stderr,"Displaying preset: %s\n", presetName.c_str());
}
