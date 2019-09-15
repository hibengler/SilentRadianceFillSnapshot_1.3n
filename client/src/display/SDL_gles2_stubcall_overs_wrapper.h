/* generated with convert_gl2_h_into_gles2_stubcall_overs_h.sno 
  Generator by Hibbard M. Engler
  */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#include <SDL2/SDL_test_common.h>

#if defined(__IPHONEOS__) || defined(__ANDROID__) || defined(__EMSCRIPTEN__) || defined(__NACL__) \
  || defined(__WINDOWS__) || defined(__LINUX__)
  #define HAVE_OPENGLES2
 #endif

#ifdef HAVE_OPENGLES2

#include <SDL2/SDL_opengles2.h>

typedef struct GLES2_Context
{
#define SDL_PROC(ret,func,params) ret (APIENTRY *func) params;
#include "SDL_gles2funcs.h"
#undef SDL_PROC
} GLES2_Context;

#endif // HAVE_OPENGLES2

extern int LoadContext(GLES2_Context * data);




