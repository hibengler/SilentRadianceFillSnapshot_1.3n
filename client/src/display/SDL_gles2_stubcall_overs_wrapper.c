/* generated with convert_gles2funcs_into_gles_wrapper_overs_c.sno 
  Generator by Hibbard M. Engler
  */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif


#define HAVE_OPENGLES2

#include "SDL_gles2_stubcall_overs_wrapper.h"

#ifdef HAVE_OPENGLES2

static GLES2_Context *my_context=NULL;

int LoadContext(GLES2_Context * data)
{
#if SDL_VIDEO_DRIVER_UIKIT
#define __SDL_NOGETPROCADDR__
#elif SDL_VIDEO_DRIVER_ANDROID
#define __SDL_NOGETPROCADDR__
#elif SDL_VIDEO_DRIVER_PANDORA
#define __SDL_NOGETPROCADDR__
#endif

#if defined __SDL_NOGETPROCADDR__
#define SDL_PROC(ret,func,params) data->func=func;
#else
//#define WORLD_Couldn_qt_t_load_GLES2_function__pct_s_colon___pct_s "Couldn't load GLES2 function %s: %s"
#define SDL_PROC(ret,func,params) \
    do { \
        data->func = SDL_GL_GetProcAddress(#func); \
        if ( ! data->func ) { \
            return SDL_SetError(WORLD_Couldn_qt_t_load_GLES2_function__pct_s_colon___pct_s, #func, SDL_GetError()); \
        } \
    } while ( 0 );
#endif /* __SDL_NOGETPROCADDR__ */
#include "SDL_gles2funcs.h"
#undef SDL_PROC
    my_context = data;
    return 0;
}


#pragma GCC diagnostic ignored "-Wattributes"


__declspec(dllimport) void GL_APIENTRY  glActiveTexture (GLenum texture)
{
    my_context->glActiveTexture(texture);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glActiveTexture) (GLenum texture) = &glActiveTexture;

__declspec(dllimport) void GL_APIENTRY  glAttachShader (GLuint program, GLuint shader)
{
    my_context->glAttachShader(program,shader);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glAttachShader) (GLuint program, GLuint shader) = &glAttachShader;

__declspec(dllimport) void GL_APIENTRY  glBindAttribLocation (GLuint program, GLuint index, const GLchar *name)
{
    my_context->glBindAttribLocation(program,index,name);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBindAttribLocation) (GLuint program, GLuint index, const GLchar *name) = &glBindAttribLocation;

__declspec(dllimport) void GL_APIENTRY  glBindBuffer (GLenum target, GLuint buffer)
{
    my_context->glBindBuffer(target,buffer);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBindBuffer) (GLenum target, GLuint buffer) = &glBindBuffer;

__declspec(dllimport) void GL_APIENTRY  glBindFramebuffer (GLenum target, GLuint framebuffer)
{
    my_context->glBindFramebuffer(target,framebuffer);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBindFramebuffer) (GLenum target, GLuint framebuffer) = &glBindFramebuffer;

__declspec(dllimport) void GL_APIENTRY  glBindRenderbuffer (GLenum target, GLuint renderbuffer)
{
    my_context->glBindRenderbuffer(target,renderbuffer);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBindRenderbuffer) (GLenum target, GLuint renderbuffer) = &glBindRenderbuffer;

__declspec(dllimport) void GL_APIENTRY  glBindTexture (GLenum target, GLuint texture)
{
    my_context->glBindTexture(target,texture);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBindTexture) (GLenum target, GLuint texture) = &glBindTexture;

__declspec(dllimport) void GL_APIENTRY  glBlendColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    my_context->glBlendColor(red,green,blue,alpha);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBlendColor) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = &glBlendColor;

__declspec(dllimport) void GL_APIENTRY  glBlendEquation (GLenum mode)
{
    my_context->glBlendEquation(mode);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBlendEquation) (GLenum mode) = &glBlendEquation;

__declspec(dllimport) void GL_APIENTRY  glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha)
{
    my_context->glBlendEquationSeparate(modeRGB,modeAlpha);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBlendEquationSeparate) (GLenum modeRGB, GLenum modeAlpha) = &glBlendEquationSeparate;

__declspec(dllimport) void GL_APIENTRY  glBlendFunc (GLenum sfactor, GLenum dfactor)
{
    my_context->glBlendFunc(sfactor,dfactor);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBlendFunc) (GLenum sfactor, GLenum dfactor) = &glBlendFunc;

__declspec(dllimport) void GL_APIENTRY  glBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    my_context->glBlendFuncSeparate(sfactorRGB,dfactorRGB,sfactorAlpha,dfactorAlpha);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBlendFuncSeparate) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) = &glBlendFuncSeparate;

