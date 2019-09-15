 
#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengles2.h>

#else

#include <SDL2/SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>
#include "SDL_gles2_stubcall_overs_wrapper.h"

#endif

// WORLD_INTERNATIONAL off
// Shader sources
const GLchar* vertexSource =
    "attribute vec4 position;    \n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = vec4(position.xyz, 1.0);  \n"
    "}                            \n";
const GLchar* fragmentSource =
    "precision mediump float;\n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = vec4 (0.8,0.1, 0.8, 1.0 );\n"
    "}                                            \n";

// WORLD_INTERNATIONAL on



GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};
GLfloat delta = .05;
static GLES2_Context ctx;
SDL_Window *wnd;








int main(int argc, char** argv)
{
fprintf(stderr,"b1\n");
//#define WORLD_display_triangle_none "none"
if (!SDL_SetHint(SDL_HINT_VIDEO_WIN_D3DCOMPILER,WORLD_display_triangle_none)) {
  fprintf(stderr,"cant turn off decompiler: %s\n",SDL_GetError());
  }
//#define WORLD_display_triangle_1 "1"
if (!SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER,WORLD_display_triangle_1)) {
  fprintf(stderr,"cant hint GLES: %s\n",SDL_GetError());
  }
    // SDL_Init(SDL_INIT_VIDEO);
if (SDL_VideoInit(NULL)) {
  fprintf(stderr,"cant init video: %s\n",SDL_GetError());
  }
if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES)) {
  fprintf(stderr,"cant set profile to ES: %s\n",SDL_GetError());
  }
if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2)) {
  fprintf(stderr,"all set attributsd dont work yet\n");
  }
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
SDL_GL_SetSwapInterval(0);
SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		       
//#define WORLD_display_triangle_test "test"
    wnd=    SDL_CreateWindow(WORLD_display_triangle_test, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		       
SDL_GLContext glc = SDL_GL_CreateContext(wnd);
if (!glc) {fprintf(stderr,"no context: %s\n",SDL_GetError());}

if (SDL_GL_MakeCurrent(wnd,glc)) {
  fprintf(stderr,"not good current context: %s\n",SDL_GetError());
  }  // new

SDL_Renderer *rdr = SDL_CreateRenderer(
        wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
                  
if (!rdr) {fprintf(stderr,"no renderer: %s\n",SDL_GetError());}

/* Important: call this *after* creating the context */
if (LoadContext(&ctx) < 0) {
//#define WORLD_Could_not_load_GLES2_functions_lf_ "Could not load GLES2 functions\n"
    SDL_Log(WORLD_Could_not_load_GLES2_functions_lf_);
    exit;
    return 0;
    }
			     


    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);


    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

void main_loop() {
        SDL_Event e;

        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) exit(-1);
        }

        glUseProgram(shaderProgram);


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);



vertices[0] += delta;
if (vertices[0]>1.) delta = - delta;
if (vertices[0]<0.) delta = - delta;


    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    
//    SDL_GL_SwapWindow(wnd);
};

    

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while(1) main_loop();
#endif

    return 0;
}
