
RAYLIB_VERSION     = 1.9.4
RAYLIB_API_VERSION = 1
RAYLIB_PATH        = ..
# RAYLIB_RELEASE_PATH points to provided binaries and your immediate build of raylib.
# It is further modified below by PLATFORM below.
RAYLIB_RELEASE_PATH  ?= $(RAYLIB_PATH)/release/libs


CFLAGS =-g -I. -I/usr/local/include/libprojectM -I/usr/local/FTGLES2 -I/usr/local/include/freetype2 -I/usr/include -I/usr/include/GL -I.. -I../libpng -I../libbpg -DLINUX_CLIENT=1  -D$(PLATFORM) -D$(GRAPHICS)
CPPFLAGS =-g -I. -std=c++11 -I/usr/local/include/libprojectM -I/usr/local/FTGLES2 -I/usr/local/include/freetype2 -I/usr/include -I/usr/include/GL -I.. -I../libpng -I../libbpg -DLINUX_CLIENT=1  -D$(PLATFORM) -D$(GRAPHICS)

PLATFORM             ?= PLATFORM_SILENT_RADIANCE

# Library type used for raylib: STATIC (.a) or SHARED (.so/.dll)
RAYLIB_LIBTYPE       ?= STATIC

# Build mode for library: DEBUG or RELEASE
RAYLIB_BUILD_MODE    ?= DEBUG

# Included raylib audio module on compilation
# NOTE: Some programs like tools could not require audio support
INCLUDE_AUDIO_MODULE ?= FALSE

# Use OpenAL Soft backend for audio
USE_OPENAL_BACKEND   ?= FALSE

# OpenAL Soft audio backend forced on HTML5 and OSX (see below)
#ifeq ($(PLATFORM),PLATFORM_WEB)
#    USE_OPENAL_BACKEND = TRUE
#endif

# Use external GLFW library instead of rglfw module
USE_EXTERNAL_GLFW    ?= FALSE

# Use Wayland display server protocol on Linux desktop
# by default it uses X11 windowing system
USE_WAYLAND_DISPLAY  ?= FALSE

# See below for more GRAPHICS options.

# Use cross-compiler for PLATFORM_RPI



# Determine PLATFORM_OS in case PLATFORM_DESKTOP selected
ifeq ($(PLATFORM),PLATFORM_SILENT_RADIANCE)
    # No uname.exe on MinGW!, but OS=Windows_NT on Windows!
    # ifeq ($(UNAME),Msys) -> Windows
    ifeq ($(OS),Windows_NT)
        PLATFORM_OS=WINDOWS
    else
        UNAMEOS=$(shell uname)
        ifeq ($(UNAMEOS),Linux)
            PLATFORM_OS=LINUX
        endif
        ifeq ($(UNAMEOS),FreeBSD)
            PLATFORM_OS=FREEBSD
        endif
        ifeq ($(UNAMEOS),Darwin)
            PLATFORM_OS=OSX
        endif
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
    UNAMEOS=$(shell uname)
    ifeq ($(UNAMEOS),Linux)
        PLATFORM_OS=LINUX
    endif
endif

# Force OpenAL Soft audio backend for OSX platform
# NOTE 1: mini_al library does not support CoreAudio yet
# NOTE 2: Required OpenAL libraries should be available on OSX


    GRAPHICS = GRAPHICS_API_OPENGL_ES2
# eglGetProcAddresss is called, but not available in ubuntu nvidia
#    GRAPHICS = GRAPHICS_API_OPENGL_33








#CFLAGS += -O1 -Wall -std=c99 -D_DEFAULT_SOURCE -fgnu89-inline -Wno-missing-braces -Werror=pointer-arith
#CFLAGS += -O1 -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Werror=pointer-arith
CFLAGS += -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Werror=pointer-arith  
CPPFLAGS += -Wall -D_DEFAULT_SOURCE -Wno-missing-braces -Werror=pointer-arith  
#^^^^^^no optimization for debugging
# Additional flags for compiler (if desired)
#  -Wextra                  enables some extra warning flags that are not enabled by -Wall
#  -Wmissing-prototypes     warn if a global function is defined without a previous prototype declaration
#  -Wstrict-prototypes      warn if a function is declared or defined without specifying the argument types
#  -Werror=implicit-function-declaration   catch function calls without prior declaration
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    CFLAGS += -Werror=implicit-function-declaration
    CPPFLAGS += -Werror=implicit-function-declaration
endif

    
# Define include paths for required headers
# NOTE: Several external required libraries (stb and others)
INCLUDE_PATHS = -I. -Iexternal 

ifeq ($(PLATFORM),PLATFORM_DESKTOP)   
    ifeq ($(PLATFORM_OS),FREEBSD)
        INCLUDE_PATHS += -I/usr/local/include
        LDFLAGS += -L. -Lsrc -L/usr/local/lib 
    endif
    ifeq ($(USE_EXTERNAL_GLFW),TRUE)  
        LDFLAGS += -lglfw
    endif
endif


CFLAGS +=  $(SRTOPDEFS) -g -D$(PLATFORM) -D$(GRAPHICS) -DLARRY_GLES_2_DIRECT
CPPFLAGS +=  $(SRTOPDEFS) -g -D$(PLATFORM) -D$(GRAPHICS) -DLARRY_GLES_2_DIRECT

system: ../display_stuff.a 



# note - core.o is for raylib, and we are taking it out
# we are not using it right now, and it is slow compared to 
# more direct opengl es 2.0
# also shapes.o text.o and utils.o and models.o
#
# instead, we do not_rl.o
#
# later these two can coexist, but not right now.

#CFLAGS = -g -I/usr/include/GL -I.. -I../libpng -I../libbpg -DLINUX_CLIENT=1  -D$(PLATFORM) -D$(GRAPHICS)

../display_stuff.a: display_stuff.a
	cp display_stuff.a ..


display_stuff.a: 	backward_worm.o default.o  display_main_linux.o display_set.o helper.o  justin2.o  \
	    network_cube.o srikant1.o purple_string.o top_menu.o display_set.o main_menu.o jason.o words.o \
	    letters.o letters_ascii.o letters_render_opengles2.o  \
	    larry_harvey_has_a_possee.o visual_voice.o \
	     mathias1.o \
			       impact.o staff.o \
				  not_rl.o       \
					   larry_harvey_hater_robots.o \
		window_shop.o buy_it_now.o \
		projectmSR.o projectm.o projectm_pp.o  \
					              visual_voice.o  dennis1.o 
	echo $(PLATFORM) strtopdefs $(SRTOPDEFS)
	ar rcs display_stuff.a backward_worm.o default.o  display_main_linux.o display_set.o helper.o  justin2.o  \
	            mathias1.o srikant1.o network_cube.o purple_string.o top_menu.o display_set.o main_menu.o words.o  \
	    letters.o letters_ascii.o letters_render_opengles2.o jason.o  larry_harvey_hater_robots.o \
	     larry_harvey_has_a_possee.o  staff.o  dennis1.o \
		window_shop.o buy_it_now.o not_rl.o \
		projectmSR.o projectm.o	projectm_pp.o    \
        \
        \
        \
        impact.o \
       \
        visual_voice.o 

		    



		    