__declspec(dllimport) void GL_APIENTRY  glBufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
    my_context->glBufferData(target,size,data,usage);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBufferData) (GLenum target, GLsizeiptr size, const void *data, GLenum usage) = &glBufferData;

__declspec(dllimport) void GL_APIENTRY  glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{
    my_context->glBufferSubData(target,offset,size,data);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBufferSubData) (GLenum target, GLintptr offset, GLsizeiptr size, const void *data) = &glBufferSubData;

__declspec(dllimport) GLenum GL_APIENTRY  glCheckFramebufferStatus (GLenum target)
{
    return my_context->glCheckFramebufferStatus(target);
}
 __declspec(dllexport) GLenum GL_APIENTRY  (*__imp_glCheckFramebufferStatus) (GLenum target) = &glCheckFramebufferStatus;

__declspec(dllimport) void GL_APIENTRY  glClear (GLbitfield mask)
{
    my_context->glClear(mask);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glClear) (GLbitfield mask) = &glClear;

__declspec(dllimport) void GL_APIENTRY  glClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    my_context->glClearColor(red,green,blue,alpha);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glClearColor) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = &glClearColor;

__declspec(dllimport) void GL_APIENTRY  glClearDepthf (GLfloat d)
{
    my_context->glClearDepthf(d);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glClearDepthf) (GLfloat d) = &glClearDepthf;

__declspec(dllimport) void GL_APIENTRY  glClearStencil (GLint s)
{
    my_context->glClearStencil(s);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glClearStencil) (GLint s) = &glClearStencil;

__declspec(dllimport) void GL_APIENTRY  glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    my_context->glColorMask(red,green,blue,alpha);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glColorMask) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) = &glColorMask;

__declspec(dllimport) void GL_APIENTRY  glCompileShader (GLuint shader)
{
    my_context->glCompileShader(shader);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glCompileShader) (GLuint shader) = &glCompileShader;

__declspec(dllimport) void GL_APIENTRY  glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
{
    my_context->glCompressedTexImage2D(target,level,internalformat,width,height,border,imageSize,data);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glCompressedTexImage2D) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data) = &glCompressedTexImage2D;

__declspec(dllimport) void GL_APIENTRY  glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
{
    my_context->glCompressedTexSubImage2D(target,level,xoffset,yoffset,width,height,format,imageSize,data);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glCompressedTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data) = &glCompressedTexSubImage2D;

__declspec(dllimport) void GL_APIENTRY  glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    my_context->glCopyTexImage2D(target,level,internalformat,x,y,width,height,border);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glCopyTexImage2D) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) = &glCopyTexImage2D;

__declspec(dllimport) void GL_APIENTRY  glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    my_context->glCopyTexSubImage2D(target,level,xoffset,yoffset,x,y,width,height);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glCopyTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) = &glCopyTexSubImage2D;

__declspec(dllimport) GLuint GL_APIENTRY  glCreateProgram (void)
{
    return my_context->glCreateProgram();
}
 __declspec(dllexport) GLuint GL_APIENTRY  (*__imp_glCreateProgram) (void) = &glCreateProgram;

__declspec(dllimport) GLuint GL_APIENTRY  glCreateShader (GLenum type)
{
    return my_context->glCreateShader(type);
}
 __declspec(dllexport) GLuint GL_APIENTRY  (*__imp_glCreateShader) (GLenum type) = &glCreateShader;

__declspec(dllimport) void GL_APIENTRY  glCullFace (GLenum mode)
{
    my_context->glCullFace(mode);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glCullFace) (GLenum mode) = &glCullFace;

__declspec(dllimport) void GL_APIENTRY  glDeleteBuffers (GLsizei n, const GLuint *buffers)
{
    my_context->glDeleteBuffers(n,buffers);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDeleteBuffers) (GLsizei n, const GLuint *buffers) = &glDeleteBuffers;

__declspec(dllimport) void GL_APIENTRY  glDeleteFramebuffers (GLsizei n, const GLuint *framebuffers)
{
    my_context->glDeleteFramebuffers(n,framebuffers);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDeleteFramebuffers) (GLsizei n, const GLuint *framebuffers) = &glDeleteFramebuffers;

__declspec(dllimport) void GL_APIENTRY  glDeleteProgram (GLuint program)
{
    my_context->glDeleteProgram(program);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDeleteProgram) (GLuint program) = &glDeleteProgram;

__declspec(dllimport) void GL_APIENTRY  glDeleteRenderbuffers (GLsizei n, const GLuint *renderbuffers)
{
    my_context->glDeleteRenderbuffers(n,renderbuffers);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDeleteRenderbuffers) (GLsizei n, const GLuint *renderbuffers) = &glDeleteRenderbuffers;

