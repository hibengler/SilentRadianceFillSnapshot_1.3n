/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler and Ivonne Lauger

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



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES 1  
#include "glue.h"
#include "packet_summary.h"
#include "display_set.h"
#include "display_main.h"
#include "helper.h"

// WORLD_INTERNATIONAL off
// Shader sources
static const GLchar* vertexSource =
    "attribute vec4 position;    \n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = vec4(position.xyz, 1.0);  \n"
    "}                            \n";
static const GLchar* fragmentSource =
    "precision mediump float;\n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = vec4 (0.8,0.1, 0.9, 1.0 );\n"
    "}                                            \n";
// WORLD_INTERNATIONAL on

static    GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};
static GLfloat upper_left_test[3] = {-1.0f,1.0f,0.0f};

static GLfloat delta = .05;


static     GLuint vbo;
static     GLuint vertexShader;
static  GLuint fragmentShader;
static    GLuint shaderProgram;
static     GLint posAttrib;
static int ww;
static int hh;

static float max(float a, float b){
        if(a>=b) return a;
        else return b;
}

static float min(float a, float b){
        if(a<b) return a;
        else return b;
}

int ivonne1_setupGraphics(int w, int h) {
ww=w;hh=h;
if (ran_setup_graphics_once_before) return(1);
    // Create Vertex Array Object
//    glGenVertexArraysOES(1, &vao);
//    glBindVertexArrayOES(vao);
    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &vbo);
        checkGlError("a");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
        checkGlError("b");
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        checkGlError("c");

    // Create and compile the vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
        checkGlError("d");
    glCompileShader(vertexShader);
        checkGlError("e");


    // Create and compile the fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            checkGlError("f");
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
            checkGlError("g");
    glCompileShader(fragmentShader);
            checkGlError("h");



    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
            checkGlError("i");
    glDeleteShader(vertexShader);
            checkGlError("j");
    glDeleteShader(fragmentShader);
            checkGlError("k");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return 1;

}


void ivonne1_renderBackground() {
                glClearColor(0.f*0.4f,0.0f*0.4f,0.f,1.f); /* black */
        checkGlError("glClearColora");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}



void ivonne1_renderFrame() {
    glUseProgram(shaderProgram);
                checkGlError("j");

    glLineWidth(1.f);
                checkGlError("k");
	     
    // Specify the layout of the vertex data
    posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
                checkGlError("l");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
                checkGlError("m");
    
vertices[0] += delta;
if (vertices[0]>1.) delta = - delta;
if (vertices[0]<0.) delta = - delta;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
                checkGlError("n");
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
                checkGlError("o");
    
    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
                checkGlError("p");
    glEnableVertexAttribArray(posAttrib);
                checkGlError("q");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
                checkGlError("r");
    // Draw a triangle from the 3 vertices
    
     // Draw a triangle from the 3 vertices
             glDrawArrays(GL_TRIANGLES, 0, 3);
                checkGlError("tri");
    
glBindBuffer(GL_ARRAY_BUFFER, 0);
                checkGlError("s");

}


static GLfloat upper_right_test[3] = {1.0f,1.0f,0.0f};
static GLfloat lower_left_test[3] = {-1.0f,-1.0f,0.0f};
static GLfloat lower_right_test[3] = {1.0f,-1.0f,0.0f};

static glMatrix modelMatrix;

extern void ivonne1_renderButton(int program,
   int mColorHandle,int menu_position_handle,glMatrix *view) {
 return;
}

void ivonne1_step() {
}





void ivonne1_renderForeground() {
}



void ivonne1_touchEvent(int action,float x,float y) {
if (action==1) {
  push_display_set(2);
  }
}


// WORLD_INTERNATIONAL off
const display_set ivonne1_display_set = {
  .name = "ivonne1",
  .setupGraphics = ivonne1_setupGraphics,
  .init = NULL,
  .setupGL = NULL,
  .changeScreen = NULL,
  .step = ivonne1_step,
  .renderBackground = ivonne1_renderBackground,
  .renderMain  = ivonne1_renderFrame,
  .renderForeground = ivonne1_renderForeground,
  .renderButton = ivonne1_renderButton,
  .touchEvent = NULL,
  .keyEvent = NULL,
  .finishStreaming = NULL
  };
// WORLD_INTERNATIONAL on

/*
$Log: ivonne1.c,v $
Revision 1.8  2019/04/28 04:11:16  hib
wip - changing display_set to separate init, glsetup, and screen changed

Revision 1.7  2019/04/15 08:13:56  hib
cleaning international

Revision 1.6  2019/03/17 12:58:24  hib
Updated copyrights

Revision 1.5  2019/03/14 12:47:55  hib
wip - working on makeing the store look good

Revision 1.4  2018/12/29 00:23:12  ivonne
*** empty log message ***


*/
