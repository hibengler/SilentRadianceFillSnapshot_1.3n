#ifndef SOUNDIO_PORTAUDIO_H
#define SOUNDIO_PORTAUDIO_H

#include "glue.h"
extern int soundin_init();
extern int soundin_process_aubio();
extern int soundin_finish();

extern float soundin_factor; // usually 1


#endif