__declspec(dllimport) void GL_APIENTRY  glDeleteShader (GLuint shader)
{
    my_context->glDeleteShader(shader);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDeleteShader) (GLuint shader) = &glDeleteShader;

__declspec(dllimport) void GL_APIENTRY  glDeleteTextures (GLsizei n, const GLuint *textures)
{
    my_context->glDeleteTextures(n,textures);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDeleteTextures) (GLsizei n, const GLuint *textures) = &glDeleteTextures;

__declspec(dllimport) void GL_APIENTRY  glDepthFunc (GLenum func)
{
    my_context->glDepthFunc(func);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDepthFunc) (GLenum func) = &glDepthFunc;

__declspec(dllimport) void GL_APIENTRY  glDepthMask (GLboolean flag)
{
    my_context->glDepthMask(flag);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDepthMask) (GLboolean flag) = &glDepthMask;

__declspec(dllimport) void GL_APIENTRY  glDepthRangef (GLfloat n, GLfloat f)
{
    my_context->glDepthRangef(n,f);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDepthRangef) (GLfloat n, GLfloat f) = &glDepthRangef;

__declspec(dllimport) void GL_APIENTRY  glDetachShader (GLuint program, GLuint shader)
{
    my_context->glDetachShader(program,shader);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDetachShader) (GLuint program, GLuint shader) = &glDetachShader;

__declspec(dllimport) void GL_APIENTRY  glDisable (GLenum cap)
{
    my_context->glDisable(cap);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDisable) (GLenum cap) = &glDisable;

__declspec(dllimport) void GL_APIENTRY  glDisableVertexAttribArray (GLuint index)
{
    my_context->glDisableVertexAttribArray(index);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDisableVertexAttribArray) (GLuint index) = &glDisableVertexAttribArray;

__declspec(dllimport) void GL_APIENTRY  glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
    my_context->glDrawArrays(mode,first,count);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDrawArrays) (GLenum mode, GLint first, GLsizei count) = &glDrawArrays;

__declspec(dllimport) void GL_APIENTRY  glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices)
{
    my_context->glDrawElements(mode,count,type,indices);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDrawElements) (GLenum mode, GLsizei count, GLenum type, const void *indices) = &glDrawElements;

__declspec(dllimport) void GL_APIENTRY  glEnable (GLenum cap)
{
    my_context->glEnable(cap);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glEnable) (GLenum cap) = &glEnable;

__declspec(dllimport) void GL_APIENTRY  glEnableVertexAttribArray (GLuint index)
{
    my_context->glEnableVertexAttribArray(index);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glEnableVertexAttribArray) (GLuint index) = &glEnableVertexAttribArray;

__declspec(dllimport) void GL_APIENTRY  glFinish (void)
{
    my_context->glFinish();
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glFinish) (void) = &glFinish;

__declspec(dllimport) void GL_APIENTRY  glFlush (void)
{
    my_context->glFlush();
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glFlush) (void) = &glFlush;

__declspec(dllimport) void GL_APIENTRY  glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    my_context->glFramebufferRenderbuffer(target,attachment,renderbuffertarget,renderbuffer);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glFramebufferRenderbuffer) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) = &glFramebufferRenderbuffer;

__declspec(dllimport) void GL_APIENTRY  glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    my_context->glFramebufferTexture2D(target,attachment,textarget,texture,level);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glFramebufferTexture2D) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) = &glFramebufferTexture2D;

__declspec(dllimport) void GL_APIENTRY  glFrontFace (GLenum mode)
{
    my_context->glFrontFace(mode);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glFrontFace) (GLenum mode) = &glFrontFace;

__declspec(dllimport) void GL_APIENTRY  glGenBuffers (GLsizei n, GLuint *buffers)
{
    my_context->glGenBuffers(n,buffers);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGenBuffers) (GLsizei n, GLuint *buffers) = &glGenBuffers;

__declspec(dllimport) void GL_APIENTRY  glGenerateMipmap (GLenum target)
{
    my_context->glGenerateMipmap(target);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGenerateMipmap) (GLenum target) = &glGenerateMipmap;

__declspec(dllimport) void GL_APIENTRY  glGenFramebuffers (GLsizei n, GLuint *framebuffers)
{
    my_context->glGenFramebuffers(n,framebuffers);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGenFramebuffers) (GLsizei n, GLuint *framebuffers) = &glGenFramebuffers;

__declspec(dllimport) void GL_APIENTRY  glGenRenderbuffers (GLsizei n, GLuint *renderbuffers)
{
    my_context->glGenRenderbuffers(n,renderbuffers);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGenRenderbuffers) (GLsizei n, GLuint *renderbuffers) = &glGenRenderbuffers;

