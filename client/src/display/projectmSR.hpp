//
//  projectmSR.hpp
// derived frrom pmSDL.hpp - part of SDLprojectM
//  Created by Mischa Spiegelmock on 2017-09-18.
//
// modified heavily by Hibbard M. Engler for Silent Radiance
//

#ifndef projectmSR_hpp
#define projectmSR_hpp
#include "glue.h"

#include "projectM-opengl.h"
#ifdef ANDROID
#include "projectM.hpp"
#else
#include <projectM.hpp>
#endif


#include <iostream>
#include <sys/stat.h>


// DATADIR_PATH should be set by the root Makefile if this is being
// built with autotools.
#ifndef DATADIR_PATH
    #ifdef DEBUG
        #define DATADIR_PATH "."
        #warning "DATADIR_PATH is not defined - falling back to ./"
    #else
        #define DATADIR_PATH "/usr/local/share/projectM"
#ifndef WIN32
        #warning "DATADIR_PATH is not defined - falling back to /usr/local/share/projectM"
#endif /** WIN32 */
    #endif
#endif


class projectMSR : public projectM {
public:
    bool done;

    projectMSR(Settings settings, int flags);
    projectMSR(std::string config_file, int flags);
    void init(const bool renderToTexture = false);
    int openAudioInput();
    void beginAudioCapture();
    void endAudioCapture();
    void toggleFullScreen();
    void resize(unsigned int width, unsigned int height);
    void renderFrame();
    void pollEvent();
    void maximize();
    std::string getActivePresetName();
    void addFakePCM();
    void addPCMFromPacketSummary();
    virtual void presetSwitchedEvent(bool isHardCut, size_t index) const;

private:
    bool isFullScreen;
    unsigned int width, height;
    bool renderToTexture;
    GLuint programID = 0;
    GLuint m_vbo = 0;
    GLuint m_vao = 0;
    GLuint textureID = 0;

    // audio input device characteristics
    unsigned short audioChannelsCount;
    unsigned short audioSampleRate;
    unsigned short audioSampleCount;

    void renderTexture();
};

#endif /* projectmSR_hpp */