__declspec(dllimport) void GL_APIENTRY  glGenTextures (GLsizei n, GLuint *textures)
{
    my_context->glGenTextures(n,textures);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGenTextures) (GLsizei n, GLuint *textures) = &glGenTextures;

__declspec(dllimport) void GL_APIENTRY  glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    my_context->glGetActiveAttrib(program,index,bufSize,length,size,type,name);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetActiveAttrib) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name) = &glGetActiveAttrib;

__declspec(dllimport) void GL_APIENTRY  glGetActiveUniform (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    my_context->glGetActiveUniform(program,index,bufSize,length,size,type,name);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetActiveUniform) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name) = &glGetActiveUniform;

__declspec(dllimport) void GL_APIENTRY  glGetAttachedShaders (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders)
{
    my_context->glGetAttachedShaders(program,maxCount,count,shaders);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetAttachedShaders) (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders) = &glGetAttachedShaders;

__declspec(dllimport) GLint GL_APIENTRY  glGetAttribLocation (GLuint program, const GLchar *name)
{
    return my_context->glGetAttribLocation(program,name);
}
 __declspec(dllexport) GLint GL_APIENTRY  (*__imp_glGetAttribLocation) (GLuint program, const GLchar *name) = &glGetAttribLocation;

__declspec(dllimport) void GL_APIENTRY  glGetBooleanv (GLenum pname, GLboolean *data)
{
    my_context->glGetBooleanv(pname,data);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetBooleanv) (GLenum pname, GLboolean *data) = &glGetBooleanv;

__declspec(dllimport) void GL_APIENTRY  glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params)
{
    my_context->glGetBufferParameteriv(target,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetBufferParameteriv) (GLenum target, GLenum pname, GLint *params) = &glGetBufferParameteriv;

__declspec(dllimport) GLenum GL_APIENTRY  glGetError (void)
{
    return my_context->glGetError();
}
 __declspec(dllexport) GLenum GL_APIENTRY  (*__imp_glGetError) (void) = &glGetError;

__declspec(dllimport) void GL_APIENTRY  glGetFloatv (GLenum pname, GLfloat *data)
{
    my_context->glGetFloatv(pname,data);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetFloatv) (GLenum pname, GLfloat *data) = &glGetFloatv;

__declspec(dllimport) void GL_APIENTRY  glGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint *params)
{
    my_context->glGetFramebufferAttachmentParameteriv(target,attachment,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetFramebufferAttachmentParameteriv) (GLenum target, GLenum attachment, GLenum pname, GLint *params) = &glGetFramebufferAttachmentParameteriv;

__declspec(dllimport) void GL_APIENTRY  glGetIntegerv (GLenum pname, GLint *data)
{
    my_context->glGetIntegerv(pname,data);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetIntegerv) (GLenum pname, GLint *data) = &glGetIntegerv;

__declspec(dllimport) void GL_APIENTRY  glGetProgramiv (GLuint program, GLenum pname, GLint *params)
{
    my_context->glGetProgramiv(program,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetProgramiv) (GLuint program, GLenum pname, GLint *params) = &glGetProgramiv;

__declspec(dllimport) void GL_APIENTRY  glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    my_context->glGetProgramInfoLog(program,bufSize,length,infoLog);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetProgramInfoLog) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) = &glGetProgramInfoLog;

__declspec(dllimport) void GL_APIENTRY  glGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint *params)
{
    my_context->glGetRenderbufferParameteriv(target,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetRenderbufferParameteriv) (GLenum target, GLenum pname, GLint *params) = &glGetRenderbufferParameteriv;

__declspec(dllimport) void GL_APIENTRY  glGetShaderiv (GLuint shader, GLenum pname, GLint *params)
{
    my_context->glGetShaderiv(shader,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetShaderiv) (GLuint shader, GLenum pname, GLint *params) = &glGetShaderiv;

__declspec(dllimport) void GL_APIENTRY  glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    my_context->glGetShaderInfoLog(shader,bufSize,length,infoLog);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetShaderInfoLog) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) = &glGetShaderInfoLog;

__declspec(dllimport) void GL_APIENTRY  glGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision)
{
    my_context->glGetShaderPrecisionFormat(shadertype,precisiontype,range,precision);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetShaderPrecisionFormat) (GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision) = &glGetShaderPrecisionFormat;

__declspec(dllimport) void GL_APIENTRY  glGetShaderSource (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source)
{
    my_context->glGetShaderSource(shader,bufSize,length,source);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetShaderSource) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source) = &glGetShaderSource;

__declspec(dllimport) const GLubyte * GL_APIENTRY  glGetString (GLenum name)
{
    return my_context->glGetString(name);
}
 __declspec(dllexport) const GLubyte * GL_APIENTRY  (*__imp_glGetString) (GLenum name) = &glGetString;

__declspec(dllimport) void GL_APIENTRY  glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
    my_context->glGetTexParameterfv(target,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetTexParameterfv) (GLenum target, GLenum pname, GLfloat *params) = &glGetTexParameterfv;

__declspec(dllimport) void GL_APIENTRY  glGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
{
    my_context->glGetTexParameteriv(target,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetTexParameteriv) (GLenum target, GLenum pname, GLint *params) = &glGetTexParameteriv;

__declspec(dllimport) void GL_APIENTRY  glGetUniformfv (GLuint program, GLint location, GLfloat *params)
{
    my_context->glGetUniformfv(program,location,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetUniformfv) (GLuint program, GLint location, GLfloat *params) = &glGetUniformfv;

__declspec(dllimport) void GL_APIENTRY  glGetUniformiv (GLuint program, GLint location, GLint *params)
{
    my_context->glGetUniformiv(program,location,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetUniformiv) (GLuint program, GLint location, GLint *params) = &glGetUniformiv;

__declspec(dllimport) GLint GL_APIENTRY  glGetUniformLocation (GLuint program, const GLchar *name)
{
    return my_context->glGetUniformLocation(program,name);
}
 __declspec(dllexport) GLint GL_APIENTRY  (*__imp_glGetUniformLocation) (GLuint program, const GLchar *name) = &glGetUniformLocation;

__declspec(dllimport) void GL_APIENTRY  glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat *params)
{
    my_context->glGetVertexAttribfv(index,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetVertexAttribfv) (GLuint index, GLenum pname, GLfloat *params) = &glGetVertexAttribfv;

__declspec(dllimport) void GL_APIENTRY  glGetVertexAttribiv (GLuint index, GLenum pname, GLint *params)
{
    my_context->glGetVertexAttribiv(index,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetVertexAttribiv) (GLuint index, GLenum pname, GLint *params) = &glGetVertexAttribiv;

__declspec(dllimport) void GL_APIENTRY  glGetVertexAttribPointerv (GLuint index, GLenum pname, void **pointer)
{
    my_context->glGetVertexAttribPointerv(index,pname,pointer);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGetVertexAttribPointerv) (GLuint index, GLenum pname, void **pointer) = &glGetVertexAttribPointerv;

__declspec(dllimport) void GL_APIENTRY  glHint (GLenum target, GLenum mode)
{
    my_context->glHint(target,mode);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glHint) (GLenum target, GLenum mode) = &glHint;

__declspec(dllimport) GLboolean GL_APIENTRY  glIsBuffer (GLuint buffer)
{
    return my_context->glIsBuffer(buffer);
}
 __declspec(dllexport) GLboolean GL_APIENTRY  (*__imp_glIsBuffer) (GLuint buffer) = &glIsBuffer;

__declspec(dllimport) GLboolean GL_APIENTRY  glIsEnabled (GLenum cap)
{
    return my_context->glIsEnabled(cap);
}
 __declspec(dllexport) GLboolean GL_APIENTRY  (*__imp_glIsEnabled) (GLenum cap) = &glIsEnabled;

__declspec(dllimport) GLboolean GL_APIENTRY  glIsFramebuffer (GLuint framebuffer)
{
    return my_context->glIsFramebuffer(framebuffer);
}
 __declspec(dllexport) GLboolean GL_APIENTRY  (*__imp_glIsFramebuffer) (GLuint framebuffer) = &glIsFramebuffer;

__declspec(dllimport) GLboolean GL_APIENTRY  glIsProgram (GLuint program)
{
    return my_context->glIsProgram(program);
}
 __declspec(dllexport) GLboolean GL_APIENTRY  (*__imp_glIsProgram) (GLuint program) = &glIsProgram;

__declspec(dllimport) GLboolean GL_APIENTRY  glIsRenderbuffer (GLuint renderbuffer)
{
    return my_context->glIsRenderbuffer(renderbuffer);
}
 __declspec(dllexport) GLboolean GL_APIENTRY  (*__imp_glIsRenderbuffer) (GLuint renderbuffer) = &glIsRenderbuffer;

__declspec(dllimport) GLboolean GL_APIENTRY  glIsShader (GLuint shader)
{
    return my_context->glIsShader(shader);
}
 __declspec(dllexport) GLboolean GL_APIENTRY  (*__imp_glIsShader) (GLuint shader) = &glIsShader;

__declspec(dllimport) GLboolean GL_APIENTRY  glIsTexture (GLuint texture)
{
    return my_context->glIsTexture(texture);
}
 __declspec(dllexport) GLboolean GL_APIENTRY  (*__imp_glIsTexture) (GLuint texture) = &glIsTexture;

__declspec(dllimport) void GL_APIENTRY  glLineWidth (GLfloat width)
{
    my_context->glLineWidth(width);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glLineWidth) (GLfloat width) = &glLineWidth;

__declspec(dllimport) void GL_APIENTRY  glLinkProgram (GLuint program)
{
    my_context->glLinkProgram(program);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glLinkProgram) (GLuint program) = &glLinkProgram;

__declspec(dllimport) void GL_APIENTRY  glPixelStorei (GLenum pname, GLint param)
{
    my_context->glPixelStorei(pname,param);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glPixelStorei) (GLenum pname, GLint param) = &glPixelStorei;

__declspec(dllimport) void GL_APIENTRY  glPolygonOffset (GLfloat factor, GLfloat units)
{
    my_context->glPolygonOffset(factor,units);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glPolygonOffset) (GLfloat factor, GLfloat units) = &glPolygonOffset;

__declspec(dllimport) void GL_APIENTRY  glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels)
{
    my_context->glReadPixels(x,y,width,height,format,type,pixels);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glReadPixels) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels) = &glReadPixels;

__declspec(dllimport) void GL_APIENTRY  glReleaseShaderCompiler (void)
{
    my_context->glReleaseShaderCompiler();
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glReleaseShaderCompiler) (void) = &glReleaseShaderCompiler;

__declspec(dllimport) void GL_APIENTRY  glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    my_context->glRenderbufferStorage(target,internalformat,width,height);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glRenderbufferStorage) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height) = &glRenderbufferStorage;

__declspec(dllimport) void GL_APIENTRY  glSampleCoverage (GLfloat value, GLboolean invert)
{
    my_context->glSampleCoverage(value,invert);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glSampleCoverage) (GLfloat value, GLboolean invert) = &glSampleCoverage;

__declspec(dllimport) void GL_APIENTRY  glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
    my_context->glScissor(x,y,width,height);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glScissor) (GLint x, GLint y, GLsizei width, GLsizei height) = &glScissor;

__declspec(dllimport) void GL_APIENTRY  glShaderBinary (GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length)
{
    my_context->glShaderBinary(count,shaders,binaryformat,binary,length);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glShaderBinary) (GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length) = &glShaderBinary;

__declspec(dllimport) void GL_APIENTRY  glShaderSource (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)
{
    my_context->glShaderSource(shader,count,string,length);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glShaderSource) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length) = &glShaderSource;

__declspec(dllimport) void GL_APIENTRY  glStencilFunc (GLenum func, GLint ref, GLuint mask)
{
    my_context->glStencilFunc(func,ref,mask);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glStencilFunc) (GLenum func, GLint ref, GLuint mask) = &glStencilFunc;

__declspec(dllimport) void GL_APIENTRY  glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask)
{
    my_context->glStencilFuncSeparate(face,func,ref,mask);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glStencilFuncSeparate) (GLenum face, GLenum func, GLint ref, GLuint mask) = &glStencilFuncSeparate;

__declspec(dllimport) void GL_APIENTRY  glStencilMask (GLuint mask)
{
    my_context->glStencilMask(mask);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glStencilMask) (GLuint mask) = &glStencilMask;

__declspec(dllimport) void GL_APIENTRY  glStencilMaskSeparate (GLenum face, GLuint mask)
{
    my_context->glStencilMaskSeparate(face,mask);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glStencilMaskSeparate) (GLenum face, GLuint mask) = &glStencilMaskSeparate;

__declspec(dllimport) void GL_APIENTRY  glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
    my_context->glStencilOp(fail,zfail,zpass);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glStencilOp) (GLenum fail, GLenum zfail, GLenum zpass) = &glStencilOp;

__declspec(dllimport) void GL_APIENTRY  glStencilOpSeparate (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    my_context->glStencilOpSeparate(face,sfail,dpfail,dppass);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glStencilOpSeparate) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) = &glStencilOpSeparate;

__declspec(dllimport) void GL_APIENTRY  glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
{
    my_context->glTexImage2D(target,level,internalformat,width,height,border,format,type,pixels);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glTexImage2D) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) = &glTexImage2D;

__declspec(dllimport) void GL_APIENTRY  glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    my_context->glTexParameterf(target,pname,param);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glTexParameterf) (GLenum target, GLenum pname, GLfloat param) = &glTexParameterf;

__declspec(dllimport) void GL_APIENTRY  glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
    my_context->glTexParameterfv(target,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glTexParameterfv) (GLenum target, GLenum pname, const GLfloat *params) = &glTexParameterfv;

__declspec(dllimport) void GL_APIENTRY  glTexParameteri (GLenum target, GLenum pname, GLint param)
{
    my_context->glTexParameteri(target,pname,param);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glTexParameteri) (GLenum target, GLenum pname, GLint param) = &glTexParameteri;

__declspec(dllimport) void GL_APIENTRY  glTexParameteriv (GLenum target, GLenum pname, const GLint *params)
{
    my_context->glTexParameteriv(target,pname,params);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glTexParameteriv) (GLenum target, GLenum pname, const GLint *params) = &glTexParameteriv;

__declspec(dllimport) void GL_APIENTRY  glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
    my_context->glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) = &glTexSubImage2D;

__declspec(dllimport) void GL_APIENTRY  glUniform1f (GLint location, GLfloat v0)
{
    my_context->glUniform1f(location,v0);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform1f) (GLint location, GLfloat v0) = &glUniform1f;

__declspec(dllimport) void GL_APIENTRY  glUniform1fv (GLint location, GLsizei count, const GLfloat *value)
{
    my_context->glUniform1fv(location,count,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform1fv) (GLint location, GLsizei count, const GLfloat *value) = &glUniform1fv;

__declspec(dllimport) void GL_APIENTRY  glUniform1i (GLint location, GLint v0)
{
    my_context->glUniform1i(location,v0);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform1i) (GLint location, GLint v0) = &glUniform1i;

__declspec(dllimport) void GL_APIENTRY  glUniform1iv (GLint location, GLsizei count, const GLint *value)
{
    my_context->glUniform1iv(location,count,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform1iv) (GLint location, GLsizei count, const GLint *value) = &glUniform1iv;

__declspec(dllimport) void GL_APIENTRY  glUniform2f (GLint location, GLfloat v0, GLfloat v1)
{
    my_context->glUniform2f(location,v0,v1);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform2f) (GLint location, GLfloat v0, GLfloat v1) = &glUniform2f;

__declspec(dllimport) void GL_APIENTRY  glUniform2fv (GLint location, GLsizei count, const GLfloat *value)
{
    my_context->glUniform2fv(location,count,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform2fv) (GLint location, GLsizei count, const GLfloat *value) = &glUniform2fv;

__declspec(dllimport) void GL_APIENTRY  glUniform2i (GLint location, GLint v0, GLint v1)
{
    my_context->glUniform2i(location,v0,v1);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform2i) (GLint location, GLint v0, GLint v1) = &glUniform2i;

__declspec(dllimport) void GL_APIENTRY  glUniform2iv (GLint location, GLsizei count, const GLint *value)
{
    my_context->glUniform2iv(location,count,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform2iv) (GLint location, GLsizei count, const GLint *value) = &glUniform2iv;

__declspec(dllimport) void GL_APIENTRY  glUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    my_context->glUniform3f(location,v0,v1,v2);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform3f) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2) = &glUniform3f;

__declspec(dllimport) void GL_APIENTRY  glUniform3fv (GLint location, GLsizei count, const GLfloat *value)
{
    my_context->glUniform3fv(location,count,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform3fv) (GLint location, GLsizei count, const GLfloat *value) = &glUniform3fv;

__declspec(dllimport) void GL_APIENTRY  glUniform3i (GLint location, GLint v0, GLint v1, GLint v2)
{
    my_context->glUniform3i(location,v0,v1,v2);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform3i) (GLint location, GLint v0, GLint v1, GLint v2) = &glUniform3i;

__declspec(dllimport) void GL_APIENTRY  glUniform3iv (GLint location, GLsizei count, const GLint *value)
{
    my_context->glUniform3iv(location,count,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform3iv) (GLint location, GLsizei count, const GLint *value) = &glUniform3iv;

__declspec(dllimport) void GL_APIENTRY  glUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    my_context->glUniform4f(location,v0,v1,v2,v3);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform4f) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) = &glUniform4f;

__declspec(dllimport) void GL_APIENTRY  glUniform4fv (GLint location, GLsizei count, const GLfloat *value)
{
    my_context->glUniform4fv(location,count,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform4fv) (GLint location, GLsizei count, const GLfloat *value) = &glUniform4fv;

__declspec(dllimport) void GL_APIENTRY  glUniform4i (GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    my_context->glUniform4i(location,v0,v1,v2,v3);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform4i) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3) = &glUniform4i;

__declspec(dllimport) void GL_APIENTRY  glUniform4iv (GLint location, GLsizei count, const GLint *value)
{
    my_context->glUniform4iv(location,count,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniform4iv) (GLint location, GLsizei count, const GLint *value) = &glUniform4iv;

__declspec(dllimport) void GL_APIENTRY  glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    my_context->glUniformMatrix2fv(location,count,transpose,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniformMatrix2fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) = &glUniformMatrix2fv;

__declspec(dllimport) void GL_APIENTRY  glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    my_context->glUniformMatrix3fv(location,count,transpose,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniformMatrix3fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) = &glUniformMatrix3fv;

__declspec(dllimport) void GL_APIENTRY  glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    my_context->glUniformMatrix4fv(location,count,transpose,value);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUniformMatrix4fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) = &glUniformMatrix4fv;

__declspec(dllimport) void GL_APIENTRY  glUseProgram (GLuint program)
{
    my_context->glUseProgram(program);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glUseProgram) (GLuint program) = &glUseProgram;

__declspec(dllimport) void GL_APIENTRY  glValidateProgram (GLuint program)
{
    my_context->glValidateProgram(program);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glValidateProgram) (GLuint program) = &glValidateProgram;

__declspec(dllimport) void GL_APIENTRY  glVertexAttrib1f (GLuint index, GLfloat x)
{
    my_context->glVertexAttrib1f(index,x);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glVertexAttrib1f) (GLuint index, GLfloat x) = &glVertexAttrib1f;

__declspec(dllimport) void GL_APIENTRY  glVertexAttrib1fv (GLuint index, const GLfloat *v)
{
    my_context->glVertexAttrib1fv(index,v);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glVertexAttrib1fv) (GLuint index, const GLfloat *v) = &glVertexAttrib1fv;

__declspec(dllimport) void GL_APIENTRY  glVertexAttrib2f (GLuint index, GLfloat x, GLfloat y)
{
    my_context->glVertexAttrib2f(index,x,y);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glVertexAttrib2f) (GLuint index, GLfloat x, GLfloat y) = &glVertexAttrib2f;

__declspec(dllimport) void GL_APIENTRY  glVertexAttrib2fv (GLuint index, const GLfloat *v)
{
    my_context->glVertexAttrib2fv(index,v);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glVertexAttrib2fv) (GLuint index, const GLfloat *v) = &glVertexAttrib2fv;

__declspec(dllimport) void GL_APIENTRY  glVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    my_context->glVertexAttrib3f(index,x,y,z);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glVertexAttrib3f) (GLuint index, GLfloat x, GLfloat y, GLfloat z) = &glVertexAttrib3f;

__declspec(dllimport) void GL_APIENTRY  glVertexAttrib3fv (GLuint index, const GLfloat *v)
{
    my_context->glVertexAttrib3fv(index,v);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glVertexAttrib3fv) (GLuint index, const GLfloat *v) = &glVertexAttrib3fv;

__declspec(dllimport) void GL_APIENTRY  glVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    my_context->glVertexAttrib4f(index,x,y,z,w);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glVertexAttrib4f) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) = &glVertexAttrib4f;

__declspec(dllimport) void GL_APIENTRY  glVertexAttrib4fv (GLuint index, const GLfloat *v)
{
    my_context->glVertexAttrib4fv(index,v);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glVertexAttrib4fv) (GLuint index, const GLfloat *v) = &glVertexAttrib4fv;

__declspec(dllimport) void GL_APIENTRY  glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
{
    my_context->glVertexAttribPointer(index,size,type,normalized,stride,pointer);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glVertexAttribPointer) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) = &glVertexAttribPointer;

__declspec(dllimport) void GL_APIENTRY  glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    my_context->glViewport(x,y,width,height);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glViewport) (GLint x, GLint y, GLsizei width, GLsizei height) = &glViewport;

__declspec(dllimport) void GL_APIENTRY  glBindVertexArrayOES (GLuint array)
{
    my_context->glBindVertexArrayOES(array);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glBindVertexArrayOES) (GLuint array) = &glBindVertexArrayOES;

__declspec(dllimport) void GL_APIENTRY  glDeleteVertexArraysOES (GLsizei n, const GLuint *arrays)
{
    my_context->glDeleteVertexArraysOES(n,arrays);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glDeleteVertexArraysOES) (GLsizei n, const GLuint *arrays) = &glDeleteVertexArraysOES;

__declspec(dllimport) void GL_APIENTRY  glGenVertexArraysOES (GLsizei n, GLuint *arrays)
{
    my_context->glGenVertexArraysOES(n,arrays);
}
 __declspec(dllexport) void GL_APIENTRY  (*__imp_glGenVertexArraysOES) (GLsizei n, GLuint *arrays) = &glGenVertexArraysOES;

__declspec(dllimport) GLboolean GL_APIENTRY  glIsVertexArrayOES (GLuint array)
{
    return my_context->glIsVertexArrayOES(array);
}
 __declspec(dllexport) GLboolean GL_APIENTRY  (*__imp_glIsVertexArrayOES) (GLuint array) = &glIsVertexArrayOES;



#endif //HAVE_OPENGLES2
