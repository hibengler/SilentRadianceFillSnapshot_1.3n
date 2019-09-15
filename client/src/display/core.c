/**********************************************************************************************
*
*   raylib.core - Basic functions to manage windows, OpenGL context and input on multiple platforms
*
*   PLATFORMS SUPPORTED: 
*       - PLATFORM_DESKTOP: Windows (Win32, Win64)
*       - PLATFORM_DESKTOP: Linux (X11 desktop mode)
*       - PLATFORM_DESKTOP: FreeBSD (X11 desktop)
*       - PLATFORM_DESKTOP: OSX/macOS
*       - PLATFORM_ANDROID: Android 4.0 (ARM, ARM64) 
*       - PLATFORM_RPI:     Raspberry Pi 0,1,2,3 (Raspbian)
*       - PLATFORM_WEB:     HTML5 with asm.js (Chrome, Firefox)
*       - PLATFORM_UWP:     Windows 10 App, Windows Phone, Xbox One
*       - PLATFORM_SILENT_RADIANCE:  Limited functionality as per window management, and just meant ti implement drawing 
*                                 a frame on silentRadiance, with or without other frames involved.
*                            This lets SilentRadiance use raylib, without raylib controlling the windows and stuff.
*
*   CONFIGURATION:
*
*   #define PLATFORM_DESKTOP
*       Windowing and input system configured for desktop platforms: Windows, Linux, OSX, FreeBSD
*       NOTE: Oculus Rift CV1 requires PLATFORM_DESKTOP for mirror rendering - View [rlgl] module to enable it
*
*   #define PLATFORM_ANDROID
*       Windowing and input system configured for Android device, app activity managed internally in this module.
*       NOTE: OpenGL ES 2.0 is required and graphic device is managed by EGL
*
*   #define PLATFORM_RPI
*       Windowing and input system configured for Raspberry Pi i native mode (no X.org required, tested on Raspbian), 
*       graphic device is managed by EGL and inputs are processed is raw mode, reading from /dev/input/
*
*   #define PLATFORM_WEB
*       Windowing and input system configured for HTML5 (run on browser), code converted from C to asm.js
*       using emscripten compiler. OpenGL ES 2.0 required for direct translation to WebGL equivalent code.
*
*   #define PLATFORM_UWP
*       Universal Windows Platform support, using OpenGL ES 2.0 through ANGLE on multiple Windows platforms,
*       including Windows 10 App, Windows Phone and Xbox One platforms.
*
*   #define SUPPORT_DEFAULT_FONT (default)
*       Default font is loaded on window initialization to be available for the user to render simple text.
*       NOTE: If enabled, uses external module functions to load default raylib font (module: text)
*
*   #define SUPPORT_CAMERA_SYSTEM
*       Camera module is included (camera.h) and multiple predefined cameras are available: free, 1st/3rd person, orbital
*
*   #define SUPPORT_GESTURES_SYSTEM
*       Gestures module is included (gestures.h) to support gestures detection: tap, hold, swipe, drag
*
*   #define SUPPORT_MOUSE_GESTURES
*       Mouse gestures are directly mapped like touches and processed by gestures system.
*
*   #define SUPPORT_BUSY_WAIT_LOOP
*       Use busy wait loop for timming sync, if not defined, a high-resolution timer is setup and used
*
*   #define SUPPORT_GIF_RECORDING
*       Allow automatic gif recording of current screen pressing CTRL+F12, defined in KeyCallback()
*
*   DEPENDENCIES:
*       rglfw    - Manage graphic device, OpenGL context and inputs on PLATFORM_DESKTOP (linux)
*       raymath  - 3D math functionality (Vector2, Vector3, Matrix, Quaternion)
*       camera   - Multiple 3D camera modes (free, orbital, 1st person, 3rd person)
*       gestures - Gestures system for touch-ready devices (or simulated from mouse inputs)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2018 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/
// WORLD_INTERNATIONAL off
// ^^ hint is unrelated to raylib

// Default configuration flags (supported features)
//-------------------------------------------------
#define SUPPORT_DEFAULT_FONT
#define SUPPORT_MOUSE_GESTURES
#define SUPPORT_CAMERA_SYSTEM
#define SUPPORT_GESTURES_SYSTEM
#define SUPPORT_BUSY_WAIT_LOOP
#define SUPPORT_GIF_RECORDING
//-------------------------------------------------

// shapes.c:
#define USE_DEFAULT_FONT_TEXTURE



// Default configuration flags (supported features)
//-------------------------------------------------
#define SUPPORT_VR_SIMULATOR
#define SUPPORT_DISTORTION_SHADER
//-------------------------------------------------

// textures.c
// Default configuration flags (supported features)
//-------------------------------------------------
#define SUPPORT_FILEFORMAT_PNG
#define SUPPORT_FILEFORMAT_DDS
#define SUPPORT_FILEFORMAT_HDR
#define SUPPORT_FILEFORMAT_KTX
#define SUPPORT_FILEFORMAT_ASTC
#define SUPPORT_IMAGE_MANIPULATION
#define SUPPORT_IMAGE_GENERATION
//-------------------------------------------------

#include "raylib.h"
#include "rlgl.h"

#include <stdio.h>                  // Required for: fopen(), fclose(), fread()... [Used only on LoadText()]
#include <stdlib.h>                 // Required for: malloc(), free(), rand()
#include <string.h>                 // Required for: strcmp(), strlen(), strtok() [Used only in extensions loading]
#include <math.h>                   // Required for: atan2()

#include "utils.h"              // Required for: fopen() Android mapping


//#if !defined(RLGL_STANDALONE)
//    #include "raymath.h"            // Required for: Vector3 and Matrix functions
//#endif

#ifdef PLATFORM_SILENT_RADIANCE	    
#include "glue.h"
#include "display_main.h"
#ifdef __APPLE__
#include "display_main_osx.h"
#endif
#endif

#if defined(GRAPHICS_API_OPENGL_11)
    #if defined(__APPLE__)
        #include <OpenGL/gl.h>      // OpenGL 1.1 library for OSX
        #include <OpenGL/glext.h>
    #else
		#if defined(_MSC_VER)		// Using MSVC compiler, requires some additional definitions
			// APIENTRY for OpenGL function pointer declarations is required
			#ifndef APIENTRY
				#ifdef _WIN32
					#define APIENTRY __stdcall
				#else
					#define APIENTRY
				#endif
			#endif
			// WINGDIAPI definition. Some Windows OpenGL headers need it
			#if !defined(WINGDIAPI) && defined(_WIN32)
				#define WINGDIAPI __declspec(dllimport)
			#endif
		#endif

		#include <GL/gl.h>          // OpenGL 1.1 library
    #endif
#endif

#if defined(GRAPHICS_API_OPENGL_21)
    #define GRAPHICS_API_OPENGL_33
#endif

#if defined(GRAPHICS_API_OPENGL_33)
    #if defined(__APPLEOSX__)
        #include <OpenGL/gl3.h>     // OpenGL 3 library for OSX
        #include <OpenGL/gl3ext.h>
    #else
        #define GLAD_IMPLEMENTATION
        #if defined(RLGL_STANDALONE)
            #include "glad.h"           // GLAD extensions loading library, includes OpenGL headers
        #else
            #include "external/glad.h"  // GLAD extensions loading library, includes OpenGL headers
        #endif
    #endif
#endif

#if defined(GRAPHICS_API_OPENGL_ES2)

        // EGL library
#if defined(__APPLE__)
#include "EGL/egl.h"
    #include <OpenGLES/ES2/gl.h>         // OpenGL ES 2.0 library
    #include <OpenGLES/ES2/glext.h>       // OpenGL ES 2.0 extensions library
#else
#ifndef _WIN32
#include <EGL/egl.h>
#endif
    #include <GLES2/gl2.h>          // OpenGL ES 2.0 library
    #include <GLES2/gl2ext.h>       // OpenGL ES 2.0 extensions library

#endif
#endif

#if defined(RLGL_STANDALONE)
    #include <stdarg.h>             // Required for: va_list, va_start(), vfprintf(), va_end() [Used only on TraceLog()]
#endif

#if !defined(GRAPHICS_API_OPENGL_11) && defined(SUPPORT_DISTORTION_SHADER)
    #include "shader_distortion.h"  // Distortion shader to be embedded
#endif


#if (defined(__linux__) || defined(PLATFORM_WEB)) && _POSIX_C_SOURCE < 199309L
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309L // Required for CLOCK_MONOTONIC if compiled with c99 without gnu ext.
#endif

//#include "rlgl.h"           // raylib OpenGL abstraction layer to OpenGL 1.1, 3.3+ or ES2
#include "utils.h"          // Required for: fopen() Android mapping

#define RAYMATH_IMPLEMENTATION  // Use raymath as a header-only library (includes implementation)
#define RAYMATH_EXTERN_INLINE   // Compile raymath functions as static inline (remember, it's a compiler hint)
#include "raymath.h"            // Required for: Vector3 and Matrix functions

#if defined(SUPPORT_GESTURES_SYSTEM)
    #define GESTURES_IMPLEMENTATION
    #include "gestures.h"       // Gestures detection functionality
#endif

#if defined(SUPPORT_CAMERA_SYSTEM) && !defined(PLATFORM_ANDROID)
    #define CAMERA_IMPLEMENTATION
    #include "camera.h"         // Camera system functionality
#endif

#if defined(SUPPORT_GIF_RECORDING)
    #define RGIF_IMPLEMENTATION
    #include "external/rgif.h"   // Support GIF recording
#endif

#include <stdio.h>          // Standard input / output lib
#include <stdlib.h>         // Required for: malloc(), free(), rand(), atexit()
#include <stdint.h>         // Required for: typedef unsigned long long int uint64_t, used by hi-res timer
#include <time.h>           // Required for: time() - Android/RPI hi-res timer (NOTE: Linux only!)
#include <math.h>           // Required for: tan() [Used in Begin3dMode() to set perspective]
#include <string.h>         // Required for: strrchr(), strcmp()
//#include <errno.h>          // Macros for reporting and retrieving error conditions through error codes

#if defined(_WIN32)
    #include <direct.h>             // Required for: _getch(), _chdir()
    #define GETCWD _getcwd          // NOTE: MSDN recommends not to use getcwd(), chdir()
    #define CHDIR _chdir
#else
    #include "unistd.h"             // Required for: getch(), chdir() (POSIX)
    #define GETCWD(a,b) getcwd(a,b)
    #define CHDIR chdir
#endif

#if defined(__linux__) || defined(PLATFORM_WEB)
    #include <sys/time.h>           // Required for: timespec, nanosleep(), select() - POSIX
#elif defined(__APPLEOSX__)
    #include <unistd.h>             // Required for: usleep()
#endif

#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    //#define GLFW_INCLUDE_NONE     // Disable the standard OpenGL header inclusion on GLFW3
    #include <GLFW/glfw3.h>         // GLFW3 library: Windows, OpenGL context and Input management

    #if defined(__linux__)
        #define GLFW_EXPOSE_NATIVE_X11   // Linux specific definitions for getting
        #define GLFW_EXPOSE_NATIVE_GLX   // native functions like glfwGetX11Window
        #include <GLFW/glfw3native.h>    // which are required for hiding mouse
    #endif
    //#include <GL/gl.h>        // OpenGL functions (GLFW3 already includes gl.h)

    #if !defined(SUPPORT_BUSY_WAIT_LOOP) && defined(_WIN32)
    // NOTE: Those functions require linking with winmm library
    unsigned int __stdcall timeBeginPeriod(unsigned int uPeriod);
    unsigned int __stdcall timeEndPeriod(unsigned int uPeriod);
    #endif
#endif

#if defined(PLATFORM_ANDROID)
    //#include <android/sensor.h>           // Android sensors functions (accelerometer, gyroscope, light...)
    #include <android/window.h>             // Defines AWINDOW_FLAG_FULLSCREEN and others
    #include <android_native_app_glue.h>    // Defines basic app state struct and manages activity

    #include <EGL/egl.h>        // Khronos EGL library - Native platform display device control functions
    #include <GLES2/gl2.h>      // Khronos OpenGL ES 2.0 library
#endif

#if defined(PLATFORM_RPI)
    #include <fcntl.h>          // POSIX file control definitions - open(), creat(), fcntl()
    #include <unistd.h>         // POSIX standard function definitions - read(), close(), STDIN_FILENO
    #include <termios.h>        // POSIX terminal control definitions - tcgetattr(), tcsetattr()
    #include <pthread.h>        // POSIX threads management (mouse input)

    #include <sys/ioctl.h>      // UNIX System call for device-specific input/output operations - ioctl()
    #include <linux/kd.h>       // Linux: KDSKBMODE, K_MEDIUMRAM constants definition
    #include <linux/input.h>    // Linux: Keycodes constants definition (KEY_A, ...)
    #include <linux/joystick.h> // Linux: Joystick support library

    #include "bcm_host.h"       // Raspberry Pi VideoCore IV access functions

    #include "EGL/egl.h"        // Khronos EGL library - Native platform display device control functions
    #include "EGL/eglext.h"     // Khronos EGL library - Extensions
    #include "GLES2/gl2.h"      // Khronos OpenGL ES 2.0 library
#endif

#if defined(PLATFORM_UWP)
    #include "EGL/egl.h"        // Khronos EGL library - Native platform display device control functions
    #include "EGL/eglext.h"     // Khronos EGL library - Extensions
    #include "GLES2/gl2.h"      // Khronos OpenGL ES 2.0 library
#endif

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
    #include <emscripten/html5.h>
#endif

#if defined(PLATFORM_SILENT_RADIANCE)
	#include "tim1.h"
	#include "packet_summary.h"
	#include "display_main.h"
	#include "display_set.h"
	#include "main_menu.h"
	#include "helper.h"
#endif
	



//tectures.c:



#define STB_PERLIN_IMPLEMENTATION
#include "external/stb_perlin.h"// Required for: stb_perlin_fbm_noise3

// Support only desired texture formats on stb_image
#if !defined(SUPPORT_FILEFORMAT_BMP)
    #define STBI_NO_BMP
#endif
#if !defined(SUPPORT_FILEFORMAT_PNG)
    #define STBI_NO_PNG
#endif
#if !defined(SUPPORT_FILEFORMAT_TGA)
    #define STBI_NO_TGA
#endif
#if !defined(SUPPORT_FILEFORMAT_JPG)
    #define STBI_NO_JPEG        // Image format .jpg and .jpeg
#endif
#if !defined(SUPPORT_FILEFORMAT_PSD)
    #define STBI_NO_PSD
#endif
#if !defined(SUPPORT_FILEFORMAT_GIF)
    #define STBI_NO_GIF
#endif
#if !defined(SUPPORT_FILEFORMAT_HDR)
    #define STBI_NO_HDR
#endif

// Image fileformats not supported by default
#define STBI_NO_PIC
#define STBI_NO_PNM             // Image format .ppm and .pgm

#if (defined(SUPPORT_FILEFORMAT_BMP) || defined(SUPPORT_FILEFORMAT_PNG) || defined(SUPPORT_FILEFORMAT_TGA) || \
     defined(SUPPORT_FILEFORMAT_JPG) || defined(SUPPORT_FILEFORMAT_PSD) || defined(SUPPORT_FILEFORMAT_GIF) || \
     defined(SUPPORT_FILEFORMAT_HDR))
    #define STB_IMAGE_IMPLEMENTATION
    #include "external/stb_image.h"     // Required for: stbi_load_from_file()
                                        // NOTE: Used to read image data (multiple formats support)
#endif
                                
#if defined(SUPPORT_IMAGE_MANIPULATION)
    #define STB_IMAGE_RESIZE_IMPLEMENTATION
    #include "external/stb_image_resize.h"  // Required for: stbir_resize_uint8()
                                            // NOTE: Used for image scaling on ImageResize()
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#if defined(PLATFORM_RPI)
    // Old device inputs system
    #define DEFAULT_KEYBOARD_DEV      STDIN_FILENO              // Standard input
    #define DEFAULT_MOUSE_DEV         "/dev/input/mouse0"       // Mouse input
    #define DEFAULT_TOUCH_DEV         "/dev/input/event4"       // Touch input virtual device (created by ts_uinput)
    #define DEFAULT_GAMEPAD_DEV       "/dev/input/js"           // Gamepad input (base dev for all gamepads: js0, js1, ...)

    // New device input events (evdev) (must be detected)
    //#define DEFAULT_KEYBOARD_DEV    "/dev/input/eventN"
    //#define DEFAULT_MOUSE_DEV       "/dev/input/eventN"
    //#define DEFAULT_GAMEPAD_DEV     "/dev/input/eventN"

    #define MOUSE_SENSITIVITY         0.8f
#endif

#define MAX_GAMEPADS              4         // Max number of gamepads supported
#define MAX_GAMEPAD_BUTTONS       32        // Max bumber of buttons supported (per gamepad)
#define MAX_GAMEPAD_AXIS          8         // Max number of axis supported (per gamepad)

#define STORAGE_FILENAME        "storage.data"

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
static GLFWwindow *window;                      // Native window (graphic device)
#endif

static bool windowReady = false;                // Check if window has been initialized successfully
static bool windowMinimized = false;            // Check if window has been minimized

#if defined(PLATFORM_ANDROID)
static struct android_app *app;                 // Android activity
static struct android_poll_source *source;      // Android events polling source
static int ident, events;                       // Android ALooper_pollAll() variables
static const char *internalDataPath;            // Android internal data path to write data (/data/data/<package>/files)

static bool appEnabled = true;                  // Used to detec if app is active
static bool contextRebindRequired = false;      // Used to know context rebind required
#endif

#if defined(PLATFORM_RPI)
static EGL_DISPMANX_WINDOW_T nativeWindow;      // Native window (graphic device)

// Keyboard input variables
// NOTE: For keyboard we will use the standard input (but reconfigured...)
static struct termios defaultKeyboardSettings;  // Used to store default keyboard settings
static int defaultKeyboardMode;                 // Used to store default keyboard mode

// Mouse input variables
static int mouseStream = -1;                    // Mouse device file descriptor
static bool mouseReady = false;                 // Flag to know if mouse is ready
static pthread_t mouseThreadId;                 // Mouse reading thread id

// Touch input variables
static int touchStream = -1;                    // Touch device file descriptor
static bool touchReady = false;                 // Flag to know if touch interface is ready
static pthread_t touchThreadId;                 // Touch reading thread id

// Gamepad input variables
static int gamepadStream[MAX_GAMEPADS] = { -1 };// Gamepad device file descriptor
static pthread_t gamepadThreadId;               // Gamepad reading thread id
static char gamepadName[64];                    // Gamepad name holder
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI) || defined(PLATFORM_UWP)
static EGLDisplay display;              // Native display device (physical screen connection)
static EGLSurface surface;              // Surface to draw on, framebuffers (connected to context)
static EGLContext context;              // Graphic context, mode in which drawing can be done
static EGLConfig config;                // Graphic config
static uint64_t baseTime;               // Base time measure for hi-res timer
static bool windowShouldClose = false;  // Flag to set window for closing
#endif


#if defined(PLATFORM_UWP)
static EGLNativeWindowType uwpWindow;
#endif


#if defined(PLATFORM_SILENT_RADIANCE)
// Should have code to check if init was called and prevent/ignore future calls
static uint64_t baseTime;               // Base time measure for hi-res timer
static bool windowShouldClose = false;  // Flag to set window for closing
#endif


// Display size-related data
static unsigned int displayWidth, displayHeight; // Display width and height (monitor, device-screen, LCD, ...)
static int screenWidth, screenHeight;       // Screen width and height (used render area)
static int renderWidth, renderHeight;       // Framebuffer width and height (render area, including black bars if required)
static int renderOffsetX = 0;               // Offset X from render area (must be divided by 2)
static int renderOffsetY = 0;               // Offset Y from render area (must be divided by 2)
static bool fullscreen = false;             // Fullscreen mode (useful only for PLATFORM_DESKTOP)
static Matrix downscaleView;                // Matrix to downscale view (in case screen size bigger than display size)
static bool cursorHidden = false;           // Track if cursor is hidden
static bool cursorOnScreen = false;         // Tracks if cursor is inside client area

#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_RPI) || defined(PLATFORM_WEB) || defined(PLATFORM_UWP) || defined(PLATFORM_SILENT_RADIANCE)
static const char *windowTitle = NULL;      // Window text title...
static int screenshotCounter = 0;           // Screenshots counter

// Register mouse states
static char previousMouseState[3] = { 0 };  // Registers previous mouse button state
static char currentMouseState[3] = { 0 };   // Registers current mouse button state
static int previousMouseWheelY = 0;         // Registers previous mouse wheel variation
static int currentMouseWheelY = 0;          // Registers current mouse wheel variation

// Register gamepads states
static bool gamepadReady[MAX_GAMEPADS] = { false };             // Flag to know if gamepad is ready
static float gamepadAxisState[MAX_GAMEPADS][MAX_GAMEPAD_AXIS];  // Gamepad axis state
static char previousGamepadState[MAX_GAMEPADS][MAX_GAMEPAD_BUTTONS];    // Previous gamepad buttons state
static char currentGamepadState[MAX_GAMEPADS][MAX_GAMEPAD_BUTTONS];     // Current gamepad buttons state

// Keyboard configuration
#define SR_KEY_ESCAPE 23342
static int exitKey = SR_KEY_ESCAPE;            // Default exit key (ESC)
#endif

// Register keyboard states
static char previousKeyState[512] = { 0 };  // Registers previous frame key state
static char currentKeyState[512] = { 0 };   // Registers current frame key state

static int lastKeyPressed = -1;             // Register last key pressed
static int lastGamepadButtonPressed = -1;   // Register last gamepad button pressed
static int gamepadAxisCount = 0;            // Register number of available gamepad axis

static Vector2 mousePosition;               // Mouse position on screen

#if defined(PLATFORM_WEB)
static bool toggleCursorLock = false;       // Ask for cursor pointer lock on next click
#endif

static Vector2 touchPosition[MAX_TOUCH_POINTS]; // Touch position on screen

#if defined(PLATFORM_DESKTOP)
static char **dropFilesPath;                // Store dropped files paths as strings
static int dropFilesCount = 0;              // Count stored strings
#endif

static double currentTime, previousTime;    // Used to track timmings
static double updateTime, drawTime;         // Time measures for update and draw
static double frameTime = 0.0;              // Time measure for one frame
static double targetTime = 0.0;             // Desired time for one frame, if 0 not applied

static unsigned char configFlags = 0;       // Configuration flags (bit based)
static bool showLogo = false;               // Track if showing logo at init is enabled

#if defined(SUPPORT_GIF_RECORDING)
static int gifFramesCounter = 0;
static bool gifRecording = false;
#endif

//----------------------------------------------------------------------------------
// Other Modules Functions Declaration (required by core)
//----------------------------------------------------------------------------------
#if defined(SUPPORT_DEFAULT_FONT)
extern void LoadDefaultFont(void);          // [Module: text] Loads default font on InitWindow()
extern void UnloadDefaultFont(void);        // [Module: text] Unloads default font from GPU memory
#endif

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static bool InitGraphicsDevice(int width, int height);  // Initialize graphics device
static void SetupFramebufferSize(int displayWidth, int displayHeight);
static void InitTimer(void);                            // Initialize timer
static void Wait(float ms);                             // Wait for some milliseconds (stop program execution)
static bool GetKeyStatus(int key);                      // Returns if a key has been pressed
static bool GetMouseButtonStatus(int button);           // Returns if a mouse button has been pressed
static void PollInputEvents(void);                      // Register user events
static void SwapBuffers(void);                          // Copy back buffer to front buffers
static void LogoAnimation(void);                        // Plays raylib logo appearing animation
static void SetupViewport(void);                        // Set viewport parameters

#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
static void ErrorCallback(int error, const char *description);                             // GLFW3 Error Callback, runs on GLFW3 error
static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);  // GLFW3 Keyboard Callback, runs on key pressed
static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);     // GLFW3 Mouse Button Callback, runs on mouse button pressed
static void MouseCursorPosCallback(GLFWwindow *window, double x, double y);                // GLFW3 Cursor Position Callback, runs on mouse move
static void CharCallback(GLFWwindow *window, unsigned int key);                            // GLFW3 Char Key Callback, runs on key pressed (get char value)
static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);            // GLFW3 Srolling Callback, runs on mouse wheel
static void CursorEnterCallback(GLFWwindow *window, int enter);                            // GLFW3 Cursor Enter Callback, cursor enters client area
static void WindowSizeCallback(GLFWwindow *window, int width, int height);                 // GLFW3 WindowSize Callback, runs when window is resized
static void WindowIconifyCallback(GLFWwindow *window, int iconified);                      // GLFW3 WindowIconify Callback, runs when window is minimized/restored
#endif

#if defined(PLATFORM_DESKTOP)
static void WindowDropCallback(GLFWwindow *window, int count, const char **paths);         // GLFW3 Window Drop Callback, runs when drop files into window
#endif

#if defined(PLATFORM_ANDROID)
static void AndroidCommandCallback(struct android_app *app, int32_t cmd);                  // Process Android activity lifecycle commands
static int32_t AndroidInputCallback(struct android_app *app, AInputEvent *event);          // Process Android inputs
#endif

#if defined(PLATFORM_WEB)
static EM_BOOL EmscriptenFullscreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData);
static EM_BOOL EmscriptenKeyboardCallback(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData);
static EM_BOOL EmscriptenMouseCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
static EM_BOOL EmscriptenTouchCallback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
static EM_BOOL EmscriptenGamepadCallback(int eventType, const EmscriptenGamepadEvent *gamepadEvent, void *userData);
#endif

#if defined(PLATFORM_RPI)
static void InitKeyboard(void);                         // Init raw keyboard system (standard input reading)
static void ProcessKeyboard(void);                      // Process keyboard events
static void RestoreKeyboard(void);                      // Restore keyboard system
static void InitMouse(void);                            // Mouse initialization (including mouse thread)
static void *MouseThread(void *arg);                    // Mouse reading thread
static void InitTouch(void);                            // Touch device initialization (including touch thread)
static void *TouchThread(void *arg);                    // Touch device reading thread
static void InitGamepad(void);                          // Init raw gamepad input
static void *GamepadThread(void *arg);                  // Mouse reading thread
#endif

#if defined(PLATFORM_UWP)
// Define functions required to manage inputs
#endif

#if defined(_WIN32)
    // NOTE: We include Sleep() function signature here to avoid windows.h inclusion
    void __stdcall Sleep(unsigned long msTimeout);      // Required for Wait()
#endif


#if defined(PLATFORM_SILENT_RADIANCE) 
static GLuint one_color_program;
static GLuint full_color_program;
static GLuint gvPositionHandle;
static GLuint mPositionHandle ;
static float width_in_pixels;
static float height_in_pixels;
static float width_in_inches;
static float height_in_inches;
static float height_to_inches;
static float rotation_in_radians;
static float relative_width;
static float relative_height;
static float width_to_inches;
static int circle_factor;
//static lcontext *lc;
static glMatrix modelMatrix;
static glMatrix effectMatrix; /* used to do effects like zoom near/zoom away */
static glMatrix viewMatrix;
static glMatrix projectionMatrix;

#endif

//----------------------------------------------------------------------------------
// Module Functions Definition - Window and OpenGL Context Functions
//----------------------------------------------------------------------------------
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_RPI) || defined(PLATFORM_WEB) || defined(PLATFORM_UWP) || defined(PLATFORM_SILENT_RADIANCE)
// Initialize window and OpenGL context
// NOTE: data parameter could be used to pass any kind of required data to the initialization
void InitWindow(int width, int height, void *data)
{
#if defined(PLATFORM_SILENT_RADIANCE) 
    loadIdentity(&modelMatrix);  
    rotate_apple_only(screen_orientation,&modelMatrix);
    loadIdentity(&effectMatrix);  
    loadIdentity(&viewMatrix);  
    loadIdentity(&projectionMatrix);  
    pass_menu_setup_info(
 	&width_in_pixels,
 	&height_in_pixels,
 	&width_in_inches,
 	&height_in_inches,
 	&height_to_inches,
 	&rotation_in_radians,
 	&relative_width,
 	&relative_height,
 	&width_to_inches,
 	&circle_factor,
 	&one_color_program,
 	&full_color_program,
 	&gvPositionHandle,
  	&projectionMatrix,
  	&mPositionHandle
 );


    
    
  width = width;
  height = height;
  if (ran_setup_graphics_once_before    ) {
    return;
    }
#endif  
      TraceLog(LOG_INFO, "Initializing raylib (v1.9.4-dev)");

#if defined(PLATFORM_DESKTOP)
    windowTitle = (char *)data;
#endif

#if defined(PLATFORM_UWP)
    uwpWindow = (EGLNativeWindowType)data;
#endif

    // Init hi-res timer
    InitTimer();

    // Init graphics device (display device and OpenGL context)
    // NOTE: returns true if window and graphic device has been initialized successfully
    windowReady = InitGraphicsDevice(width, height);
    if (!windowReady) return;

#if defined(SUPPORT_DEFAULT_FONT)
    // Load default font
    // NOTE: External function (defined in module: text)
    LoadDefaultFont();
#endif
logit("j\n");
#if defined(PLATFORM_RPI)
    // Init raw input system
    InitMouse();        // Mouse init
    InitTouch();        // Touch init
    InitKeyboard();     // Keyboard init
    InitGamepad();      // Gamepad init
#endif

#if defined(PLATFORM_WEB)
    emscripten_set_fullscreenchange_callback(0, 0, 1, EmscriptenFullscreenChangeCallback);
    
    // Support keyboard events
    emscripten_set_keypress_callback("#canvas", NULL, 1, EmscriptenKeyboardCallback);
    
    // Support mouse events
    emscripten_set_click_callback("#canvas", NULL, 1, EmscriptenMouseCallback);

    // Support touch events
    emscripten_set_touchstart_callback("#canvas", NULL, 1, EmscriptenTouchCallback);
    emscripten_set_touchend_callback("#canvas", NULL, 1, EmscriptenTouchCallback);
    emscripten_set_touchmove_callback("#canvas", NULL, 1, EmscriptenTouchCallback);
    emscripten_set_touchcancel_callback("#canvas", NULL, 1, EmscriptenTouchCallback);
    //emscripten_set_touchstart_callback(0, NULL, 1, Emscripten_HandleTouch);
    //emscripten_set_touchend_callback("#canvas", data, 0, Emscripten_HandleTouch);

    // Support gamepad events (not provided by GLFW3 on emscripten)
    emscripten_set_gamepadconnected_callback(NULL, 1, EmscriptenGamepadCallback);
    emscripten_set_gamepaddisconnected_callback(NULL, 1, EmscriptenGamepadCallback);
#endif

    mousePosition.x = (float)screenWidth/2.0f;
    mousePosition.y = (float)screenHeight/2.0f;

    // raylib logo appearing animation (if enabled)
    if (showLogo)
    {
        SetTargetFPS(60);
        LogoAnimation();
    }
logit("k ok\n");
}
#endif

#if defined(PLATFORM_ANDROID)
// Initialize window and OpenGL context (and Android activity)
// NOTE: data parameter could be used to pass any kind of required data to the initialization
void InitWindow(int width, int height, void *data)
{
    TraceLog(LOG_INFO, "Initializing raylib (v1.9.4-dev)");

    screenWidth = width;
    screenHeight = height;

    // Input data is android app pointer
    app = (struct android_app *)data;
    internalDataPath = app->activity->internalDataPath;

    // Set desired windows flags before initializing anything
    ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_FULLSCREEN, 0);  //AWINDOW_FLAG_SCALED, AWINDOW_FLAG_DITHER
    //ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_FORCE_NOT_FULLSCREEN, AWINDOW_FLAG_FULLSCREEN);

    int orientation = AConfiguration_getOrientation(app->config);

    if (orientation == ACONFIGURATION_ORIENTATION_PORT) TraceLog(LOG_INFO, "PORTRAIT window orientation");
    else if (orientation == ACONFIGURATION_ORIENTATION_LAND) TraceLog(LOG_INFO, "LANDSCAPE window orientation");

    // TODO: Automatic orientation doesn't seem to work
    if (width <= height)
    {
        AConfiguration_setOrientation(app->config, ACONFIGURATION_ORIENTATION_PORT);
        TraceLog(LOG_WARNING, "Window set to portraid mode");
    }
    else
    {
        AConfiguration_setOrientation(app->config, ACONFIGURATION_ORIENTATION_LAND);
        TraceLog(LOG_WARNING, "Window set to landscape mode");
    }

    //AConfiguration_getDensity(app->config);
    //AConfiguration_getKeyboard(app->config);
    //AConfiguration_getScreenSize(app->config);
    //AConfiguration_getScreenLong(app->config);

    app->onAppCmd = AndroidCommandCallback;
    app->onInputEvent = AndroidInputCallback;

    InitAssetManager(app->activity->assetManager);

    TraceLog(LOG_INFO, "Android app initialized successfully");

    // Wait for window to be initialized (display and context)
    while (!windowReady)
    {
        // Process events loop
        while ((ident = ALooper_pollAll(0, NULL, &events,(void**)&source)) >= 0)
        {
            // Process this event
            if (source != NULL) source->process(app, source);

            // NOTE: Never close window, native activity is controlled by the system!
            //if (app->destroyRequested != 0) windowShouldClose = true;
        }
    }
}
#endif

// Close window and unload OpenGL context
void CloseWindow(void)
{
#if defined(SUPPORT_GIF_RECORDING)
    if (gifRecording)
    {
        GifEnd();
        gifRecording = false;
    }
#endif
#if defined(PLATFORM_SILENT_RADIANCE)
  return;
#endif
    
#if defined(SUPPORT_DEFAULT_FONT)
    UnloadDefaultFont();
#endif

    rlglClose();                // De-init rlgl

#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    glfwDestroyWindow(window);
    glfwTerminate();
#endif

#if !defined(SUPPORT_BUSY_WAIT_LOOP) && defined(_WIN32)
    timeEndPeriod(1);           // Restore time period
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI) || defined(PLATFORM_UWP)
    // Close surface, context and display
    if (display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(display, surface);
            surface = EGL_NO_SURFACE;
        }

        if (context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(display, context);
            context = EGL_NO_CONTEXT;
        }

        eglTerminate(display);
        display = EGL_NO_DISPLAY;
    }
#endif

#if defined(PLATFORM_RPI)
    // Wait for mouse and gamepad threads to finish before closing
    // NOTE: Those threads should already have finished at this point
    // because they are controlled by windowShouldClose variable

    windowShouldClose = true;   // Added to force threads to exit when the close window is called

    pthread_join(mouseThreadId, NULL);
    pthread_join(touchThreadId, NULL);
    pthread_join(gamepadThreadId, NULL);
#endif

    TraceLog(LOG_INFO, "Window closed successfully");
}

// Check if window has been initialized successfully
bool IsWindowReady(void)
{
    return windowReady;
}

// Check if KEY_ESCAPE pressed or Close icon pressed
bool WindowShouldClose(void)
{
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    if (windowReady)
    {
        // While window minimized, stop loop execution
        while (windowMinimized) glfwWaitEvents();

        return (glfwWindowShouldClose(window));
    }
    else return true;
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI) || defined(PLATFORM_UWP) 
    if (windowReady) return windowShouldClose;
    else return true;
#endif
#if defined(PLATFORM_SILENT_RADIANCE)
return false;
#endif
}

// Check if window has been minimized (or lost focus)
bool IsWindowMinimized(void)
{
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB) || defined(PLATFORM_UWP)
    return windowMinimized;
#else
    return false;
#endif
}

// Toggle fullscreen mode (only PLATFORM_DESKTOP)
void ToggleFullscreen(void)
{
#if defined(PLATFORM_DESKTOP)
    fullscreen = !fullscreen;          // Toggle fullscreen flag

    // NOTE: glfwSetWindowMonitor() doesn't work properly (bugs)
    if (fullscreen) glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, screenWidth, screenHeight, GLFW_DONT_CARE);
    else glfwSetWindowMonitor(window, NULL, 0, 0, screenWidth, screenHeight, GLFW_DONT_CARE);
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI)
    TraceLog(LOG_WARNING, "Could not toggle to windowed mode");
#endif
}

// Set icon for window (only PLATFORM_DESKTOP)
void SetWindowIcon(Image image)
{
#if defined(PLATFORM_DESKTOP)
    ImageFormat(&image, UNCOMPRESSED_R8G8B8A8);

    GLFWimage icon[1];
    
    icon[0].width = image.width;
    icon[0].height = image.height;
    icon[0].pixels = (unsigned char *)image.data;

    // NOTE: We only support one image icon
    glfwSetWindowIcon(window, 1, icon);

    // TODO: Support multi-image icons --> image.mipmaps
#endif
}

// Set title for window (only PLATFORM_DESKTOP)
void SetWindowTitle(const char *title)
{
#if defined(PLATFORM_DESKTOP)
    glfwSetWindowTitle(window, title);
#endif
}

// Set window position on screen (windowed mode)
void SetWindowPosition(int x, int y)
{
#if defined(PLATFORM_DESKTOP)
    glfwSetWindowPos(window, x, y);
#endif
}

// Set monitor for the current window (fullscreen mode)
void SetWindowMonitor(int monitor)
{
#if defined(PLATFORM_DESKTOP)
    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    
    if ((monitor >= 0) && (monitor < monitorCount)) 
    {
        //glfwSetWindowMonitor(window, monitors[monitor], 0, 0, screenWidth, screenHeight, GLFW_DONT_CARE);
        TraceLog(LOG_INFO, "Selected fullscreen monitor: [%i] %s", monitor, glfwGetMonitorName(monitors[monitor]));
    }
    else TraceLog(LOG_WARNING, "Selected monitor not found");
#endif
}

// Set window minimum dimensions (FLAG_WINDOW_RESIZABLE)
void SetWindowMinSize(int width, int height)
{
#if defined(PLATFORM_DESKTOP)
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowSizeLimits(window, width, height, mode->width, mode->height);
#endif
}

// Get current screen width
int GetScreenWidth(void)
{
    return screenWidth;
}

// Get current screen height
int GetScreenHeight(void)
{
    return screenHeight;
}

// Show mouse cursor
void ShowCursor()
{
#if defined(PLATFORM_DESKTOP)
    #if defined(__linux__)
        XUndefineCursor(glfwGetX11Display(), glfwGetX11Window(window));
    #else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    #endif
#endif
    cursorHidden = false;
}

// Hides mouse cursor
void HideCursor()
{
#if defined(PLATFORM_DESKTOP)
    #if defined(__linux__)
        XColor col;
        const char nil[] = {0};

        Pixmap pix = XCreateBitmapFromData(glfwGetX11Display(), glfwGetX11Window(window), nil, 1, 1);
        Cursor cur = XCreatePixmapCursor(glfwGetX11Display(), pix, pix, &col, &col, 0, 0);

        XDefineCursor(glfwGetX11Display(), glfwGetX11Window(window), cur);
        XFreeCursor(glfwGetX11Display(), cur);
    #else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    #endif
#endif
    cursorHidden = true;
}

// Check if cursor is not visible
bool IsCursorHidden()
{
    return cursorHidden;
}

// Enables cursor (unlock cursor)
void EnableCursor()
{
#if defined(PLATFORM_DESKTOP)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif
#if defined(PLATFORM_WEB)
    toggleCursorLock = true;
#endif
    cursorHidden = false;
}

// Disables cursor (lock cursor)
void DisableCursor()
{
#if defined(PLATFORM_DESKTOP)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
#if defined(PLATFORM_WEB)
    toggleCursorLock = true;
#endif
    cursorHidden = true;
}

// Set background color (framebuffer clear color)
void ClearBackground(Color color)
{
#ifndef PLATFORM_SILENT_RADIANCE
// we just draw, the background is not ours to control
    rlClearColor(color.r, color.g, color.b, color.a);   // Set clear color
    rlClearScreenBuffers();                             // Clear current framebuffers
#endif
}


#ifdef PLATFORM_SILENT_RADIANCE
// Set background color (framebuffer clear color)
void ClearBackgroundForReals(Color color)
{
// we just draw, the background is not ours to control
    rlClearColor(color.r, color.g, color.b, color.a);   // Set clear color
    rlClearScreenBuffers();                             // Clear current framebuffers
}
#endif


#ifdef PLATFORM_SILENT_RADIANCE
static int begun_drawing=0;
#endif


extern void rlglSetupDefaultStates(void);

// Setup canvas (framebuffer) to start drawing
void BeginDrawing(void)
{
#ifdef PLATFORM_SILENT_RADIANCE
// BeginDrawing might be called by many independent raylib interfaces - many games or visualiz4ers
// so we need to handle, for SILENT_RADIANCE, this by doing the init stuff only once
rlglSetupDefaultStates();
if (begun_drawing) {
  begun_drawing++;
  return;
  }
else {
  begun_drawing++;
  }
#endif
    currentTime = GetTime();            // Number of elapsed seconds since InitTimer()
    updateTime = currentTime - previousTime;
    previousTime = currentTime;

    rlLoadIdentity();                   // Reset current matrix (MODELVIEW)
    rlMultMatrixf(MatrixToFloat(downscaleView));       // If downscale required, apply it here

    //rlTranslatef(0.375, 0.375, 0);    // HACK to have 2D pixel-perfect drawing on OpenGL 1.1
                                        // NOTE: Not required with OpenGL 3.3+
}

extern void rlglKindaResroreFromDefaultStates(void);

// End canvas drawing and swap buffers (double buffering)
void EndDrawing(void)
{
#ifndef PLATFORM_SILENT_RADIANCE
    rlglDraw();                     // Draw Buffers (Only OpenGL 3+ and ES2)

#if defined(SUPPORT_GIF_RECORDING)

    #define GIF_RECORD_FRAMERATE    10

    if (gifRecording)
    {
        gifFramesCounter++;
        
        // NOTE: We record one gif frame every 10 game frames
        if ((gifFramesCounter%GIF_RECORD_FRAMERATE) == 0)
        {
            // Get image data for the current frame (from backbuffer)
            // NOTE: This process is very slow... :(
            unsigned char *screenData = rlReadScreenPixels(screenWidth, screenHeight);
            GifWriteFrame(screenData, screenWidth, screenHeight, 10, 8, false);
            
            free(screenData);   // Free image data
        }
        
        if (((gifFramesCounter/15)%2) == 1)
        {
            DrawCircle(30, screenHeight - 20, 10, RED);
            DrawText("RECORDING", 50, screenHeight - 25, 10, MAROON);
        }
        
        rlglDraw();                 // Draw RECORDING message
    }
#endif
logit("swappy\n");   
    SwapBuffers();                  // Copy back buffer to front buffer
    PollInputEvents();              // Poll user events
#endif


#ifdef PLATFORM_SILENT_RADIANCE
rlglDraw();
begun_drawing--;
rlglKindaResroreFromDefaultStates();
if (begun_drawing>0) {
  return;
  }
if (begun_drawing<0) {
  begun_drawing=0;
  return;
  }
// When begun_drawing is 0, we will do the freme time stuff
#endif  
    // Frame time control system
    currentTime = GetTime();
    drawTime = currentTime - previousTime;
    previousTime = currentTime;
    
    frameTime = updateTime + drawTime;

    // Wait for some milliseconds...
    if (frameTime < targetTime)
    {
#ifndef PLATFORM_SILENT_RADIANCE    
        Wait((targetTime - frameTime)*1000.0f);
// waits are handled elsewhere in silentRadiance	
#endif	

        currentTime = GetTime();
        double extraTime = currentTime - previousTime;
        previousTime = currentTime;

        frameTime += extraTime;
    }

}

// Initialize 2D mode with custom camera (2D)
void Begin2dMode(Camera2D camera)
{
    rlglDraw();                         // Draw Buffers (Only OpenGL 3+ and ES2)

    rlLoadIdentity();                   // Reset current matrix (MODELVIEW)

    // Camera rotation and scaling is always relative to target
    Matrix matOrigin = MatrixTranslate(-camera.target.x, -camera.target.y, 0.0f);
    Matrix matRotation = MatrixRotate((Vector3){ 0.0f, 0.0f, 1.0f }, camera.rotation*DEG2RAD);
    Matrix matScale = MatrixScale(camera.zoom, camera.zoom, 1.0f);
    Matrix matTranslation = MatrixTranslate(camera.offset.x + camera.target.x, camera.offset.y + camera.target.y, 0.0f);

    Matrix matTransform = MatrixMultiply(MatrixMultiply(matOrigin, MatrixMultiply(matScale, matRotation)), matTranslation);

    rlMultMatrixf(MatrixToFloat(matTransform));
}

// Ends 2D mode with custom camera
void End2dMode(void)
{
    rlglDraw();                         // Draw Buffers (Only OpenGL 3+ and ES2)

    rlLoadIdentity();                   // Reset current matrix (MODELVIEW)
}

// Initializes 3D mode with custom camera (3D)
void Begin3dMode(Camera camera)
{
    rlglDraw();                         // Draw Buffers (Only OpenGL 3+ and ES2)
    
    rlMatrixMode(RL_PROJECTION);        // Switch to projection matrix
    rlPushMatrix();                     // Save previous matrix, which contains the settings for the 2d ortho projection
    rlLoadIdentity();                   // Reset current matrix (PROJECTION)

    // Setup perspective projection
    float aspect = (float)screenWidth/(float)screenHeight;
    double top = 0.01*tan(camera.fovy*0.5*DEG2RAD);
    double right = top*aspect;

    // NOTE: zNear and zFar values are important when computing depth buffer values
    rlFrustum(-right, right, -top, top, 0.01, 1000.0);

    rlMatrixMode(RL_MODELVIEW);         // Switch back to modelview matrix
    rlLoadIdentity();                   // Reset current matrix (MODELVIEW)

    // Setup Camera view
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);
    rlMultMatrixf(MatrixToFloat(matView));      // Multiply MODELVIEW matrix by view matrix (camera)

    rlEnableDepthTest();                // Enable DEPTH_TEST for 3D
}

// Ends 3D mode and returns to default 2D orthographic mode
void End3dMode(void)
{
    rlglDraw();                         // Process internal buffers (update + draw)

    rlMatrixMode(RL_PROJECTION);        // Switch to projection matrix
    rlPopMatrix();                      // Restore previous matrix (PROJECTION) from matrix stack

    rlMatrixMode(RL_MODELVIEW);         // Get back to modelview matrix
    rlLoadIdentity();                   // Reset current matrix (MODELVIEW)

    rlDisableDepthTest();               // Disable DEPTH_TEST for 2D
}

// Initializes render texture for drawing
void BeginTextureMode(RenderTexture2D target)
{
    rlglDraw();                         // Draw Buffers (Only OpenGL 3+ and ES2)

    rlEnableRenderTexture(target.id);   // Enable render target
    
    rlClearScreenBuffers();             // Clear render texture buffers

    // Set viewport to framebuffer size
    rlViewport(0, 0, target.texture.width, target.texture.height);

    rlMatrixMode(RL_PROJECTION);        // Switch to PROJECTION matrix
    rlLoadIdentity();                   // Reset current matrix (PROJECTION)

    // Set orthographic projection to current framebuffer size
    // NOTE: Configured top-left corner as (0, 0)
    rlOrtho(0, target.texture.width, target.texture.height, 0, 0.0f, 1.0f);

    rlMatrixMode(RL_MODELVIEW);         // Switch back to MODELVIEW matrix
    rlLoadIdentity();                   // Reset current matrix (MODELVIEW)

    //rlScalef(0.0f, -1.0f, 0.0f);      // Flip Y-drawing (?)
}

// Ends drawing to render texture
void EndTextureMode(void)
{
    rlglDraw();                         // Draw Buffers (Only OpenGL 3+ and ES2)

    rlDisableRenderTexture();           // Disable render target

    // Set viewport to default framebuffer size (screen size)
    SetupViewport();

    rlMatrixMode(RL_PROJECTION);        // Switch to PROJECTION matrix
    rlLoadIdentity();                   // Reset current matrix (PROJECTION)

    // Set orthographic projection to current framebuffer size
    // NOTE: Configured top-left corner as (0, 0)
    rlOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, 0.0f, 1.0f);

    rlMatrixMode(RL_MODELVIEW);         // Switch back to MODELVIEW matrix
    rlLoadIdentity();                   // Reset current matrix (MODELVIEW)
}

// Returns a ray trace from mouse position
Ray GetMouseRay(Vector2 mousePosition, Camera camera)
{
    Ray ray;

    // Calculate normalized device coordinates
    // NOTE: y value is negative
    float x = (2.0f*mousePosition.x)/(float)GetScreenWidth() - 1.0f;
    float y = 1.0f - (2.0f*mousePosition.y)/(float)GetScreenHeight();
    float z = 1.0f;

    // Store values in a vector
    Vector3 deviceCoords = { x, y, z };

    TraceLog(LOG_DEBUG, "Device coordinates: (%f, %f, %f)", deviceCoords.x, deviceCoords.y, deviceCoords.z);

    // Calculate projection matrix from perspective
    Matrix matProj = MatrixPerspective(camera.fovy*DEG2RAD, ((double)GetScreenWidth()/(double)GetScreenHeight()), 0.01, 1000.0);

    // Calculate view matrix from camera look at
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

    // Unproject far/near points
    Vector3 nearPoint = rlUnproject((Vector3){ deviceCoords.x, deviceCoords.y, 0.0f }, matProj, matView);
    Vector3 farPoint = rlUnproject((Vector3){ deviceCoords.x, deviceCoords.y, 1.0f }, matProj, matView);

    // Calculate normalized direction vector
    Vector3 direction = Vector3Subtract(farPoint, nearPoint);
    Vector3Normalize(&direction);

    // Apply calculated vectors to ray
    ray.position = camera.position;
    ray.direction = direction;

    return ray;
}

// Returns the screen space position from a 3d world space position
Vector2 GetWorldToScreen(Vector3 position, Camera camera)
{
    // Calculate projection matrix (from perspective instead of frustum
    Matrix matProj = MatrixPerspective(camera.fovy*DEG2RAD, (double)GetScreenWidth()/(double)GetScreenHeight(), 0.01, 1000.0);

    // Calculate view matrix from camera look at (and transpose it)
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

    // Convert world position vector to quaternion
    Quaternion worldPos = { position.x, position.y, position.z, 1.0f };

    // Transform world position to view
    QuaternionTransform(&worldPos, matView);

    // Transform result to projection (clip space position)
    QuaternionTransform(&worldPos, matProj);

    // Calculate normalized device coordinates (inverted y)
    Vector3 ndcPos = { worldPos.x/worldPos.w, -worldPos.y/worldPos.w, worldPos.z/worldPos.w };

    // Calculate 2d screen position vector
    Vector2 screenPosition = { (ndcPos.x + 1.0f)/2.0f*(float)GetScreenWidth(), (ndcPos.y + 1.0f)/2.0f*(float)GetScreenHeight() };

    return screenPosition;
}

// Get transform matrix for camera
Matrix GetCameraMatrix(Camera camera)
{
    return MatrixLookAt(camera.position, camera.target, camera.up);
}

// Set target FPS (maximum)
void SetTargetFPS(int fps)
{
    if (fps < 1) targetTime = 0.0;
    else targetTime = 1.0/(double)fps;

    TraceLog(LOG_INFO, "Target time per frame: %02.03f milliseconds", (float)targetTime*1000);
}

// Returns current FPS
int GetFPS(void)
{
    return (int)(1.0f/GetFrameTime());
}

// Returns time in seconds for last frame drawn
float GetFrameTime(void)
{
    // NOTE: We round value to milliseconds
    return (float)frameTime;
}

// Get elapsed time measure in seconds since InitTimer()
// NOTE: On PLATFORM_DESKTOP InitTimer() is called on InitWindow()
// NOTE: On PLATFORM_DESKTOP, timer is initialized on glfwInit()
double GetTime(void)
{
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    return glfwGetTime();                   // Elapsed time since glfwInit()
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI) || defined(PLATFORM_SILENT_RADIANCE)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t time = (uint64_t)ts.tv_sec*1000000000LLU + (uint64_t)ts.tv_nsec;

    return (double)(time - baseTime)*1e-9;  // Elapsed time since InitTimer()
#endif
}

// Converts Color to float array and normalizes
float *ColorToFloat(Color color)
{
    static float buffer[4];

    buffer[0] = (float)color.r/255;
    buffer[1] = (float)color.g/255;
    buffer[2] = (float)color.b/255;
    buffer[3] = (float)color.a/255;

    return buffer;
}

// Returns a Color struct from hexadecimal value
Color GetColor(int hexValue)
{
    Color color;

    color.r = (unsigned char)(hexValue >> 24) & 0xFF;
    color.g = (unsigned char)(hexValue >> 16) & 0xFF;
    color.b = (unsigned char)(hexValue >> 8) & 0xFF;
    color.a = (unsigned char)hexValue & 0xFF;

    return color;
}

// Returns hexadecimal value for a Color
int GetHexValue(Color color)
{
    return (((int)color.r << 24) | ((int)color.g << 16) | ((int)color.b << 8) | (int)color.a);
}

// Returns a random value between min and max (both included)
int GetRandomValue(int min, int max)
{
    if (min > max)
    {
        int tmp = max;
        max = min;
        min = tmp;
    }

    return (rand()%(abs(max-min)+1) + min);
}

// Color fade-in or fade-out, alpha goes from 0.0f to 1.0f
Color Fade(Color color, float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    else if (alpha > 1.0f) alpha = 1.0f;

    return (Color){color.r, color.g, color.b, (unsigned char)(255.0f*alpha)};
}

// Activate raylib logo at startup (can be done with flags)
void ShowLogo(void)
{
    showLogo = true;
}

// Setup window configuration flags (view FLAGS)
void SetConfigFlags(unsigned char flags)
{
    configFlags = flags;

    if (configFlags & FLAG_SHOW_LOGO) showLogo = true;
    if (configFlags & FLAG_FULLSCREEN_MODE) fullscreen = true;
}

// NOTE TraceLog() function is located in [utils.h]

// Takes a screenshot of current screen (saved a .png)
void TakeScreenshot(const char *fileName)
{
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_RPI)
    unsigned char *imgData = rlReadScreenPixels(renderWidth, renderHeight);
    SavePNG(fileName, imgData, renderWidth, renderHeight, 4); // Save image as PNG
    free(imgData);

    TraceLog(LOG_INFO, "Screenshot taken: %s", fileName);
#endif
}

// Check file extension
bool IsFileExtension(const char *fileName, const char *ext)
{
    bool result = false;
    const char *fileExt;
    
    if ((fileExt = strrchr(fileName, '.')) != NULL)
    {
        if (strcmp(fileExt, ext) == 0) result = true;
    }

    return result;
}

// Get pointer to extension for a filename string
const char *GetExtension(const char *fileName)
{
    const char *dot = strrchr(fileName, '.');
    
    if (!dot || dot == fileName) return "";
    
    return (dot + 1);
}

// Get pointer to filename for a path string
const char *GetFileName(const char *filePath)
{
    const char *fileName = strrchr(filePath, '\\');
    
    if (!fileName || fileName == filePath) return filePath;

    return fileName + 1;
}


// Get directory for a given fileName (with path)
const char *GetDirectoryPath(const char *fileName)
{
    char *lastSlash = NULL;
    static char filePath[256];      // MAX_DIRECTORY_PATH_SIZE = 256
    memset(filePath, 0, 256);
    
    lastSlash = strrchr(fileName, '\\');
    strncpy(filePath, fileName, strlen(fileName) - (strlen(lastSlash) - 1));
    filePath[strlen(fileName) - strlen(lastSlash)] = '\0';
    
    return filePath;
}

// Get current working directory
const char *GetWorkingDirectory(void)
{
    static char currentDir[256];    // MAX_DIRECTORY_PATH_SIZE = 256
    memset(currentDir, 0, 256);
    
    GETCWD(currentDir, 256 - 1);
    
    return currentDir;
}

// Change working directory, returns true if success
bool ChangeDirectory(const char *dir)
{
    return (CHDIR(dir) == 0);
}

#if defined(PLATFORM_DESKTOP)
// Check if a file has been dropped into window
bool IsFileDropped(void)
{
    if (dropFilesCount > 0) return true;
    else return false;
}

// Get dropped files names
char **GetDroppedFiles(int *count)
{
    *count = dropFilesCount;
    return dropFilesPath;
}

// Clear dropped files paths buffer
void ClearDroppedFiles(void)
{
    if (dropFilesCount > 0)
    {
        for (int i = 0; i < dropFilesCount; i++) free(dropFilesPath[i]);

        free(dropFilesPath);

        dropFilesCount = 0;
    }
}
#endif

// Save integer value to storage file (to defined position)
// NOTE: Storage positions is directly related to file memory layout (4 bytes each integer)
void StorageSaveValue(int position, int value)
{
    FILE *storageFile = NULL;

    char path[128];
#if defined(PLATFORM_ANDROID)
    strcpy(path, internalDataPath);
    strcat(path, "/");
    strcat(path, STORAGE_FILENAME);
#else
    strcpy(path, STORAGE_FILENAME);
#endif

    // Try open existing file to append data
    storageFile = fopen(path, "rb+");

    // If file doesn't exist, create a new storage data file
    if (!storageFile) storageFile = fopen(path, "wb");

    if (!storageFile) TraceLog(LOG_WARNING, "Storage data file could not be created");
    else
    {
        // Get file size
        fseek(storageFile, 0, SEEK_END);
        int fileSize = ftell(storageFile);  // Size in bytes
        fseek(storageFile, 0, SEEK_SET);

        if (fileSize < (position*4)) TraceLog(LOG_WARNING, "Storage position could not be found");
        else
        {
            fseek(storageFile, (position*4), SEEK_SET);
            fwrite(&value, 1, 4, storageFile);
        }

        fclose(storageFile);
    }
}

// Load integer value from storage file (from defined position)
// NOTE: If requested position could not be found, value 0 is returned
int StorageLoadValue(int position)
{
    int value = 0;

    char path[128];
#if defined(PLATFORM_ANDROID)
    strcpy(path, internalDataPath);
    strcat(path, "/");
    strcat(path, STORAGE_FILENAME);
#else
    strcpy(path, STORAGE_FILENAME);
#endif

    // Try open existing file to append data
    FILE *storageFile = fopen(path, "rb");

    if (!storageFile) TraceLog(LOG_WARNING, "Storage data file could not be found");
    else
    {
        // Get file size
        fseek(storageFile, 0, SEEK_END);
        int fileSize = ftell(storageFile);      // Size in bytes
        rewind(storageFile);

        if (fileSize < (position*4)) TraceLog(LOG_WARNING, "Storage position could not be found");
        else
        {
            fseek(storageFile, (position*4), SEEK_SET);
            fread(&value, 4, 1, storageFile);   // Read 1 element of 4 bytes size
        }

        fclose(storageFile);
    }

    return value;
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Input (Keyboard, Mouse, Gamepad) Functions
//----------------------------------------------------------------------------------
// Detect if a key has been pressed once
bool IsKeyPressed(int key)
{
    bool pressed = false;

    if ((currentKeyState[key] != previousKeyState[key]) && (currentKeyState[key] == 1)) pressed = true;
    else pressed = false;

    return pressed;
}

// Detect if a key is being pressed (key held down)
bool IsKeyDown(int key)
{
    if (GetKeyStatus(key) == 1) return true;
    else return false;
}

// Detect if a key has been released once
bool IsKeyReleased(int key)
{
    bool released = false;

    if ((currentKeyState[key] != previousKeyState[key]) && (currentKeyState[key] == 0)) released = true;
    else released = false;

    return released;
}

// Detect if a key is NOT being pressed (key not held down)
bool IsKeyUp(int key)
{
    if (GetKeyStatus(key) == 0) return true;
    else return false;
}

// Get the last key pressed
int GetKeyPressed(void)
{
    return lastKeyPressed;
}

// Set a custom key to exit program
// NOTE: default exitKey is ESCAPE
void SetExitKey(int key)
{
#ifndef PLATFORM_SILENT_RADIANCE
#if !defined(PLATFORM_ANDROID)
    exitKey = key;
#endif
#endif
}

// NOTE: Gamepad support not implemented in emscripten GLFW3 (PLATFORM_WEB)

// Detect if a gamepad is available
bool IsGamepadAvailable(int gamepad)
{
    bool result = false;

#if !defined(PLATFORM_ANDROID)
    if ((gamepad < MAX_GAMEPADS) && gamepadReady[gamepad]) result = true;
#endif

    return result;
}

// Check gamepad name (if available)
bool IsGamepadName(int gamepad, const char *name)
{
    bool result = false;

#if !defined(PLATFORM_ANDROID)
    const char *gamepadName = NULL;

    if (gamepadReady[gamepad]) gamepadName = GetGamepadName(gamepad);
    if ((name != NULL) && (gamepadName != NULL)) result = (strcmp(name, gamepadName) == 0);
#endif

    return result;
}

// Return gamepad internal name id
const char *GetGamepadName(int gamepad)
{
#if defined(PLATFORM_DESKTOP)
    if (gamepadReady[gamepad]) return glfwGetJoystickName(gamepad);
    else return NULL;
#elif defined(PLATFORM_RPI)
    if (gamepadReady[gamepad]) ioctl(gamepadStream[gamepad], JSIOCGNAME(64), &gamepadName);

    return gamepadName;
#else
    return NULL;
#endif
}

// Return gamepad axis count
int GetGamepadAxisCount(int gamepad)
{
#if defined(PLATFORM_RPI)
    int axisCount = 0;
    if (gamepadReady[gamepad]) ioctl(gamepadStream[gamepad], JSIOCGAXES, &axisCount);
    gamepadAxisCount = axisCount;
#endif
    return gamepadAxisCount;
}

// Return axis movement vector for a gamepad
float GetGamepadAxisMovement(int gamepad, int axis)
{
    float value = 0;

#if !defined(PLATFORM_ANDROID)
    if ((gamepad < MAX_GAMEPADS) && gamepadReady[gamepad] && (axis < MAX_GAMEPAD_AXIS)) value = gamepadAxisState[gamepad][axis];
#endif

    return value;
}

// Detect if a gamepad button has been pressed once
bool IsGamepadButtonPressed(int gamepad, int button)
{
    bool pressed = false;

#if !defined(PLATFORM_ANDROID)
    if ((gamepad < MAX_GAMEPADS) && gamepadReady[gamepad] && (button < MAX_GAMEPAD_BUTTONS) &&
        (currentGamepadState[gamepad][button] != previousGamepadState[gamepad][button]) &&
        (currentGamepadState[gamepad][button] == 1)) pressed = true;
#endif

    return pressed;
}

// Detect if a gamepad button is being pressed
bool IsGamepadButtonDown(int gamepad, int button)
{
    bool result = false;

#if !defined(PLATFORM_ANDROID)
    if ((gamepad < MAX_GAMEPADS) && gamepadReady[gamepad] && (button < MAX_GAMEPAD_BUTTONS) &&
        (currentGamepadState[gamepad][button] == 1)) result = true;
#endif

    return result;
}

// Detect if a gamepad button has NOT been pressed once
bool IsGamepadButtonReleased(int gamepad, int button)
{
    bool released = false;

#if !defined(PLATFORM_ANDROID)
    if ((gamepad < MAX_GAMEPADS) && gamepadReady[gamepad] && (button < MAX_GAMEPAD_BUTTONS) &&
        (currentGamepadState[gamepad][button] != previousGamepadState[gamepad][button]) &&
        (currentGamepadState[gamepad][button] == 0)) released = true;
#endif

    return released;
}

// Detect if a mouse button is NOT being pressed
bool IsGamepadButtonUp(int gamepad, int button)
{
    bool result = false;

#if !defined(PLATFORM_ANDROID)
    if ((gamepad < MAX_GAMEPADS) && gamepadReady[gamepad] && (button < MAX_GAMEPAD_BUTTONS) &&
        (currentGamepadState[gamepad][button] == 0)) result = true;
#endif

    return result;
}

// Get the last gamepad button pressed
int GetGamepadButtonPressed(void)
{
    return lastGamepadButtonPressed;
}

// Detect if a mouse button has been pressed once
bool IsMouseButtonPressed(int button)
{
    bool pressed = false;

// TODO: Review, gestures could be not supported despite being on Android platform!
#if defined(PLATFORM_ANDROID)
    if (IsGestureDetected(GESTURE_TAP)) pressed = true;
#else
    if ((currentMouseState[button] != previousMouseState[button]) && (currentMouseState[button] == 1)) pressed = true;
#endif

    return pressed;
}

// Detect if a mouse button is being pressed
bool IsMouseButtonDown(int button)
{
    bool down = false;

#if defined(PLATFORM_ANDROID)
    if (IsGestureDetected(GESTURE_HOLD)) down = true;
#else
    if (GetMouseButtonStatus(button) == 1) down = true;
#endif

    return down;
}

// Detect if a mouse button has been released once
bool IsMouseButtonReleased(int button)
{
    bool released = false;

#if !defined(PLATFORM_ANDROID)
    if ((currentMouseState[button] != previousMouseState[button]) && (currentMouseState[button] == 0)) released = true;
#endif

    return released;
}

// Detect if a mouse button is NOT being pressed
bool IsMouseButtonUp(int button)
{
    bool up = false;

#if !defined(PLATFORM_ANDROID)
    if (GetMouseButtonStatus(button) == 0) up = true;
#endif

    return up;
}

// Returns mouse position X
int GetMouseX(void)
{
#if defined(PLATFORM_ANDROID)
    return (int)touchPosition[0].x;
#else
    return (int)mousePosition.x;
#endif
}

// Returns mouse position Y
int GetMouseY(void)
{
#if defined(PLATFORM_ANDROID)
    return (int)touchPosition[0].x;
#else
    return (int)mousePosition.y;
#endif
}

// Returns mouse position XY
Vector2 GetMousePosition(void)
{
#if defined(PLATFORM_ANDROID)
    return GetTouchPosition(0);
#else
    return mousePosition;
#endif
}

// Set mouse position XY
void SetMousePosition(Vector2 position)
{
    mousePosition = position;
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    // NOTE: emscripten not implemented
    glfwSetCursorPos(window, position.x, position.y);
#endif
}

// Returns mouse wheel movement Y
int GetMouseWheelMove(void)
{
#if defined(PLATFORM_ANDROID)
    return 0;
#elif defined(PLATFORM_WEB)
    return previousMouseWheelY/100;
#else
    return previousMouseWheelY;
#endif
}

// Returns touch position X for touch point 0 (relative to screen size)
int GetTouchX(void)
{
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_WEB)
    return (int)touchPosition[0].x;
#else   // PLATFORM_DESKTOP, PLATFORM_RPI
    return GetMouseX();
#endif
}

// Returns touch position Y for touch point 0 (relative to screen size)
int GetTouchY(void)
{
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_WEB)
    return (int)touchPosition[0].y;
#else   // PLATFORM_DESKTOP, PLATFORM_RPI
    return GetMouseY();
#endif
}

// Returns touch position XY for a touch point index (relative to screen size)
// TODO: Touch position should be scaled depending on display size and render size
Vector2 GetTouchPosition(int index)
{
    Vector2 position = { -1.0f, -1.0f };

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_WEB)
    if (index < MAX_TOUCH_POINTS) position = touchPosition[index];
    else TraceLog(LOG_WARNING, "Required touch point out of range (Max touch points: %i)", MAX_TOUCH_POINTS);

    if ((screenWidth > displayWidth) || (screenHeight > displayHeight))
    {
        // TODO: Review touch position scaling for screenSize vs displaySize
        position.x = position.x*((float)screenWidth/(float)(displayWidth - renderOffsetX)) - renderOffsetX/2;
        position.y = position.y*((float)screenHeight/(float)(displayHeight - renderOffsetY)) - renderOffsetY/2;
    }
    else
    {
        position.x = position.x*((float)renderWidth/(float)displayWidth) - renderOffsetX/2;
        position.y = position.y*((float)renderHeight/(float)displayHeight) - renderOffsetY/2;
    }
#else   // PLATFORM_DESKTOP, PLATFORM_RPI
    if (index == 0) position = GetMousePosition();
#endif

    return position;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Initialize display device and framebuffer
// NOTE: width and height represent the screen (framebuffer) desired size, not actual display size
// If width or height are 0, default display size will be used for framebuffer size
// NOTE: returns false in case graphic device could not be created
static bool InitGraphicsDevice(int width, int height)
{
    screenWidth = width;        // User desired width
    screenHeight = height;      // User desired height

    // NOTE: Framebuffer (render area - renderWidth, renderHeight) could include black bars...
    // ...in top-down or left-right to match display aspect ratio (no weird scalings)

    // Downscale matrix is required in case desired screen area is bigger than display area
    downscaleView = MatrixIdentity();



#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit())
    {
        TraceLog(LOG_WARNING, "Failed to initialize GLFW");
        return false;
    }

    // NOTE: Getting video modes is not implemented in emscripten GLFW3 version
#if defined(PLATFORM_DESKTOP)
    // Find monitor resolution
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (!monitor)
    {
        TraceLog(LOG_WARNING, "Failed to get monitor");
        return false;
    }
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    displayWidth = mode->width;
    displayHeight = mode->height;

    // Screen size security check
    if (screenWidth <= 0) screenWidth = displayWidth;
    if (screenHeight <= 0) screenHeight = displayHeight;
#endif  // defined(PLATFORM_DESKTOP)



#if defined(PLATFORM_WEB)
    displayWidth = screenWidth;
    displayHeight = screenHeight;
#endif  // defined(PLATFORM_WEB)

    glfwDefaultWindowHints();                       // Set default windows hints

    // Check some Window creation flags
    if (configFlags & FLAG_WINDOW_RESIZABLE) glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);   // Resizable window
    else glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);  // Avoid window being resizable

    if (configFlags & FLAG_WINDOW_DECORATED) glfwWindowHint(GLFW_DECORATED, GL_TRUE);   // Border and buttons on Window
    
    if (configFlags & FLAG_WINDOW_TRANSPARENT)
    {
        // TODO: Enable transparent window (not ready yet on GLFW 3.2)
    }

    if (configFlags & FLAG_MSAA_4X_HINT)
    {
        glfwWindowHint(GLFW_SAMPLES, 4);            // Enables multisampling x4 (MSAA), default is 0
        TraceLog(LOG_INFO, "Trying to enable MSAA x4");
    }
    
    //glfwWindowHint(GLFW_RED_BITS, 8);             // Framebuffer red color component bits
    //glfwWindowHint(GLFW_DEPTH_BITS, 16);          // Depthbuffer bits (24 by default)
    //glfwWindowHint(GLFW_REFRESH_RATE, 0);         // Refresh rate for fullscreen window
    //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);    // Default OpenGL API to use. Alternative: GLFW_OPENGL_ES_API
    //glfwWindowHint(GLFW_AUX_BUFFERS, 0);          // Number of auxiliar buffers

    // NOTE: When asking for an OpenGL context version, most drivers provide highest supported version
    // with forward compatibility to older OpenGL versions.
    // For example, if using OpenGL 1.1, driver can provide a 4.3 context forward compatible.

    // Check selection OpenGL version
    if (rlGetVersion() == OPENGL_21)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);        // Choose OpenGL major version (just hint)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);        // Choose OpenGL minor version (just hint)
    }
    else if (rlGetVersion() == OPENGL_33)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);        // Choose OpenGL major version (just hint)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);        // Choose OpenGL minor version (just hint)
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Profiles Hint: Only 3.3 and above!
                                                                       // Other values: GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_COMPAT_PROFILE
#if defined(__APPLEOSX__)
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // OSX Requires
#else
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE); // Fordward Compatibility Hint: Only 3.3 and above!
#endif
        //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    }

    if (fullscreen)
    {
        // Obtain recommended displayWidth/displayHeight from a valid videomode for the monitor
        int count;
        const GLFWvidmode *modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);

        // Get closest videomode to desired screenWidth/screenHeight
        for (int i = 0; i < count; i++)
        {
            if (modes[i].width >= screenWidth)
            {
                if (modes[i].height >= screenHeight)
                {
                    displayWidth = modes[i].width;
                    displayHeight = modes[i].height;
                    break;
                }
            }
        }

        TraceLog(LOG_WARNING, "Closest fullscreen videomode: %i x %i", displayWidth, displayHeight);

        // NOTE: ISSUE: Closest videomode could not match monitor aspect-ratio, for example,
        // for a desired screen size of 800x450 (16:9), closest supported videomode is 800x600 (4:3),
        // framebuffer is rendered correctly but once displayed on a 16:9 monitor, it gets stretched
        // by the sides to fit all monitor space...

        // At this point we need to manage render size vs screen size
        // NOTE: This function uses and modifies global module variables:
        //       screenWidth/screenHeight - renderWidth/renderHeight - downscaleView
        SetupFramebufferSize(displayWidth, displayHeight);

        window = glfwCreateWindow(displayWidth, displayHeight, windowTitle, glfwGetPrimaryMonitor(), NULL);

        // NOTE: Full-screen change, not working properly...
        //glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, screenWidth, screenHeight, GLFW_DONT_CARE);
    }
    else
    {
        // No-fullscreen window creation
        window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);

        if (window)
        {
#if defined(PLATFORM_DESKTOP)
            // Center window on screen
            int windowPosX = displayWidth/2 - screenWidth/2;
            int windowPosY = displayHeight/2 - screenHeight/2;

            if (windowPosX < 0) windowPosX = 0;
            if (windowPosY < 0) windowPosY = 0;

            glfwSetWindowPos(window, windowPosX, windowPosY);
#endif
            renderWidth = screenWidth;
            renderHeight = screenHeight;
        }
    }

    if (!window)
    {
        glfwTerminate();
        TraceLog(LOG_WARNING, "GLFW Failed to initialize Window");
        return false;
    }
    else
    {
        TraceLog(LOG_INFO, "Display device initialized successfully");
#if defined(PLATFORM_DESKTOP)
        TraceLog(LOG_INFO, "Display size: %i x %i", displayWidth, displayHeight);
#endif
        TraceLog(LOG_INFO, "Render size: %i x %i", renderWidth, renderHeight);
        TraceLog(LOG_INFO, "Screen size: %i x %i", screenWidth, screenHeight);
        TraceLog(LOG_INFO, "Viewport offsets: %i, %i", renderOffsetX, renderOffsetY);
    }

    glfwSetWindowSizeCallback(window, WindowSizeCallback);      // NOTE: Resizing not allowed by default!
    glfwSetCursorEnterCallback(window, CursorEnterCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, MouseCursorPosCallback);   // Track mouse position changes
    glfwSetCharCallback(window, CharCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetWindowIconifyCallback(window, WindowIconifyCallback);
#if defined(PLATFORM_DESKTOP)
    glfwSetDropCallback(window, WindowDropCallback);
#endif

    glfwMakeContextCurrent(window);
    
    // Try to disable GPU V-Sync by default, set framerate using SetTargetFPS()
    // NOTE: V-Sync can be enabled by graphic driver configuration
    glfwSwapInterval(0);                

#if defined(PLATFORM_DESKTOP)
    // Load OpenGL 3.3 extensions
    // NOTE: GLFW loader function is passed as parameter
    rlLoadExtensions(glfwGetProcAddress);
#endif

    // Try to enable GPU V-Sync, so frames are limited to screen refresh rate (60Hz -> 60 FPS)
    // NOTE: V-Sync can be enabled by graphic driver configuration
    if (configFlags & FLAG_VSYNC_HINT)
    {
        glfwSwapInterval(1);
        TraceLog(LOG_INFO, "Trying to enable VSYNC");
    }
#endif // defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)


#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI) || defined(PLATFORM_UWP)
    fullscreen = true;

    // Screen size security check
    if (screenWidth <= 0) screenWidth = displayWidth;
    if (screenHeight <= 0) screenHeight = displayHeight;

#if defined(PLATFORM_RPI)
    bcm_host_init();

    DISPMANX_ELEMENT_HANDLE_T dispmanElement;
    DISPMANX_DISPLAY_HANDLE_T dispmanDisplay;
    DISPMANX_UPDATE_HANDLE_T dispmanUpdate;

    VC_RECT_T dstRect;
    VC_RECT_T srcRect;
#endif

    EGLint samples = 0;
    EGLint sampleBuffer = 0;
    if (configFlags & FLAG_MSAA_4X_HINT)
    {
        samples = 4;
        sampleBuffer = 1;
        TraceLog(LOG_INFO, "Trying to enable MSAA x4");
    }

    const EGLint framebufferAttribs[] =
    {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,     // Type of context support -> Required on RPI?
        //EGL_SURFACE_TYPE, EGL_WINDOW_BIT,          // Don't use it on Android!
        EGL_RED_SIZE, 8,            // RED color bit depth (alternative: 5)
        EGL_GREEN_SIZE, 8,          // GREEN color bit depth (alternative: 6)
        EGL_BLUE_SIZE, 8,           // BLUE color bit depth (alternative: 5)
        //EGL_ALPHA_SIZE, 8,        // ALPHA bit depth (required for transparent framebuffer)
        //EGL_TRANSPARENT_TYPE, EGL_NONE, // Request transparent framebuffer (EGL_TRANSPARENT_RGB does not work on RPI)
        EGL_DEPTH_SIZE, 16,         // Depth buffer size (Required to use Depth testing!)
        //EGL_STENCIL_SIZE, 8,      // Stencil buffer size
        EGL_SAMPLE_BUFFERS, sampleBuffer,    // Activate MSAA
        EGL_SAMPLES, samples,       // 4x Antialiasing if activated (Free on MALI GPUs)
        EGL_NONE
    };

    const EGLint contextAttribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

#if defined(PLATFORM_UWP)
    const EGLint surfaceAttributes[] =
    {
        // EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER is part of the same optimization as EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER (see above).
        // If you have compilation issues with it then please update your Visual Studio templates.
        EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_NONE
    };

    const EGLint defaultDisplayAttributes[] =
    {
        // These are the default display attributes, used to request ANGLE's D3D11 renderer.
        // eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

        // EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance benefits on mobile devices.
        // Its syntax is subject to change, though. Please update your Visual Studio templates if you experience compilation issues with it.
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        
        // EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call 
        // the IDXGIDevice3::Trim method on behalf of the application when it gets suspended. 
        // Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification requirement.
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };
    
    const EGLint fl9_3DisplayAttributes[] =
    {
        // These can be used to request ANGLE's D3D11 renderer, with D3D11 Feature Level 9_3.
        // These attributes are used if the call to eglInitialize fails with the default display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };

    const EGLint warpDisplayAttributes[] =
    {
        // These attributes can be used to request D3D11 WARP.
        // They are used if eglInitialize fails with both the default display attributes and the 9_3 display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE,
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };
    
    EGLConfig config = NULL;
#ifndef PLATFORM_SILENT_RADIANCE
    // eglGetPlatformDisplayEXT is an alternative to eglGetDisplay. It allows us to pass in display attributes, used to configure D3D11.
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)(eglGetProcAddress("eglGetPlatformDisplayEXT"));
#else
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = NULL;
#endif
    if (!eglGetPlatformDisplayEXT)
    {
        TraceLog(LOG_WARNING, "Failed to get function eglGetPlatformDisplayEXT");
        return false;
    }

    //
    // To initialize the display, we make three sets of calls to eglGetPlatformDisplayEXT and eglInitialize, with varying 
    // parameters passed to eglGetPlatformDisplayEXT:
    // 1) The first calls uses "defaultDisplayAttributes" as a parameter. This corresponds to D3D11 Feature Level 10_0+.
    // 2) If eglInitialize fails for step 1 (e.g. because 10_0+ isn't supported by the default GPU), then we try again 
    //    using "fl9_3DisplayAttributes". This corresponds to D3D11 Feature Level 9_3.
    // 3) If eglInitialize fails for step 2 (e.g. because 9_3+ isn't supported by the default GPU), then we try again 
    //    using "warpDisplayAttributes".  This corresponds to D3D11 Feature Level 11_0 on WARP, a D3D11 software rasterizer.
    //
    
    // This tries to initialize EGL to D3D11 Feature Level 10_0+. See above comment for details.
    display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
    if (display == EGL_NO_DISPLAY)
    {
        TraceLog(LOG_WARNING, "Failed to initialize EGL display");
        return false;
    }
        
    if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
    {
        // This tries to initialize EGL to D3D11 Feature Level 9_3, if 10_0+ is unavailable (e.g. on some mobile devices).
        display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, fl9_3DisplayAttributes);
        if (display == EGL_NO_DISPLAY)
        {
            TraceLog(LOG_WARNING, "Failed to initialize EGL display");
            return false;
        }

        if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
        {
            // This initializes EGL to D3D11 Feature Level 11_0 on WARP, if 9_3+ is unavailable on the default GPU.
            display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, warpDisplayAttributes);
            if (display == EGL_NO_DISPLAY) 
            {
                TraceLog(LOG_WARNING, "Failed to initialize EGL display");
                return false;
            }

            if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
            {
                // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
                TraceLog(LOG_WARNING, "Failed to initialize EGL");
                return false;
            }
        }
    }

	//SetupFramebufferSize(displayWidth, displayHeight);

    EGLint numConfigs = 0;
    if ((eglChooseConfig(display, framebufferAttribs, &config, 1, &numConfigs) == EGL_FALSE) || (numConfigs == 0))
    {
        TraceLog(LOG_WARNING, "Failed to choose first EGLConfig");
        return false;
    }

    // Create a PropertySet and initialize with the EGLNativeWindowType.
    //PropertySet^ surfaceCreationProperties = ref new PropertySet();
    //surfaceCreationProperties->Insert(ref new String(EGLNativeWindowTypeProperty), window);     // CoreWindow^ window

    // You can configure the surface to render at a lower resolution and be scaled up to 
    // the full window size. The scaling is often free on mobile hardware.
    //
    // One way to configure the SwapChainPanel is to specify precisely which resolution it should render at.
    // Size customRenderSurfaceSize = Size(800, 600);
    // surfaceCreationProperties->Insert(ref new String(EGLRenderSurfaceSizeProperty), PropertyValue::CreateSize(customRenderSurfaceSize));
    //
    // Another way is to tell the SwapChainPanel to render at a certain scale factor compared to its size.
    // e.g. if the SwapChainPanel is 1920x1280 then setting a factor of 0.5f will make the app render at 960x640
    // float customResolutionScale = 0.5f;
    // surfaceCreationProperties->Insert(ref new String(EGLRenderResolutionScaleProperty), PropertyValue::CreateSingle(customResolutionScale));

    
    // eglCreateWindowSurface() requires a EGLNativeWindowType parameter, 
    // In Windows platform: typedef HWND EGLNativeWindowType;
    
    
    // Property: EGLNativeWindowTypeProperty
    // Type: IInspectable
    // Description: Set this property to specify the window type to use for creating a surface.
    //              If this property is missing, surface creation will fail.
    //
    //const wchar_t EGLNativeWindowTypeProperty[] = L"EGLNativeWindowTypeProperty";
    
    //https://stackoverflow.com/questions/46550182/how-to-create-eglsurface-using-c-winrt-and-angle
    
    //surface = eglCreateWindowSurface(display, config, reinterpret_cast<IInspectable*>(surfaceCreationProperties), surfaceAttributes);
    surface = eglCreateWindowSurface(display, config, uwpWindow, surfaceAttributes);
    if (surface == EGL_NO_SURFACE)
    {
        TraceLog(LOG_WARNING, "Failed to create EGL fullscreen surface");
        return false;
    }

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT)
    {
        TraceLog(LOG_WARNING, "Failed to create EGL context");
        return false;
    }

	// Get EGL display window size 
	eglQuerySurface(display, surface, EGL_WIDTH, &screenWidth);
	eglQuerySurface(display, surface, EGL_HEIGHT, &screenHeight);
    

#else   // not PLATFORM_ANDROID, PLATFORM_RPI, or PLATFORM_UWP
    EGLint numConfigs;

#ifndef PLATFORM_SILENT_RADIANCE
// display is not set and all this egl initialize is not run
    // Get an EGL display connection
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) 
    {
        TraceLog(LOG_WARNING, "Failed to initialize EGL display");
        return false;
    }

    // Initialize the EGL display connection
    if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
    {
        // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
        TraceLog(LOG_WARNING, "Failed to initialize EGL");
        return false;
    }

    // Get an appropriate EGL framebuffer configuration
    eglChooseConfig(display, framebufferAttribs, &config, 1, &numConfigs);

    // Set rendering API
    eglBindAPI(EGL_OPENGL_ES_API);

    // Create an EGL rendering context
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT)
    {
        TraceLog(LOG_WARNING, "Failed to create EGL context");
        return false;
    }
#endif  // not silent radiance
    
#endif

    // Create an EGL window surface
    //---------------------------------------------------------------------------------
#if defined(PLATFORM_ANDROID)
    EGLint displayFormat;

    displayWidth = ANativeWindow_getWidth(app->window);
    displayHeight = ANativeWindow_getHeight(app->window);

    // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is guaranteed to be accepted by ANativeWindow_setBuffersGeometry()
    // As soon as we picked a EGLConfig, we can safely reconfigure the ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &displayFormat);

    // At this point we need to manage render size vs screen size
    // NOTE: This function use and modify global module variables: screenWidth/screenHeight and renderWidth/renderHeight and downscaleView
    SetupFramebufferSize(displayWidth, displayHeight);

    ANativeWindow_setBuffersGeometry(app->window, renderWidth, renderHeight, displayFormat);
    //ANativeWindow_setBuffersGeometry(app->window, 0, 0, displayFormat);       // Force use of native display size

    surface = eglCreateWindowSurface(display, config, app->window, NULL);
#endif  // defined(PLATFORM_ANDROID)

#if defined(PLATFORM_RPI)
    graphics_get_display_size(0, &displayWidth, &displayHeight);

    // At this point we need to manage render size vs screen size
    // NOTE: This function use and modify global module variables: screenWidth/screenHeight and renderWidth/renderHeight and downscaleView
    SetupFramebufferSize(displayWidth, displayHeight);

    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.width = displayWidth;
    dstRect.height = displayHeight;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.width = renderWidth << 16;
    srcRect.height = renderHeight << 16;

    // NOTE: RPI dispmanx windowing system takes care of srcRec scaling to dstRec by hardware (no cost)
    // Take care that renderWidth/renderHeight fit on displayWidth/displayHeight aspect ratio

    VC_DISPMANX_ALPHA_T alpha;
    alpha.flags = DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS;
    alpha.opacity = 255;    // Set transparency level for framebuffer, requires EGLAttrib: EGL_TRANSPARENT_TYPE
    alpha.mask = 0;

    dispmanDisplay = vc_dispmanx_display_open(0);   // LCD
    dispmanUpdate = vc_dispmanx_update_start(0);

    dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay, 0/*layer*/, &dstRect, 0/*src*/,
                                            &srcRect, DISPMANX_PROTECTION_NONE, &alpha, 0/*clamp*/, DISPMANX_NO_ROTATE);

    nativeWindow.element = dispmanElement;
    nativeWindow.width = renderWidth;
    nativeWindow.height = renderHeight;
    vc_dispmanx_update_submit_sync(dispmanUpdate);

    surface = eglCreateWindowSurface(display, config, &nativeWindow, NULL);
    //---------------------------------------------------------------------------------
#endif  // defined(PLATFORM_RPI)
    // There must be at least one frame displayed before the buffers are swapped
    //eglSwapInterval(display, 1);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
    {
        TraceLog(LOG_WARNING, "Unable to attach EGL rendering context to EGL surface");
        return false;
    }
    else
    {
        // Grab the width and height of the surface
        //eglQuerySurface(display, surface, EGL_WIDTH, &renderWidth);
        //eglQuerySurface(display, surface, EGL_HEIGHT, &renderHeight);

        TraceLog(LOG_INFO, "Display device initialized successfully");
        TraceLog(LOG_INFO, "Display size: %i x %i", displayWidth, displayHeight);
        TraceLog(LOG_INFO, "Render size: %i x %i", renderWidth, renderHeight);
        TraceLog(LOG_INFO, "Screen size: %i x %i", screenWidth, screenHeight);
        TraceLog(LOG_INFO, "Viewport offsets: %i, %i", renderOffsetX, renderOffsetY);
    }
#endif // defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI)

	renderWidth = screenWidth;
	renderHeight = screenHeight;
	
    // Initialize OpenGL context (states and resources)
    // NOTE: screenWidth and screenHeight not used, just stored as globals
    rlglInit(screenWidth, screenHeight);


    // Setup default viewport
    SetupViewport();

    // Initialize internal projection and modelview matrices
    // NOTE: Default to orthographic projection mode with top-left corner at (0,0)
    rlMatrixMode(RL_PROJECTION);                // Switch to PROJECTION matrix
    rlLoadIdentity();                           // Reset current matrix (PROJECTION)
    rlOrtho(0, renderWidth - renderOffsetX, renderHeight - renderOffsetY, 0, 0.0f, 1.0f);
    rlMatrixMode(RL_MODELVIEW);                 // Switch back to MODELVIEW matrix
    rlLoadIdentity();                           // Reset current matrix (MODELVIEW)

#ifndef PLATFORM_SILENT_RADIANCE
    ClearBackground(RAYWHITE);      // Default background color for raylib games :P
#endif

#if defined(PLATFORM_ANDROID)
    windowReady = true;             // IMPORTANT!
#endif
    return true;
}

// Set viewport parameters
static void SetupViewport(void)
{
#if defined(__APPLEOSX__)
    // Get framebuffer size of current window
    // NOTE: Required to handle HighDPI display correctly on OSX because framebuffer
    // is automatically reasized to adapt to new DPI.
    // When OS does that, it can be detected using GLFW3 callback: glfwSetFramebufferSizeCallback()
    int fbWidth, fbHeight;
 //??? IOS   glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    rlViewport(renderOffsetX/2, renderOffsetY/2,  fbWidth - renderOffsetX, fbHeight - renderOffsetY);
#else
    // Initialize screen viewport (area of the screen that you will actually draw to)
    // NOTE: Viewport must be recalculated if screen is resized
    rlViewport(renderOffsetX/2, renderOffsetY/2, renderWidth - renderOffsetX, renderHeight - renderOffsetY);
#endif
}

// Compute framebuffer size relative to screen size and display size
// NOTE: Global variables renderWidth/renderHeight and renderOffsetX/renderOffsetY can be modified
static void SetupFramebufferSize(int displayWidth, int displayHeight)
{
    // Calculate renderWidth and renderHeight, we have the display size (input params) and the desired screen size (global var)
    if ((screenWidth > displayWidth) || (screenHeight > displayHeight))
    {
        TraceLog(LOG_WARNING, "DOWNSCALING: Required screen size (%ix%i) is bigger than display size (%ix%i)", screenWidth, screenHeight, displayWidth, displayHeight);

        // Downscaling to fit display with border-bars
        float widthRatio = (float)displayWidth/(float)screenWidth;
        float heightRatio = (float)displayHeight/(float)screenHeight;

        if (widthRatio <= heightRatio)
        {
            renderWidth = displayWidth;
            renderHeight = (int)round((float)screenHeight*widthRatio);
            renderOffsetX = 0;
            renderOffsetY = (displayHeight - renderHeight);
        }
        else
        {
            renderWidth = (int)round((float)screenWidth*heightRatio);
            renderHeight = displayHeight;
            renderOffsetX = (displayWidth - renderWidth);
            renderOffsetY = 0;
        }

        // NOTE: downscale matrix required!
        float scaleRatio = (float)renderWidth/(float)screenWidth;

        downscaleView = MatrixScale(scaleRatio, scaleRatio, scaleRatio);

        // NOTE: We render to full display resolution!
        // We just need to calculate above parameters for downscale matrix and offsets
        renderWidth = displayWidth;
        renderHeight = displayHeight;

        TraceLog(LOG_WARNING, "Downscale matrix generated, content will be rendered at: %i x %i", renderWidth, renderHeight);
    }
    else if ((screenWidth < displayWidth) || (screenHeight < displayHeight))
    {
        // Required screen size is smaller than display size
        TraceLog(LOG_INFO, "UPSCALING: Required screen size: %i x %i -> Display size: %i x %i", screenWidth, screenHeight, displayWidth, displayHeight);

        // Upscaling to fit display with border-bars
        float displayRatio = (float)displayWidth/(float)displayHeight;
        float screenRatio = (float)screenWidth/(float)screenHeight;

        if (displayRatio <= screenRatio)
        {
            renderWidth = screenWidth;
            renderHeight = (int)round((float)screenWidth/displayRatio);
            renderOffsetX = 0;
            renderOffsetY = (renderHeight - screenHeight);
        }
        else
        {
            renderWidth = (int)round((float)screenHeight*displayRatio);
            renderHeight = screenHeight;
            renderOffsetX = (renderWidth - screenWidth);
            renderOffsetY = 0;
        }
    }
    else    // screen == display
    {
        renderWidth = screenWidth;
        renderHeight = screenHeight;
        renderOffsetX = 0;
        renderOffsetY = 0;
    }
}

// Initialize hi-resolution timer
static void InitTimer(void)
{
    srand(time(NULL));              // Initialize random seed
    
#if !defined(SUPPORT_BUSY_WAIT_LOOP) && defined(_WIN32)
    timeBeginPeriod(1);             // Setup high-resolution timer to 1ms (granularity of 1-2 ms)
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI)
    struct timespec now;

    if (clock_gettime(CLOCK_MONOTONIC, &now) == 0)  // Success
    {
        baseTime = (uint64_t)now.tv_sec*1000000000LLU + (uint64_t)now.tv_nsec;
    }
    else TraceLog(LOG_WARNING, "No hi-resolution timer available");
#endif

    previousTime = GetTime();       // Get time as double
}

// Wait for some milliseconds (stop program execution)
// NOTE: Sleep() granularity could be around 10 ms, it means, Sleep() could
// take longer than expected... for that reason we use the busy wait loop
// http://stackoverflow.com/questions/43057578/c-programming-win32-games-sleep-taking-longer-than-expected
static void Wait(float ms)
{
#if defined(SUPPORT_BUSY_WAIT_LOOP)
    double prevTime = GetTime();
    double nextTime = 0.0;

    // Busy wait loop
    while ((nextTime - prevTime) < ms/1000.0f) nextTime = GetTime();
#else
    #if defined(_WIN32)
        Sleep((unsigned int)ms);
    #elif defined(__linux__) || defined(PLATFORM_WEB)
        struct timespec req = { 0 };
        time_t sec = (int)(ms/1000.0f);
        ms -= (sec*1000);
        req.tv_sec = sec;
        req.tv_nsec = ms*1000000L;

        // NOTE: Use nanosleep() on Unix platforms... usleep() it's deprecated.
        while (nanosleep(&req, &req) == -1) continue;
    #elif defined(__APPLEOSX__)
        usleep(ms*1000.0f);
    #endif
#endif
}

// Get one key state
static bool GetKeyStatus(int key)
{
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    return glfwGetKey(window, key);
#elif defined(PLATFORM_ANDROID)
    // NOTE: Android supports up to 260 keys
    if (key < 0 || key > 260) return false;
    else return currentKeyState[key];
#elif defined(PLATFORM_RPI)
    // NOTE: Keys states are filled in PollInputEvents()
    if (key < 0 || key > 511) return false;
    else return currentKeyState[key];
#elif defined(PLATFORM_SILENT_RADIANCE)
  return false;
#endif
}

// Get one mouse button state
static bool GetMouseButtonStatus(int button)
{
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    return glfwGetMouseButton(window, button);
#elif defined(PLATFORM_ANDROID)
    // TODO: Check for virtual mouse?
    return false;
#elif defined(PLATFORM_RPI)
    // NOTE: Mouse buttons states are filled in PollInputEvents()
    return currentMouseState[button];
#elif defined(PLATFORM_SILENT_RADIANCE)
    return false;
#endif
}

// Poll (store) all input events
static void PollInputEvents(void)
{
#if defined(SUPPORT_GESTURES_SYSTEM)
    // NOTE: Gestures update must be called every frame to reset gestures correctly
    // because ProcessGestureEvent() is just called on an event, not every frame
    UpdateGestures();
#endif

    // Reset last key pressed registered
    lastKeyPressed = -1;

#if !defined(PLATFORM_RPI)
    // Reset last gamepad button/axis registered state
    lastGamepadButtonPressed = -1;
    gamepadAxisCount = 0;
#endif

#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    // Mouse input polling
    double mouseX;
    double mouseY;

    glfwGetCursorPos(window, &mouseX, &mouseY);

    mousePosition.x = (float)mouseX;
    mousePosition.y = (float)mouseY;

    // Keyboard input polling (automatically managed by GLFW3 through callback)

    // Register previous keys states
    for (int i = 0; i < 512; i++) previousKeyState[i] = currentKeyState[i];

    // Register previous mouse states
    for (int i = 0; i < 3; i++) previousMouseState[i] = currentMouseState[i];

    previousMouseWheelY = currentMouseWheelY;
    currentMouseWheelY = 0;
#endif

#if defined(PLATFORM_DESKTOP)
    // Check if gamepads are ready
    // NOTE: We do it here in case of disconection
    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        if (glfwJoystickPresent(i)) gamepadReady[i] = true;
        else gamepadReady[i] = false;
    }

    // Register gamepads buttons events
    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        if (gamepadReady[i])     // Check if gamepad is available
        {
            // Register previous gamepad states
            for (int k = 0; k < MAX_GAMEPAD_BUTTONS; k++) previousGamepadState[i][k] = currentGamepadState[i][k];

            // Get current gamepad state
            // NOTE: There is no callback available, so we get it manually
            const unsigned char *buttons;
            int buttonsCount;

            buttons = glfwGetJoystickButtons(i, &buttonsCount);

            for (int k = 0; (buttons != NULL) && (k < buttonsCount) && (buttonsCount < MAX_GAMEPAD_BUTTONS); k++)
            {
                if (buttons[k] == GLFW_PRESS)
                {
                    currentGamepadState[i][k] = 1;
                    lastGamepadButtonPressed = k;
                }
                else currentGamepadState[i][k] = 0;
            }

            // Get current axis state
            const float *axes;
            int axisCount = 0;

            axes = glfwGetJoystickAxes(i, &axisCount);

            for (int k = 0; (axes != NULL) && (k < axisCount) && (k < MAX_GAMEPAD_AXIS); k++)
            {
                gamepadAxisState[i][k] = axes[k];
            }

            gamepadAxisCount = axisCount;
        }
    }

    glfwPollEvents();       // Register keyboard/mouse events (callbacks)... and window events!
#endif

// Gamepad support using emscripten API
// NOTE: GLFW3 joystick functionality not available in web
#if defined(PLATFORM_WEB)
    // Get number of gamepads connected
    int numGamepads = emscripten_get_num_gamepads();

    for (int i = 0; (i < numGamepads) && (i < MAX_GAMEPADS); i++)
    {
        // Register previous gamepad button states
        for (int k = 0; k < MAX_GAMEPAD_BUTTONS; k++) previousGamepadState[i][k] = currentGamepadState[i][k];

        EmscriptenGamepadEvent gamepadState;

        int result = emscripten_get_gamepad_status(i, &gamepadState);

        if (result == EMSCRIPTEN_RESULT_SUCCESS)
        {
            // Register buttons data for every connected gamepad
            for (int j = 0; (j < gamepadState.numButtons) && (j < MAX_GAMEPAD_BUTTONS); j++)
            {
                if (gamepadState.digitalButton[j] == 1)
                {
                    currentGamepadState[i][j] = 1;
                    lastGamepadButtonPressed = j;
                }
                else currentGamepadState[i][j] = 0;

                //printf("Gamepad %d, button %d: Digital: %d, Analog: %g\n", gamepadState.index, j, gamepadState.digitalButton[j], gamepadState.analogButton[j]);
            }

            // Register axis data for every connected gamepad
            for (int j = 0; (j < gamepadState.numAxes) && (j < MAX_GAMEPAD_AXIS); j++)
            {
                gamepadAxisState[i][j] = gamepadState.axis[j];
            }

            gamepadAxisCount = gamepadState.numAxes;
        }
    }
#endif

#if defined(PLATFORM_ANDROID)
    // Register previous keys states
    // NOTE: Android supports up to 260 keys
    for (int i = 0; i < 260; i++) previousKeyState[i] = currentKeyState[i];

    // Poll Events (registered events)
    // NOTE: Activity is paused if not enabled (appEnabled)
    while ((ident = ALooper_pollAll(appEnabled ? 0 : -1, NULL, &events,(void**)&source)) >= 0)
    {
        // Process this event
        if (source != NULL) source->process(app, source);

        // NOTE: Never close window, native activity is controlled by the system!
        if (app->destroyRequested != 0)
        {
            //TraceLog(LOG_INFO, "Closing Window...");
            //windowShouldClose = true;
            //ANativeActivity_finish(app->activity);
        }
    }
#endif

#if defined(PLATFORM_RPI)
    // NOTE: Mouse input events polling is done asynchonously in another pthread - MouseThread()

    // NOTE: Keyboard reading could be done using input_event(s) reading or just read from stdin,
    // we use method 2 (stdin) but maybe in a future we should change to method 1...
    ProcessKeyboard();

    // NOTE: Gamepad (Joystick) input events polling is done asynchonously in another pthread - GamepadThread()
#endif
}

// Copy back buffer to front buffers
static void SwapBuffers(void)
{
#ifndef PLATFORM_SILENTRADIANCE
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    glfwSwapBuffers(window);
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI) || defined(PLATFORM_UWP)
    eglSwapBuffers(display, surface);
#endif
#endif
}

#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
// GLFW3 Error Callback, runs on GLFW3 error
static void ErrorCallback(int error, const char *description)
{
    TraceLog(LOG_WARNING, "[GLFW3 Error] Code: %i Decription: %s", error, description);
}

// GLFW3 Srolling Callback, runs on mouse wheel
static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    currentMouseWheelY = (int)yoffset;
}

// GLFW3 Keyboard Callback, runs on key pressed
static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == exitKey && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);

        // NOTE: Before closing window, while loop must be left!
    }
#if defined(PLATFORM_DESKTOP)
    else if (key == GLFW_KEY_F12 && action == GLFW_PRESS)
    {
    #if defined(SUPPORT_GIF_RECORDING)
        if (mods == GLFW_MOD_CONTROL)
        {
            if (gifRecording)
            {
                GifEnd();
                gifRecording = false;
                
                TraceLog(LOG_INFO, "End animated GIF recording");
            }
            else 
            {
                gifRecording = true;
                gifFramesCounter = 0;
                
                // NOTE: delay represents the time between frames in the gif, if we capture a gif frame every
                // 10 game frames and each frame trakes 16.6ms (60fps), delay between gif frames should be ~16.6*10.
                GifBegin(FormatText("screenrec%03i.gif", screenshotCounter), screenWidth, screenHeight, (int)(GetFrameTime()*10.0f), 8, false);
                screenshotCounter++;
                
                TraceLog(LOG_INFO, "Begin animated GIF recording: %s", FormatText("screenrec%03i.gif", screenshotCounter));
            }
        }
        else
    #endif  // SUPPORT_GIF_RECORDING
        {
            TakeScreenshot(FormatText("screenshot%03i.png", screenshotCounter));
            screenshotCounter++;
        }
    }
#endif  // PLATFORM_DESKTOP
    else
    {
        currentKeyState[key] = action;
        
        // NOTE: lastKeyPressed already registered on CharCallback()
        //if (action == GLFW_PRESS) lastKeyPressed = key;
    }
}

// GLFW3 Mouse Button Callback, runs on mouse button pressed
static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    currentMouseState[button] = action;

#if defined(SUPPORT_GESTURES_SYSTEM) && defined(SUPPORT_MOUSE_GESTURES)
    // Process mouse events as touches to be able to use mouse-gestures
    GestureEvent gestureEvent;

    // Register touch actions
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) gestureEvent.touchAction = TOUCH_DOWN;
    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) gestureEvent.touchAction = TOUCH_UP;

    // NOTE: TOUCH_MOVE event is registered in MouseCursorPosCallback()

    // Assign a pointer ID
    gestureEvent.pointerId[0] = 0;

    // Register touch points count
    gestureEvent.pointCount = 1;

    // Register touch points position, only one point registered
    gestureEvent.position[0] = GetMousePosition();

    // Normalize gestureEvent.position[0] for screenWidth and screenHeight
    gestureEvent.position[0].x /= (float)GetScreenWidth();
    gestureEvent.position[0].y /= (float)GetScreenHeight();

    // Gesture data is sent to gestures system for processing
    ProcessGestureEvent(gestureEvent);
#endif
}

// GLFW3 Cursor Position Callback, runs on mouse move
static void MouseCursorPosCallback(GLFWwindow *window, double x, double y)
{
#if defined(SUPPORT_GESTURES_SYSTEM) && defined(SUPPORT_MOUSE_GESTURES)
    // Process mouse events as touches to be able to use mouse-gestures
    GestureEvent gestureEvent;

    gestureEvent.touchAction = TOUCH_MOVE;

    // Assign a pointer ID
    gestureEvent.pointerId[0] = 0;

    // Register touch points count
    gestureEvent.pointCount = 1;

    // Register touch points position, only one point registered
    gestureEvent.position[0] = (Vector2){ (float)x, (float)y };

    touchPosition[0] = gestureEvent.position[0];

    // Normalize gestureEvent.position[0] for screenWidth and screenHeight
    gestureEvent.position[0].x /= (float)GetScreenWidth();
    gestureEvent.position[0].y /= (float)GetScreenHeight();

    // Gesture data is sent to gestures system for processing
    ProcessGestureEvent(gestureEvent);
#endif
}

// GLFW3 Char Key Callback, runs on key down (get unicode char value)
static void CharCallback(GLFWwindow *window, unsigned int key)
{  
    // NOTE: Registers any key down considering OS keyboard layout but
    // do not detects action events, those should be managed by user...
    // https://github.com/glfw/glfw/issues/668#issuecomment-166794907
    // http://www.glfw.org/docs/latest/input_guide.html#input_char
    
    lastKeyPressed = key;
}

// GLFW3 CursorEnter Callback, when cursor enters the window
static void CursorEnterCallback(GLFWwindow *window, int enter)
{
    if (enter == true) cursorOnScreen = true;
    else cursorOnScreen = false;
}

// GLFW3 WindowSize Callback, runs when window is resized
// NOTE: Window resizing not allowed by default
static void WindowSizeCallback(GLFWwindow *window, int width, int height)
{
    // If window is resized, viewport and projection matrix needs to be re-calculated
    rlViewport(0, 0, width, height);            // Set viewport width and height
    rlMatrixMode(RL_PROJECTION);                // Switch to PROJECTION matrix
    rlLoadIdentity();                           // Reset current matrix (PROJECTION)
    rlOrtho(0, width, height, 0, 0.0f, 1.0f);   // Orthographic projection mode with top-left corner at (0,0)
    rlMatrixMode(RL_MODELVIEW);                 // Switch back to MODELVIEW matrix
    rlLoadIdentity();                           // Reset current matrix (MODELVIEW)
    rlClearScreenBuffers();                     // Clear screen buffers (color and depth)

    // Window size must be updated to be used on 3D mode to get new aspect ratio (Begin3dMode())
    // NOTE: Be careful! GLFW3 will choose the closest fullscreen resolution supported by current monitor,
    // for example, if reescaling back to 800x450 (desired), it could set 720x480 (closest fullscreen supported)
    screenWidth = width;
    screenHeight = height;
    renderWidth = width;
    renderHeight = height;

    // NOTE: Postprocessing texture is not scaled to new size
}

// GLFW3 WindowIconify Callback, runs when window is minimized/restored
static void WindowIconifyCallback(GLFWwindow* window, int iconified)
{
    if (iconified) windowMinimized = true;  // The window was iconified
    else windowMinimized = false;           // The window was restored
}
#endif

#if defined(PLATFORM_DESKTOP)
// GLFW3 Window Drop Callback, runs when drop files into window
// NOTE: Paths are stored in dinamic memory for further retrieval
// Everytime new files are dropped, old ones are discarded
static void WindowDropCallback(GLFWwindow *window, int count, const char **paths)
{
    ClearDroppedFiles();

    dropFilesPath = (char **)malloc(sizeof(char *)*count);

    for (int i = 0; i < count; i++)
    {
        dropFilesPath[i] = (char *)malloc(sizeof(char)*256);     // Max path length set to 256 char
        strcpy(dropFilesPath[i], paths[i]);
    }

    dropFilesCount = count;
}
#endif

#if defined(PLATFORM_ANDROID)
// Android: Process activity lifecycle commands
static void AndroidCommandCallback(struct android_app *app, int32_t cmd)
{
    switch (cmd)
    {
        case APP_CMD_START:
        {
            //rendering = true;
            TraceLog(LOG_INFO, "APP_CMD_START");
        } break;
        case APP_CMD_RESUME:
        {
            TraceLog(LOG_INFO, "APP_CMD_RESUME");
        } break;
        case APP_CMD_INIT_WINDOW:
        {
            TraceLog(LOG_INFO, "APP_CMD_INIT_WINDOW");

            if (app->window != NULL)
            {
                if (contextRebindRequired)
                {
                    // Reset screen scaling to full display size
                    EGLint displayFormat;
                    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &displayFormat);
                    ANativeWindow_setBuffersGeometry(app->window, renderWidth, renderHeight, displayFormat);

                    // Recreate display surface and re-attach OpenGL context
                    surface = eglCreateWindowSurface(display, config, app->window, NULL);
                    eglMakeCurrent(display, surface, surface, context);

                    contextRebindRequired = false;
                }
                else
                {
                    // Init graphics device (display device and OpenGL context)
                    InitGraphicsDevice(screenWidth, screenHeight);
                    
                    // Init hi-res timer
                    InitTimer();

                    #if defined(SUPPORT_DEFAULT_FONT)
                    // Load default font
                    // NOTE: External function (defined in module: text)
                    LoadDefaultFont();
                    #endif

                    // TODO: GPU assets reload in case of lost focus (lost context)
                    // NOTE: This problem has been solved just unbinding and rebinding context from display
                    /*
                    if (assetsReloadRequired)
                    {
                        for (int i = 0; i < assetsCount; i++)
                        {
                            // TODO: Unload old asset if required

                            // Load texture again to pointed texture
                            (*textureAsset + i) = LoadTexture(assetPath[i]);
                        }
                    }
                    */

                    // raylib logo appearing animation (if enabled)
                    if (showLogo)
                    {
                        SetTargetFPS(60);   // Not required on Android
                        LogoAnimation();
                    }
                }
            }
        } break;
        case APP_CMD_GAINED_FOCUS:
        {
            TraceLog(LOG_INFO, "APP_CMD_GAINED_FOCUS");
            appEnabled = true;
            //ResumeMusicStream();
        } break;
        case APP_CMD_PAUSE:
        {
            TraceLog(LOG_INFO, "APP_CMD_PAUSE");
        } break;
        case APP_CMD_LOST_FOCUS:
        {
            //DrawFrame();
            TraceLog(LOG_INFO, "APP_CMD_LOST_FOCUS");
            appEnabled = false;
            //PauseMusicStream();
        } break;
        case APP_CMD_TERM_WINDOW:
        {
            // Dettach OpenGL context and destroy display surface
            // NOTE 1: Detaching context before destroying display surface avoids losing our resources (textures, shaders, VBOs...)
            // NOTE 2: In some cases (too many context loaded), OS could unload context automatically... :(
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroySurface(display, surface);

            contextRebindRequired = true;

            TraceLog(LOG_INFO, "APP_CMD_TERM_WINDOW");
        } break;
        case APP_CMD_SAVE_STATE:
        {
            TraceLog(LOG_INFO, "APP_CMD_SAVE_STATE");
        } break;
        case APP_CMD_STOP:
        {
            TraceLog(LOG_INFO, "APP_CMD_STOP");
        } break;
        case APP_CMD_DESTROY:
        {
            // TODO: Finish activity?
            //ANativeActivity_finish(app->activity);

            TraceLog(LOG_INFO, "APP_CMD_DESTROY");
        } break;
        case APP_CMD_CONFIG_CHANGED:
        {
            //AConfiguration_fromAssetManager(app->config, app->activity->assetManager);
            //print_cur_config(app);

            // Check screen orientation here!

            TraceLog(LOG_INFO, "APP_CMD_CONFIG_CHANGED");
        } break;
        default: break;
    }
}

// Android: Get input events
static int32_t AndroidInputCallback(struct android_app *app, AInputEvent *event)
{
    //http://developer.android.com/ndk/reference/index.html

    int type = AInputEvent_getType(event);

    if (type == AINPUT_EVENT_TYPE_MOTION)
    {
        // Get first touch position
        touchPosition[0].x = AMotionEvent_getX(event, 0);
        touchPosition[0].y = AMotionEvent_getY(event, 0);

        // Get second touch position
        touchPosition[1].x = AMotionEvent_getX(event, 1);
        touchPosition[1].y = AMotionEvent_getY(event, 1);
    }
    else if (type == AINPUT_EVENT_TYPE_KEY)
    {
        int32_t keycode = AKeyEvent_getKeyCode(event);
        //int32_t AKeyEvent_getMetaState(event);

        // Save current button and its state
        // NOTE: Android key action is 0 for down and 1 for up
        if (AKeyEvent_getAction(event) == 0)
        {
            currentKeyState[keycode] = 1;  // Key down
            lastKeyPressed = keycode;
        }
        else currentKeyState[keycode] = 0;  // Key up

        if (keycode == AKEYCODE_POWER)
        {
            // Let the OS handle input to avoid app stuck. Behaviour: CMD_PAUSE -> CMD_SAVE_STATE -> CMD_STOP -> CMD_CONFIG_CHANGED -> CMD_LOST_FOCUS
            // Resuming Behaviour: CMD_START -> CMD_RESUME -> CMD_CONFIG_CHANGED -> CMD_CONFIG_CHANGED -> CMD_GAINED_FOCUS
            // It seems like locking mobile, screen size (CMD_CONFIG_CHANGED) is affected.
            // NOTE: AndroidManifest.xml must have <activity android:configChanges="orientation|keyboardHidden|screenSize" >
            // Before that change, activity was calling CMD_TERM_WINDOW and CMD_DESTROY when locking mobile, so that was not a normal behaviour
            return 0;
        }
        else if ((keycode == AKEYCODE_BACK) || (keycode == AKEYCODE_MENU))
        {
            // Eat BACK_BUTTON and AKEYCODE_MENU, just do nothing... and don't let to be handled by OS!
            return 1;
        }
        else if ((keycode == AKEYCODE_VOLUME_UP) || (keycode == AKEYCODE_VOLUME_DOWN))
        {
            // Set default OS behaviour
            return 0;
        }
    }

    int32_t action = AMotionEvent_getAction(event);
    unsigned int flags = action & AMOTION_EVENT_ACTION_MASK;

#if defined(SUPPORT_GESTURES_SYSTEM)
    GestureEvent gestureEvent;

    // Register touch actions
    if (flags == AMOTION_EVENT_ACTION_DOWN) gestureEvent.touchAction = TOUCH_DOWN;
    else if (flags == AMOTION_EVENT_ACTION_UP) gestureEvent.touchAction = TOUCH_UP;
    else if (flags == AMOTION_EVENT_ACTION_MOVE) gestureEvent.touchAction = TOUCH_MOVE;

    // Register touch points count
    gestureEvent.pointCount = AMotionEvent_getPointerCount(event);

    // Register touch points id
    gestureEvent.pointerId[0] = AMotionEvent_getPointerId(event, 0);
    gestureEvent.pointerId[1] = AMotionEvent_getPointerId(event, 1);

    // Register touch points position
    // NOTE: Only two points registered
    gestureEvent.position[0] = (Vector2){ AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0) };
    gestureEvent.position[1] = (Vector2){ AMotionEvent_getX(event, 1), AMotionEvent_getY(event, 1) };

    // Normalize gestureEvent.position[x] for screenWidth and screenHeight
    gestureEvent.position[0].x /= (float)GetScreenWidth();
    gestureEvent.position[0].y /= (float)GetScreenHeight();

    gestureEvent.position[1].x /= (float)GetScreenWidth();
    gestureEvent.position[1].y /= (float)GetScreenHeight();

    // Gesture data is sent to gestures system for processing
    ProcessGestureEvent(gestureEvent);
#else
    
    // Support only simple touch position
    if (flags == AMOTION_EVENT_ACTION_DOWN)
    {
        // Get first touch position
        touchPosition[0].x = AMotionEvent_getX(event, 0);
        touchPosition[0].y = AMotionEvent_getY(event, 0);
        
        touchPosition[0].x /= (float)GetScreenWidth();
        touchPosition[0].y /= (float)GetScreenHeight();
    }
#endif

    return 0;
}
#endif

#if defined(PLATFORM_WEB)

// Register fullscreen change events
static EM_BOOL EmscriptenFullscreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData)
{
    //isFullscreen: int e->isFullscreen
    //fullscreenEnabled: int e->fullscreenEnabled
    //fs element nodeName: (char *) e->nodeName
    //fs element id: (char *) e->id
    //Current element size: (int) e->elementWidth, (int) e->elementHeight
    //Screen size:(int) e->screenWidth, (int) e->screenHeight

    if (e->isFullscreen)
    {
        TraceLog(LOG_INFO, "Canvas scaled to fullscreen. ElementSize: (%ix%i), ScreenSize(%ix%i)", e->elementWidth, e->elementHeight, e->screenWidth, e->screenHeight);
    }
    else
    {
        TraceLog(LOG_INFO, "Canvas scaled to windowed. ElementSize: (%ix%i), ScreenSize(%ix%i)", e->elementWidth, e->elementHeight, e->screenWidth, e->screenHeight);
    }

    // TODO: Depending on scaling factor (screen vs element), calculate factor to scale mouse/touch input

    return 0;
}

// Register keyboard input events
static EM_BOOL EmscriptenKeyboardCallback(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    if ((eventType == EMSCRIPTEN_EVENT_KEYPRESS) && (strcmp(keyEvent->code, "Escape") == 0))
    {
        emscripten_exit_pointerlock();
    }

    return 0;
}

// Register mouse input events
static EM_BOOL EmscriptenMouseCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    // Lock mouse pointer when click on screen
    if ((eventType == EMSCRIPTEN_EVENT_CLICK) && toggleCursorLock)
    {
        EmscriptenPointerlockChangeEvent plce;
        emscripten_get_pointerlock_status(&plce);

        if (!plce.isActive) emscripten_request_pointerlock(0, 1);
        else
        {
            emscripten_exit_pointerlock();
            emscripten_get_pointerlock_status(&plce);
            //if (plce.isActive) TraceLog(LOG_WARNING, "Pointer lock exit did not work!");
        }
        
        toggleCursorLock = false;
    }
    
    return 0;
}

// Register touch input events
static EM_BOOL EmscriptenTouchCallback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    /*
    for (int i = 0; i < touchEvent->numTouches; i++)
    {
        long x, y, id;

        if (!touchEvent->touches[i].isChanged) continue;

        id = touchEvent->touches[i].identifier;
        x = touchEvent->touches[i].canvasX;
        y = touchEvent->touches[i].canvasY;
    }

    printf("%s, numTouches: %d %s%s%s%s\n", emscripten_event_type_to_string(eventType), event->numTouches,
           event->ctrlKey ? " CTRL" : "", event->shiftKey ? " SHIFT" : "", event->altKey ? " ALT" : "", event->metaKey ? " META" : "");

    for (int i = 0; i < event->numTouches; ++i)
    {
        const EmscriptenTouchPoint *t = &event->touches[i];

        printf("  %ld: screen: (%ld,%ld), client: (%ld,%ld), page: (%ld,%ld), isChanged: %d, onTarget: %d, canvas: (%ld, %ld)\n",
          t->identifier, t->screenX, t->screenY, t->clientX, t->clientY, t->pageX, t->pageY, t->isChanged, t->onTarget, t->canvasX, t->canvasY);
    }
    */

    GestureEvent gestureEvent;

    // Register touch actions
    if (eventType == EMSCRIPTEN_EVENT_TOUCHSTART) gestureEvent.touchAction = TOUCH_DOWN;
    else if (eventType == EMSCRIPTEN_EVENT_TOUCHEND) gestureEvent.touchAction = TOUCH_UP;
    else if (eventType == EMSCRIPTEN_EVENT_TOUCHMOVE) gestureEvent.touchAction = TOUCH_MOVE;

    // Register touch points count
    gestureEvent.pointCount = touchEvent->numTouches;

    // Register touch points id
    gestureEvent.pointerId[0] = touchEvent->touches[0].identifier;
    gestureEvent.pointerId[1] = touchEvent->touches[1].identifier;

    // Register touch points position
    // NOTE: Only two points registered
    // TODO: Touch data should be scaled accordingly!
    //gestureEvent.position[0] = (Vector2){ touchEvent->touches[0].canvasX, touchEvent->touches[0].canvasY };
    //gestureEvent.position[1] = (Vector2){ touchEvent->touches[1].canvasX, touchEvent->touches[1].canvasY };
    gestureEvent.position[0] = (Vector2){ touchEvent->touches[0].targetX, touchEvent->touches[0].targetY };
    gestureEvent.position[1] = (Vector2){ touchEvent->touches[1].targetX, touchEvent->touches[1].targetY };

    touchPosition[0] = gestureEvent.position[0];
    touchPosition[1] = gestureEvent.position[1];

    // Normalize gestureEvent.position[x] for screenWidth and screenHeight
    gestureEvent.position[0].x /= (float)GetScreenWidth();
    gestureEvent.position[0].y /= (float)GetScreenHeight();

    gestureEvent.position[1].x /= (float)GetScreenWidth();
    gestureEvent.position[1].y /= (float)GetScreenHeight();

    // Gesture data is sent to gestures system for processing
    ProcessGestureEvent(gestureEvent);

    return 1;
}

// Register connected/disconnected gamepads events
static EM_BOOL EmscriptenGamepadCallback(int eventType, const EmscriptenGamepadEvent *gamepadEvent, void *userData)
{
    /*
    printf("%s: timeStamp: %g, connected: %d, index: %ld, numAxes: %d, numButtons: %d, id: \"%s\", mapping: \"%s\"\n",
           eventType != 0 ? emscripten_event_type_to_string(eventType) : "Gamepad state",
           gamepadEvent->timestamp, gamepadEvent->connected, gamepadEvent->index, gamepadEvent->numAxes, gamepadEvent->numButtons, gamepadEvent->id, gamepadEvent->mapping);

    for(int i = 0; i < gamepadEvent->numAxes; ++i) printf("Axis %d: %g\n", i, gamepadEvent->axis[i]);
    for(int i = 0; i < gamepadEvent->numButtons; ++i) printf("Button %d: Digital: %d, Analog: %g\n", i, gamepadEvent->digitalButton[i], gamepadEvent->analogButton[i]);
    */

    if ((gamepadEvent->connected) && (gamepadEvent->index < MAX_GAMEPADS)) gamepadReady[gamepadEvent->index] = true;
    else gamepadReady[gamepadEvent->index] = false;

    // TODO: Test gamepadEvent->index

    return 0;
}
#endif

#if defined(PLATFORM_RPI)
// Initialize Keyboard system (using standard input)
static void InitKeyboard(void)
{
    // NOTE: We read directly from Standard Input (stdin) - STDIN_FILENO file descriptor

    // Make stdin non-blocking (not enough, need to configure to non-canonical mode)
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);          // F_GETFL: Get the file access mode and the file status flags
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);     // F_SETFL: Set the file status flags to the value specified

    // Save terminal keyboard settings and reconfigure terminal with new settings
    struct termios keyboardNewSettings;
    tcgetattr(STDIN_FILENO, &defaultKeyboardSettings);    // Get current keyboard settings
    keyboardNewSettings = defaultKeyboardSettings;

    // New terminal settings for keyboard: turn off buffering (non-canonical mode), echo and key processing
    // NOTE: ISIG controls if ^C and ^Z generate break signals or not
    keyboardNewSettings.c_lflag &= ~(ICANON | ECHO | ISIG);
    //keyboardNewSettings.c_iflag &= ~(ISTRIP | INLCR | ICRNL | IGNCR | IXON | IXOFF);
    keyboardNewSettings.c_cc[VMIN] = 1;
    keyboardNewSettings.c_cc[VTIME] = 0;

    // Set new keyboard settings (change occurs immediately)
    tcsetattr(STDIN_FILENO, TCSANOW, &keyboardNewSettings);

    // NOTE: Reading directly from stdin will give chars already key-mapped by kernel to ASCII or UNICODE, we change that! -> WHY???

    // Save old keyboard mode to restore it at the end
    if (ioctl(STDIN_FILENO, KDGKBMODE, &defaultKeyboardMode) < 0)
    {
        // NOTE: It could mean we are using a remote keyboard through ssh!
        TraceLog(LOG_WARNING, "Could not change keyboard mode (SSH keyboard?)");
    }
    else
    {
        // We reconfigure keyboard mode to get:
        //    - scancodes (K_RAW)
        //    - keycodes (K_MEDIUMRAW)
        //    - ASCII chars (K_XLATE)
        //    - UNICODE chars (K_UNICODE)
        ioctl(STDIN_FILENO, KDSKBMODE, K_XLATE);
    }

    // Register keyboard restore when program finishes
    atexit(RestoreKeyboard);
}

// Process keyboard inputs
// TODO: Most probably input reading and processing should be in a separate thread
static void ProcessKeyboard(void)
{
    #define MAX_KEYBUFFER_SIZE      32      // Max size in bytes to read

    // Keyboard input polling (fill keys[256] array with status)
    int bufferByteCount = 0;                // Bytes available on the buffer
    char keysBuffer[MAX_KEYBUFFER_SIZE];    // Max keys to be read at a time

    // Reset pressed keys array
    for (int i = 0; i < 512; i++) currentKeyState[i] = 0;

    // Read availables keycodes from stdin
    bufferByteCount = read(STDIN_FILENO, keysBuffer, MAX_KEYBUFFER_SIZE);     // POSIX system call

    // Fill all read bytes (looking for keys)
    for (int i = 0; i < bufferByteCount; i++)
    {
        TraceLog(LOG_DEBUG, "Bytes on keysBuffer: %i", bufferByteCount);

        //printf("Key(s) bytes: ");
        //for (int i = 0; i < bufferByteCount; i++) printf("0x%02x ", keysBuffer[i]);
        //printf("\n");

        // NOTE: If (key == 0x1b), depending on next key, it could be a special keymap code!
        // Up -> 1b 5b 41 / Left -> 1b 5b 44 / Right -> 1b 5b 43 / Down -> 1b 5b 42
        if (keysBuffer[i] == 0x1b)
        {
            // Detect ESC to stop program
            if (bufferByteCount == 1) currentKeyState[256] = 1; // raylib key: KEY_ESCAPE
            else
            {
                if (keysBuffer[i + 1] == 0x5b)    // Special function key
                {
                    if ((keysBuffer[i + 2] == 0x5b) || (keysBuffer[i + 2] == 0x31) || (keysBuffer[i + 2] == 0x32))
                    {
                        // Process special function keys (F1 - F12)
                        switch (keysBuffer[i + 3])
                        {
                            case 0x41: currentKeyState[290] = 1; break;    // raylib KEY_F1
                            case 0x42: currentKeyState[291] = 1; break;    // raylib KEY_F2
                            case 0x43: currentKeyState[292] = 1; break;    // raylib KEY_F3
                            case 0x44: currentKeyState[293] = 1; break;    // raylib KEY_F4
                            case 0x45: currentKeyState[294] = 1; break;    // raylib KEY_F5
                            case 0x37: currentKeyState[295] = 1; break;    // raylib KEY_F6
                            case 0x38: currentKeyState[296] = 1; break;    // raylib KEY_F7
                            case 0x39: currentKeyState[297] = 1; break;    // raylib KEY_F8
                            case 0x30: currentKeyState[298] = 1; break;    // raylib KEY_F9
                            case 0x31: currentKeyState[299] = 1; break;    // raylib KEY_F10
                            case 0x33: currentKeyState[300] = 1; break;    // raylib KEY_F11
                            case 0x34: currentKeyState[301] = 1; break;    // raylib KEY_F12
                            default: break;
                        }

                        if (keysBuffer[i + 2] == 0x5b) i += 4;
                        else if ((keysBuffer[i + 2] == 0x31) || (keysBuffer[i + 2] == 0x32)) i += 5;
                    }
                    else
                    {
                        switch (keysBuffer[i + 2])
                        {
                            case 0x41: currentKeyState[265] = 1; break;    // raylib KEY_UP
                            case 0x42: currentKeyState[264] = 1; break;    // raylib KEY_DOWN
                            case 0x43: currentKeyState[262] = 1; break;    // raylib KEY_RIGHT
                            case 0x44: currentKeyState[263] = 1; break;    // raylib KEY_LEFT
                            default: break;
                        }

                        i += 3;  // Jump to next key
                    }

                    // NOTE: Some keys are not directly keymapped (CTRL, ALT, SHIFT)
                }
            }
        }
        else if (keysBuffer[i] == 0x0a) currentKeyState[257] = 1;     // raylib KEY_ENTER (don't mix with <linux/input.h> KEY_*)
        else if (keysBuffer[i] == 0x7f) currentKeyState[259] = 1;     // raylib KEY_BACKSPACE
        else
        {
            TraceLog(LOG_DEBUG, "Pressed key (ASCII): 0x%02x", keysBuffer[i]);

            // Translate lowercase a-z letters to A-Z
            if ((keysBuffer[i] >= 97) && (keysBuffer[i] <= 122))
            {
                currentKeyState[(int)keysBuffer[i] - 32] = 1;
            }
            else currentKeyState[(int)keysBuffer[i]] = 1;
        }
    }

    // Check exit key (same functionality as GLFW3 KeyCallback())
    if (currentKeyState[exitKey] == 1) windowShouldClose = true;

    // Check screen capture key (raylib key: KEY_F12)
    if (currentKeyState[301] == 1)
    {
        TakeScreenshot(FormatText("screenshot%03i.png", screenshotCounter));
        screenshotCounter++;
    }
}

// Restore default keyboard input
static void RestoreKeyboard(void)
{
    // Reset to default keyboard settings
    tcsetattr(STDIN_FILENO, TCSANOW, &defaultKeyboardSettings);

    // Reconfigure keyboard to default mode
    ioctl(STDIN_FILENO, KDSKBMODE, defaultKeyboardMode);
}

// Mouse initialization (including mouse thread)
static void InitMouse(void)
{
    // NOTE: We can use /dev/input/mice to read from all available mice
    if ((mouseStream = open(DEFAULT_MOUSE_DEV, O_RDONLY|O_NONBLOCK)) < 0)
    {
        TraceLog(LOG_WARNING, "Mouse device could not be opened, no mouse available");
    }
    else
    {
        mouseReady = true;

        int error = pthread_create(&mouseThreadId, NULL, &MouseThread, NULL);

        if (error != 0) TraceLog(LOG_WARNING, "Error creating mouse input event thread");
        else TraceLog(LOG_INFO, "Mouse device initialized successfully");
    }
}

// Mouse reading thread
// NOTE: We need a separate thread to avoid loosing mouse events,
// if too much time passes between reads, queue gets full and new events override older ones...
static void *MouseThread(void *arg)
{
    const unsigned char XSIGN = (1 << 4);
    const unsigned char YSIGN = (1 << 5);

    typedef struct {
        char buttons;
        char dx, dy;
    } MouseEvent;

    MouseEvent mouse;

    int mouseRelX = 0;
    int mouseRelY = 0;

    while (!windowShouldClose)
    {
        if (read(mouseStream, &mouse, sizeof(MouseEvent)) == (int)sizeof(MouseEvent))
        {
            if ((mouse.buttons & 0x08) == 0) break;   // This bit should always be set

            // Check Left button pressed
            if ((mouse.buttons & 0x01) > 0) currentMouseState[0] = 1;
            else currentMouseState[0] = 0;

            // Check Right button pressed
            if ((mouse.buttons & 0x02) > 0) currentMouseState[1] = 1;
            else currentMouseState[1] = 0;

            // Check Middle button pressed
            if ((mouse.buttons & 0x04) > 0) currentMouseState[2] = 1;
            else currentMouseState[2] = 0;

            mouseRelX = (int)mouse.dx;
            mouseRelY = (int)mouse.dy;

            if ((mouse.buttons & XSIGN) > 0) mouseRelX = -1*(255 - mouseRelX);
            if ((mouse.buttons & YSIGN) > 0) mouseRelY = -1*(255 - mouseRelY);

            // NOTE: Mouse movement is normalized to not be screen resolution dependant
            // We suppose 2*255 (max relative movement) is equivalent to screenWidth (max pixels width)
            // Result after normalization is multiplied by MOUSE_SENSITIVITY factor

            mousePosition.x += (float)mouseRelX*((float)screenWidth/(2*255))*MOUSE_SENSITIVITY;
            mousePosition.y -= (float)mouseRelY*((float)screenHeight/(2*255))*MOUSE_SENSITIVITY;

            if (mousePosition.x < 0) mousePosition.x = 0;
            if (mousePosition.y < 0) mousePosition.y = 0;

            if (mousePosition.x > screenWidth) mousePosition.x = screenWidth;
            if (mousePosition.y > screenHeight) mousePosition.y = screenHeight;
       }
       //else read(mouseStream, &mouse, 1);   // Try to sync up again
    }

    return NULL;
}

// Touch initialization (including touch thread)
static void InitTouch(void)
{
    if ((touchStream = open(DEFAULT_TOUCH_DEV, O_RDONLY|O_NONBLOCK)) < 0)
    {
        TraceLog(LOG_WARNING, "Touch device could not be opened, no touchscreen available");
    }
    else
    {
        touchReady = true;

        int error = pthread_create(&touchThreadId, NULL, &TouchThread, NULL);

        if (error != 0) TraceLog(LOG_WARNING, "Error creating touch input event thread");
        else TraceLog(LOG_INFO, "Touch device initialized successfully");
    }
}

// Touch reading thread.
// This reads from a Virtual Input Event /dev/input/event4 which is
// created by the ts_uinput daemon. This takes, filters and scales
// raw input from the Touchscreen (which appears in /dev/input/event3)
// based on the Calibration data referenced by tslib.
static void *TouchThread(void *arg)
{
    struct input_event ev;
    GestureEvent gestureEvent;

    while (!windowShouldClose)
    {
        if (read(touchStream, &ev, sizeof(ev)) == (int)sizeof(ev))
        {
            // if pressure > 0 then simulate left mouse button click
            if (ev.type == EV_ABS && ev.code == 24 && ev.value == 0 && currentMouseState[0] == 1)
            {
                currentMouseState[0] = 0;
                gestureEvent.touchAction = TOUCH_UP;
                gestureEvent.pointCount = 1;
                gestureEvent.pointerId[0] = 0;
                gestureEvent.pointerId[1] = 1;
                gestureEvent.position[0] = (Vector2){ mousePosition.x, mousePosition.y };
                gestureEvent.position[1] = (Vector2){ mousePosition.x, mousePosition.y };
                gestureEvent.position[0].x /= (float)GetScreenWidth();
                gestureEvent.position[0].y /= (float)GetScreenHeight();
                gestureEvent.position[1].x /= (float)GetScreenWidth();
                gestureEvent.position[1].y /= (float)GetScreenHeight();
                ProcessGestureEvent(gestureEvent);
            }
            if (ev.type == EV_ABS && ev.code == 24 && ev.value > 0 && currentMouseState[0] == 0)
            {
                currentMouseState[0] = 1;
                gestureEvent.touchAction = TOUCH_DOWN;
                gestureEvent.pointCount = 1;
                gestureEvent.pointerId[0] = 0;
                gestureEvent.pointerId[1] = 1;
                gestureEvent.position[0] = (Vector2){ mousePosition.x, mousePosition.y };
                gestureEvent.position[1] = (Vector2){ mousePosition.x, mousePosition.y };
                gestureEvent.position[0].x /= (float)GetScreenWidth();
                gestureEvent.position[0].y /= (float)GetScreenHeight();
                gestureEvent.position[1].x /= (float)GetScreenWidth();
                gestureEvent.position[1].y /= (float)GetScreenHeight();
                ProcessGestureEvent(gestureEvent);
            }
            // x & y values supplied by event4 have been scaled & de-jittered using tslib calibration data
            if (ev.type == EV_ABS && ev.code == 0)
            {
                mousePosition.x = ev.value;
                if (mousePosition.x < 0) mousePosition.x = 0;
                if (mousePosition.x > screenWidth) mousePosition.x = screenWidth;
                gestureEvent.touchAction = TOUCH_MOVE;
                gestureEvent.pointCount = 1;
                gestureEvent.pointerId[0] = 0;
                gestureEvent.pointerId[1] = 1;
                gestureEvent.position[0] = (Vector2){ mousePosition.x, mousePosition.y };
                gestureEvent.position[1] = (Vector2){ mousePosition.x, mousePosition.y };
                gestureEvent.position[0].x /= (float)GetScreenWidth();
                gestureEvent.position[0].y /= (float)GetScreenHeight();
                gestureEvent.position[1].x /= (float)GetScreenWidth();
                gestureEvent.position[1].y /= (float)GetScreenHeight();
                ProcessGestureEvent(gestureEvent);
            }
            if (ev.type == EV_ABS && ev.code == 1)
            {
                mousePosition.y = ev.value;
                if (mousePosition.y < 0) mousePosition.y = 0;
                if (mousePosition.y > screenHeight) mousePosition.y = screenHeight;
                gestureEvent.touchAction = TOUCH_MOVE;
                gestureEvent.pointCount = 1;
                gestureEvent.pointerId[0] = 0;
                gestureEvent.pointerId[1] = 1;
                gestureEvent.position[0] = (Vector2){ mousePosition.x, mousePosition.y };
                gestureEvent.position[1] = (Vector2){ mousePosition.x, mousePosition.y };
                gestureEvent.position[0].x /= (float)GetScreenWidth();
                gestureEvent.position[0].y /= (float)GetScreenHeight();
                gestureEvent.position[1].x /= (float)GetScreenWidth();
                gestureEvent.position[1].y /= (float)GetScreenHeight();
                ProcessGestureEvent(gestureEvent);
            }

        }
    }
    return NULL;
}

// Init gamepad system
static void InitGamepad(void)
{
    char gamepadDev[128] = "";

    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        sprintf(gamepadDev, "%s%i", DEFAULT_GAMEPAD_DEV, i);

        if ((gamepadStream[i] = open(gamepadDev, O_RDONLY|O_NONBLOCK)) < 0)
        {
            // NOTE: Only show message for first gamepad
            if (i == 0) TraceLog(LOG_WARNING, "Gamepad device could not be opened, no gamepad available");
        }
        else
        {
            gamepadReady[i] = true;

            // NOTE: Only create one thread
            if (i == 0)
            {
                int error = pthread_create(&gamepadThreadId, NULL, &GamepadThread, NULL);

                if (error != 0) TraceLog(LOG_WARNING, "Error creating gamepad input event thread");
                else  TraceLog(LOG_INFO, "Gamepad device initialized successfully");
            }
        }
    }
}

// Process Gamepad (/dev/input/js0)
static void *GamepadThread(void *arg)
{
    #define JS_EVENT_BUTTON         0x01    // Button pressed/released
    #define JS_EVENT_AXIS           0x02    // Joystick axis moved
    #define JS_EVENT_INIT           0x80    // Initial state of device

    struct js_event {
        unsigned int time;      // event timestamp in milliseconds
        short value;            // event value
        unsigned char type;     // event type
        unsigned char number;   // event axis/button number
    };

    // Read gamepad event
    struct js_event gamepadEvent;

    while (!windowShouldClose)
    {
        for (int i = 0; i < MAX_GAMEPADS; i++)
        {
            if (read(gamepadStream[i], &gamepadEvent, sizeof(struct js_event)) == (int)sizeof(struct js_event))
            {
                gamepadEvent.type &= ~JS_EVENT_INIT;     // Ignore synthetic events

                // Process gamepad events by type
                if (gamepadEvent.type == JS_EVENT_BUTTON)
                {
                    TraceLog(LOG_DEBUG, "Gamepad button: %i, value: %i", gamepadEvent.number, gamepadEvent.value);

                    if (gamepadEvent.number < MAX_GAMEPAD_BUTTONS)
                    {
                        // 1 - button pressed, 0 - button released
                        currentGamepadState[i][gamepadEvent.number] = (int)gamepadEvent.value;

                        if ((int)gamepadEvent.value == 1) lastGamepadButtonPressed = gamepadEvent.number;
                        else lastGamepadButtonPressed = -1;
                    }
                }
                else if (gamepadEvent.type == JS_EVENT_AXIS)
                {
                    TraceLog(LOG_DEBUG, "Gamepad axis: %i, value: %i", gamepadEvent.number, gamepadEvent.value);

                    if (gamepadEvent.number < MAX_GAMEPAD_AXIS)
                    {
                        // NOTE: Scaling of gamepadEvent.value to get values between -1..1
                        gamepadAxisState[i][gamepadEvent.number] = (float)gamepadEvent.value/32768;
                    }
                }
            }
        }
    }

    return NULL;
}
#endif      // PLATFORM_RPI

// Plays raylib logo appearing animation
static void LogoAnimation(void)
{
#if !defined(PLATFORM_WEB)
    int logoPositionX = screenWidth/2 - 128;
    int logoPositionY = screenHeight/2 - 128;

    int framesCounter = 0;
    int lettersCount = 0;

    int topSideRecWidth = 16;
    int leftSideRecHeight = 16;

    int bottomSideRecWidth = 16;
    int rightSideRecHeight = 16;

    int state = 0;                  // Tracking animation states (State Machine)
    float alpha = 1.0f;             // Useful for fading

    while (!WindowShouldClose() && (state != 4))    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (state == 0)                 // State 0: Small box blinking
        {
            framesCounter++;

            if (framesCounter == 84)
            {
                state = 1;
                framesCounter = 0;      // Reset counter... will be used later...
            }
        }
        else if (state == 1)            // State 1: Top and left bars growing
        {
            topSideRecWidth += 4;
            leftSideRecHeight += 4;

            if (topSideRecWidth == 256) state = 2;
        }
        else if (state == 2)            // State 2: Bottom and right bars growing
        {
            bottomSideRecWidth += 4;
            rightSideRecHeight += 4;

            if (bottomSideRecWidth == 256) state = 3;
        }
        else if (state == 3)            // State 3: Letters appearing (one by one)
        {
            framesCounter++;

            if (framesCounter/12)       // Every 12 frames, one more letter!
            {
                lettersCount++;
                framesCounter = 0;
            }

            if (lettersCount >= 10)     // When all letters have appeared, just fade out everything
            {
                alpha -= 0.02f;

                if (alpha <= 0.0f)
                {
                    alpha = 0.0f;
                    state = 4;
                }
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            if (state == 0)
            {
                if ((framesCounter/12)%2) DrawRectangle(logoPositionX, logoPositionY, 16, 16, BLACK);
            }
            else if (state == 1)
            {
                DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
                DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);
            }
            else if (state == 2)
            {
                DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
                DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);

                DrawRectangle(logoPositionX + 240, logoPositionY, 16, rightSideRecHeight, BLACK);
                DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, BLACK);
            }
            else if (state == 3)
            {
                DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, Fade(BLACK, alpha));
                DrawRectangle(logoPositionX, logoPositionY + 16, 16, leftSideRecHeight - 32, Fade(BLACK, alpha));

                DrawRectangle(logoPositionX + 240, logoPositionY + 16, 16, rightSideRecHeight - 32, Fade(BLACK, alpha));
                DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, Fade(BLACK, alpha));

                DrawRectangle(screenWidth/2 - 112, screenHeight/2 - 112, 224, 224, Fade(RAYWHITE, alpha));

                DrawText(SubText("raylib", 0, lettersCount), screenWidth/2 - 44, screenHeight/2 + 48, 50, Fade(BLACK, alpha));
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
#endif

    showLogo = false;  // Prevent for repeating when reloading window (Android)
}





/*??? rlgl */
/**********************************************************************************************
*
*   rlgl - raylib OpenGL abstraction layer
*
*   rlgl is a wrapper for multiple OpenGL versions (1.1, 2.1, 3.3 Core, ES 2.0) to 
*   pseudo-OpenGL 1.1 style functions (rlVertex, rlTranslate, rlRotate...). 
*
*   When chosing an OpenGL version greater than OpenGL 1.1, rlgl stores vertex data on internal
*   VBO buffers (and VAOs if available). It requires calling 3 functions:
*       rlglInit()  - Initialize internal buffers and auxiliar resources
*       rlglDraw()  - Process internal buffers and send required draw calls
*       rlglClose() - De-initialize internal buffers data and other auxiliar resources
*
*   CONFIGURATION:
*
*   #define GRAPHICS_API_OPENGL_11
*   #define GRAPHICS_API_OPENGL_21
*   #define GRAPHICS_API_OPENGL_33
*   #define GRAPHICS_API_OPENGL_ES2
*       Use selected OpenGL backend
*
*   #define RLGL_STANDALONE
*       Use rlgl as standalone library (no raylib dependency)
*
*   #define SUPPORT_VR_SIMULATOR
*       Support VR simulation functionality (stereo rendering)
*
*   #define SUPPORT_DISTORTION_SHADER
*       Include stereo rendering distortion shader (shader_distortion.h)
*
*   DEPENDENCIES:
*       raymath     - 3D math functionality (Vector3, Matrix, Quaternion)
*       GLAD        - OpenGL extensions loading (OpenGL 3.3 Core only)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2018 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/


//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define MATRIX_STACK_SIZE          16   // Matrix stack max size
#define MAX_DRAWS_BY_TEXTURE      256   // Draws are organized by texture changes
#define TEMP_VERTEX_BUFFER_SIZE  4096   // Temporal Vertex Buffer (required for vertex-transformations)
                                        // NOTE: Every vertex are 3 floats (12 bytes)

#ifndef GL_SHADING_LANGUAGE_VERSION
    #define GL_SHADING_LANGUAGE_VERSION         0x8B8C
#endif

#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
    #define GL_COMPRESSED_RGB_S3TC_DXT1_EXT     0x83F0
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
    #define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT    0x83F1
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
    #define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT    0x83F2
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
    #define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT    0x83F3
#endif
#ifndef GL_ETC1_RGB8_OES
    #define GL_ETC1_RGB8_OES                    0x8D64
#endif
#ifndef GL_COMPRESSED_RGB8_ETC2
    #define GL_COMPRESSED_RGB8_ETC2             0x9274
#endif
#ifndef GL_COMPRESSED_RGBA8_ETC2_EAC
    #define GL_COMPRESSED_RGBA8_ETC2_EAC        0x9278
#endif
#ifndef GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
    #define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG  0x8C00
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
    #define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_4x4_KHR
    #define GL_COMPRESSED_RGBA_ASTC_4x4_KHR     0x93b0
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_8x8_KHR
    #define GL_COMPRESSED_RGBA_ASTC_8x8_KHR     0x93b7
#endif

#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
    #define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT   0x84FF
#endif

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
    #define GL_TEXTURE_MAX_ANISOTROPY_EXT       0x84FE
#endif

#if defined(GRAPHICS_API_OPENGL_11)
    #define GL_UNSIGNED_SHORT_5_6_5             0x8363
    #define GL_UNSIGNED_SHORT_5_5_5_1           0x8034
    #define GL_UNSIGNED_SHORT_4_4_4_4           0x8033
#endif

#if defined(GRAPHICS_API_OPENGL_21)
    #define GL_LUMINANCE                        0x1909
    #define GL_LUMINANCE_ALPHA                  0x190A
#endif

#if defined(GRAPHICS_API_OPENGL_ES2)
    #define glClearDepth                glClearDepthf
    #define GL_READ_FRAMEBUFFER         GL_FRAMEBUFFER
    #define GL_DRAW_FRAMEBUFFER         GL_FRAMEBUFFER
#endif

// Default vertex attribute names on shader to set location points
#define DEFAULT_ATTRIB_POSITION_NAME    "vertexPosition"    // shader-location = 0
#define DEFAULT_ATTRIB_TEXCOORD_NAME    "vertexTexCoord"    // shader-location = 1
#define DEFAULT_ATTRIB_NORMAL_NAME      "vertexNormal"      // shader-location = 2
#define DEFAULT_ATTRIB_COLOR_NAME       "vertexColor"       // shader-location = 3
#define DEFAULT_ATTRIB_TANGENT_NAME     "vertexTangent"     // shader-location = 4
#define DEFAULT_ATTRIB_TEXCOORD2_NAME   "vertexTexCoord2"   // shader-location = 5

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
typedef struct DynamicBuffer {
    int vCounter;               // vertex position counter to process (and draw) from full buffer
    int tcCounter;              // vertex texcoord counter to process (and draw) from full buffer
    int cCounter;               // vertex color counter to process (and draw) from full buffer
    float *vertices;            // vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float *texcoords;           // vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    unsigned char *colors;      // vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
    unsigned int *indices;      // vertex indices (in case vertex data comes indexed) (6 indices per quad)
#elif defined(GRAPHICS_API_OPENGL_ES2)
    unsigned short *indices;    // vertex indices (in case vertex data comes indexed) (6 indices per quad)
                                // NOTE: 6*2 byte = 12 byte, not alignment problem!
#endif
    unsigned int vaoId;         // OpenGL Vertex Array Object id
    unsigned int vboId[4];      // OpenGL Vertex Buffer Objects id (4 types of vertex data)
} DynamicBuffer;

// Draw call type
// NOTE: Used to track required draw-calls, organized by texture
typedef struct DrawCall {
    int vertexCount;
    GLuint vaoId;
    GLuint textureId;
    GLuint shaderId;

    Matrix projection;
    Matrix modelview;

    // TODO: Store additional draw state data
    //int blendMode;
    //Guint fboId;
} DrawCall;

#if defined(SUPPORT_VR_SIMULATOR)
#ifndef PLATFORM_SILENT_RADIANCE
// VR Stereo rendering configuration for simulator
typedef struct VrStereoConfig {
    RenderTexture2D stereoFbo;      // VR stereo rendering framebuffer
    Shader distortionShader;        // VR stereo rendering distortion shader
    Rectangle eyesViewport[2];      // VR stereo rendering eyes viewports
    Matrix eyesProjection[2];       // VR stereo rendering eyes projection matrices
    Matrix eyesViewOffset[2];       // VR stereo rendering eyes view offset matrices
} VrStereoConfig;
#endif
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
static Matrix stack[MATRIX_STACK_SIZE];
static int stackCounter = 0;

static Matrix modelview;
static Matrix projection;
static Matrix *currentMatrix;
static int currentMatrixMode;

static int currentDrawMode;

static float currentDepth = -1.0f;

static DynamicBuffer lines;                 // Default dynamic buffer for lines data
static DynamicBuffer triangles;             // Default dynamic buffer for triangles data
static DynamicBuffer quads;                 // Default dynamic buffer for quads data (used to draw textures)

// Default buffers draw calls
static DrawCall *draws;
static int drawsCounter;

// Temp vertex buffer to be used with rlTranslate, rlRotate, rlScale
static Vector3 *tempBuffer;
static int tempBufferCount = 0;
static bool useTempBuffer = false;

// Shaders
static unsigned int defaultVShaderId;       // Default vertex shader id (used by default shader program)
static unsigned int defaultFShaderId;       // Default fragment shader Id (used by default shader program)

static Shader defaultShader;                // Basic shader, support vertex color and diffuse texture
static Shader currentShader;                // Shader to be used on rendering (by default, defaultShader)

// Extension supported flag: VAO
static bool vaoSupported = false;           // VAO support (OpenGL ES2 could not support VAO extension)

// Extension supported flag: Compressed textures
static bool texCompETC1Supported = false;   // ETC1 texture compression support
static bool texCompETC2Supported = false;   // ETC2/EAC texture compression support
static bool texCompPVRTSupported = false;   // PVR texture compression support
static bool texCompASTCSupported = false;   // ASTC texture compression support

#if defined(SUPPORT_VR_SIMULATOR)
// VR global variables
#ifdef PLATFORM_SILENT_RADIANCE
VrStereoConfig vrConfig;         // VR stereo configuration for simulator
#else
static VrStereoConfig vrConfig; // non static version
#endif

static bool vrSimulatorReady = false;   // VR simulator ready flag
static bool vrStereoRender = false;     // VR stereo rendering enabled/disabled flag
                                        // NOTE: This flag is useful to render data over stereo image (i.e. FPS)
#endif  // defined(SUPPORT_VR_SIMULATOR)

#endif  // defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)

// Extension supported flag: Anisotropic filtering
static bool texAnisotropicFilterSupported = false;  // Anisotropic texture filtering support
static float maxAnisotropicLevel = 0.0f;        // Maximum anisotropy level supported (minimum is 2.0f)

// Extension supported flag: Clamp mirror wrap mode
static bool texClampMirrorSupported = false;    // Clamp mirror wrap mode supported

#if defined(GRAPHICS_API_OPENGL_ES2)
#ifndef __APPLE__
#ifdef ANDROID
// NOTE: VAO functionality is exposed through extensions (OES)
static GLvoid (*glGenVertexArrays)(GLsizei n, GLuint *arrays) ;
static GLvoid (*glBindVertexArray)(GLuint array);
static GLvoid (*glDeleteVertexArrays)(GLsizei n, const GLuint *arrays);
#else
// NOTE: VAO functionality is exposed through extensions (OES)
static PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays;
static PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray;
static PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays;
//static PFNGLISVERTEXARRAYOESPROC glIsVertexArray;        // NOTE: Fails in WebGL, omitted
#endif
#else 
// NOTE: VAO functionality is exposed through extensions (OES)
static GLvoid (*glGenVertexArrays)(GLsizei n, GLuint *arrays) ;
static GLvoid (*glBindVertexArray)(GLuint array);
static GLvoid (*glDeleteVertexArrays)(GLsizei n, const GLuint *arrays);
#endif
#endif

static bool debugMarkerSupported = false;

// Compressed textures support flags
static bool texCompDXTSupported = false;    // DDS texture compression support
static bool texNPOTSupported = false;       // NPOT textures full support
static bool texFloatSupported = false;      // float textures support (32 bit per channel)

static int blendMode = 0;   // Track current blending mode

// White texture useful for plain color polys (required by shader)
static unsigned int whiteTexture;

// Default framebuffer size
static int screenWidth;     // Default framebuffer width
static int screenHeight;    // Default framebuffer height

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
static unsigned int CompileShader(const char *shaderStr, int type);     // Compile custom shader and return shader id
static unsigned int LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId);  // Load custom shader program

static Shader LoadShaderDefault(void);      // Load default shader (just vertex positioning and texture coloring)
static void SetShaderDefaultLocations(Shader *shader); // Bind default shader locations (attributes and uniforms)
static void UnloadShaderDefault(void);      // Unload default shader

static void LoadBuffersDefault(void);       // Load default internal buffers (lines, triangles, quads)
static void UpdateBuffersDefault(void);     // Update default internal buffers (VAOs/VBOs) with vertex data
static void DrawBuffersDefault(void);       // Draw default internal buffers vertex data
static void UnloadBuffersDefault(void);     // Unload default internal buffers vertex data from CPU and GPU

static void GenDrawCube(void);              // Generate and draw cube
static void GenDrawQuad(void);              // Generate and draw quad

// Get OpenGL internal formats and data type from raylib PixelFormat
static void GetGlFormats(int format, int *glInternalFormat, int *glFormat, int *glType);

#if defined(SUPPORT_VR_SIMULATOR)
static void SetStereoConfig(VrDeviceInfo info); // Configure stereo rendering (including distortion shader) with HMD device parameters
static void SetStereoView(int eye, Matrix matProjection, Matrix matModelView); // Set internal projection and modelview matrix depending on eye
#endif

#endif  // defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)

#if defined(GRAPHICS_API_OPENGL_11)
static int GenerateMipmaps(unsigned char *data, int baseWidth, int baseHeight);
static Color *GenNextMipmap(Color *srcData, int srcWidth, int srcHeight);
#endif

//----------------------------------------------------------------------------------
// Module Functions Definition - Matrix operations
//----------------------------------------------------------------------------------

#if defined(GRAPHICS_API_OPENGL_11)

// Fallback to OpenGL 1.1 function calls
//---------------------------------------
void rlMatrixMode(int mode)
{
    switch (mode)
    {
        case RL_PROJECTION: glMatrixMode(GL_PROJECTION); break;
        case RL_MODELVIEW: glMatrixMode(GL_MODELVIEW); break;
        case RL_TEXTURE: glMatrixMode(GL_TEXTURE); break;
        default: break;
    }
}

void rlFrustum(double left, double right, double bottom, double top, double zNear, double zFar)
{
    glFrustum(left, right, bottom, top, zNear, zFar);
}

void rlOrtho(double left, double right, double bottom, double top, double zNear, double zFar)
{
    glOrtho(left, right, bottom, top, zNear, zFar);
}

void rlPushMatrix(void) { glPushMatrix(); }
void rlPopMatrix(void) { glPopMatrix(); }
void rlLoadIdentity(void) { glLoadIdentity(); }
void rlTranslatef(float x, float y, float z) { glTranslatef(x, y, z); }
void rlRotatef(float angleDeg, float x, float y, float z) { glRotatef(angleDeg, x, y, z); }
void rlScalef(float x, float y, float z) { glScalef(x, y, z); }
void rlMultMatrixf(float *matf) { glMultMatrixf(matf); }

#elif defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)

// Choose the current matrix to be transformed
void rlMatrixMode(int mode)
{
    if (mode == RL_PROJECTION) currentMatrix = &projection;
    else if (mode == RL_MODELVIEW) currentMatrix = &modelview;
    //else if (mode == RL_TEXTURE) // Not supported

    currentMatrixMode = mode;
}

// Push the current matrix to stack
void rlPushMatrix(void)
{
    if (stackCounter == MATRIX_STACK_SIZE - 1)
    {
        TraceLog(LOG_ERROR, "Stack Buffer Overflow (MAX %i Matrix)", MATRIX_STACK_SIZE);
    }

    stack[stackCounter] = *currentMatrix;
    rlLoadIdentity();       // TODO: Review matrix stack logic!
    stackCounter++;

    if (currentMatrixMode == RL_MODELVIEW) useTempBuffer = true;
}

// Pop lattest inserted matrix from stack
void rlPopMatrix(void)
{
    if (stackCounter > 0)
    {
        Matrix mat = stack[stackCounter - 1];
        *currentMatrix = mat;
        stackCounter--;
    }
}

// Reset current matrix to identity matrix
void rlLoadIdentity(void)
{
    *currentMatrix = MatrixIdentity();
}

// Multiply the current matrix by a translation matrix
void rlTranslatef(float x, float y, float z)
{
    Matrix matTranslation = MatrixTranslate(x, y, z);

    // NOTE: We transpose matrix with multiplication order
    *currentMatrix = MatrixMultiply(matTranslation, *currentMatrix);
}

// Multiply the current matrix by a rotation matrix
void rlRotatef(float angleDeg, float x, float y, float z)
{
    Matrix matRotation = MatrixIdentity();

    Vector3 axis = (Vector3){ x, y, z };
    Vector3Normalize(&axis);
    matRotation = MatrixRotate(axis, angleDeg*DEG2RAD);

    // NOTE: We transpose matrix with multiplication order
    *currentMatrix = MatrixMultiply(matRotation, *currentMatrix);
}

// Multiply the current matrix by a scaling matrix
void rlScalef(float x, float y, float z)
{
    Matrix matScale = MatrixScale(x, y, z);

    // NOTE: We transpose matrix with multiplication order
    *currentMatrix = MatrixMultiply(matScale, *currentMatrix);
}

// Multiply the current matrix by another matrix
void rlMultMatrixf(float *matf)
{
    // Matrix creation from array
    Matrix mat = { matf[0], matf[4], matf[8], matf[12],
                   matf[1], matf[5], matf[9], matf[13],
                   matf[2], matf[6], matf[10], matf[14],
                   matf[3], matf[7], matf[11], matf[15] };

    *currentMatrix = MatrixMultiply(*currentMatrix, mat);
}

// Multiply the current matrix by a perspective matrix generated by parameters
void rlFrustum(double left, double right, double bottom, double top, double near, double far)
{
    Matrix matPerps = MatrixFrustum(left, right, bottom, top, near, far);

    *currentMatrix = MatrixMultiply(*currentMatrix, matPerps);
}

// Multiply the current matrix by an orthographic matrix generated by parameters
void rlOrtho(double left, double right, double bottom, double top, double near, double far)
{
    Matrix matOrtho = MatrixOrtho(left, right, bottom, top, near, far);

    *currentMatrix = MatrixMultiply(*currentMatrix, matOrtho);
}

#endif

// Set the viewport area (transformation from normalized device coordinates to window coordinates)
// NOTE: Updates global variables: screenWidth, screenHeight
void rlViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Vertex level operations
//----------------------------------------------------------------------------------
#if defined(GRAPHICS_API_OPENGL_11)

// Fallback to OpenGL 1.1 function calls
//---------------------------------------
void rlBegin(int mode)
{
    switch (mode)
    {
        case RL_LINES: glBegin(GL_LINES); break;
        case RL_TRIANGLES: glBegin(GL_TRIANGLES); break;
        case RL_QUADS: glBegin(GL_QUADS); break;
        default: break;
    }
}

void rlEnd() { glEnd(); }
void rlVertex2i(int x, int y) { glVertex2i(x, y); }
void rlVertex2f(float x, float y) { glVertex2f(x, y); }
void rlVertex3f(float x, float y, float z) { glVertex3f(x, y, z); }
void rlTexCoord2f(float x, float y) { glTexCoord2f(x, y); }
void rlNormal3f(float x, float y, float z) { glNormal3f(x, y, z); }
void rlColor4ub(byte r, byte g, byte b, byte a) { glColor4ub(r, g, b, a); }
void rlColor3f(float x, float y, float z) { glColor3f(x, y, z); }
void rlColor4f(float x, float y, float z, float w) { glColor4f(x, y, z, w); }

#elif defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)

// Initialize drawing mode (how to organize vertex)
void rlBegin(int mode)
{
    // Draw mode can only be RL_LINES, RL_TRIANGLES and RL_QUADS
    currentDrawMode = mode;
}

// Finish vertex providing
void rlEnd(void)
{
    if (useTempBuffer)
    {
        // NOTE: In this case, *currentMatrix is already transposed because transposing has been applied
        // independently to translation-scale-rotation matrices -> t(M1 x M2) = t(M2) x t(M1)
        // This way, rlTranslatef(), rlRotatef()... behaviour is the same than OpenGL 1.1

        // Apply transformation matrix to all temp vertices
        for (int i = 0; i < tempBufferCount; i++) Vector3Transform(&tempBuffer[i], *currentMatrix);

        // Deactivate tempBuffer usage to allow rlVertex3f do its job
        useTempBuffer = false;

        // Copy all transformed vertices to right VAO
        for (int i = 0; i < tempBufferCount; i++) rlVertex3f(tempBuffer[i].x, tempBuffer[i].y, tempBuffer[i].z);

        // Reset temp buffer
        tempBufferCount = 0;
    }

    // Make sure vertexCount is the same for vertices-texcoords-normals-colors
    // NOTE: In OpenGL 1.1, one glColor call can be made for all the subsequent glVertex calls.
    switch (currentDrawMode)
    {
        case RL_LINES:
        {
            if (lines.vCounter != lines.cCounter)
            {
                int addColors = lines.vCounter - lines.cCounter;

                for (int i = 0; i < addColors; i++)
                {
                    lines.colors[4*lines.cCounter] = lines.colors[4*lines.cCounter - 4];
                    lines.colors[4*lines.cCounter + 1] = lines.colors[4*lines.cCounter - 3];
                    lines.colors[4*lines.cCounter + 2] = lines.colors[4*lines.cCounter - 2];
                    lines.colors[4*lines.cCounter + 3] = lines.colors[4*lines.cCounter - 1];

                    lines.cCounter++;
                }
            }
        } break;
        case RL_TRIANGLES:
        {
            if (triangles.vCounter != triangles.cCounter)
            {
                int addColors = triangles.vCounter - triangles.cCounter;

                for (int i = 0; i < addColors; i++)
                {
                    triangles.colors[4*triangles.cCounter] = triangles.colors[4*triangles.cCounter - 4];
                    triangles.colors[4*triangles.cCounter + 1] = triangles.colors[4*triangles.cCounter - 3];
                    triangles.colors[4*triangles.cCounter + 2] = triangles.colors[4*triangles.cCounter - 2];
                    triangles.colors[4*triangles.cCounter + 3] = triangles.colors[4*triangles.cCounter - 1];

                    triangles.cCounter++;
                }
            }
        } break;
        case RL_QUADS:
        {
            // Make sure colors count match vertex count
            if (quads.vCounter != quads.cCounter)
            {
                int addColors = quads.vCounter - quads.cCounter;

                for (int i = 0; i < addColors; i++)
                {
                    quads.colors[4*quads.cCounter] = quads.colors[4*quads.cCounter - 4];
                    quads.colors[4*quads.cCounter + 1] = quads.colors[4*quads.cCounter - 3];
                    quads.colors[4*quads.cCounter + 2] = quads.colors[4*quads.cCounter - 2];
                    quads.colors[4*quads.cCounter + 3] = quads.colors[4*quads.cCounter - 1];

                    quads.cCounter++;
                }
            }

            // Make sure texcoords count match vertex count
            if (quads.vCounter != quads.tcCounter)
            {
                int addTexCoords = quads.vCounter - quads.tcCounter;

                for (int i = 0; i < addTexCoords; i++)
                {
                    quads.texcoords[2*quads.tcCounter] = 0.0f;
                    quads.texcoords[2*quads.tcCounter + 1] = 0.0f;

                    quads.tcCounter++;
                }
            }

            // TODO: Make sure normals count match vertex count... if normals support is added in a future... :P

        } break;
        default: break;
    }

    // NOTE: Depth increment is dependant on rlOrtho(): z-near and z-far values,
    // as well as depth buffer bit-depth (16bit or 24bit or 32bit)
    // Correct increment formula would be: depthInc = (zfar - znear)/pow(2, bits)
    currentDepth += (1.0f/20000.0f);
    
    // TODO: Verify internal buffers limits
    // NOTE: Before launching draw, verify no matrix are left in the stack!
    // NOTE: Probably a lines/triangles margin should be left, rlEnd could be called
    // after an undetermined number of triangles buffered (check shapes::DrawPoly())
    if ((lines.vCounter/2 >= MAX_LINES_BATCH - 2) ||
        (triangles.vCounter/3 >= MAX_TRIANGLES_BATCH - 16) ||
        (quads.vCounter/4 >= MAX_QUADS_BATCH - 2)) rlglDraw();
}

// Define one vertex (position)
void rlVertex3f(float x, float y, float z)
{
    if (useTempBuffer)
    {
        tempBuffer[tempBufferCount].x = x;
        tempBuffer[tempBufferCount].y = y;
        tempBuffer[tempBufferCount].z = z;
        tempBufferCount++;
    }
    else
    {
        switch (currentDrawMode)
        {
            case RL_LINES:
            {
                // Verify that MAX_LINES_BATCH limit not reached
                if (lines.vCounter/2 < MAX_LINES_BATCH)
                {
                    lines.vertices[3*lines.vCounter] = x;
                    lines.vertices[3*lines.vCounter + 1] = y;
                    lines.vertices[3*lines.vCounter + 2] = z;

                    lines.vCounter++;
                }
                else TraceLog(LOG_ERROR, "MAX_LINES_BATCH overflow");

            } break;
            case RL_TRIANGLES:
            {
                // Verify that MAX_TRIANGLES_BATCH limit not reached
                if (triangles.vCounter/3 < MAX_TRIANGLES_BATCH)
                {
                    triangles.vertices[3*triangles.vCounter] = x;
                    triangles.vertices[3*triangles.vCounter + 1] = y;
                    triangles.vertices[3*triangles.vCounter + 2] = z;

                    triangles.vCounter++;
                }
                else TraceLog(LOG_ERROR, "MAX_TRIANGLES_BATCH overflow");

            } break;
            case RL_QUADS:
            {
                // Verify that MAX_QUADS_BATCH limit not reached
                if (quads.vCounter/4 < MAX_QUADS_BATCH)
                {
                    quads.vertices[3*quads.vCounter] = x;
                    quads.vertices[3*quads.vCounter + 1] = y;
                    quads.vertices[3*quads.vCounter + 2] = z;

                    quads.vCounter++;

                    draws[drawsCounter - 1].vertexCount++;
                }
                else TraceLog(LOG_ERROR, "MAX_QUADS_BATCH overflow");

            } break;
            default: break;
        }
    }
}

// Define one vertex (position)
void rlVertex2f(float x, float y)
{
    rlVertex3f(x, y, currentDepth);
}

// Define one vertex (position)
void rlVertex2i(int x, int y)
{
    rlVertex3f((float)x, (float)y, currentDepth);
}

// Define one vertex (texture coordinate)
// NOTE: Texture coordinates are limited to QUADS only
void rlTexCoord2f(float x, float y)
{
    if (currentDrawMode == RL_QUADS)
    {
        quads.texcoords[2*quads.tcCounter] = x;
        quads.texcoords[2*quads.tcCounter + 1] = y;

        quads.tcCounter++;
    }
}

// Define one vertex (normal)
// NOTE: Normals limited to TRIANGLES only ?
void rlNormal3f(float x, float y, float z)
{
    // TODO: Normals usage...
}

// Define one vertex (color)
void rlColor4ub(byte x, byte y, byte z, byte w)
{
    switch (currentDrawMode)
    {
        case RL_LINES:
        {
            lines.colors[4*lines.cCounter] = x;
            lines.colors[4*lines.cCounter + 1] = y;
            lines.colors[4*lines.cCounter + 2] = z;
            lines.colors[4*lines.cCounter + 3] = w;

            lines.cCounter++;

        } break;
        case RL_TRIANGLES:
        {
            triangles.colors[4*triangles.cCounter] = x;
            triangles.colors[4*triangles.cCounter + 1] = y;
            triangles.colors[4*triangles.cCounter + 2] = z;
            triangles.colors[4*triangles.cCounter + 3] = w;

            triangles.cCounter++;

        } break;
        case RL_QUADS:
        {
            quads.colors[4*quads.cCounter] = x;
            quads.colors[4*quads.cCounter + 1] = y;
            quads.colors[4*quads.cCounter + 2] = z;
            quads.colors[4*quads.cCounter + 3] = w;

            quads.cCounter++;

        } break;
        default: break;
    }
}

// Define one vertex (color)
void rlColor4f(float r, float g, float b, float a)
{
    rlColor4ub((byte)(r*255), (byte)(g*255), (byte)(b*255), (byte)(a*255));
}

// Define one vertex (color)
void rlColor3f(float x, float y, float z)
{
    rlColor4ub((byte)(x*255), (byte)(y*255), (byte)(z*255), 255);
}

#endif

//----------------------------------------------------------------------------------
// Module Functions Definition - OpenGL equivalent functions (common to 1.1, 3.3+, ES2)
//----------------------------------------------------------------------------------

// Enable texture usage
void rlEnableTexture(unsigned int id)
{
#if defined(GRAPHICS_API_OPENGL_11)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, id);
#endif

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    if (draws[drawsCounter - 1].textureId != id)
    {
        if (draws[drawsCounter - 1].vertexCount > 0) drawsCounter++;
        
        if (drawsCounter >= MAX_DRAWS_BY_TEXTURE)
        {
            rlglDraw();
            drawsCounter = 1;
        }

        draws[drawsCounter - 1].textureId = id;
        draws[drawsCounter - 1].vertexCount = 0;
    }
#endif
}

// Disable texture usage
void rlDisableTexture(void)
{
#if defined(GRAPHICS_API_OPENGL_11)
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
#else
    // NOTE: If quads batch limit is reached,
    // we force a draw call and next batch starts
    if (quads.vCounter/4 >= MAX_QUADS_BATCH) rlglDraw();
#endif
}

// Set texture parameters (wrap mode/filter mode)
void rlTextureParameters(unsigned int id, int param, int value)
{
    glBindTexture(GL_TEXTURE_2D, id);

    switch (param)
    {
        case RL_TEXTURE_WRAP_S:
        case RL_TEXTURE_WRAP_T:
        {
            if ((value == RL_WRAP_CLAMP_MIRROR) && !texClampMirrorSupported) TraceLog(LOG_WARNING, "Clamp mirror wrap mode not supported");
            else glTexParameteri(GL_TEXTURE_2D, param, value);
        } break;
        case RL_TEXTURE_MAG_FILTER:
        case RL_TEXTURE_MIN_FILTER: glTexParameteri(GL_TEXTURE_2D, param, value); break;
        case RL_TEXTURE_ANISOTROPIC_FILTER:
        {
            if (value <= maxAnisotropicLevel) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
            else if (maxAnisotropicLevel > 0.0f)
            {
                TraceLog(LOG_WARNING, "[TEX ID %i] Maximum anisotropic filter level supported is %iX", id, maxAnisotropicLevel);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
            }
            else TraceLog(LOG_WARNING, "Anisotropic filtering not supported");
        } break;
        default: break;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

// Enable rendering to texture (fbo)
void rlEnableRenderTexture(unsigned int id)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    if (checkGlError("r0")) {
      logit("error on render texture %d\n",id);
      }
    

    //glDisable(GL_CULL_FACE);    // Allow double side drawing for texture flipping
    //glCullFace(GL_FRONT);
#endif
}

// Disable rendering to texture
void rlDisableRenderTexture(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (checkGlError("r0")) {
      logit("undone render  texture\n");
      }

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
#endif
}

// Enable depth test
void rlEnableDepthTest(void)
{
    glEnable(GL_DEPTH_TEST);
}

// Disable depth test
void rlDisableDepthTest(void)
{
    glDisable(GL_DEPTH_TEST);
}

// Enable wire mode
void rlEnableWireMode(void)
{
#if defined (GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
    // NOTE: glPolygonMode() not available on OpenGL ES
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
}

// Disable wire mode
void rlDisableWireMode(void)
{
#if defined (GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
    // NOTE: glPolygonMode() not available on OpenGL ES
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

// Unload texture from GPU memory
void rlDeleteTextures(unsigned int id)
{
    if (id > 0) glDeleteTextures(1, &id);
}

// Unload render texture from GPU memory
void rlDeleteRenderTextures(RenderTexture2D target)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    if (target.id > 0) glDeleteFramebuffers(1, &target.id);
    if (target.texture.id > 0) glDeleteTextures(1, &target.texture.id);
    if (target.depth.id > 0) glDeleteTextures(1, &target.depth.id);

    TraceLog(LOG_INFO, "[FBO ID %i] Unloaded render texture data from VRAM (GPU)", target.id);
#endif
}

// Unload shader from GPU memory
void rlDeleteShader(unsigned int id)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    if (id != 0) glDeleteProgram(id);
#endif
}

// Unload vertex data (VAO) from GPU memory
void rlDeleteVertexArrays(unsigned int id)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    if (vaoSupported)
    {
        if (id != 0) glDeleteVertexArrays(1, &id);
        TraceLog(LOG_INFO, "[VAO ID %i] Unloaded model data from VRAM (GPU)", id);
    }
#endif
}

// Unload vertex data (VBO) from GPU memory
void rlDeleteBuffers(unsigned int id)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    if (id != 0)
    {
        glDeleteBuffers(1, &id);
        if (!vaoSupported) TraceLog(LOG_INFO, "[VBO ID %i] Unloaded model vertex data from VRAM (GPU)", id);
    }
#endif
}

// Clear color buffer with color
void rlClearColor(byte r, byte g, byte b, byte a)
{
    // Color values clamp to 0.0f(0) and 1.0f(255)
    float cr = (float)r/255;
    float cg = (float)g/255;
    float cb = (float)b/255;
    float ca = (float)a/255;

    glClearColor(cr, cg, cb, ca);
}

// Clear used screen buffers (color and depth)
void rlClearScreenBuffers(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear used buffers: Color and Depth (Depth is used for 3D)
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);     // Stencil buffer not used...
}

//----------------------------------------------------------------------------------
// Module Functions Definition - rlgl Functions
//----------------------------------------------------------------------------------

// Initialize rlgl: OpenGL extensions, default buffers/shaders/textures, OpenGL states
void rlglInit(int width, int height)
{
logit("g1\n");
    // Check OpenGL information and capabilities
    //------------------------------------------------------------------------------

    // Print current OpenGL and GLSL version
        TraceLog(LOG_INFO, "GPU: Vendor:   %s", glGetString(GL_VENDOR));
    TraceLog(LOG_INFO, "GPU: Renderer: %s", glGetString(GL_RENDERER));
    TraceLog(LOG_INFO, "GPU: Version:  %s", glGetString(GL_VERSION));
    TraceLog(LOG_INFO, "GPU: GLSL:     %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // NOTE: We can get a bunch of extra information about GPU capabilities (glGet*)
    //int maxTexSize;
    //glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    //TraceLog(LOG_INFO, "GL_MAX_TEXTURE_SIZE: %i", maxTexSize);

    //GL_MAX_TEXTURE_IMAGE_UNITS
    //GL_MAX_VIEWPORT_DIMS

    //int numAuxBuffers;
    //glGetIntegerv(GL_AUX_BUFFERS, &numAuxBuffers);
    //TraceLog(LOG_INFO, "GL_AUX_BUFFERS: %i", numAuxBuffers);

    //GLint numComp = 0;
    //GLint format[32] = { 0 };
    //glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numComp);
    //glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, format);
    //for (int i = 0; i < numComp; i++) TraceLog(LOG_INFO, "Supported compressed format: 0x%x", format[i]);

    // NOTE: We don't need that much data on screen... right now...

#if defined(GRAPHICS_API_OPENGL_11)
    //TraceLog(LOG_INFO, "OpenGL 1.1 (or driver default) profile initialized");
#endif

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    // Get supported extensions list
    GLint numExt = 0;
logit("g2\n");

#if defined(GRAPHICS_API_OPENGL_33)

    // NOTE: On OpenGL 3.3 VAO and NPOT are supported by default
    vaoSupported = true;
    texNPOTSupported = true;
    texFloatSupported = true;

    // We get a list of available extensions and we check for some of them (compressed textures)
    // NOTE: We don't need to check again supported extensions but we do (GLAD already dealt with that)
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);

#ifdef _MSC_VER
    const char **extList = malloc(sizeof(const char *)*numExt);
#else
    const char *extList[numExt];
#endif

    for (int i = 0; i < numExt; i++) extList[i] = (char *)glGetStringi(GL_EXTENSIONS, i);

#elif defined(GRAPHICS_API_OPENGL_ES2)
    char *extensions = (char *)glGetString(GL_EXTENSIONS);  // One big const string

    // NOTE: We have to duplicate string because glGetString() returns a const value
    // If not duplicated, it fails in some systems (Raspberry Pi)
    // Equivalent to function: char *strdup(const char *str)
    char *extensionsDup;
    size_t len = strlen(extensions) + 1;
    void *newstr = malloc(len);
    if (newstr == NULL) extensionsDup = NULL;
    extensionsDup = (char *)memcpy(newstr, extensions, len);

    // NOTE: String could be splitted using strtok() function (string.h)
    // NOTE: strtok() modifies the received string, it can not be const

    char *extList[512];     // Allocate 512 strings pointers (2 KB)

    extList[numExt] = strtok(extensionsDup, " ");

    while (extList[numExt] != NULL)
    {
        numExt++;
        extList[numExt] = strtok(NULL, " ");
    }

    free(extensionsDup);    // Duplicated string must be deallocated

    numExt -= 1;
#endif

    TraceLog(LOG_INFO, "Number of supported extensions: %i", numExt);

    // Show supported extensions
    //for (int i = 0; i < numExt; i++)  TraceLog(LOG_INFO, "Supported extension: %s", extList[i]);

    // Check required extensions
    for (int i = 0; i < numExt; i++)
    {
#if defined(GRAPHICS_API_OPENGL_ES2)
        // Check VAO support
        // NOTE: Only check on OpenGL ES, OpenGL 3.3 has VAO support as core feature
        if (strcmp(extList[i], (const char *)"GL_OES_vertex_array_object") == 0)
        {
#ifdef PLATFORM_SILENT_RADIANCE	    
            vaoSupported = false;
#else
            vaoSupported = true;
#endif
            // The extension is supported by our hardware and driver, try to get related functions pointers
            // NOTE: emscripten does not support VAOs natively, it uses emulation and it reduces overall performance...
#ifdef __APPLE__
            glBindVertexArray = &glBindVertexArrayOES;
            glGenVertexArrays = &glGenVertexArraysOES;
            glDeleteVertexArrays = &glDeleteVertexArraysOES;

/*
            glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)special_glGetProcAddress("glBindVertexArrayOES");
            glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)special_glGetProcAddress("glGenVertexArraysOES");
            glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC)special_glGetProcAddress("glDeleteVertexArraysOES");
*/
#else
#ifdef ANDROID
            glBindVertexArray = &glBindVertexArrayOES;
            glGenVertexArrays = &glGenVertexArraysOES;
            glDeleteVertexArrays = &glDeleteVertexArraysOES;
#else
#ifndef _WIN32
            glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
            glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
            glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
#endif
#ifdef _WIN32
glGenVertexArrays = NULL;
glBindVertexArray = NULL;
glDeleteVertexArrays = NULL;

#endif

#endif
#endif    
//#else	    
//            glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)glXGetProcAddressARB("glGenVertexArraysOES");
//            glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)glXGetProcAddressARB("glBindVertexArrayOES");
//            glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC)glXGetProcAddressARB("glDeleteVertexArraysOES");
//#endif	    
            //glIsVertexArray = (PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress("glIsVertexArrayOES");     // NOTE: Fails in WebGL, omitted
        }

        // Check NPOT textures support
        // NOTE: Only check on OpenGL ES, OpenGL 3.3 has NPOT textures full support as core feature
        if (strcmp(extList[i], (const char *)"GL_OES_texture_npot") == 0) texNPOTSupported = true;
        
        // Check texture float support
        if (strcmp(extList[i], (const char *)"GL_OES_texture_float") == 0) texFloatSupported = true;
#endif

        // DDS texture compression support
        if ((strcmp(extList[i], (const char *)"GL_EXT_texture_compression_s3tc") == 0) ||
            (strcmp(extList[i], (const char *)"GL_WEBGL_compressed_texture_s3tc") == 0) ||
            (strcmp(extList[i], (const char *)"GL_WEBKIT_WEBGL_compressed_texture_s3tc") == 0)) texCompDXTSupported = true;

        // ETC1 texture compression support
        if ((strcmp(extList[i], (const char *)"GL_OES_compressed_ETC1_RGB8_texture") == 0) ||
            (strcmp(extList[i], (const char *)"GL_WEBGL_compressed_texture_etc1") == 0)) texCompETC1Supported = true;

        // ETC2/EAC texture compression support
        if (strcmp(extList[i], (const char *)"GL_ARB_ES3_compatibility") == 0) texCompETC2Supported = true;

        // PVR texture compression support
        if (strcmp(extList[i], (const char *)"GL_IMG_texture_compression_pvrtc") == 0) texCompPVRTSupported = true;

        // ASTC texture compression support
        if (strcmp(extList[i], (const char *)"GL_KHR_texture_compression_astc_hdr") == 0) texCompASTCSupported = true;

        // Anisotropic texture filter support
        if (strcmp(extList[i], (const char *)"GL_EXT_texture_filter_anisotropic") == 0)
        {
            texAnisotropicFilterSupported = true;
            glGetFloatv(0x84FF, &maxAnisotropicLevel);   // GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
        }

        // Clamp mirror wrap mode supported
        if (strcmp(extList[i], (const char *)"GL_EXT_texture_mirror_clamp") == 0) texClampMirrorSupported = true;
        
        // Debug marker support
        if(strcmp(extList[i], (const char *)"GL_EXT_debug_marker") == 0) debugMarkerSupported = true;
    }

#if defined(_MSC_VER)
    //free(extList);
#endif

#if defined(GRAPHICS_API_OPENGL_ES2)
    if (vaoSupported) TraceLog(LOG_INFO, "[EXTENSION] VAO extension detected, VAO functions initialized successfully");
    else TraceLog(LOG_WARNING, "[EXTENSION] VAO extension not found, VAO usage not supported");

    if (texNPOTSupported) TraceLog(LOG_INFO, "[EXTENSION] NPOT textures extension detected, full NPOT textures supported");
    else TraceLog(LOG_WARNING, "[EXTENSION] NPOT textures extension not found, limited NPOT support (no-mipmaps, no-repeat)");
#endif

    if (texCompDXTSupported) TraceLog(LOG_INFO, "[EXTENSION] DXT compressed textures supported");
    if (texCompETC1Supported) TraceLog(LOG_INFO, "[EXTENSION] ETC1 compressed textures supported");
    if (texCompETC2Supported) TraceLog(LOG_INFO, "[EXTENSION] ETC2/EAC compressed textures supported");
    if (texCompPVRTSupported) TraceLog(LOG_INFO, "[EXTENSION] PVRT compressed textures supported");
    if (texCompASTCSupported) TraceLog(LOG_INFO, "[EXTENSION] ASTC compressed textures supported");

    if (texAnisotropicFilterSupported) TraceLog(LOG_INFO, "[EXTENSION] Anisotropic textures filtering supported (max: %.0fX)", maxAnisotropicLevel);
    if (texClampMirrorSupported) TraceLog(LOG_INFO, "[EXTENSION] Clamp mirror wrap texture mode supported");

    if (debugMarkerSupported) TraceLog(LOG_INFO, "[EXTENSION] Debug Marker supported");

    // Initialize buffers, default shaders and default textures
    //----------------------------------------------------------

    // Init default white texture
    unsigned char pixels[4] = { 255, 255, 255, 255 };   // 1 pixel RGBA (4 bytes)

    whiteTexture = rlLoadTexture(pixels, 1, 1, UNCOMPRESSED_R8G8B8A8, 1);

    if (whiteTexture != 0) TraceLog(LOG_INFO, "[TEX ID %i] Base white texture loaded successfully", whiteTexture);
    else TraceLog(LOG_WARNING, "Base white texture could not be loaded");

    // Init default Shader (customized for GL 3.3 and ES2)
    defaultShader = LoadShaderDefault();
    currentShader = defaultShader;

    // Init default vertex arrays buffers (lines, triangles, quads)
    LoadBuffersDefault();

    // Init temp vertex buffer, used when transformation required (translate, rotate, scale)
    tempBuffer = (Vector3 *)malloc(sizeof(Vector3)*TEMP_VERTEX_BUFFER_SIZE);

    for (int i = 0; i < TEMP_VERTEX_BUFFER_SIZE; i++) tempBuffer[i] = Vector3Zero();

    // Init draw calls tracking system
    draws = (DrawCall *)malloc(sizeof(DrawCall)*MAX_DRAWS_BY_TEXTURE);

    for (int i = 0; i < MAX_DRAWS_BY_TEXTURE; i++)
    {
        draws[i].textureId = 0;
        draws[i].vertexCount = 0;
    }

    drawsCounter = 1;
    draws[drawsCounter - 1].textureId = whiteTexture;
    currentDrawMode = RL_TRIANGLES;     // Set default draw mode

    // Init internal matrix stack (emulating OpenGL 1.1)
    for (int i = 0; i < MATRIX_STACK_SIZE; i++) stack[i] = MatrixIdentity();

    // Init internal projection and modelview matrices
    projection = MatrixIdentity();
    modelview = MatrixIdentity();
    currentMatrix = &modelview;
#endif      // defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)


    // Initialize OpenGL default states
    //----------------------------------------------------------
    // Init state: Depth test
    glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
    glDisable(GL_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

    // Init state: Blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
    glEnable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

#ifndef PLATFORM_SILENT_RADIANCE
    // Init state: Culling
    // NOTE: All shapes/models triangles are drawn CCW
    glCullFace(GL_BACK);                                    // Cull the back face (default)
    glFrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
    glEnable(GL_CULL_FACE);                                 // Enable backface culling
#endif      
      // hib - this backface culling goofs up the other visualizers, so we will turn it on and off with begin_drawing for SILENTRADIANCE



#if defined(GRAPHICS_API_OPENGL_11)
    // Init state: Color hints (deprecated in OpenGL 3.0+)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      // Improve quality of color and texture coordinate interpolation
    glShadeModel(GL_SMOOTH);                                // Smooth shading between vertex (vertex colors interpolation)
#endif

#ifndef PLATFORM_SILENT_RADIANCE
/* We do not clear things because the background is not ours  in SILENTRADIANCE */
    // Init state: Color/Depth buffers clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
    glClearDepth(1.0f);                                     // Set clear depth value (default)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear color and depth buffers (depth buffer required for 3D)
#endif    

    // Store screen size into global variables
    screenWidth = width;
    screenHeight = height;

    TraceLog(LOG_INFO, "OpenGL default states initialized successfully");
}



#ifdef PLATFORM_SILENT_RADIANCE
void rlglSetupDefaultStates() {
    // Init state: Depth test
    glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
    glDisable(GL_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

    // Init state: Blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
    glEnable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

    // Init state: Culling
    // NOTE: All shapes/models triangles are drawn CCW
// turn off this culls tuff all together    
//    glCullFace(GL_BACK);                                    // Cull the back face (default)
//    glFrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
//    glEnable(GL_CULL_FACE);                                 // Enable backface culling
      // hib - this backface culling goofs up the other visualizers, so we will turn it on and off with begin_drawing for SILENTRADIANCE

}




void rlglKindaResroreFromDefaultStates() {
    // Init state: Depth test
    glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
    glDisable(GL_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

    // Init state: Blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
    glDisable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

    // Init state: Culling
    // NOTE: All shapes/models triangles are drawn CCW
    glDisable(GL_CULL_FACE);                                 // Enable backface culling
      // hib - this backface culling goofs up the other visualizers, so we will turn it on and off with begin_drawing for SILENTRADIANCE

}
#endif


// Vertex Buffer Object deinitialization (memory free)
void rlglClose(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    UnloadShaderDefault();              // Unload default shader
    UnloadBuffersDefault();             // Unload default buffers (lines, triangles, quads)  
    glDeleteTextures(1, &whiteTexture); // Unload default texture
    
    TraceLog(LOG_INFO, "[TEX ID %i] Unloaded texture data (base white texture) from VRAM", whiteTexture);

    free(draws);
#endif
}

// Drawing batches: triangles, quads, lines
void rlglDraw(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    // NOTE: In a future version, models could be stored in a stack...
    //for (int i = 0; i < modelsCount; i++) rlDrawMesh(models[i]->mesh, models[i]->material, models[i]->transform);

    // NOTE: Default buffers upload and draw
    UpdateBuffersDefault();
    DrawBuffersDefault();       // NOTE: Stereo rendering is checked inside
#endif
}

// Returns current OpenGL version
int rlGetVersion(void)
{
#if defined(GRAPHICS_API_OPENGL_11)
    return OPENGL_11;
#elif defined(GRAPHICS_API_OPENGL_21)
    #if defined(__APPLEOSX__)
        return OPENGL_33;   // NOTE: Force OpenGL 3.3 on OSX
    #else
        return OPENGL_21;
    #endif
#elif defined(GRAPHICS_API_OPENGL_33)
    return OPENGL_33;
#elif defined(GRAPHICS_API_OPENGL_ES2)
    return OPENGL_ES_20;
#endif
}

// Set debug marker
void rlSetDebugMarker(const char *text)
{
#if defined(GRAPHICS_API_OPENGL_33)
    if (debugMarkerSupported) glInsertEventMarkerEXT(0, text);
#endif
}

// Load OpenGL extensions
// NOTE: External loader function could be passed as a pointer
void rlLoadExtensions(void *loader)
{
#if defined(GRAPHICS_API_OPENGL_21) || defined(GRAPHICS_API_OPENGL_33)
    // NOTE: glad is generated and contains only required OpenGL 3.3 Core extensions (and lower versions)
    #if !defined(__APPLEOSX__)
        if (!gladLoadGLLoader((GLADloadproc)loader)) TraceLog(LOG_WARNING, "GLAD: Cannot load OpenGL extensions");
        else TraceLog(LOG_INFO, "GLAD: OpenGL extensions loaded successfully");

        #if defined(GRAPHICS_API_OPENGL_21)
        if (GLAD_GL_VERSION_2_1) TraceLog(LOG_INFO, "OpenGL 2.1 profile supported");
        #elif defined(GRAPHICS_API_OPENGL_33)
        if(GLAD_GL_VERSION_3_3) TraceLog(LOG_INFO, "OpenGL 3.3 Core profile supported");
        else TraceLog(LOG_ERROR, "OpenGL 3.3 Core profile not supported");
        #endif
    #endif

    // With GLAD, we can check if an extension is supported using the GLAD_GL_xxx booleans
    //if (GLAD_GL_ARB_vertex_array_object) // Use GL_ARB_vertex_array_object
#endif
}

// Get world coordinates from screen coordinates
Vector3 rlUnproject(Vector3 source, Matrix proj, Matrix view)
{
    Vector3 result = { 0.0f, 0.0f, 0.0f };

    // Calculate unproject matrix (multiply view patrix by projection matrix) and invert it
    Matrix matViewProj = MatrixMultiply(view, proj);
    MatrixInvert(&matViewProj);

    // Create quaternion from source point
    Quaternion quat = { source.x, source.y, source.z, 1.0f };

    // Multiply quat point by unproject matrix
    QuaternionTransform(&quat, matViewProj);

    // Normalized world points in vectors
    result.x = quat.x/quat.w;
    result.y = quat.y/quat.w;
    result.z = quat.z/quat.w;

    return result;
}

// Convert image data to OpenGL texture (returns OpenGL valid Id)
unsigned int rlLoadTexture(void *data, int width, int height, int format, int mipmapCount)
{
    glBindTexture(GL_TEXTURE_2D, 0);    // Free any old binding

    GLuint id = 0;

    // Check texture format support by OpenGL 1.1 (compressed textures not supported)
#if defined(GRAPHICS_API_OPENGL_11)
    if (format >= COMPRESSED_DXT1_RGB)
    {
        TraceLog(LOG_WARNING, "OpenGL 1.1 does not support GPU compressed texture formats");
        return id;
    }
#endif

    if ((!texCompDXTSupported) && ((format == COMPRESSED_DXT1_RGB) || (format == COMPRESSED_DXT1_RGBA) ||
        (format == COMPRESSED_DXT3_RGBA) || (format == COMPRESSED_DXT5_RGBA)))
    {
        TraceLog(LOG_WARNING, "DXT compressed texture format not supported");
        return id;
    }
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    if ((!texCompETC1Supported) && (format == COMPRESSED_ETC1_RGB))
    {
        TraceLog(LOG_WARNING, "ETC1 compressed texture format not supported");
        return id;
    }

    if ((!texCompETC2Supported) && ((format == COMPRESSED_ETC2_RGB) || (format == COMPRESSED_ETC2_EAC_RGBA)))
    {
        TraceLog(LOG_WARNING, "ETC2 compressed texture format not supported");
        return id;
    }

    if ((!texCompPVRTSupported) && ((format == COMPRESSED_PVRT_RGB) || (format == COMPRESSED_PVRT_RGBA)))
    {
        TraceLog(LOG_WARNING, "PVRT compressed texture format not supported");
        return id;
    }

    if ((!texCompASTCSupported) && ((format == COMPRESSED_ASTC_4x4_RGBA) || (format == COMPRESSED_ASTC_8x8_RGBA)))
    {
        TraceLog(LOG_WARNING, "ASTC compressed texture format not supported");
        return id;
    }
#endif

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &id);              // Generate Pointer to the texture

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    //glActiveTexture(GL_TEXTURE0);     // If not defined, using GL_TEXTURE0 by default (shader texture)
#endif

    glBindTexture(GL_TEXTURE_2D, id);
    
    int mipWidth = width;
    int mipHeight = height;
    int mipOffset = 0;          // Mipmap data offset
    
    TraceLog(LOG_DEBUG, "Load texture from data memory address: 0x%x", data);
    
    // Load the different mipmap levels
    for (int i = 0; i < mipmapCount; i++)
    {
        unsigned int mipSize = GetPixelDataSize(mipWidth, mipHeight, format);
        
        int glInternalFormat, glFormat, glType;
        GetGlFormats(format, &glInternalFormat, &glFormat, &glType);
        
        TraceLog(LOG_DEBUG, "Load mipmap level %i (%i x %i), size: %i, offset: %i", i, mipWidth, mipHeight, mipSize, mipOffset);
        
        if (glInternalFormat != -1)
        {
            if (format < COMPRESSED_DXT1_RGB) glTexImage2D(GL_TEXTURE_2D, i, glInternalFormat, mipWidth, mipHeight, 0, glFormat, glType, (unsigned char *)data + mipOffset);
            else glCompressedTexImage2D(GL_TEXTURE_2D, i, glInternalFormat, mipWidth, mipHeight, 0, mipSize, (unsigned char *)data + mipOffset);
        
        #if defined(GRAPHICS_API_OPENGL_33)
            if (format == UNCOMPRESSED_GRAYSCALE)
            {
                GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
            }
            else if (format == UNCOMPRESSED_GRAY_ALPHA)
            {
                GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
            }
        #endif
        }
    
        mipWidth /= 2;
        mipHeight /= 2;
        mipOffset += mipSize;
        
        // Security check for NPOT textures
        if (mipWidth < 1) mipWidth = 1;
        if (mipHeight < 1) mipHeight = 1;
    }

    // Texture parameters configuration
    // NOTE: glTexParameteri does NOT affect texture uploading, just the way it's used
#if defined(GRAPHICS_API_OPENGL_ES2)
    // NOTE: OpenGL ES 2.0 with no GL_OES_texture_npot support (i.e. WebGL) has limited NPOT support, so CLAMP_TO_EDGE must be used
    if (texNPOTSupported)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
    }
    else
    {
        // NOTE: If using negative texture coordinates (LoadOBJ()), it does not work!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);       // Set texture to clamp on x-axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);       // Set texture to clamp on y-axis
    }
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
#endif

    // Magnification and minification filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR

#if defined(GRAPHICS_API_OPENGL_33)
    if (mipmapCount > 1)
    {
        // Activate Trilinear filtering if mipmaps are available
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
#endif

    // At this point we have the texture loaded in GPU and texture parameters configured

    // NOTE: If mipmaps were not in data, they are not generated automatically

    // Unbind current texture
    glBindTexture(GL_TEXTURE_2D, 0);

    if (id > 0) TraceLog(LOG_INFO, "[TEX ID %i] Texture created successfully (%ix%i - %i mipmaps)", id, width, height, mipmapCount);
    else TraceLog(LOG_WARNING, "Texture could not be created");

    return id;
}

// Update already loaded texture in GPU with new data
void rlUpdateTexture(unsigned int id, int width, int height, int format, const void *data)
{
    glBindTexture(GL_TEXTURE_2D, id);
   
    int glInternalFormat, glFormat, glType;
    GetGlFormats(format, &glInternalFormat, &glFormat, &glType);

    if ((glInternalFormat != -1) && (format < COMPRESSED_DXT1_RGB))
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, glFormat, glType, (unsigned char *)data);
    }
    else TraceLog(LOG_WARNING, "Texture format updating not supported");
}

// Unload texture from GPU memory
void rlUnloadTexture(unsigned int id)
{
    if (id > 0) glDeleteTextures(1, &id);
}


// Load a texture to be used for rendering (fbo with color and depth attachments)
RenderTexture2D rlLoadRenderTexture(int width, int height)
{
    RenderTexture2D target;

    target.id = 0;

    target.texture.id = 0;
    target.texture.width = width;
    target.texture.height = height;
    target.texture.format = UNCOMPRESSED_R8G8B8A8;
    target.texture.mipmaps = 1;

    target.depth.id = 0;
    target.depth.width = width;
    target.depth.height = height;
    target.depth.format = 19;       //DEPTH_COMPONENT_24BIT
    target.depth.mipmaps = 1;
    
    logit("rendet text 2d width %d height %d\n",width,height);

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    
    
    // Create the texture that will serve as the color attachment for the framebuffer
    glGenTextures(1, &target.texture.id);
    checkGlError("c0");
    glBindTexture(GL_TEXTURE_2D, target.texture.id);
    checkGlError("c1");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    checkGlError("c2");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGlError("c3");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    checkGlError("c4");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    checkGlError("c5");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    checkGlError("c6");
    glBindTexture(GL_TEXTURE_2D, 0);
    checkGlError("c7");

#if defined(GRAPHICS_API_OPENGL_21) || defined(GRAPHICS_API_OPENGL_ES2)
    #define USE_DEPTH_RENDERBUFFER
#else
    #define USE_DEPTH_TEXTURE
#endif

#if defined(USE_DEPTH_RENDERBUFFER)
    // Create the renderbuffer that will serve as the depth attachment for the framebuffer.
    glGenRenderbuffers(1, &target.depth.id);
    checkGlError("e0");
    
    glBindRenderbuffer(GL_RENDERBUFFER, target.depth.id);
    checkGlError("e1");
    #ifdef __APPLE__
//    tell_apple_context_about_render_buffer();    
//    checkGlError("e2");
     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);    
    checkGlError("e3");
    
    #else
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);        
    #endif
       // GL_DEPTH_COMPONENT24 not supported on Android
#elif defined(USE_DEPTH_TEXTURE)
    // NOTE: We can also use a texture for depth buffer (GL_ARB_depth_texture/GL_OES_depth_texture extension required)
    // A renderbuffer is simpler than a texture and could offer better performance on embedded devices
    glGenTextures(1, &target.depth.id);
    checkGlError("b0");
    glBindTexture(GL_TEXTURE_2D, target.depth.id);
    checkGlError("b1");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    checkGlError("b2");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGlError("b3");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    checkGlError("b4");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkGlError("b5");
#ifdef __APPLE__
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    checkGlError("a0");
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    checkGlError("a01");
#endif    
    glBindTexture(GL_TEXTURE_2D, 0);
    checkGlError("a02");
    
#endif

    checkGlError("a10ss");
    // Create the framebuffer object
    glGenFramebuffers(1, &target.id);
    checkGlError("a10");
    glBindFramebuffer(GL_FRAMEBUFFER, target.id);
    checkGlError("a1");

    // Attach color texture and depth renderbuffer to FBO
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target.texture.id, 0);
    checkGlError("a2");
#if defined(USE_DEPTH_RENDERBUFFER)
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target.depth.id);
    checkGlError("a3");
    
#elif defined(USE_DEPTH_TEXTURE)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, target.depth.id, 0);
    checkGlError("a4");
    
#endif

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    checkGlError("a5");

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        TraceLog(LOG_WARNING, "Framebuffer object could not be created...");

        switch (status)
        {
            case GL_FRAMEBUFFER_UNSUPPORTED: TraceLog(LOG_WARNING, "Framebuffer is unsupported"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: TraceLog(LOG_WARNING, "Framebuffer incomplete attachment"); break;
#if defined(GRAPHICS_API_OPENGL_ES2)
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: TraceLog(LOG_WARNING, "Framebuffer incomplete dimensions"); break;
#endif
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: TraceLog(LOG_WARNING, "Framebuffer incomplete missing attachment"); break;
            default: break;
        }

        glDeleteTextures(1, &target.texture.id);
#if defined(USE_DEPTH_RENDERBUFFER)
        glDeleteRenderbuffers(1, &target.depth.id);
#elif defined(USE_DEPTH_TEXTURE)
        glDeleteTextures(1, &target.depth.id);
#endif	
        glDeleteFramebuffers(1, &target.id);
	target.id=0;
	target.depth.id=0;
	target.texture.id=0;
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
    }
    else TraceLog(LOG_INFO, "[FBO ID %i] Framebuffer object created successfully", target.id);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);



#endif

    return target;
}

// Generate mipmap data for selected texture
void rlGenerateMipmaps(Texture2D *texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->id);

    // Check if texture is power-of-two (POT)
    bool texIsPOT = false;

    if (((texture->width > 0) && ((texture->width & (texture->width - 1)) == 0)) &&
        ((texture->height > 0) && ((texture->height & (texture->height - 1)) == 0))) texIsPOT = true;

    if ((texIsPOT) || (texNPOTSupported))
    {
#if defined(GRAPHICS_API_OPENGL_11)
        // Compute required mipmaps
        void *data = rlReadTexturePixels(*texture);

        // NOTE: data size is reallocated to fit mipmaps data
        // NOTE: CPU mipmap generation only supports RGBA 32bit data
        int mipmapCount = GenerateMipmaps(data, texture->width, texture->height);

        int size = texture->width*texture->height*4;  // RGBA 32bit only
        int offset = size;

        int mipWidth = texture->width/2;
        int mipHeight = texture->height/2;

        // Load the mipmaps
        for (int level = 1; level < mipmapCount; level++)
        {
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA8, mipWidth, mipHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)data + offset);

            size = mipWidth*mipHeight*4;
            offset += size;

            mipWidth /= 2;
            mipHeight /= 2;
        }

        TraceLog(LOG_WARNING, "[TEX ID %i] Mipmaps generated manually on CPU side", texture->id);

        // NOTE: Once mipmaps have been generated and data has been uploaded to GPU VRAM, we can discard RAM data
        free(data);

        texture->mipmaps = mipmapCount + 1;
#endif

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
        //glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);   // Hint for mipmaps generation algorythm: GL_FASTEST, GL_NICEST, GL_DONT_CARE
        glGenerateMipmap(GL_TEXTURE_2D);    // Generate mipmaps automatically
        TraceLog(LOG_INFO, "[TEX ID %i] Mipmaps generated automatically", texture->id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);   // Activate Trilinear filtering for mipmaps

        #define MIN(a,b) (((a)<(b))?(a):(b))
        #define MAX(a,b) (((a)>(b))?(a):(b))

        texture->mipmaps =  1 + (int)floor(log(MAX(texture->width, texture->height))/log(2));
#endif
    }
    else TraceLog(LOG_WARNING, "[TEX ID %i] Mipmaps can not be generated", texture->id);

    glBindTexture(GL_TEXTURE_2D, 0);
}

// Upload vertex data into a VAO (if supported) and VBO
// TODO: Check if mesh has already been loaded in GPU
void rlLoadMesh(Mesh *mesh, bool dynamic)
{
    mesh->vaoId = 0;        // Vertex Array Object
    mesh->vboId[0] = 0;     // Vertex positions VBO
    mesh->vboId[1] = 0;     // Vertex texcoords VBO
    mesh->vboId[2] = 0;     // Vertex normals VBO
    mesh->vboId[3] = 0;     // Vertex colors VBO
    mesh->vboId[4] = 0;     // Vertex tangents VBO
    mesh->vboId[5] = 0;     // Vertex texcoords2 VBO
    mesh->vboId[6] = 0;     // Vertex indices VBO

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    int drawHint = GL_STATIC_DRAW;
    if (dynamic) drawHint = GL_DYNAMIC_DRAW;

    if (vaoSupported)
    {
        // Initialize Quads VAO (Buffer A)
        glGenVertexArrays(1, &mesh->vaoId);
        glBindVertexArray(mesh->vaoId);
    }

    // NOTE: Attributes must be uploaded considering default locations points

    // Enable vertex attributes: position (shader-location = 0)
    glGenBuffers(1, &mesh->vboId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->vertexCount, mesh->vertices, drawHint);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(0);

    // Enable vertex attributes: texcoords (shader-location = 1)
    glGenBuffers(1, &mesh->vboId[1]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vboId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*mesh->vertexCount, mesh->texcoords, drawHint);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(1);

    // Enable vertex attributes: normals (shader-location = 2)
    if (mesh->normals != NULL)
    {
        glGenBuffers(1, &mesh->vboId[2]);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vboId[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->vertexCount, mesh->normals, drawHint);
        glVertexAttribPointer(2, 3, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(2);
    }
    else
    {
        // Default color vertex attribute set to WHITE
        glVertexAttrib3f(2, 1.0f, 1.0f, 1.0f);
        glDisableVertexAttribArray(2);
    }

    // Default color vertex attribute (shader-location = 3)
    if (mesh->colors != NULL)
    {
        glGenBuffers(1, &mesh->vboId[3]);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vboId[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned char)*4*mesh->vertexCount, mesh->colors, drawHint);
        glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
        glEnableVertexAttribArray(3);
    }
    else
    {
        // Default color vertex attribute set to WHITE
        glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);
        glDisableVertexAttribArray(3);
    }

    // Default tangent vertex attribute (shader-location = 4)
    if (mesh->tangents != NULL)
    {
        glGenBuffers(1, &mesh->vboId[4]);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vboId[4]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->vertexCount, mesh->tangents, drawHint);
        glVertexAttribPointer(4, 3, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(4);
    }
    else
    {
        // Default tangents vertex attribute
        glVertexAttrib3f(4, 0.0f, 0.0f, 0.0f);
        glDisableVertexAttribArray(4);
    }

    // Default texcoord2 vertex attribute (shader-location = 5)
    if (mesh->texcoords2 != NULL)
    {
        glGenBuffers(1, &mesh->vboId[5]);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vboId[5]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*mesh->vertexCount, mesh->texcoords2, drawHint);
        glVertexAttribPointer(5, 2, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(5);
    }
    else
    {
        // Default tangents vertex attribute
        glVertexAttrib2f(5, 0.0f, 0.0f);
        glDisableVertexAttribArray(5);
    }

    if (mesh->indices != NULL)
    {
        glGenBuffers(1, &mesh->vboId[6]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vboId[6]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*mesh->triangleCount*3, mesh->indices, GL_STATIC_DRAW);
    }

    if (vaoSupported)
    {
        if (mesh->vaoId > 0) TraceLog(LOG_INFO, "[VAO ID %i] Mesh uploaded successfully to VRAM (GPU)", mesh->vaoId);
        else TraceLog(LOG_WARNING, "Mesh could not be uploaded to VRAM (GPU)");
    }
    else
    {
        TraceLog(LOG_INFO, "[VBOs] Mesh uploaded successfully to VRAM (GPU)");
    }
#endif
}

// Update vertex data on GPU (upload new data to one buffer)
void rlUpdateMesh(Mesh mesh, int buffer, int numVertex)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    // Activate mesh VAO
    if (vaoSupported) glBindVertexArray(mesh.vaoId);

    switch (buffer)
    {
        case 0:     // Update vertices (vertex position)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[0]);
            if (numVertex >= mesh.vertexCount) glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numVertex, mesh.vertices, GL_DYNAMIC_DRAW);
            else glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*3*numVertex, mesh.vertices);

        } break;
        case 1:     // Update texcoords (vertex texture coordinates)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[1]);
            if (numVertex >= mesh.vertexCount) glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*numVertex, mesh.texcoords, GL_DYNAMIC_DRAW);
            else glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*2*numVertex, mesh.texcoords);

        } break;
        case 2:     // Update normals (vertex normals)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[2]);
            if (numVertex >= mesh.vertexCount) glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numVertex, mesh.normals, GL_DYNAMIC_DRAW);
            else glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*3*numVertex, mesh.normals);

        } break;
        case 3:     // Update colors (vertex colors)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[3]);
            if (numVertex >= mesh.vertexCount) glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*numVertex, mesh.colors, GL_DYNAMIC_DRAW);
            else glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned char)*4*numVertex, mesh.colors);

        } break;
        case 4:     // Update tangents (vertex tangents)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[4]);
            if (numVertex >= mesh.vertexCount) glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numVertex, mesh.tangents, GL_DYNAMIC_DRAW);
            else glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*3*numVertex, mesh.tangents);
        } break;
        case 5:     // Update texcoords2 (vertex second texture coordinates)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[5]);
            if (numVertex >= mesh.vertexCount) glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*numVertex, mesh.texcoords2, GL_DYNAMIC_DRAW);
            else glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*2*numVertex, mesh.texcoords2);
        } break;
        default: break;
    }

    // Unbind the current VAO
    if (vaoSupported) glBindVertexArray(0);

    // Another option would be using buffer mapping...
    //mesh.vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    // Now we can modify vertices
    //glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
}

// Draw a 3d mesh with material and transform
void rlDrawMesh(Mesh mesh, Material material, Matrix transform)
{
#if defined(GRAPHICS_API_OPENGL_11)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, material.maps[MAP_DIFFUSE].texture.id);

    // NOTE: On OpenGL 1.1 we use Vertex Arrays to draw model
    glEnableClientState(GL_VERTEX_ARRAY);                   // Enable vertex array
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);            // Enable texture coords array
    if (mesh.normals != NULL) glEnableClientState(GL_NORMAL_ARRAY);     // Enable normals array
    if (mesh.colors != NULL) glEnableClientState(GL_COLOR_ARRAY);       // Enable colors array

    glVertexPointer(3, GL_FLOAT, 0, mesh.vertices);         // Pointer to vertex coords array
    glTexCoordPointer(2, GL_FLOAT, 0, mesh.texcoords);      // Pointer to texture coords array
    if (mesh.normals != NULL) glNormalPointer(GL_FLOAT, 0, mesh.normals);           // Pointer to normals array
    if (mesh.colors != NULL) glColorPointer(4, GL_UNSIGNED_BYTE, 0, mesh.colors);   // Pointer to colors array

    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(transform));
        rlColor4ub(material.maps[MAP_DIFFUSE].color.r, material.maps[MAP_DIFFUSE].color.g, material.maps[MAP_DIFFUSE].color.b, material.maps[MAP_DIFFUSE].color.a);

        if (mesh.indices != NULL) glDrawElements(GL_TRIANGLES, mesh.triangleCount*3, GL_UNSIGNED_SHORT, mesh.indices);
        else glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
    rlPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);                  // Disable vertex array
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);           // Disable texture coords array
    if (mesh.normals != NULL) glDisableClientState(GL_NORMAL_ARRAY);    // Disable normals array
    if (mesh.colors != NULL) glDisableClientState(GL_NORMAL_ARRAY);     // Disable colors array

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
#endif

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    // Bind shader program
    glUseProgram(material.shader.id);   

    // Matrices and other values required by shader
    //-----------------------------------------------------
    // Calculate and send to shader model matrix (used by PBR shader)
    if (material.shader.locs[LOC_MATRIX_MODEL] != -1) SetShaderValueMatrix(material.shader, material.shader.locs[LOC_MATRIX_MODEL], transform);
    
    // Upload to shader material.colDiffuse
    if (material.shader.locs[LOC_COLOR_DIFFUSE] != -1)
        glUniform4f(material.shader.locs[LOC_COLOR_DIFFUSE], (float)material.maps[MAP_DIFFUSE].color.r/255.0f, 
                                                           (float)material.maps[MAP_DIFFUSE].color.g/255.0f, 
                                                           (float)material.maps[MAP_DIFFUSE].color.b/255.0f, 
                                                           (float)material.maps[MAP_DIFFUSE].color.a/255.0f);

    // Upload to shader material.colSpecular (if available)
    if (material.shader.locs[LOC_COLOR_SPECULAR] != -1) 
        glUniform4f(material.shader.locs[LOC_COLOR_SPECULAR], (float)material.maps[MAP_SPECULAR].color.r/255.0f, 
                                                               (float)material.maps[MAP_SPECULAR].color.g/255.0f, 
                                                               (float)material.maps[MAP_SPECULAR].color.b/255.0f, 
                                                               (float)material.maps[MAP_SPECULAR].color.a/255.0f);
                                                               
    if (material.shader.locs[LOC_MATRIX_VIEW] != -1) SetShaderValueMatrix(material.shader, material.shader.locs[LOC_MATRIX_VIEW], modelview);
    if (material.shader.locs[LOC_MATRIX_PROJECTION] != -1) SetShaderValueMatrix(material.shader, material.shader.locs[LOC_MATRIX_PROJECTION], projection);

    // At this point the modelview matrix just contains the view matrix (camera)
    // That's because Begin3dMode() sets it an no model-drawing function modifies it, all use rlPushMatrix() and rlPopMatrix()
    Matrix matView = modelview;         // View matrix (camera)
    Matrix matProjection = projection;  // Projection matrix (perspective)

    // Calculate model-view matrix combining matModel and matView
    Matrix matModelView = MatrixMultiply(transform, matView);           // Transform to camera-space coordinates
    //-----------------------------------------------------

    // Bind active texture maps (if available)
    for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
    {
        if (material.maps[i].texture.id > 0)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            if ((i == MAP_IRRADIANCE) || (i == MAP_PREFILTER) || (i == MAP_CUBEMAP)) glBindTexture(GL_TEXTURE_CUBE_MAP, material.maps[i].texture.id);
            else glBindTexture(GL_TEXTURE_2D, material.maps[i].texture.id);

            glUniform1i(material.shader.locs[LOC_MAP_DIFFUSE + i], i);
        }
    }

    // Bind vertex array objects (or VBOs)
    if (vaoSupported) glBindVertexArray(mesh.vaoId);
    else
    {
        // TODO: Simplify VBO binding into a for loop

        // Bind mesh VBO data: vertex position (shader-location = 0)
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[0]);
        glVertexAttribPointer(material.shader.locs[LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(material.shader.locs[LOC_VERTEX_POSITION]);

        // Bind mesh VBO data: vertex texcoords (shader-location = 1)
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[1]);
        glVertexAttribPointer(material.shader.locs[LOC_VERTEX_TEXCOORD01], 2, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(material.shader.locs[LOC_VERTEX_TEXCOORD01]);

        // Bind mesh VBO data: vertex normals (shader-location = 2, if available)
        if (material.shader.locs[LOC_VERTEX_NORMAL] != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[2]);
            glVertexAttribPointer(material.shader.locs[LOC_VERTEX_NORMAL], 3, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray(material.shader.locs[LOC_VERTEX_NORMAL]);
        }

        // Bind mesh VBO data: vertex colors (shader-location = 3, if available)
        if (material.shader.locs[LOC_VERTEX_COLOR] != -1)
        {
            if (mesh.vboId[3] != 0)
            {
                glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[3]);
                glVertexAttribPointer(material.shader.locs[LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
                glEnableVertexAttribArray(material.shader.locs[LOC_VERTEX_COLOR]);
		checkGlError("777color1");
            }
            else
            {
                // Set default value for unused attribute
                // NOTE: Required when using default shader and no VAO support
                glVertexAttrib4f(material.shader.locs[LOC_VERTEX_COLOR], 1.0f, 1.0f, 1.0f, 1.0f);
                glDisableVertexAttribArray(material.shader.locs[LOC_VERTEX_COLOR]);
		checkGlError("777color2");
            }
        }

        // Bind mesh VBO data: vertex tangents (shader-location = 4, if available)
        if (material.shader.locs[LOC_VERTEX_TANGENT] != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[4]);
            glVertexAttribPointer(material.shader.locs[LOC_VERTEX_TANGENT], 3, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray(material.shader.locs[LOC_VERTEX_TANGENT]);
        }

        // Bind mesh VBO data: vertex texcoords2 (shader-location = 5, if available)
        if (material.shader.locs[LOC_VERTEX_TEXCOORD02] != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId[5]);
            glVertexAttribPointer(material.shader.locs[LOC_VERTEX_TEXCOORD02], 2, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray(material.shader.locs[LOC_VERTEX_TEXCOORD02]);
        }

        if (mesh.indices != NULL) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vboId[6]);
    }

    int eyesCount = 1;
#if defined(SUPPORT_VR_SIMULATOR)
    if (vrStereoRender) eyesCount = 2;
#endif
    
    for (int eye = 0; eye < eyesCount; eye++)
    {
        if (eyesCount == 1) modelview = matModelView;
        #if defined(SUPPORT_VR_SIMULATOR)
        else SetStereoView(eye, matProjection, matModelView);
        #endif

        // Calculate model-view-projection matrix (MVP)
        Matrix matMVP = MatrixMultiply(modelview, projection);        // Transform to screen-space coordinates

        // Send combined model-view-projection matrix to shader
        glUniformMatrix4fv(material.shader.locs[LOC_MATRIX_MVP], 1, false, MatrixToFloat(matMVP));

        // Draw call!
        if (mesh.indices != NULL) glDrawElements(GL_TRIANGLES, mesh.triangleCount*3, GL_UNSIGNED_SHORT, 0); // Indexed vertices draw
        else glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
    }
    
    // Unbind all binded texture maps
    for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);       // Set shader active texture
        if ((i == MAP_IRRADIANCE) || (i == MAP_PREFILTER) || (i == MAP_CUBEMAP)) glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        else glBindTexture(GL_TEXTURE_2D, 0);   // Unbind current active texture
    }

    // Unind vertex array objects (or VBOs)
    if (vaoSupported) glBindVertexArray(0);
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (mesh.indices != NULL) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // Unbind shader program
    glUseProgram(0);

    // Restore projection/modelview matrices
    // NOTE: In stereo rendering matrices are being modified to fit every eye
    projection = matProjection;
    modelview = matView;
#endif
}

// Unload mesh data from CPU and GPU
void rlUnloadMesh(Mesh *mesh)
{
    if (mesh->vertices != NULL) free(mesh->vertices);
    if (mesh->texcoords != NULL) free(mesh->texcoords);
    if (mesh->normals != NULL) free(mesh->normals);
    if (mesh->colors != NULL) free(mesh->colors);
    if (mesh->tangents != NULL) free(mesh->tangents);
    if (mesh->texcoords2 != NULL) free(mesh->texcoords2);
    if (mesh->indices != NULL) free(mesh->indices);

    rlDeleteBuffers(mesh->vboId[0]);   // vertex
    rlDeleteBuffers(mesh->vboId[1]);   // texcoords
    rlDeleteBuffers(mesh->vboId[2]);   // normals
    rlDeleteBuffers(mesh->vboId[3]);   // colors
    rlDeleteBuffers(mesh->vboId[4]);   // tangents
    rlDeleteBuffers(mesh->vboId[5]);   // texcoords2
    rlDeleteBuffers(mesh->vboId[6]);   // indices

    rlDeleteVertexArrays(mesh->vaoId);
}

// Read screen pixel data (color buffer)
unsigned char *rlReadScreenPixels(int width, int height)
{
    unsigned char *screenData = (unsigned char *)calloc(width*height*4, sizeof(unsigned char));

    // NOTE: glReadPixels returns image flipped vertically -> (0,0) is the bottom left corner of the framebuffer
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, screenData);

    // Flip image vertically!
    unsigned char *imgData = (unsigned char *)malloc(width*height*sizeof(unsigned char)*4);

    for (int y = height - 1; y >= 0; y--)
    {
        for (int x = 0; x < (width*4); x++)
        {
            // Flip line
            imgData[((height - 1) - y)*width*4 + x] = screenData[(y*width*4) + x];

            // Set alpha component value to 255 (no trasparent image retrieval)
            // NOTE: Alpha value has already been applied to RGB in framebuffer, we don't need it!
            if (((x + 1)%4) == 0) imgData[((height - 1) - y)*width*4 + x] = 255;
        }
    }

    free(screenData);

    return imgData;     // NOTE: image data should be freed
}

// Read texture pixel data
// NOTE: glGetTexImage() is not available on OpenGL ES 2.0
// Texture2D width and height are required on OpenGL ES 2.0. There is no way to get it from texture id.
void *rlReadTexturePixels(Texture2D texture)
{
    void *pixels = NULL;

#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
    glBindTexture(GL_TEXTURE_2D, texture.id);

    // NOTE: Using texture.id, we can retrieve some texture info (but not on OpenGL ES 2.0)
    /*
    int width, height, format;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
    // Other texture info: GL_TEXTURE_RED_SIZE, GL_TEXTURE_GREEN_SIZE, GL_TEXTURE_BLUE_SIZE, GL_TEXTURE_ALPHA_SIZE
    */
    
    // NOTE: Each row written to or read from by OpenGL pixel operations like glGetTexImage are aligned to a 4 byte boundary by default, which may add some padding.
    // Use glPixelStorei to modify padding with the GL_[UN]PACK_ALIGNMENT setting.
    // GL_PACK_ALIGNMENT affects operations that read from OpenGL memory (glReadPixels, glGetTexImage, etc.)
    // GL_UNPACK_ALIGNMENT affects operations that write to OpenGL memory (glTexImage, etc.)
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    int glInternalFormat, glFormat, glType;
	GetGlFormats(texture.format, &glInternalFormat, &glFormat, &glType);
    unsigned int size = GetPixelDataSize(texture.width, texture.height, texture.format);
    
    if ((glInternalFormat != -1) && (texture.format < COMPRESSED_DXT1_RGB))
    {
        pixels = (unsigned char *)malloc(size);
        glGetTexImage(GL_TEXTURE_2D, 0, glFormat, glType, pixels);
    }
    else TraceLog(LOG_WARNING, "Texture data retrieval not suported for pixel format");

    glBindTexture(GL_TEXTURE_2D, 0);
#endif

#if defined(GRAPHICS_API_OPENGL_ES2)
    RenderTexture2D fbo = rlLoadRenderTexture(texture.width, texture.height);

    // NOTE: Two possible Options:
    // 1 - Bind texture to color fbo attachment and glReadPixels()
    // 2 - Create an fbo, activate it, render quad with texture, glReadPixels()

#define GET_TEXTURE_FBO_OPTION_1    // It works
#if defined(GET_TEXTURE_FBO_OPTION_1)
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach our texture to FBO -> Texture must be RGBA
    // NOTE: Previoust attached texture is automatically detached
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.id, 0);

    pixels = (unsigned char *)malloc(texture.width*texture.height*4*sizeof(unsigned char));

    // NOTE: We read data as RGBA because FBO texture is configured as RGBA, despite binding a RGB texture...
    glReadPixels(0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Re-attach internal FBO color texture before deleting it
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texture.id, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#elif defined(GET_TEXTURE_FBO_OPTION_2)
    // Render texture to fbo
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepthf(1.0f);
    //glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);
    
    glViewport(0, 0, texture.width, texture.height);
    rlOrtho(0.0, texture.width, texture.height, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(GetShaderDefault().id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    GenDrawQuad();
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    
    pixels = (unsigned char *)malloc(texture.width*texture.height*4*sizeof(unsigned char));

    glReadPixels(0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Bind framebuffer 0, which means render to back buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Reset viewport dimensions to default
    glViewport(0, 0, screenWidth, screenHeight);
    
#endif // GET_TEXTURE_FBO_OPTION

    // Clean up temporal fbo
    rlDeleteRenderTextures(fbo);

#endif

    return pixels;
}

/*
// TODO: Record draw calls to be processed in batch
// NOTE: Global state must be kept
void rlRecordDraw(void)
{
    // TODO: Before adding a new draw, check if anything changed from last stored draw
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    draws[drawsCounter].vaoId = currentState.vaoId;             // lines.id, trangles.id, quads.id?
    draws[drawsCounter].textureId = currentState.textureId;     // whiteTexture?
    draws[drawsCounter].shaderId = currentState.shaderId;       // defaultShader.id
    draws[drawsCounter].projection = projection;
    draws[drawsCounter].modelview = modelview;
    draws[drawsCounter].vertexCount = currentState.vertexCount;

    drawsCounter++;
#endif
}
*/

//----------------------------------------------------------------------------------
// Module Functions Definition - Shaders Functions
// NOTE: Those functions are exposed directly to the user in raylib.h
//----------------------------------------------------------------------------------

// Get default internal texture (white texture)
Texture2D GetTextureDefault(void)
{
    Texture2D texture;

    texture.id = whiteTexture;
    texture.width = 1;
    texture.height = 1;
    texture.mipmaps = 1;
    texture.format = UNCOMPRESSED_R8G8B8A8;

    return texture;
}

// Get default shader
Shader GetShaderDefault(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    return defaultShader;
#else
    Shader shader = { 0 };
    return shader;
#endif
}

// Load text data from file
// NOTE: text chars array should be freed manually
char *LoadText(const char *fileName)
{
    FILE *textFile;
    char *text = NULL;

    int count = 0;

    if (fileName != NULL)
    {
        textFile = fopen(fileName,"rt");

        if (textFile != NULL)
        {
            fseek(textFile, 0, SEEK_END);
            count = ftell(textFile);
            rewind(textFile);

            if (count > 0)
            {
                text = (char *)malloc(sizeof(char)*(count + 1));
                count = fread(text, sizeof(char), count, textFile);
                text[count] = '\0';
            }

            fclose(textFile);
        }
        else TraceLog(LOG_WARNING, "[%s] Text file could not be opened", fileName);
    }

    return text;
}

// Load shader from files and bind default locations
// NOTE: If shader string is NULL, using default vertex/fragment shaders
Shader LoadShader(const char *vsFileName, const char *fsFileName)
{
    Shader shader = { 0 };

    char *vShaderStr = NULL;
    char *fShaderStr = NULL;

    if (vsFileName != NULL) vShaderStr = LoadText(vsFileName);
    if (fsFileName != NULL) fShaderStr = LoadText(fsFileName);
    
    shader = LoadShaderCode(vShaderStr, fShaderStr);
        
    if (vShaderStr != NULL) free(vShaderStr);
    if (fShaderStr != NULL) free(fShaderStr);

    return shader;
}

// Load shader from code strings
// NOTE: If shader string is NULL, using default vertex/fragment shaders
Shader LoadShaderCode(char *vsCode, char *fsCode)
{
    Shader shader = { 0 };

    // NOTE: All locations must be reseted to -1 (no location)
    for (int i = 0; i < MAX_SHADER_LOCATIONS; i++) shader.locs[i] = -1;
    
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    unsigned int vertexShaderId = defaultVShaderId;
    unsigned int fragmentShaderId = defaultFShaderId;
    
    if (vsCode != NULL) vertexShaderId = CompileShader(vsCode, GL_VERTEX_SHADER);
    if (fsCode != NULL) fragmentShaderId = CompileShader(fsCode, GL_FRAGMENT_SHADER);
    
    if ((vertexShaderId == defaultVShaderId) && (fragmentShaderId == defaultFShaderId)) shader = defaultShader;
    else 
    {
        shader.id = LoadShaderProgram(vertexShaderId, fragmentShaderId);

        if (vertexShaderId != defaultVShaderId) glDeleteShader(vertexShaderId);
        if (fragmentShaderId != defaultFShaderId) glDeleteShader(fragmentShaderId);

        if (shader.id == 0)
        {
            TraceLog(LOG_WARNING, "Custom shader could not be loaded");
            shader = defaultShader;
        }
        
        // After shader loading, we TRY to set default location names
        if (shader.id > 0) SetShaderDefaultLocations(&shader);
    }
    
    // Get available shader uniforms
    // NOTE: This information is useful for debug...
    int uniformCount = -1;
    
    glGetProgramiv(shader.id, GL_ACTIVE_UNIFORMS, &uniformCount);
    
    for(int i = 0; i < uniformCount; i++)
    {
        int namelen = -1;
        int num = -1;
        char name[256]; // Assume no variable names longer than 256
        GLenum type = GL_ZERO;

        // Get the name of the uniforms
        glGetActiveUniform(shader.id, i,sizeof(name) - 1, &namelen, &num, &type, name);
        
        name[namelen] = 0;

        // Get the location of the named uniform
        GLuint location = glGetUniformLocation(shader.id, name);
        
        TraceLog(LOG_DEBUG, "[SHDR ID %i] Active uniform [%s] set at location: %i", shader.id, name, location);
    }
#endif
    
    return shader;
}

// Unload shader from GPU memory (VRAM)
void UnloadShader(Shader shader)
{
    if (shader.id > 0)
    {
        rlDeleteShader(shader.id);
        TraceLog(LOG_INFO, "[SHDR ID %i] Unloaded shader program data", shader.id);
    }
}

// Begin custom shader mode
void BeginShaderMode(Shader shader)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    if (currentShader.id != shader.id)
    {
        rlglDraw();
        currentShader = shader;
    }
#endif
}

// End custom shader mode (returns to default shader)
void EndShaderMode(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    BeginShaderMode(defaultShader);
#endif
}

// Get shader uniform location
int GetShaderLocation(Shader shader, const char *uniformName)
{
    int location = -1;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    location = glGetUniformLocation(shader.id, uniformName);

    if (location == -1) TraceLog(LOG_WARNING, "[SHDR ID %i] Shader uniform [%s] COULD NOT BE FOUND", shader.id, uniformName);
    else TraceLog(LOG_INFO, "[SHDR ID %i] Shader uniform [%s] set at location: %i", shader.id, uniformName, location);
#endif
    return location;
}

// Set shader uniform value (float)
void SetShaderValue(Shader shader, int uniformLoc, const float *value, int size)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    glUseProgram(shader.id);

    if (size == 1) glUniform1fv(uniformLoc, 1, value);          // Shader uniform type: float
    else if (size == 2) glUniform2fv(uniformLoc, 1, value);     // Shader uniform type: vec2
    else if (size == 3) glUniform3fv(uniformLoc, 1, value);     // Shader uniform type: vec3
    else if (size == 4) glUniform4fv(uniformLoc, 1, value);     // Shader uniform type: vec4
    else TraceLog(LOG_WARNING, "Shader value float array size not supported");

    //glUseProgram(0);      // Avoid reseting current shader program, in case other uniforms are set
#endif
}

// Set shader uniform value (int)
void SetShaderValuei(Shader shader, int uniformLoc, const int *value, int size)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    glUseProgram(shader.id);

    if (size == 1) glUniform1iv(uniformLoc, 1, value);          // Shader uniform type: int
    else if (size == 2) glUniform2iv(uniformLoc, 1, value);     // Shader uniform type: ivec2
    else if (size == 3) glUniform3iv(uniformLoc, 1, value);     // Shader uniform type: ivec3
    else if (size == 4) glUniform4iv(uniformLoc, 1, value);     // Shader uniform type: ivec4
    else TraceLog(LOG_WARNING, "Shader value int array size not supported");

    //glUseProgram(0);
#endif
}

// Set shader uniform value (matrix 4x4)
void SetShaderValueMatrix(Shader shader, int uniformLoc, Matrix mat)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    glUseProgram(shader.id);

    glUniformMatrix4fv(uniformLoc, 1, false, MatrixToFloat(mat));

    //glUseProgram(0);
#endif
}

// Set a custom projection matrix (replaces internal projection matrix)
void SetMatrixProjection(Matrix proj)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    projection = proj;
#endif
}

// Set a custom modelview matrix (replaces internal modelview matrix)
void SetMatrixModelview(Matrix view)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    modelview = view;
#endif
}

// Return internal modelview matrix
Matrix GetMatrixModelview() 
{
    Matrix matrix = MatrixIdentity();
#if defined(GRAPHICS_API_OPENGL_11)
    float mat[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mat);
#else
    matrix = modelview;
#endif
    return matrix;
}

// Generate cubemap texture from HDR texture
// TODO: OpenGL ES 2.0 does not support GL_RGB16F texture format, neither GL_DEPTH_COMPONENT24
Texture2D GenTextureCubemap(Shader shader, Texture2D skyHDR, int size)
{
    Texture2D cubemap = { 0 };
#if defined(GRAPHICS_API_OPENGL_33) // || defined(GRAPHICS_API_OPENGL_ES2)  
    // NOTE: SetShaderDefaultLocations() already setups locations for projection and view Matrix in shader
    // Other locations should be setup externally in shader before calling the function
    
    // Set up depth face culling and cubemap seamless
    glDisable(GL_CULL_FACE);
#if defined(GRAPHICS_API_OPENGL_33)
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);     // Flag not supported on OpenGL ES 2.0
#endif
    

    // Setup framebuffer
    unsigned int fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Set up cubemap to render and attach to framebuffer
    // NOTE: faces are stored with 16 bit floating point values
    glGenTextures(1, &cubemap.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    for (unsigned int i = 0; i < 6; i++) 
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#if defined(GRAPHICS_API_OPENGL_33)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  // Flag not supported on OpenGL ES 2.0
#endif
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create projection (transposed) and different views for each face
    Matrix fboProjection = MatrixPerspective(90.0*DEG2RAD, 1.0, 0.01, 1000.0);
    //MatrixTranspose(&fboProjection);
    Matrix fboViews[6] = {
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 1.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ -1.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, 1.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, 1.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, -1.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, 1.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, -1.0f }, (Vector3){ 0.0f, -1.0f, 0.0f })
    };

    // Convert HDR equirectangular environment map to cubemap equivalent
    glUseProgram(shader.id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyHDR.id);
    SetShaderValueMatrix(shader, shader.locs[LOC_MATRIX_PROJECTION], fboProjection);

    // Note: don't forget to configure the viewport to the capture dimensions
    glViewport(0, 0, size, size);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    for (unsigned int i = 0; i < 6; i++)
    {
        SetShaderValueMatrix(shader, shader.locs[LOC_MATRIX_VIEW], fboViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap.id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GenDrawCube();
    }

    // Unbind framebuffer and textures
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Reset viewport dimensions to default
    glViewport(0, 0, screenWidth, screenHeight);
    //glEnable(GL_CULL_FACE);

    cubemap.width = size;
    cubemap.height = size;
#endif
    return cubemap;
}

// Generate irradiance texture using cubemap data
// TODO: OpenGL ES 2.0 does not support GL_RGB16F texture format, neither GL_DEPTH_COMPONENT24
Texture2D GenTextureIrradiance(Shader shader, Texture2D cubemap, int size)
{
    Texture2D irradiance = { 0 };
    
#if defined(GRAPHICS_API_OPENGL_33) // || defined(GRAPHICS_API_OPENGL_ES2)
    // NOTE: SetShaderDefaultLocations() already setups locations for projection and view Matrix in shader
    // Other locations should be setup externally in shader before calling the function
    
    // Setup framebuffer
    unsigned int fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    // Create an irradiance cubemap, and re-scale capture FBO to irradiance scale
    glGenTextures(1, &irradiance.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance.id);
    for (unsigned int i = 0; i < 6; i++) 
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Create projection (transposed) and different views for each face
    Matrix fboProjection = MatrixPerspective(90.0*DEG2RAD, 1.0, 0.01, 1000.0);
    //MatrixTranspose(&fboProjection);
    Matrix fboViews[6] = {
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 1.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ -1.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, 1.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, 1.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, -1.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, 1.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, -1.0f }, (Vector3){ 0.0f, -1.0f, 0.0f })
    };

    // Solve diffuse integral by convolution to create an irradiance cubemap
    glUseProgram(shader.id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    SetShaderValueMatrix(shader, shader.locs[LOC_MATRIX_PROJECTION], fboProjection);

    // Note: don't forget to configure the viewport to the capture dimensions
    glViewport(0, 0, size, size);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    for (unsigned int i = 0; i < 6; i++)
    {
        SetShaderValueMatrix(shader, shader.locs[LOC_MATRIX_VIEW], fboViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance.id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GenDrawCube();
    }

    // Unbind framebuffer and textures
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Reset viewport dimensions to default
    glViewport(0, 0, screenWidth, screenHeight);

    irradiance.width = size;
    irradiance.height = size;
#endif
    return irradiance;
}

// Generate prefilter texture using cubemap data
// TODO: OpenGL ES 2.0 does not support GL_RGB16F texture format, neither GL_DEPTH_COMPONENT24
Texture2D GenTexturePrefilter(Shader shader, Texture2D cubemap, int size)
{
    Texture2D prefilter = { 0 };
    
#if defined(GRAPHICS_API_OPENGL_33) // || defined(GRAPHICS_API_OPENGL_ES2)
    // NOTE: SetShaderDefaultLocations() already setups locations for projection and view Matrix in shader
    // Other locations should be setup externally in shader before calling the function
    // TODO: Locations should be taken out of this function... too shader dependant...
    int roughnessLoc = GetShaderLocation(shader, "roughness");
    
    // Setup framebuffer
    unsigned int fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    // Create a prefiltered HDR environment map
    glGenTextures(1, &prefilter.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter.id);
    for (unsigned int i = 0; i < 6; i++) 
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate mipmaps for the prefiltered HDR texture
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    // Create projection (transposed) and different views for each face
    Matrix fboProjection = MatrixPerspective(90.0*DEG2RAD, 1.0, 0.01, 1000.0);
    //MatrixTranspose(&fboProjection);
    Matrix fboViews[6] = {
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 1.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ -1.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, 1.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, 1.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, -1.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, 1.0f }, (Vector3){ 0.0f, -1.0f, 0.0f }),
        MatrixLookAt((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, -1.0f }, (Vector3){ 0.0f, -1.0f, 0.0f })
    };

    // Prefilter HDR and store data into mipmap levels
    glUseProgram(shader.id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    SetShaderValueMatrix(shader, shader.locs[LOC_MATRIX_PROJECTION], fboProjection);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    #define MAX_MIPMAP_LEVELS   5   // Max number of prefilter texture mipmaps

    for (unsigned int mip = 0; mip < MAX_MIPMAP_LEVELS; mip++)
    {
        // Resize framebuffer according to mip-level size.
        unsigned int mipWidth  = size*powf(0.5f, mip);
        unsigned int mipHeight = size*powf(0.5f, mip);
        
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip/(float)(MAX_MIPMAP_LEVELS - 1);
        glUniform1f(roughnessLoc, roughness);

        for (unsigned int i = 0; i < 6; ++i)
        {
            SetShaderValueMatrix(shader, shader.locs[LOC_MATRIX_VIEW], fboViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter.id, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            GenDrawCube();
        }
    }

    // Unbind framebuffer and textures
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Reset viewport dimensions to default
    glViewport(0, 0, screenWidth, screenHeight);

    prefilter.width = size;
    prefilter.height = size;
#endif
    return prefilter;
}

// Generate BRDF texture using cubemap data
// TODO: OpenGL ES 2.0 does not support GL_RGB16F texture format, neither GL_DEPTH_COMPONENT24
Texture2D GenTextureBRDF(Shader shader, Texture2D cubemap, int size)
{
    Texture2D brdf = { 0 };
#if defined(GRAPHICS_API_OPENGL_33) // || defined(GRAPHICS_API_OPENGL_ES2)
    // Generate BRDF convolution texture
    glGenTextures(1, &brdf.id);
    glBindTexture(GL_TEXTURE_2D, brdf.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, size, size, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Render BRDF LUT into a quad using FBO
    unsigned int fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf.id, 0);
    
    glViewport(0, 0, size, size);
    glUseProgram(shader.id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GenDrawQuad();

    // Unbind framebuffer and textures
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Reset viewport dimensions to default
    glViewport(0, 0, screenWidth, screenHeight);
   
    brdf.width = size;
    brdf.height = size;
#endif
    return brdf;
}

// Begin blending mode (alpha, additive, multiplied)
// NOTE: Only 3 blending modes supported, default blend mode is alpha
void BeginBlendMode(int mode)
{
    if ((blendMode != mode) && (mode < 3))
    {
        rlglDraw();

        switch (mode)
        {
            case BLEND_ALPHA: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
            case BLEND_ADDITIVE: glBlendFunc(GL_SRC_ALPHA, GL_ONE); break; // Alternative: glBlendFunc(GL_ONE, GL_ONE);
            case BLEND_MULTIPLIED: glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA); break;
            default: break;
        }

        blendMode = mode;
    }
}

// End blending mode (reset to default: alpha blending)
void EndBlendMode(void)
{
    BeginBlendMode(BLEND_ALPHA);
}

#if defined(SUPPORT_VR_SIMULATOR)
// Get VR device information for some standard devices
VrDeviceInfo GetVrDeviceInfo(int vrDeviceType)
{
    VrDeviceInfo hmd = { 0 };                // Current VR device info
    
    switch (vrDeviceType)
    {
        case HMD_DEFAULT_DEVICE:
        case HMD_OCULUS_RIFT_CV1:
        {
            // Oculus Rift CV1 parameters
            // NOTE: CV1 represents a complete HMD redesign compared to previous versions,
            // new Fresnel-hybrid-asymmetric lenses have been added and, consequently,
            // previous parameters (DK2) and distortion shader (DK2) doesn't work any more.
            // I just defined a set of parameters for simulator that approximate to CV1 stereo rendering
            // but result is not the same obtained with Oculus PC SDK.
            hmd.hResolution = 2160;                 // HMD horizontal resolution in pixels
            hmd.vResolution = 1200;                 // HMD vertical resolution in pixels
            hmd.hScreenSize = 0.133793f;            // HMD horizontal size in meters
            hmd.vScreenSize = 0.0669f;              // HMD vertical size in meters
            hmd.vScreenCenter = 0.04678f;           // HMD screen center in meters
            hmd.eyeToScreenDistance = 0.041f;       // HMD distance between eye and display in meters
            hmd.lensSeparationDistance = 0.07f;     // HMD lens separation distance in meters
            hmd.interpupillaryDistance = 0.07f;     // HMD IPD (distance between pupils) in meters
            hmd.lensDistortionValues[0] = 1.0f;     // HMD lens distortion constant parameter 0
            hmd.lensDistortionValues[1] = 0.22f;    // HMD lens distortion constant parameter 1
            hmd.lensDistortionValues[2] = 0.24f;    // HMD lens distortion constant parameter 2
            hmd.lensDistortionValues[3] = 0.0f;     // HMD lens distortion constant parameter 3
            hmd.chromaAbCorrection[0] = 0.996f;     // HMD chromatic aberration correction parameter 0
            hmd.chromaAbCorrection[1] = -0.004f;    // HMD chromatic aberration correction parameter 1
            hmd.chromaAbCorrection[2] = 1.014f;     // HMD chromatic aberration correction parameter 2
            hmd.chromaAbCorrection[3] = 0.0f;       // HMD chromatic aberration correction parameter 3
            
            TraceLog(LOG_INFO, "Initializing VR Simulator (Oculus Rift CV1)");
        } break;
        case HMD_OCULUS_RIFT_DK2:
        {
            // Oculus Rift DK2 parameters
            hmd.hResolution = 1280;                 // HMD horizontal resolution in pixels
            hmd.vResolution = 800;                  // HMD vertical resolution in pixels
            hmd.hScreenSize = 0.14976f;             // HMD horizontal size in meters
            hmd.vScreenSize = 0.09356f;             // HMD vertical size in meters
            hmd.vScreenCenter = 0.04678f;           // HMD screen center in meters
            hmd.eyeToScreenDistance = 0.041f;       // HMD distance between eye and display in meters
            hmd.lensSeparationDistance = 0.0635f;   // HMD lens separation distance in meters
            hmd.interpupillaryDistance = 0.064f;    // HMD IPD (distance between pupils) in meters
            hmd.lensDistortionValues[0] = 1.0f;     // HMD lens distortion constant parameter 0
            hmd.lensDistortionValues[1] = 0.22f;    // HMD lens distortion constant parameter 1
            hmd.lensDistortionValues[2] = 0.24f;    // HMD lens distortion constant parameter 2
            hmd.lensDistortionValues[3] = 0.0f;     // HMD lens distortion constant parameter 3
            hmd.chromaAbCorrection[0] = 0.996f;     // HMD chromatic aberration correction parameter 0
            hmd.chromaAbCorrection[1] = -0.004f;    // HMD chromatic aberration correction parameter 1
            hmd.chromaAbCorrection[2] = 1.014f;     // HMD chromatic aberration correction parameter 2
            hmd.chromaAbCorrection[3] = 0.0f;       // HMD chromatic aberration correction parameter 3
            
            TraceLog(LOG_INFO, "Initializing VR Simulator (Oculus Rift DK2)");
        } break; 
        case HMD_OCULUS_GO:
        {
            // TODO: Provide device display and lens parameters
        }
        case HMD_VALVE_HTC_VIVE:
        {
            // TODO: Provide device display and lens parameters
        }
        case HMD_SONY_PSVR:
        {
            // TODO: Provide device display and lens parameters
        }
        default: break;
    }
    
    return hmd;
}

// Init VR simulator for selected device parameters
// NOTE: It modifies the global variable: VrStereoConfig vrConfig
void InitVrSimulator(VrDeviceInfo info)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    // Initialize framebuffer and textures for stereo rendering
    // NOTE: Screen size should match HMD aspect ratio
    vrConfig.stereoFbo = rlLoadRenderTexture(screenWidth, screenHeight);
    
#if defined(SUPPORT_DISTORTION_SHADER)
    // Load distortion shader
    unsigned int vertexShaderId = CompileShader(distortionVShaderStr, GL_VERTEX_SHADER);
    checkGlError("777d0");
    unsigned int fragmentShaderId = CompileShader(distortionFShaderStr, GL_FRAGMENT_SHADER);
    checkGlError("777d1");
    vrConfig.distortionShader.id = LoadShaderProgram(vertexShaderId, fragmentShaderId);
    checkGlError("777d2");
    if (vrConfig.distortionShader.id > 0) SetShaderDefaultLocations(&vrConfig.distortionShader);
#endif

    // Set VR configutarion parameters, including distortion shader
    SetStereoConfig(info);

    vrSimulatorReady = true;
#endif

#if defined(GRAPHICS_API_OPENGL_11)
    TraceLog(LOG_WARNING, "VR Simulator not supported on OpenGL 1.1");
#endif
}

static int pretend_vrSimulatorReady=0;
static int pretend_vrStereoRender=0;
void PretendNoVr() {
pretend_vrSimulatorReady=vrSimulatorReady;
pretend_vrStereoRender=vrStereoRender; 
vrSimulatorReady=0;
vrStereoRender=0;
}

void StopPretendingNoVr() {
vrStereoRender=pretend_vrStereoRender;
vrSimulatorReady=pretend_vrSimulatorReady;
}


// Close VR simulator for current device
void CloseVrSimulator(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    if (vrSimulatorReady)
    {
        rlDeleteRenderTextures(vrConfig.stereoFbo); // Unload stereo framebuffer and texture
        #if defined(SUPPORT_DISTORTION_SHADER)
        UnloadShader(vrConfig.distortionShader);    // Unload distortion shader
        #endif
    }
#endif
}

// Detect if VR simulator is running
bool IsVrSimulatorReady(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    return vrSimulatorReady;
#else
    return false;
#endif
}

// Set VR distortion shader for stereoscopic rendering
// TODO: Review VR system to be more flexible, move distortion shader to user side
void SetVrDistortionShader(Shader shader)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    vrConfig.distortionShader = shader;

    //SetStereoConfig(info);  // TODO: Must be reviewed to set new distortion shader uniform values...
#endif
}

// Enable/Disable VR experience (device or simulator)
void ToggleVrMode(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    vrSimulatorReady = !vrSimulatorReady;

    if (!vrSimulatorReady)
    {
        vrStereoRender = false;
        
        // Reset viewport and default projection-modelview matrices
        rlViewport(0, 0, screenWidth, screenHeight);
        projection = MatrixOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0);
        modelview = MatrixIdentity();
    }
    else vrStereoRender = true;
#endif
}

// Update VR tracking (position and orientation) and camera
// NOTE: Camera (position, target, up) gets update with head tracking information
void UpdateVrTracking(Camera *camera)
{
    // TODO: Simulate 1st person camera system
}


#ifdef PLATFORM_SILENT_RADIANCE   
static int begun_vr_drawing=0;
#endif


// Begin Oculus drawing configuration
void BeginVrDrawing(void)
{
#ifdef PLATFORM_SILENT_RADIANCE
// BeginVrDrawing might be called by many independent raylib interfaces - many games or visualiz4ers 
// so we need to handle, for SILENT_RADIANCE, this by doing the init stuff only once
if (begun_vr_drawing) {
  begun_vr_drawing++;
  return;
  }
else {
  begun_vr_drawing++;
  }
#endif

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    if (vrSimulatorReady)
    {
        // Setup framebuffer for stereo rendering
        rlEnableRenderTexture(vrConfig.stereoFbo.id);

        // NOTE: If your application is configured to treat the texture as a linear format (e.g. GL_RGBA)
        // and performs linear-to-gamma conversion in GLSL or does not care about gamma-correction, then:
        //     - Require OculusBuffer format to be OVR_FORMAT_R8G8B8A8_UNORM_SRGB
        //     - Do NOT enable GL_FRAMEBUFFER_SRGB
        //glEnable(GL_FRAMEBUFFER_SRGB);

        //glViewport(0, 0, buffer.width, buffer.height);        // Useful if rendering to separate framebuffers (every eye)
        rlClearScreenBuffers();             // Clear current framebuffer(s)
        
        vrStereoRender = true;
    }
#endif
}



// End Oculus drawing process (and desktop mirror)
void EndVrDrawing(void)
{
#ifdef PLATFORM_SILENT_RADIANCE
begun_vr_drawing--;
checkGlError("p1\n");
if (begun_vr_drawing>0) {
  return;
  } 
if (begun_vr_drawing<0) {
  begun_vr_drawing=0;
  }
// When begun_drawing is 0, we will do the freme time stuff
#endif

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    if (vrSimulatorReady)
    {
        vrStereoRender = false;         // Disable stereo render
        
        rlDisableRenderTexture();       // Unbind current framebuffer

        rlClearScreenBuffers();         // Clear current framebuffer

        // Set viewport to default framebuffer size (screen size)
        rlViewport(0, 0, screenWidth, screenHeight);
  checkGlError("p4\n");

        // Let rlgl reconfigure internal matrices
        rlMatrixMode(RL_PROJECTION);                            // Enable internal projection matrix
        rlLoadIdentity();                                       // Reset internal projection matrix
        rlOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0); // Recalculate internal projection matrix
        rlMatrixMode(RL_MODELVIEW);                             // Enable internal modelview matrix
        rlLoadIdentity();                                       // Reset internal modelview matrix

#if defined(SUPPORT_DISTORTION_SHADER)
        // Draw RenderTexture (stereoFbo) using distortion shader
        currentShader = vrConfig.distortionShader;
#else
        currentShader = GetShaderDefault();
#endif

        rlEnableTexture(vrConfig.stereoFbo.texture.id);
checkGlError("p7\n");

        rlPushMatrix();
            rlBegin(RL_QUADS);
                rlColor4ub(255, 255, 255, 255);
                rlNormal3f(0.0f, 0.0f, 1.0f);

                // Bottom-left corner for texture and quad
                rlTexCoord2f(0.0f, 1.0f);
                rlVertex2f(0.0f, 0.0f);

                // Bottom-right corner for texture and quad
                rlTexCoord2f(0.0f, 0.0f);
                rlVertex2f(0.0f, vrConfig.stereoFbo.texture.height);

                // Top-right corner for texture and quad
                rlTexCoord2f(1.0f, 0.0f);
                rlVertex2f(vrConfig.stereoFbo.texture.width, vrConfig.stereoFbo.texture.height);

                // Top-left corner for texture and quad
                rlTexCoord2f(1.0f, 1.0f);
                rlVertex2f(vrConfig.stereoFbo.texture.width, 0.0f);
            rlEnd();
        rlPopMatrix();

        rlDisableTexture();
checkGlError("p8\n");

        // Update and draw render texture fbo with distortion to backbuffer
        UpdateBuffersDefault();
checkGlError("p9\n");
        DrawBuffersDefault();
checkGlError("p10\n");
        
        // Restore defaultShader
        currentShader = defaultShader;

        // Reset viewport and default projection-modelview matrices
        rlViewport(0, 0, screenWidth, screenHeight);
        projection = MatrixOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0);
        modelview = MatrixIdentity();
        
checkGlError("p11\n");
        rlDisableDepthTest();
    }
#endif
checkGlError("p12\n");
}
#endif          // SUPPORT_VR_SIMULATOR

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
// Compile custom shader and return shader id
static unsigned int CompileShader(const char *shaderStr, int type)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderStr, NULL);

    GLint success = 0;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE)
    {
        TraceLog(LOG_WARNING, "[SHDR ID %i] Failed to compile shader...", shader);
        int maxLength = 0;
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

#if defined(_MSC_VER)
        char *log = malloc(maxLength);
#else
        char log[maxLength];
#endif
        glGetShaderInfoLog(shader, maxLength, &length, log);

        TraceLog(LOG_INFO, "%s", log);

#if defined(_MSC_VER)
        free(log);
#endif
    }
    else TraceLog(LOG_INFO, "[SHDR ID %i] Shader compiled successfully", shader);

    return shader;
}

// Load custom shader strings and return program id
static unsigned int LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId)
{
    unsigned int program = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)

    GLint success = 0;
    program = glCreateProgram();

    glAttachShader(program, vShaderId);
    glAttachShader(program, fShaderId);

    // NOTE: Default attribute shader locations must be binded before linking
    glBindAttribLocation(program, 0, DEFAULT_ATTRIB_POSITION_NAME);
    glBindAttribLocation(program, 1, DEFAULT_ATTRIB_TEXCOORD_NAME);
    glBindAttribLocation(program, 2, DEFAULT_ATTRIB_NORMAL_NAME);
    glBindAttribLocation(program, 3, DEFAULT_ATTRIB_COLOR_NAME);
    glBindAttribLocation(program, 4, DEFAULT_ATTRIB_TANGENT_NAME);
    glBindAttribLocation(program, 5, DEFAULT_ATTRIB_TEXCOORD2_NAME);

    // NOTE: If some attrib name is no found on the shader, it locations becomes -1

    glLinkProgram(program);

    // NOTE: All uniform variables are intitialised to 0 when a program links

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        TraceLog(LOG_WARNING, "[SHDR ID %i] Failed to link shader program...", program);

        int maxLength = 0;
        int length;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

#ifdef _MSC_VER
        char *log = malloc(maxLength);
#else
        char log[maxLength];
#endif
        glGetProgramInfoLog(program, maxLength, &length, log);

        TraceLog(LOG_INFO, "%s", log);

#ifdef _MSC_VER
        free(log);
#endif
        glDeleteProgram(program);

        program = 0;
    }
    else TraceLog(LOG_INFO, "[SHDR ID %i] Shader program loaded successfully", program);
#endif
    return program;
}


// Load default shader (just vertex positioning and texture coloring)
// NOTE: This shader program is used for batch buffers (lines, triangles, quads)
static Shader LoadShaderDefault(void)
{
    Shader shader = { 0 };
    
    // NOTE: All locations must be reseted to -1 (no location)
    for (int i = 0; i < MAX_SHADER_LOCATIONS; i++) shader.locs[i] = -1;

    // Vertex shader directly defined, no external file required
    char defaultVShaderStr[] =
#if defined(GRAPHICS_API_OPENGL_21)
    "#version 120                       \n"
#elif defined(GRAPHICS_API_OPENGL_ES2)
    "#version 100                       \n"
#endif
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_21)
    "attribute vec3 vertexPosition;     \n"
    "attribute vec2 vertexTexCoord;     \n"
    "attribute vec4 vertexColor;        \n"
    "varying vec2 fragTexCoord;         \n"
    "varying vec4 fragColor;            \n"
#elif defined(GRAPHICS_API_OPENGL_33)
    "#version 330                       \n"
    "in vec3 vertexPosition;            \n"
    "in vec2 vertexTexCoord;            \n"
    "in vec4 vertexColor;               \n"
    "out vec2 fragTexCoord;             \n"
    "out vec4 fragColor;                \n"
#endif
    "uniform mat4 mvp;                  \n"
    "void main()                        \n"
    "{                                  \n"
    "    fragTexCoord = vertexTexCoord; \n"
    "    fragColor = vertexColor;       \n"
    "    gl_Position = mvp*vec4(vertexPosition, 1.0); \n"
    "}                                  \n";

    // Fragment shader directly defined, no external file required
    char defaultFShaderStr[] =
#if defined(GRAPHICS_API_OPENGL_21)
    "#version 120                       \n"
#elif defined(GRAPHICS_API_OPENGL_ES2)
    "#version 100                       \n"
    "precision mediump float;           \n"     // precision required for OpenGL ES2 (WebGL)
#endif
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_21)
    "varying vec2 fragTexCoord;         \n"
    "varying vec4 fragColor;            \n"
#elif defined(GRAPHICS_API_OPENGL_33)
    "#version 330       \n"
    "in vec2 fragTexCoord;              \n"
    "in vec4 fragColor;                 \n"
    "out vec4 finalColor;               \n"
#endif
    "uniform sampler2D texture0;        \n"
    "uniform vec4 colDiffuse;           \n"
    "void main()                        \n"
    "{                                  \n"
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_21)
    "    vec4 texelColor = texture2D(texture0, fragTexCoord); \n" // NOTE: texture2D() is deprecated on OpenGL 3.3 and ES 3.0
    "    gl_FragColor = texelColor*colDiffuse*fragColor;      \n"
#elif defined(GRAPHICS_API_OPENGL_33)
    "    vec4 texelColor = texture(texture0, fragTexCoord);   \n"
    "    finalColor = texelColor*colDiffuse*fragColor;        \n"
#endif
    "}                                  \n";

    // NOTE: Compiled vertex/fragment shaders are kept for re-use
    defaultVShaderId = CompileShader(defaultVShaderStr, GL_VERTEX_SHADER);     // Compile default vertex shader
    defaultFShaderId = CompileShader(defaultFShaderStr, GL_FRAGMENT_SHADER);   // Compile default fragment shader
    
    shader.id = LoadShaderProgram(defaultVShaderId, defaultFShaderId);

    if (shader.id > 0) 
    {
        TraceLog(LOG_INFO, "[SHDR ID %i] Default shader loaded successfully", shader.id);

        // Set default shader locations: attributes locations
        shader.locs[LOC_VERTEX_POSITION] = glGetAttribLocation(shader.id, "vertexPosition");
        shader.locs[LOC_VERTEX_TEXCOORD01] = glGetAttribLocation(shader.id, "vertexTexCoord");
        shader.locs[LOC_VERTEX_COLOR] = glGetAttribLocation(shader.id, "vertexColor");
			checkGlError("777color3");

        // Set default shader locations: uniform locations
        shader.locs[LOC_MATRIX_MVP]  = glGetUniformLocation(shader.id, "mvp");
        shader.locs[LOC_COLOR_DIFFUSE] = glGetUniformLocation(shader.id, "colDiffuse");
        shader.locs[LOC_MAP_DIFFUSE] = glGetUniformLocation(shader.id, "texture0");
        
        // NOTE: We could also use below function but in case DEFAULT_ATTRIB_* points are
        // changed for external custom shaders, we just use direct bindings above
        //SetShaderDefaultLocations(&shader);
    }
    else TraceLog(LOG_WARNING, "[SHDR ID %i] Default shader could not be loaded", shader.id);

    return shader;
}

// Get location handlers to for shader attributes and uniforms
// NOTE: If any location is not found, loc point becomes -1
static void SetShaderDefaultLocations(Shader *shader)
{
    // NOTE: Default shader attrib locations have been fixed before linking:
    //          vertex position location    = 0
    //          vertex texcoord location    = 1
    //          vertex normal location      = 2
    //          vertex color location       = 3
    //          vertex tangent location     = 4
    //          vertex texcoord2 location   = 5

    // Get handles to GLSL input attibute locations
    shader->locs[LOC_VERTEX_POSITION] = glGetAttribLocation(shader->id, DEFAULT_ATTRIB_POSITION_NAME);
    shader->locs[LOC_VERTEX_TEXCOORD01] = glGetAttribLocation(shader->id, DEFAULT_ATTRIB_TEXCOORD_NAME);
    shader->locs[LOC_VERTEX_TEXCOORD02] = glGetAttribLocation(shader->id, DEFAULT_ATTRIB_TEXCOORD2_NAME);
    shader->locs[LOC_VERTEX_NORMAL] = glGetAttribLocation(shader->id, DEFAULT_ATTRIB_NORMAL_NAME);
    shader->locs[LOC_VERTEX_TANGENT] = glGetAttribLocation(shader->id, DEFAULT_ATTRIB_TANGENT_NAME);
    shader->locs[LOC_VERTEX_COLOR] = glGetAttribLocation(shader->id, DEFAULT_ATTRIB_COLOR_NAME);
			logit("vvvvvvvvvvvv2 %d\n", shader->locs[LOC_VERTEX_COLOR]);
checkGlError("777color4");

    // Get handles to GLSL uniform locations (vertex shader)
    shader->locs[LOC_MATRIX_MVP]  = glGetUniformLocation(shader->id, "mvp");
    shader->locs[LOC_MATRIX_PROJECTION]  = glGetUniformLocation(shader->id, "projection");
    shader->locs[LOC_MATRIX_VIEW]  = glGetUniformLocation(shader->id, "view");

    // Get handles to GLSL uniform locations (fragment shader)
    shader->locs[LOC_COLOR_DIFFUSE] = glGetUniformLocation(shader->id, "colDiffuse");
    shader->locs[LOC_MAP_DIFFUSE] = glGetUniformLocation(shader->id, "texture0");
    shader->locs[LOC_MAP_SPECULAR] = glGetUniformLocation(shader->id, "texture1");
    shader->locs[LOC_MAP_NORMAL] = glGetUniformLocation(shader->id, "texture2");
}

// Unload default shader
static void UnloadShaderDefault(void)
{
    glUseProgram(0);

    glDetachShader(defaultShader.id, defaultVShaderId);
    glDetachShader(defaultShader.id, defaultFShaderId);
    glDeleteShader(defaultVShaderId);
    glDeleteShader(defaultFShaderId);
  
    glDeleteProgram(defaultShader.id);
}

// Load default internal buffers (lines, triangles, quads)
static void LoadBuffersDefault(void)
{
    // [CPU] Allocate and initialize float array buffers to store vertex data (lines, triangles, quads)
    //--------------------------------------------------------------------------------------------

    // Lines - Initialize arrays (vertex position and color data)
    lines.vertices = (float *)malloc(sizeof(float)*3*2*MAX_LINES_BATCH);        // 3 float by vertex, 2 vertex by line
    lines.colors = (unsigned char *)malloc(sizeof(unsigned char)*4*2*MAX_LINES_BATCH);  // 4 float by color, 2 colors by line
    lines.texcoords = NULL;
    lines.indices = NULL;

    for (int i = 0; i < (3*2*MAX_LINES_BATCH); i++) lines.vertices[i] = 0.0f;
    for (int i = 0; i < (4*2*MAX_LINES_BATCH); i++) lines.colors[i] = 0;

    lines.vCounter = 0;
    lines.cCounter = 0;
    lines.tcCounter = 0;

    // Triangles - Initialize arrays (vertex position and color data)
    triangles.vertices = (float *)malloc(sizeof(float)*3*3*MAX_TRIANGLES_BATCH);        // 3 float by vertex, 3 vertex by triangle
    triangles.colors = (unsigned char *)malloc(sizeof(unsigned char)*4*3*MAX_TRIANGLES_BATCH);  // 4 float by color, 3 colors by triangle
    triangles.texcoords = NULL;
    triangles.indices = NULL;

    for (int i = 0; i < (3*3*MAX_TRIANGLES_BATCH); i++) triangles.vertices[i] = 0.0f;
    for (int i = 0; i < (4*3*MAX_TRIANGLES_BATCH); i++) triangles.colors[i] = 0;

    triangles.vCounter = 0;
    triangles.cCounter = 0;
    triangles.tcCounter = 0;

    // Quads - Initialize arrays (vertex position, texcoord, color data and indexes)
    quads.vertices = (float *)malloc(sizeof(float)*3*4*MAX_QUADS_BATCH);        // 3 float by vertex, 4 vertex by quad
    quads.texcoords = (float *)malloc(sizeof(float)*2*4*MAX_QUADS_BATCH);       // 2 float by texcoord, 4 texcoord by quad
    quads.colors = (unsigned char *)malloc(sizeof(unsigned char)*4*4*MAX_QUADS_BATCH);  // 4 float by color, 4 colors by quad
#if defined(GRAPHICS_API_OPENGL_33)
    quads.indices = (unsigned int *)malloc(sizeof(int)*6*MAX_QUADS_BATCH);      // 6 int by quad (indices)
#elif defined(GRAPHICS_API_OPENGL_ES2)
    quads.indices = (unsigned short *)malloc(sizeof(short)*6*MAX_QUADS_BATCH);  // 6 int by quad (indices)
#endif

    for (int i = 0; i < (3*4*MAX_QUADS_BATCH); i++) quads.vertices[i] = 0.0f;
    for (int i = 0; i < (2*4*MAX_QUADS_BATCH); i++) quads.texcoords[i] = 0.0f;
    for (int i = 0; i < (4*4*MAX_QUADS_BATCH); i++) quads.colors[i] = 0;

    int k = 0;

    // Indices can be initialized right now
    for (int i = 0; i < (6*MAX_QUADS_BATCH); i+=6)
    {
        quads.indices[i] = 4*k;
        quads.indices[i+1] = 4*k+1;
        quads.indices[i+2] = 4*k+2;
        quads.indices[i+3] = 4*k;
        quads.indices[i+4] = 4*k+2;
        quads.indices[i+5] = 4*k+3;

        k++;
    }

    quads.vCounter = 0;
    quads.tcCounter = 0;
    quads.cCounter = 0;

    TraceLog(LOG_INFO, "[CPU] Default buffers initialized successfully (lines, triangles, quads)");
    //--------------------------------------------------------------------------------------------

    // [GPU] Upload vertex data and initialize VAOs/VBOs (lines, triangles, quads)
    // NOTE: Default buffers are linked to use currentShader (defaultShader)
    //--------------------------------------------------------------------------------------------

    // Upload and link lines vertex buffers
    if (vaoSupported)
    {
        // Initialize Lines VAO
        glGenVertexArrays(1, &lines.vaoId);
        glBindVertexArray(lines.vaoId);
    }

    // Lines - Vertex buffers binding and attributes enable
    // Vertex position buffer (shader-location = 0)
    glGenBuffers(2, &lines.vboId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, lines.vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*2*MAX_LINES_BATCH, lines.vertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_POSITION]);
    glVertexAttribPointer(currentShader.locs[LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);

    // Vertex color buffer (shader-location = 3)
    glGenBuffers(2, &lines.vboId[1]);
    glBindBuffer(GL_ARRAY_BUFFER, lines.vboId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned char)*4*2*MAX_LINES_BATCH, lines.colors, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_COLOR]);
		checkGlError("777color4");
    glVertexAttribPointer(currentShader.locs[LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
		checkGlError("777color5");

    if (vaoSupported) TraceLog(LOG_INFO, "[VAO ID %i] Default buffers VAO initialized successfully (lines)", lines.vaoId);
    else TraceLog(LOG_INFO, "[VBO ID %i][VBO ID %i] Default buffers VBOs initialized successfully (lines)", lines.vboId[0], lines.vboId[1]);

    // Upload and link triangles vertex buffers
    if (vaoSupported)
    {
        // Initialize Triangles VAO
        glGenVertexArrays(1, &triangles.vaoId);
        glBindVertexArray(triangles.vaoId);
    }

    // Triangles - Vertex buffers binding and attributes enable
    // Vertex position buffer (shader-location = 0)
    glGenBuffers(1, &triangles.vboId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, triangles.vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*3*MAX_TRIANGLES_BATCH, triangles.vertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_POSITION]);
    glVertexAttribPointer(currentShader.locs[LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);

    // Vertex color buffer (shader-location = 3)
    glGenBuffers(1, &triangles.vboId[1]);
    glBindBuffer(GL_ARRAY_BUFFER, triangles.vboId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned char)*4*3*MAX_TRIANGLES_BATCH, triangles.colors, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_COLOR]);
		checkGlError("777color6");
    glVertexAttribPointer(currentShader.locs[LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
		checkGlError("777color7");

    if (vaoSupported) TraceLog(LOG_INFO, "[VAO ID %i] Default buffers VAO initialized successfully (triangles)", triangles.vaoId);
    else TraceLog(LOG_INFO, "[VBO ID %i][VBO ID %i] Default buffers VBOs initialized successfully (triangles)", triangles.vboId[0], triangles.vboId[1]);

    // Upload and link quads vertex buffers
    if (vaoSupported)
    {
        // Initialize Quads VAO
        glGenVertexArrays(1, &quads.vaoId);
        glBindVertexArray(quads.vaoId);
    }

    // Quads - Vertex buffers binding and attributes enable
    // Vertex position buffer (shader-location = 0)
    glGenBuffers(1, &quads.vboId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, quads.vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*4*MAX_QUADS_BATCH, quads.vertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_POSITION]);
    glVertexAttribPointer(currentShader.locs[LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);

    // Vertex texcoord buffer (shader-location = 1)
    glGenBuffers(1, &quads.vboId[1]);
    glBindBuffer(GL_ARRAY_BUFFER, quads.vboId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*4*MAX_QUADS_BATCH, quads.texcoords, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_TEXCOORD01]);
    glVertexAttribPointer(currentShader.locs[LOC_VERTEX_TEXCOORD01], 2, GL_FLOAT, 0, 0, 0);

    // Vertex color buffer (shader-location = 3)
    glGenBuffers(1, &quads.vboId[2]);
    glBindBuffer(GL_ARRAY_BUFFER, quads.vboId[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned char)*4*4*MAX_QUADS_BATCH, quads.colors, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_COLOR]);
		checkGlError("777color8");
    
    glVertexAttribPointer(currentShader.locs[LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
		checkGlError("777color9");

    // Fill index buffer
    glGenBuffers(1, &quads.vboId[3]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quads.vboId[3]);
#if defined(GRAPHICS_API_OPENGL_33)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*6*MAX_QUADS_BATCH, quads.indices, GL_STATIC_DRAW);
#elif defined(GRAPHICS_API_OPENGL_ES2)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short)*6*MAX_QUADS_BATCH, quads.indices, GL_STATIC_DRAW);
#endif

    if (vaoSupported) TraceLog(LOG_INFO, "[VAO ID %i] Default buffers VAO initialized successfully (quads)", quads.vaoId);
    else TraceLog(LOG_INFO, "[VBO ID %i][VBO ID %i][VBO ID %i][VBO ID %i] Default buffers VBOs initialized successfully (quads)", quads.vboId[0], quads.vboId[1], quads.vboId[2], quads.vboId[3]);

    // Unbind the current VAO
    if (vaoSupported) glBindVertexArray(0);
    //--------------------------------------------------------------------------------------------
}

// Update default internal buffers (VAOs/VBOs) with vertex array data
// NOTE: If there is not vertex data, buffers doesn't need to be updated (vertexCount > 0)
// TODO: If no data changed on the CPU arrays --> No need to re-update GPU arrays (change flag required)
static void UpdateBuffersDefault(void)
{
    // Update lines vertex buffers
    if (lines.vCounter > 0)
    {
        // Activate Lines VAO
        if (vaoSupported) glBindVertexArray(lines.vaoId);

        // Lines - vertex positions buffer
        glBindBuffer(GL_ARRAY_BUFFER, lines.vboId[0]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*2*MAX_LINES_BATCH, lines.vertices, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*3*lines.vCounter, lines.vertices);    // target - offset (in bytes) - size (in bytes) - data pointer

        // Lines - colors buffer
        glBindBuffer(GL_ARRAY_BUFFER, lines.vboId[1]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*2*MAX_LINES_BATCH, lines.colors, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned char)*4*lines.cCounter, lines.colors);
    }

    // Update triangles vertex buffers
    if (triangles.vCounter > 0)
    {
        // Activate Triangles VAO
        if (vaoSupported) glBindVertexArray(triangles.vaoId);

        // Triangles - vertex positions buffer
        glBindBuffer(GL_ARRAY_BUFFER, triangles.vboId[0]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*3*MAX_TRIANGLES_BATCH, triangles.vertices, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*3*triangles.vCounter, triangles.vertices);

        // Triangles - colors buffer
        glBindBuffer(GL_ARRAY_BUFFER, triangles.vboId[1]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*3*MAX_TRIANGLES_BATCH, triangles.colors, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned char)*4*triangles.cCounter, triangles.colors);
    }

    // Update quads vertex buffers
    if (quads.vCounter > 0)
    {
        // Activate Quads VAO
        if (vaoSupported) glBindVertexArray(quads.vaoId);

        // Quads - vertex positions buffer
        glBindBuffer(GL_ARRAY_BUFFER, quads.vboId[0]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*4*MAX_QUADS_BATCH, quads.vertices, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*3*quads.vCounter, quads.vertices);

        // Quads - texture coordinates buffer
        glBindBuffer(GL_ARRAY_BUFFER, quads.vboId[1]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*4*MAX_QUADS_BATCH, quads.texcoords, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*2*quads.vCounter, quads.texcoords);

        // Quads - colors buffer
        glBindBuffer(GL_ARRAY_BUFFER, quads.vboId[2]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*4*MAX_QUADS_BATCH, quads.colors, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned char)*4*quads.vCounter, quads.colors);

        // Another option would be using buffer mapping...
        //quads.vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
        // Now we can modify vertices
        //glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    //--------------------------------------------------------------

    // Unbind the current VAO
    if (vaoSupported) glBindVertexArray(0);
}

// Draw default internal buffers vertex data
// NOTE: We draw in this order: lines, triangles, quads
static void DrawBuffersDefault(void)
{
    Matrix matProjection = projection;
    Matrix matModelView = modelview;
    
    int eyesCount = 1;
#if defined(SUPPORT_VR_SIMULATOR)
    if (vrStereoRender) eyesCount = 2;
#endif
    checkGlError("q1");
    for (int eye = 0; eye < eyesCount; eye++)
    {
        #if defined(SUPPORT_VR_SIMULATOR)
        if (eyesCount == 2) SetStereoView(eye, matProjection, matModelView);
        #endif

        // Set current shader and upload current MVP matrix
        if ((lines.vCounter > 0) || (triangles.vCounter > 0) || (quads.vCounter > 0))
        {
            glUseProgram(currentShader.id);

            // Create modelview-projection matrix
            Matrix matMVP = MatrixMultiply(modelview, projection);

            glUniformMatrix4fv(currentShader.locs[LOC_MATRIX_MVP], 1, false, MatrixToFloat(matMVP));
            glUniform4f(currentShader.locs[LOC_COLOR_DIFFUSE], 1.0f, 1.0f, 1.0f, 1.0f);
            glUniform1i(currentShader.locs[LOC_MAP_DIFFUSE], 0);

            // NOTE: Additional map textures not considered for default buffers drawing
        }



    checkGlError("q2");

        // Draw lines buffers
        if (lines.vCounter > 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, whiteTexture);

            if (vaoSupported)
            {
                glBindVertexArray(lines.vaoId);
            }
            else
            {
                // Bind vertex attrib: position (shader-location = 0)
                glBindBuffer(GL_ARRAY_BUFFER, lines.vboId[0]);
                glVertexAttribPointer(currentShader.locs[LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);
                glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_POSITION]);

                // Bind vertex attrib: color (shader-location = 3)
                glBindBuffer(GL_ARRAY_BUFFER, lines.vboId[1]);
                glVertexAttribPointer(currentShader.locs[LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
				checkGlError("777color10");

                glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_COLOR]);
				checkGlError("777color11");

            }

    checkGlError("q3");
            glDrawArrays(GL_LINES, 0, lines.vCounter);

            if (!vaoSupported) glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Draw triangles buffers
        if (triangles.vCounter > 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, whiteTexture);

            if (vaoSupported)
            {
                glBindVertexArray(triangles.vaoId);
            }
            else
            {
                // Bind vertex attrib: position (shader-location = 0)
                glBindBuffer(GL_ARRAY_BUFFER, triangles.vboId[0]);
                glVertexAttribPointer(currentShader.locs[LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);
                glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_POSITION]);

                // Bind vertex attrib: color (shader-location = 3)
                glBindBuffer(GL_ARRAY_BUFFER, triangles.vboId[1]);
                glVertexAttribPointer(currentShader.locs[LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
				checkGlError("777color12");

                glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_COLOR]);
				checkGlError("777color13");

            }

            glDrawArrays(GL_TRIANGLES, 0, triangles.vCounter);

            if (!vaoSupported) glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    checkGlError("q4");
        // Draw quads buffers
        if (quads.vCounter > 0)
        {
            int quadsCount = 0;
            int numIndicesToProcess = 0;
            int indicesOffset = 0;

            if (vaoSupported)
            {
                glBindVertexArray(quads.vaoId);
    checkGlError("q4a1");
            }
            else
            {
                // Bind vertex attrib: position (shader-location = 0)
                glBindBuffer(GL_ARRAY_BUFFER, quads.vboId[0]);
                glVertexAttribPointer(currentShader.locs[LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);
                glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_POSITION]);
    checkGlError("q4a4");

                // Bind vertex attrib: texcoord (shader-location = 1)
                glBindBuffer(GL_ARRAY_BUFFER, quads.vboId[1]);
                glVertexAttribPointer(currentShader.locs[LOC_VERTEX_TEXCOORD01], 2, GL_FLOAT, 0, 0, 0);
                glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_TEXCOORD01]);
    checkGlError("q4a3");

                // Bind vertex attrib: color (shader-location = 3)
                glBindBuffer(GL_ARRAY_BUFFER, quads.vboId[2]);
    checkGlError("q4a2a1");
                
		if (currentShader.locs[LOC_VERTEX_COLOR] != -1) { /*Hib*/
                  glVertexAttribPointer(currentShader.locs[LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
                  glEnableVertexAttribArray(currentShader.locs[LOC_VERTEX_COLOR]);
    checkGlError("q4a2a");
		  }


                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quads.vboId[3]);
    checkGlError("q4a2");
            }

            //TraceLog(LOG_DEBUG, "Draws required per frame: %i", drawsCounter);

    checkGlError("q5");
            for (int i = 0; i < drawsCounter; i++)
            {
                quadsCount = draws[i].vertexCount/4;
                numIndicesToProcess = quadsCount*6;  // Get number of Quads*6 index by Quad

                //TraceLog(LOG_DEBUG, "Quads to render: %i - Vertex Count: %i", quadsCount, draws[i].vertexCount);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, draws[i].textureId);

                // NOTE: The final parameter tells the GPU the offset in bytes from the start of the index buffer to the location of the first index to process
    #if defined(GRAPHICS_API_OPENGL_33)
                glDrawElements(GL_TRIANGLES, numIndicesToProcess, GL_UNSIGNED_INT, (GLvoid *)(sizeof(GLuint)*indicesOffset));
    #elif defined(GRAPHICS_API_OPENGL_ES2)
                glDrawElements(GL_TRIANGLES, numIndicesToProcess, GL_UNSIGNED_SHORT, (GLvoid *)(sizeof(GLushort)*indicesOffset));
    #endif
                //GLenum err;
                //if ((err = glGetError()) != GL_NO_ERROR) TraceLog(LOG_INFO, "OpenGL error: %i", (int)err);    //GL_INVALID_ENUM!

                indicesOffset += draws[i].vertexCount/4*6;
            }

    checkGlError("q6");
            if (!vaoSupported)
            {
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            glBindTexture(GL_TEXTURE_2D, 0);    // Unbind textures
        }

        if (vaoSupported) glBindVertexArray(0); // Unbind VAO

        glUseProgram(0);    // Unbind shader program
    }
    checkGlError("q7");

    // Reset draws counter
    drawsCounter = 1;
    draws[0].textureId = whiteTexture;
    draws[0].vertexCount = 0;

    // Reset vertex counters for next frame
    lines.vCounter = 0;
    lines.cCounter = 0;
    triangles.vCounter = 0;
    triangles.cCounter = 0;
    quads.vCounter = 0;
    quads.tcCounter = 0;
    quads.cCounter = 0;

    // Reset depth for next draw
    currentDepth = -1.0f;

    // Restore projection/modelview matrices
    projection = matProjection;
    modelview = matModelView;
    checkGlError("q8");
}

// Unload default internal buffers vertex data from CPU and GPU
static void UnloadBuffersDefault(void)
{
    // Unbind everything
    if (vaoSupported) glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Delete VBOs from GPU (VRAM)
    glDeleteBuffers(1, &lines.vboId[0]);
    glDeleteBuffers(1, &lines.vboId[1]);
    glDeleteBuffers(1, &triangles.vboId[0]);
    glDeleteBuffers(1, &triangles.vboId[1]);
    glDeleteBuffers(1, &quads.vboId[0]);
    glDeleteBuffers(1, &quads.vboId[1]);
    glDeleteBuffers(1, &quads.vboId[2]);
    glDeleteBuffers(1, &quads.vboId[3]);

    if (vaoSupported)
    {
        // Delete VAOs from GPU (VRAM)
        glDeleteVertexArrays(1, &lines.vaoId);
        glDeleteVertexArrays(1, &triangles.vaoId);
        glDeleteVertexArrays(1, &quads.vaoId);
    }

    // Free vertex arrays memory from CPU (RAM)
    free(lines.vertices);
    free(lines.colors);

    free(triangles.vertices);
    free(triangles.colors);

    free(quads.vertices);
    free(quads.texcoords);
    free(quads.colors);
    free(quads.indices);
}

// Renders a 1x1 XY quad in NDC
static void GenDrawQuad(void)
{
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;
    
    float vertices[] = {
        // Positions        // Texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    // Set up plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);

    // Fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // Link vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));

    // Draw quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &quadVAO);
}

// Renders a 1x1 3D cube in NDC
static void GenDrawCube(void)
{
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;

    float vertices[] = {
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f , 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    // Set up cube VAO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    // Fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Link vertex attributes
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *)(3*sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *)(6*sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Draw cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
}

// Get OpenGL internal formats and data type from raylib PixelFormat
static void GetGlFormats(int format, int *glInternalFormat, int *glFormat, int *glType)
{
    *glInternalFormat = -1;
    *glFormat = -1;
    *glType = -1;
    
    switch (format)
    {
    #if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_21) || defined(GRAPHICS_API_OPENGL_ES2)
        // NOTE: on OpenGL ES 2.0 (WebGL), internalFormat must match format and options allowed are: GL_LUMINANCE, GL_RGB, GL_RGBA
        case UNCOMPRESSED_GRAYSCALE: *glInternalFormat = GL_LUMINANCE; *glFormat = GL_LUMINANCE; *glType = GL_UNSIGNED_BYTE; break;
        case UNCOMPRESSED_GRAY_ALPHA: *glInternalFormat = GL_LUMINANCE_ALPHA; *glFormat = GL_LUMINANCE_ALPHA; *glType = GL_UNSIGNED_BYTE; break;
        case UNCOMPRESSED_R5G6B5: *glInternalFormat = GL_RGB; *glFormat = GL_RGB; *glType = GL_UNSIGNED_SHORT_5_6_5; break;
        case UNCOMPRESSED_R8G8B8: *glInternalFormat = GL_RGB; *glFormat = GL_RGB; *glType = GL_UNSIGNED_BYTE; break;
        case UNCOMPRESSED_R5G5B5A1: *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_5_5_5_1; break;
        case UNCOMPRESSED_R4G4B4A4: *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_4_4_4_4; break;
        case UNCOMPRESSED_R8G8B8A8: *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_BYTE; break;
        #if !defined(GRAPHICS_API_OPENGL_11)
        case UNCOMPRESSED_R32: if (texFloatSupported) *glInternalFormat = GL_LUMINANCE; *glFormat = GL_LUMINANCE; *glType = GL_FLOAT; break;   // NOTE: Requires extension OES_texture_float
        case UNCOMPRESSED_R32G32B32: if (texFloatSupported) *glInternalFormat = GL_RGB; *glFormat = GL_RGB; *glType = GL_FLOAT; break;         // NOTE: Requires extension OES_texture_float
        case UNCOMPRESSED_R32G32B32A32: if (texFloatSupported) *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_FLOAT; break;    // NOTE: Requires extension OES_texture_float
        #endif
    #elif defined(GRAPHICS_API_OPENGL_33)
        case UNCOMPRESSED_GRAYSCALE: *glInternalFormat = GL_R8; *glFormat = GL_RED; *glType = GL_UNSIGNED_BYTE; break;
        case UNCOMPRESSED_GRAY_ALPHA: *glInternalFormat = GL_RG8; *glFormat = GL_RG; *glType = GL_UNSIGNED_BYTE; break;
        case UNCOMPRESSED_R5G6B5: *glInternalFormat = GL_RGB565; *glFormat = GL_RGB; *glType = GL_UNSIGNED_SHORT_5_6_5; break;
        case UNCOMPRESSED_R8G8B8: *glInternalFormat = GL_RGB8; *glFormat = GL_RGB; *glType = GL_UNSIGNED_BYTE; break;
        case UNCOMPRESSED_R5G5B5A1: *glInternalFormat = GL_RGB5_A1; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_5_5_5_1; break;
        case UNCOMPRESSED_R4G4B4A4: *glInternalFormat = GL_RGBA4; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_4_4_4_4; break;
        case UNCOMPRESSED_R8G8B8A8: *glInternalFormat = GL_RGBA8; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_BYTE; break;
        case UNCOMPRESSED_R32: if (texFloatSupported) *glInternalFormat = GL_R32F; *glFormat = GL_RED; *glType = GL_FLOAT; break;
        case UNCOMPRESSED_R32G32B32: if (texFloatSupported) *glInternalFormat = GL_RGB32F; *glFormat = GL_RGB; *glType = GL_FLOAT; break;
        case UNCOMPRESSED_R32G32B32A32: if (texFloatSupported) *glInternalFormat = GL_RGBA32F; *glFormat = GL_RGBA; *glType = GL_FLOAT; break;
    #endif
        #if !defined(GRAPHICS_API_OPENGL_11)
        case COMPRESSED_DXT1_RGB: if (texCompDXTSupported) *glInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break;
        case COMPRESSED_DXT1_RGBA: if (texCompDXTSupported) *glInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
        case COMPRESSED_DXT3_RGBA: if (texCompDXTSupported) *glInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
        case COMPRESSED_DXT5_RGBA: if (texCompDXTSupported) *glInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
        case COMPRESSED_ETC1_RGB: if (texCompETC1Supported) *glInternalFormat = GL_ETC1_RGB8_OES; break;                      // NOTE: Requires OpenGL ES 2.0 or OpenGL 4.3
        case COMPRESSED_ETC2_RGB: if (texCompETC2Supported) *glInternalFormat = GL_COMPRESSED_RGB8_ETC2; break;               // NOTE: Requires OpenGL ES 3.0 or OpenGL 4.3
        case COMPRESSED_ETC2_EAC_RGBA: if (texCompETC2Supported) *glInternalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC; break;     // NOTE: Requires OpenGL ES 3.0 or OpenGL 4.3
        case COMPRESSED_PVRT_RGB: if (texCompPVRTSupported) *glInternalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG; break;    // NOTE: Requires PowerVR GPU
        case COMPRESSED_PVRT_RGBA: if (texCompPVRTSupported) *glInternalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG; break;  // NOTE: Requires PowerVR GPU
        case COMPRESSED_ASTC_4x4_RGBA: if (texCompASTCSupported) *glInternalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR; break;  // NOTE: Requires OpenGL ES 3.1 or OpenGL 4.3
        case COMPRESSED_ASTC_8x8_RGBA: if (texCompASTCSupported) *glInternalFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR; break;  // NOTE: Requires OpenGL ES 3.1 or OpenGL 4.3
        #endif
        default: TraceLog(LOG_WARNING, "Texture format not supported"); break;
    }
}

#if defined(SUPPORT_VR_SIMULATOR)
// Configure stereo rendering (including distortion shader) with HMD device parameters
// NOTE: It modifies the global variable: VrStereoConfig vrConfig
static void SetStereoConfig(VrDeviceInfo hmd)
{
    // Compute aspect ratio
    float aspect = ((float)hmd.hResolution*0.5f)/(float)hmd.vResolution;

    // Compute lens parameters
    float lensShift = (hmd.hScreenSize*0.25f - hmd.lensSeparationDistance*0.5f)/hmd.hScreenSize;
    float leftLensCenter[2] = { 0.25f + lensShift, 0.5f };
    float rightLensCenter[2] = { 0.75f - lensShift, 0.5f };
    float leftScreenCenter[2] = { 0.25f, 0.5f };
    float rightScreenCenter[2] = { 0.75f, 0.5f };

    // Compute distortion scale parameters
    // NOTE: To get lens max radius, lensShift must be normalized to [-1..1]
    float lensRadius = fabsf(-1.0f - 4.0f*lensShift);
    float lensRadiusSq = lensRadius*lensRadius;
    float distortionScale = hmd.lensDistortionValues[0] +
                            hmd.lensDistortionValues[1]*lensRadiusSq +
                            hmd.lensDistortionValues[2]*lensRadiusSq*lensRadiusSq +
                            hmd.lensDistortionValues[3]*lensRadiusSq*lensRadiusSq*lensRadiusSq;

    TraceLog(LOG_DEBUG, "VR: Distortion Scale: %f", distortionScale);

    float normScreenWidth = 0.5f;
    float normScreenHeight = 1.0f;
    float scaleIn[2] = { 2.0f/normScreenWidth, 2.0f/normScreenHeight/aspect };
    float scale[2] = { normScreenWidth*0.5f/distortionScale, normScreenHeight*0.5f*aspect/distortionScale };

    TraceLog(LOG_DEBUG, "VR: Distortion Shader: LeftLensCenter = { %f, %f }", leftLensCenter[0], leftLensCenter[1]);
    TraceLog(LOG_DEBUG, "VR: Distortion Shader: RightLensCenter = { %f, %f }", rightLensCenter[0], rightLensCenter[1]);
    TraceLog(LOG_DEBUG, "VR: Distortion Shader: Scale = { %f, %f }", scale[0], scale[1]);
    TraceLog(LOG_DEBUG, "VR: Distortion Shader: ScaleIn = { %f, %f }", scaleIn[0], scaleIn[1]);

#if defined(SUPPORT_DISTORTION_SHADER)
    // Update distortion shader with lens and distortion-scale parameters
    SetShaderValue(vrConfig.distortionShader, GetShaderLocation(vrConfig.distortionShader, "leftLensCenter"), leftLensCenter, 2);
    SetShaderValue(vrConfig.distortionShader, GetShaderLocation(vrConfig.distortionShader, "rightLensCenter"), rightLensCenter, 2);
    SetShaderValue(vrConfig.distortionShader, GetShaderLocation(vrConfig.distortionShader, "leftScreenCenter"), leftScreenCenter, 2);
    SetShaderValue(vrConfig.distortionShader, GetShaderLocation(vrConfig.distortionShader, "rightScreenCenter"), rightScreenCenter, 2);

    SetShaderValue(vrConfig.distortionShader, GetShaderLocation(vrConfig.distortionShader, "scale"), scale, 2);
    SetShaderValue(vrConfig.distortionShader, GetShaderLocation(vrConfig.distortionShader, "scaleIn"), scaleIn, 2);
    SetShaderValue(vrConfig.distortionShader, GetShaderLocation(vrConfig.distortionShader, "hmdWarpParam"), hmd.lensDistortionValues, 4);
    SetShaderValue(vrConfig.distortionShader, GetShaderLocation(vrConfig.distortionShader, "chromaAbParam"), hmd.chromaAbCorrection, 4);
#endif

    // Fovy is normally computed with: 2*atan2(hmd.vScreenSize, 2*hmd.eyeToScreenDistance)
    // ...but with lens distortion it is increased (see Oculus SDK Documentation)
    //float fovy = 2.0f*atan2(hmd.vScreenSize*0.5f*distortionScale, hmd.eyeToScreenDistance);     // Really need distortionScale?
    float fovy = 2.0f*(float)atan2(hmd.vScreenSize*0.5f, hmd.eyeToScreenDistance);
    lensShift = lensShift * 100;
    // Compute camera projection matrices
    float projOffset = 4.0f*lensShift;      // Scaled to projection space coordinates [-1..1]
    Matrix proj = MatrixPerspective(fovy, aspect, 0.01, 1000.0);
    vrConfig.eyesProjection[0] = MatrixMultiply(proj, MatrixTranslate(projOffset, 0.0f, 0.0f));
    vrConfig.eyesProjection[1] = MatrixMultiply(proj, MatrixTranslate(-projOffset, 0.0f, 0.0f));

    // Compute camera transformation matrices
    // NOTE: Camera movement might seem more natural if we model the head.
    // Our axis of rotation is the base of our head, so we might want to add
    // some y (base of head to eye level) and -z (center of head to eye protrusion) to the camera positions.
    vrConfig.eyesViewOffset[0] = MatrixTranslate(-hmd.interpupillaryDistance*0.5f, 0.075f, 0.045f);
    vrConfig.eyesViewOffset[1] = MatrixTranslate(hmd.interpupillaryDistance*0.5f, 0.075f, 0.045f);

    // Compute eyes Viewports
    vrConfig.eyesViewport[0] = (Rectangle){ 0, 0, hmd.hResolution/2, hmd.vResolution };
    vrConfig.eyesViewport[1] = (Rectangle){ hmd.hResolution/2, 0, hmd.hResolution/2, hmd.vResolution };
}

// Set internal projection and modelview matrix depending on eyes tracking data
static void SetStereoView(int eye, Matrix matProjection, Matrix matModelView)
{
    Matrix eyeProjection = matProjection;
    Matrix eyeModelView = matModelView;

    // Setup viewport and projection/modelview matrices using tracking data
    rlViewport(eye*screenWidth/2, 0, screenWidth/2, screenHeight);

    // Apply view offset to modelview matrix
    eyeModelView = MatrixMultiply(matModelView, vrConfig.eyesViewOffset[eye]);

    // Set current eye projection matrix
    eyeProjection = vrConfig.eyesProjection[eye];

    SetMatrixModelview(eyeModelView);
    SetMatrixProjection(eyeProjection);
}
#endif      // defined(SUPPORT_VR_SIMULATOR)

#endif //defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)

#if defined(GRAPHICS_API_OPENGL_11)
// Mipmaps data is generated after image data
static int GenerateMipmaps(unsigned char *data, int baseWidth, int baseHeight)
{
    int mipmapCount = 1;                // Required mipmap levels count (including base level)
    int width = baseWidth;
    int height = baseHeight;
    int size = baseWidth*baseHeight*4;  // Size in bytes (will include mipmaps...), RGBA only

    // Count mipmap levels required
    while ((width != 1) && (height != 1))
    {
        if (width != 1) width /= 2;
        if (height != 1) height /= 2;

        TraceLog(LOG_DEBUG, "Next mipmap size: %i x %i", width, height);

        mipmapCount++;

        size += (width*height*4);       // Add mipmap size (in bytes)
    }

    TraceLog(LOG_DEBUG, "Total mipmaps required: %i", mipmapCount);
    TraceLog(LOG_DEBUG, "Total size of data required: %i", size);

    unsigned char *temp = realloc(data, size);

    if (temp != NULL) data = temp;
    else TraceLog(LOG_WARNING, "Mipmaps required memory could not be allocated");

    width = baseWidth;
    height = baseHeight;
    size = (width*height*4);

    // Generate mipmaps
    // NOTE: Every mipmap data is stored after data
    Color *image = (Color *)malloc(width*height*sizeof(Color));
    Color *mipmap = NULL;
    int offset = 0;
    int j = 0;

    for (int i = 0; i < size; i += 4)
    {
        image[j].r = data[i];
        image[j].g = data[i + 1];
        image[j].b = data[i + 2];
        image[j].a = data[i + 3];
        j++;
    }

    TraceLog(LOG_DEBUG, "Mipmap base (%ix%i)", width, height);

    for (int mip = 1; mip < mipmapCount; mip++)
    {
        mipmap = GenNextMipmap(image, width, height);

        offset += (width*height*4); // Size of last mipmap
        j = 0;

        width /= 2;
        height /= 2;
        size = (width*height*4);    // Mipmap size to store after offset

        // Add mipmap to data
        for (int i = 0; i < size; i += 4)
        {
            data[offset + i] = mipmap[j].r;
            data[offset + i + 1] = mipmap[j].g;
            data[offset + i + 2] = mipmap[j].b;
            data[offset + i + 3] = mipmap[j].a;
            j++;
        }

        free(image);

        image = mipmap;
        mipmap = NULL;
    }

    free(mipmap);       // free mipmap data

    return mipmapCount;
}

// Manual mipmap generation (basic scaling algorithm)
static Color *GenNextMipmap(Color *srcData, int srcWidth, int srcHeight)
{
    int x2, y2;
    Color prow, pcol;

    int width = srcWidth/2;
    int height = srcHeight/2;

    Color *mipmap = (Color *)malloc(width*height*sizeof(Color));

    // Scaling algorithm works perfectly (box-filter)
    for (int y = 0; y < height; y++)
    {
        y2 = 2*y;

        for (int x = 0; x < width; x++)
        {
            x2 = 2*x;

            prow.r = (srcData[y2*srcWidth + x2].r + srcData[y2*srcWidth + x2 + 1].r)/2;
            prow.g = (srcData[y2*srcWidth + x2].g + srcData[y2*srcWidth + x2 + 1].g)/2;
            prow.b = (srcData[y2*srcWidth + x2].b + srcData[y2*srcWidth + x2 + 1].b)/2;
            prow.a = (srcData[y2*srcWidth + x2].a + srcData[y2*srcWidth + x2 + 1].a)/2;

            pcol.r = (srcData[(y2+1)*srcWidth + x2].r + srcData[(y2+1)*srcWidth + x2 + 1].r)/2;
            pcol.g = (srcData[(y2+1)*srcWidth + x2].g + srcData[(y2+1)*srcWidth + x2 + 1].g)/2;
            pcol.b = (srcData[(y2+1)*srcWidth + x2].b + srcData[(y2+1)*srcWidth + x2 + 1].b)/2;
            pcol.a = (srcData[(y2+1)*srcWidth + x2].a + srcData[(y2+1)*srcWidth + x2 + 1].a)/2;

            mipmap[y*width + x].r = (prow.r + pcol.r)/2;
            mipmap[y*width + x].g = (prow.g + pcol.g)/2;
            mipmap[y*width + x].b = (prow.b + pcol.b)/2;
            mipmap[y*width + x].a = (prow.a + pcol.a)/2;
        }
    }

    TraceLog(LOG_DEBUG, "Mipmap generated successfully (%ix%i)", width, height);

    return mipmap;
}
#endif

#if defined(RLGL_STANDALONE)
// Show trace log messages (LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_DEBUG)
void TraceLog(int msgType, const char *text, ...)
{
    va_list args;
    va_start(args, text);

    switch (msgType)
    {
        case LOG_INFO: logit( "INFO: "); break;
        case LOG_ERROR: logit( "ERROR: "); break;
        case LOG_WARNING: logit( "WARNING: "); break;
        case LOG_DEBUG: logit( "DEBUG: "); break;
        default: break;
    }

    vlogit( text, args);
    logit( "\n");

    va_end(args);

    if (msgType == LOG_ERROR) exit(1);
}
#endif





/*??? shapes */
/**********************************************************************************************
*
*   raylib.shapes - Basic functions to draw 2d Shapes and check collisions
*
*   CONFIGURATION:
*
*   #define SUPPORT_QUADS_ONLY
*       Draw shapes using only QUADS, vertex are accumulated in QUADS arrays (like textures)
*
*   #define SUPPORT_TRIANGLES_ONLY
*       Draw shapes using only TRIANGLES, vertex are accumulated in TRIANGLES arrays
*
*   #define USE_DEFAULT_FONT_TEXTURE
*       Draw rectangle shapes using font texture white character instead of default white texture
*       Allows drawing rectangles and text with a single draw call, very useful for GUI systems!
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2018 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/



//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Nop...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// Not here...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static float EaseCubicInOut(float t, float b, float c, float d);    // Cubic easing

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Draw a pixel
void DrawPixel(int posX, int posY, Color color)
{
    rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2i(posX, posY);
        rlVertex2i(posX + 1, posY + 1);
    rlEnd();
}

// Draw a pixel (Vector version)
void DrawPixelV(Vector2 position, Color color)
{
    rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(position.x, position.y);
        rlVertex2f(position.x + 1.0f, position.y + 1.0f);
    rlEnd();
}

// Draw a line
void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color)
{
    rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2i(startPosX, startPosY);
        rlVertex2i(endPosX, endPosY);
    rlEnd();
}

// Draw a line  (Vector version)
void DrawLineV(Vector2 startPos, Vector2 endPos, Color color)
{
    rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(startPos.x, startPos.y);
        rlVertex2f(endPos.x, endPos.y);
    rlEnd();
}

// Draw a line defining thickness
void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color)
{
    if (startPos.x > endPos.x)
    {
        Vector2 tempPos = startPos;
        startPos = endPos;
        endPos = tempPos;
    }
    
    float dx = endPos.x - startPos.x;
    float dy = endPos.y - startPos.y;
    
    float d = sqrtf(dx*dx + dy*dy);
    float angle = asinf(dy/d);
    
    rlEnableTexture(GetTextureDefault().id);

    rlPushMatrix();
        rlTranslatef((float)startPos.x, (float)startPos.y, 0);
        rlRotatef(RAD2DEG*angle, 0, 0, 1);
        rlTranslatef(0, -thick/2.0f, 0);

        rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlNormal3f(0.0f, 0.0f, 1.0f);

            rlVertex2f(0.0f, 0.0f);
            rlVertex2f(0.0f, thick);
            rlVertex2f(d, thick);
            rlVertex2f(d, 0.0f);
        rlEnd();
    rlPopMatrix();

    rlDisableTexture();
}

// Draw line using cubic-bezier curves in-out
void DrawLineBezier(Vector2 startPos, Vector2 endPos, float thick, Color color)
{
    #define LINE_DIVISIONS         24   // Bezier line divisions

    Vector2 previous = startPos;
    Vector2 current;

    for (int i = 1; i <= LINE_DIVISIONS; i++)
    {
        // Cubic easing in-out
        // NOTE: Easing is calcutated only for y position value 
        current.y = EaseCubicInOut(i, startPos.y, endPos.y - startPos.y, LINE_DIVISIONS);
        current.x = previous.x + (endPos.x - startPos.x)/LINE_DIVISIONS;
        
        DrawLineEx(previous, current, thick, color);
        
        previous = current;
    }
}

// Draw a color-filled circle
void DrawCircle(int centerX, int centerY, float radius, Color color)
{
    DrawCircleV((Vector2){ (float)centerX, (float)centerY }, radius, color);
}

// Draw a gradient-filled circle
// NOTE: Gradient goes from center (color1) to border (color2)
void DrawCircleGradient(int centerX, int centerY, float radius, Color color1, Color color2)
{
    rlBegin(RL_TRIANGLES);
        for (int i = 0; i < 360; i += 10)
        {
            rlColor4ub(color1.r, color1.g, color1.b, color1.a);
            rlVertex2i(centerX, centerY);
            rlColor4ub(color2.r, color2.g, color2.b, color2.a);
            rlVertex2f(centerX + sinf(DEG2RAD*i)*radius, centerY + cosf(DEG2RAD*i)*radius);
            rlColor4ub(color2.r, color2.g, color2.b, color2.a);
            rlVertex2f(centerX + sinf(DEG2RAD*(i + 10))*radius, centerY + cosf(DEG2RAD*(i + 10))*radius);
        }
    rlEnd();
}

// Draw a color-filled circle (Vector version)
// NOTE: On OpenGL 3.3 and ES2 we use QUADS to avoid drawing order issues (view rlglDraw)
void DrawCircleV(Vector2 center, float radius, Color color)
{
    if (rlGetVersion() == OPENGL_11)
    {
        rlBegin(RL_TRIANGLES);
            for (int i = 0; i < 360; i += 10)
            {
                rlColor4ub(color.r, color.g, color.b, color.a);
                
                rlVertex2f(center.x, center.y);
                rlVertex2f(center.x + sinf(DEG2RAD*i)*radius, center.y + cosf(DEG2RAD*i)*radius);
                rlVertex2f(center.x + sinf(DEG2RAD*(i + 10))*radius, center.y + cosf(DEG2RAD*(i + 10))*radius);
            }
        rlEnd();
    }
    else if ((rlGetVersion() == OPENGL_21) || (rlGetVersion() == OPENGL_33) || (rlGetVersion() == OPENGL_ES_20))
    {
        rlEnableTexture(GetTextureDefault().id); // Default white texture

        rlBegin(RL_QUADS);
            for (int i = 0; i < 360; i += 20)
            {
                rlColor4ub(color.r, color.g, color.b, color.a);
                
                rlVertex2f(center.x, center.y);
                rlVertex2f(center.x + sinf(DEG2RAD*i)*radius, center.y + cosf(DEG2RAD*i)*radius);
                rlVertex2f(center.x + sinf(DEG2RAD*(i + 10))*radius, center.y + cosf(DEG2RAD*(i + 10))*radius);
                rlVertex2f(center.x + sinf(DEG2RAD*(i + 20))*radius, center.y + cosf(DEG2RAD*(i + 20))*radius);
            }
        rlEnd();
        
        rlDisableTexture();
    }
}

// Draw circle outline
void DrawCircleLines(int centerX, int centerY, float radius, Color color)
{
    rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);

        // NOTE: Circle outline is drawn pixel by pixel every degree (0 to 360)
        for (int i = 0; i < 360; i += 10)
        {
            rlVertex2f(centerX + sinf(DEG2RAD*i)*radius, centerY + cosf(DEG2RAD*i)*radius);
            rlVertex2f(centerX + sinf(DEG2RAD*(i + 10))*radius, centerY + cosf(DEG2RAD*(i + 10))*radius);
        }
   rlEnd();
}

// Draw a color-filled rectangle
void DrawRectangle(int posX, int posY, int width, int height, Color color)
{
    Vector2 position = { (float)posX, (float)posY };
    Vector2 size = { (float)width, (float)height };

    DrawRectangleV(position, size, color);
}

// Draw a color-filled rectangle (Vector version)
// NOTE: On OpenGL 3.3 and ES2 we use QUADS to avoid drawing order issues (view rlglDraw)
void DrawRectangleV(Vector2 position, Vector2 size, Color color)
{
    if (rlGetVersion() == OPENGL_11)
    {
        rlBegin(RL_TRIANGLES);
            rlColor4ub(color.r, color.g, color.b, color.a);

            rlVertex2i(position.x, position.y);
            rlVertex2i(position.x, position.y + size.y);
            rlVertex2i(position.x + size.x, position.y + size.y);

            rlVertex2i(position.x, position.y);
            rlVertex2i(position.x + size.x, position.y + size.y);
            rlVertex2i(position.x + size.x, position.y);
        rlEnd();
    }
    else if ((rlGetVersion() == OPENGL_21) || (rlGetVersion() == OPENGL_33) || (rlGetVersion() == OPENGL_ES_20))
    {
#if defined(USE_DEFAULT_FONT_TEXTURE)
        // Draw rectangle using font texture white character
        rlEnableTexture(GetDefaultFont().texture.id);

        rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlNormal3f(0.0f, 0.0f, 1.0f);

            // NOTE: Default raylib font character 95 is a white square
            rlTexCoord2f((float)GetDefaultFont().chars[95].rec.x/GetDefaultFont().texture.width, 
                         (float)GetDefaultFont().chars[95].rec.y/GetDefaultFont().texture.height);
            rlVertex2f(position.x, position.y);
            
            rlTexCoord2f((float)GetDefaultFont().chars[95].rec.x/GetDefaultFont().texture.width, 
                         (float)(GetDefaultFont().chars[95].rec.y + GetDefaultFont().chars[95].rec.height)/GetDefaultFont().texture.height);
            rlVertex2f(position.x, position.y + size.y);
            
            rlTexCoord2f((float)(GetDefaultFont().chars[95].rec.x + GetDefaultFont().chars[95].rec.width)/GetDefaultFont().texture.width, 
                         (float)(GetDefaultFont().chars[95].rec.y + GetDefaultFont().chars[95].rec.height)/GetDefaultFont().texture.height);
            rlVertex2f(position.x + size.x, position.y + size.y);
            
            rlTexCoord2f((float)(GetDefaultFont().chars[95].rec.x + GetDefaultFont().chars[95].rec.width)/GetDefaultFont().texture.width, 
                         (float)GetDefaultFont().chars[95].rec.y/GetDefaultFont().texture.height);
            rlVertex2f(position.x + size.x, position.y);
        rlEnd();
        
        rlDisableTexture();
#else
        rlEnableTexture(GetTextureDefault().id); // Default white texture

        rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlNormal3f(0.0f, 0.0f, 1.0f);

            rlTexCoord2f(0.0f, 0.0f);
            rlVertex2f(position.x, position.y);

            rlTexCoord2f(0.0f, 1.0f);
            rlVertex2f(position.x, position.y + size.y);

            rlTexCoord2f(1.0f, 1.0f);
            rlVertex2f(position.x + size.x, position.y + size.y);

            rlTexCoord2f(1.0f, 0.0f);
            rlVertex2f(position.x + size.x, position.y);
        rlEnd();

        rlDisableTexture();
#endif
    }
}

// Draw a color-filled rectangle
void DrawRectangleRec(Rectangle rec, Color color)
{
    DrawRectangle(rec.x, rec.y, rec.width, rec.height, color);
}

void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color)
{
    rlEnableTexture(GetTextureDefault().id);

    rlPushMatrix();
        rlTranslatef((float)rec.x, (float)rec.y, 0);
        rlRotatef(rotation, 0, 0, 1);
        rlTranslatef(-origin.x, -origin.y, 0);

        rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer

            rlVertex2f(0.0f, 0.0f);
            rlVertex2f(0.0f, (float)rec.height);
            rlVertex2f((float)rec.width, (float)rec.height);
            rlVertex2f((float)rec.width, 0.0f);
        rlEnd();
    rlPopMatrix();

    rlDisableTexture();
}

// Draw a vertical-gradient-filled rectangle
// NOTE: Gradient goes from bottom (color1) to top (color2)
void DrawRectangleGradientV(int posX, int posY, int width, int height, Color color1, Color color2)
{
    DrawRectangleGradientEx((Rectangle){ posX, posY, width, height }, color1, color2, color2, color1);
}

// Draw a horizontal-gradient-filled rectangle
// NOTE: Gradient goes from bottom (color1) to top (color2)
void DrawRectangleGradientH(int posX, int posY, int width, int height, Color color1, Color color2)
{
    DrawRectangleGradientEx((Rectangle){ posX, posY, width, height }, color1, color1, color2, color2);
}

// Draw a gradient-filled rectangle
// NOTE: Colors refer to corners, starting at top-lef corner and counter-clockwise
void DrawRectangleGradientEx(Rectangle rec, Color col1, Color col2, Color col3, Color col4)
{
#if defined(USE_DEFAULT_FONT_TEXTURE)
    // Draw rectangle using font texture white character
    rlEnableTexture(GetDefaultFont().texture.id);

    rlBegin(RL_QUADS);
        rlNormal3f(0.0f, 0.0f, 1.0f);

        // NOTE: Default raylib font character 95 is a white square
        rlColor4ub(col1.r, col1.g, col1.b, col1.a);
        rlTexCoord2f((float)GetDefaultFont().chars[95].rec.x/GetDefaultFont().texture.width, 
                     (float)GetDefaultFont().chars[95].rec.y/GetDefaultFont().texture.height);
        rlVertex2f(rec.x, rec.y);
        
        rlColor4ub(col2.r, col2.g, col2.b, col2.a);
        rlTexCoord2f((float)GetDefaultFont().chars[95].rec.x/GetDefaultFont().texture.width, 
                     (float)(GetDefaultFont().chars[95].rec.y + GetDefaultFont().chars[95].rec.height)/GetDefaultFont().texture.height);
        rlVertex2f(rec.x, rec.y + rec.height);
        
        rlColor4ub(col3.r, col3.g, col3.b, col3.a);
        rlTexCoord2f((float)(GetDefaultFont().chars[95].rec.x + GetDefaultFont().chars[95].rec.width)/GetDefaultFont().texture.width, 
                     (float)(GetDefaultFont().chars[95].rec.y + GetDefaultFont().chars[95].rec.height)/GetDefaultFont().texture.height);
        rlVertex2f(rec.x + rec.width, rec.y + rec.height);
        
        rlColor4ub(col4.r, col4.g, col4.b, col4.a);
        rlTexCoord2f((float)(GetDefaultFont().chars[95].rec.x + GetDefaultFont().chars[95].rec.width)/GetDefaultFont().texture.width, 
                     (float)GetDefaultFont().chars[95].rec.y/GetDefaultFont().texture.height);
        rlVertex2f(rec.x + rec.width, rec.y);
    rlEnd();
    
    rlDisableTexture();
#else
    rlEnableTexture(GetTextureDefault().id);    // Default white texture

    rlBegin(RL_QUADS);
        rlNormal3f(0.0f, 0.0f, 1.0f);

        rlColor4ub(col1.r, col1.g, col1.b, col1.a);
        rlTexCoord2f(0.0f, 0.0f);
        rlVertex2f(rec.x, rec.y);

        rlColor4ub(col2.r, col2.g, col2.b, col2.a);
        rlTexCoord2f(0.0f, 1.0f);
        rlVertex2f(rec.x, rec.y + rec.height);

        rlColor4ub(col3.r, col3.g, col3.b, col3.a);
        rlTexCoord2f(1.0f, 1.0f);
        rlVertex2f(rec.x + rec.width, rec.y + rec.height);

        rlColor4ub(col4.r, col4.g, col4.b, col4.a);
        rlTexCoord2f(1.0f, 0.0f);
        rlVertex2f(rec.x + rec.width, rec.y);
    rlEnd();

    rlDisableTexture();
#endif
}

// Draw rectangle outline
// NOTE: On OpenGL 3.3 and ES2 we use QUADS to avoid drawing order issues (view rlglDraw)
void DrawRectangleLines(int posX, int posY, int width, int height, Color color)
{   
    if (rlGetVersion() == OPENGL_11)
    {
        rlBegin(RL_LINES);
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlVertex2i(posX + 1, posY + 1);
            rlVertex2i(posX + width, posY + 1);

            rlVertex2i(posX + width, posY + 1);
            rlVertex2i(posX + width, posY + height);

            rlVertex2i(posX + width, posY + height);
            rlVertex2i(posX + 1, posY + height);

            rlVertex2i(posX + 1, posY + height);
            rlVertex2i(posX + 1, posY + 1);
        rlEnd();
    }
    else if ((rlGetVersion() == OPENGL_21) || (rlGetVersion() == OPENGL_33) || (rlGetVersion() == OPENGL_ES_20))
    {
        DrawRectangle(posX, posY, width, 1, color);
        DrawRectangle(posX + width - 1, posY + 1, 1, height - 2, color);
        DrawRectangle(posX, posY + height - 1, width, 1, color);
        DrawRectangle(posX, posY + 1, 1, height - 2, color);
    }
}

// Draw rectangle outline with extended parameters
void DrawRectangleLinesEx(Rectangle rec, int lineThick, Color color)
{   
    if (lineThick > rec.width || lineThick > rec.height)
    {
        if(rec.width > rec.height) lineThick = rec.height/2;
        else if (rec.width < rec.height) lineThick = rec.width/2;
    }        
    
    DrawRectangle(rec.x, rec.y, rec.width, lineThick, color);
    DrawRectangle(rec.x - lineThick + rec.width, rec.y + lineThick, lineThick, rec.height - lineThick*2, color);
    DrawRectangle(rec.x, rec.y + rec.height - lineThick, rec.width, lineThick, color);
    DrawRectangle(rec.x, rec.y + lineThick, lineThick, rec.height - lineThick*2, color);
}

// Draw a triangle
void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
{
    if (rlGetVersion() == OPENGL_11)
    {
        rlBegin(RL_TRIANGLES);
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlVertex2f(v1.x, v1.y);
            rlVertex2f(v2.x, v2.y);
            rlVertex2f(v3.x, v3.y);
        rlEnd();
    }
    else if ((rlGetVersion() == OPENGL_21) || (rlGetVersion() == OPENGL_33) || (rlGetVersion() == OPENGL_ES_20))
    {
        rlEnableTexture(GetTextureDefault().id); // Default white texture

        rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlVertex2f(v1.x, v1.y);
            rlVertex2f(v2.x, v2.y);
            rlVertex2f(v2.x, v2.y);
            rlVertex2f(v3.x, v3.y);
        rlEnd();
        
        rlDisableTexture();
    }
}

void DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
{
    rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(v1.x, v1.y);
        rlVertex2f(v2.x, v2.y);

        rlVertex2f(v2.x, v2.y);
        rlVertex2f(v3.x, v3.y);

        rlVertex2f(v3.x, v3.y);
        rlVertex2f(v1.x, v1.y);
    rlEnd();
}

// Draw a regular polygon of n sides (Vector version)
void DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color)
{
    if (sides < 3) sides = 3;

    rlPushMatrix();
        rlTranslatef(center.x, center.y, 0.0);
        rlRotatef(rotation, 0, 0, 1);

        rlBegin(RL_TRIANGLES);
            for (int i = 0; i < 360; i += 360/sides)
            {
                rlColor4ub(color.r, color.g, color.b, color.a);

                rlVertex2f(0, 0);
                rlVertex2f(sinf(DEG2RAD*i)*radius, cosf(DEG2RAD*i)*radius);
                rlVertex2f(sinf(DEG2RAD*(i + 360/sides))*radius, cosf(DEG2RAD*(i + 360/sides))*radius);
            }
        rlEnd();
    rlPopMatrix();
}

// Draw a closed polygon defined by points
// NOTE: Array num elements MUST be passed as parameter to function
void DrawPolyEx(Vector2 *points, int numPoints, Color color)
{
    if (numPoints >= 3)
    {
        rlBegin(RL_TRIANGLES);
            rlColor4ub(color.r, color.g, color.b, color.a);

            for (int i = 1; i < numPoints - 1; i++)
            {
                rlVertex2f(points[0].x, points[0].y);
                rlVertex2f(points[i].x, points[i].y);
                rlVertex2f(points[i + 1].x, points[i + 1].y);
            }
        rlEnd();
    }
}

// Draw polygon lines
// NOTE: Array num elements MUST be passed as parameter to function
void DrawPolyExLines(Vector2 *points, int numPoints, Color color)
{
    if (numPoints >= 2)
    {
        rlBegin(RL_LINES);
            rlColor4ub(color.r, color.g, color.b, color.a);

            for (int i = 0; i < numPoints - 1; i++)
            {
                rlVertex2f(points[i].x, points[i].y);
                rlVertex2f(points[i + 1].x, points[i + 1].y);
            }
        rlEnd();
    }
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Collision Detection functions
//----------------------------------------------------------------------------------

// Check if point is inside rectangle
bool CheckCollisionPointRec(Vector2 point, Rectangle rec)
{
    bool collision = false;

    if ((point.x >= rec.x) && (point.x <= (rec.x + rec.width)) && (point.y >= rec.y) && (point.y <= (rec.y + rec.height))) collision = true;

    return collision;
}

// Check if point is inside circle
bool CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius)
{
    return CheckCollisionCircles(point, 0, center, radius);
}

// Check if point is inside a triangle defined by three points (p1, p2, p3)
bool CheckCollisionPointTriangle(Vector2 point, Vector2 p1, Vector2 p2, Vector2 p3)
{
    bool collision = false;

    float alpha = ((p2.y - p3.y)*(point.x - p3.x) + (p3.x - p2.x)*(point.y - p3.y)) /
                  ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));

    float beta = ((p3.y - p1.y)*(point.x - p3.x) + (p1.x - p3.x)*(point.y - p3.y)) /
                 ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));

    float gamma = 1.0f - alpha - beta;

    if ((alpha > 0) && (beta > 0) & (gamma > 0)) collision = true;

    return collision;
}

// Check collision between two rectangles
bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2)
{
    bool collision = false;

    int dx = abs((rec1.x + rec1.width/2) - (rec2.x + rec2.width/2));
    int dy = abs((rec1.y + rec1.height/2) - (rec2.y + rec2.height/2));

    if ((dx <= (rec1.width/2 + rec2.width/2)) && ((dy <= (rec1.height/2 + rec2.height/2)))) collision = true;

    return collision;
}

// Check collision between two circles
bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
{
    bool collision = false;

    float dx = center2.x - center1.x;      // X distance between centers
    float dy = center2.y - center1.y;      // Y distance between centers

    float distance = sqrtf(dx*dx + dy*dy); // Distance between centers

    if (distance <= (radius1 + radius2)) collision = true;

    return collision;
}

// Check collision between circle and rectangle
// NOTE: Reviewed version to take into account corner limit case
bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec)
{
    int recCenterX = rec.x + rec.width/2;
    int recCenterY = rec.y + rec.height/2;
    
    float dx = fabsf(center.x - recCenterX);
    float dy = fabsf(center.y - recCenterY);

    if (dx > ((float)rec.width/2.0f + radius)) { return false; }
    if (dy > ((float)rec.height/2.0f + radius)) { return false; }

    if (dx <= ((float)rec.width/2.0f)) { return true; }
    if (dy <= ((float)rec.height/2.0f)) { return true; }

    float cornerDistanceSq = (dx - (float)rec.width/2.0f)*(dx - (float)rec.width/2.0f) + 
						     (dy - (float)rec.height/2.0f)*(dy - (float)rec.height/2.0f);

    return (cornerDistanceSq <= (radius*radius));
}

// Get collision rectangle for two rectangles collision
Rectangle GetCollisionRec(Rectangle rec1, Rectangle rec2)
{
    Rectangle retRec = { 0, 0, 0, 0 };

    if (CheckCollisionRecs(rec1, rec2))
    {
        int dxx = abs(rec1.x - rec2.x);
        int dyy = abs(rec1.y - rec2.y);

        if (rec1.x <= rec2.x)
        {
            if (rec1.y <= rec2.y)
            {
                retRec.x = rec2.x;
                retRec.y = rec2.y;
                retRec.width = rec1.width - dxx;
                retRec.height = rec1.height - dyy;
            }
            else
            {
                retRec.x = rec2.x;
                retRec.y = rec1.y;
                retRec.width = rec1.width - dxx;
                retRec.height = rec2.height - dyy;
            }
        }
        else
        {
            if (rec1.y <= rec2.y)
            {
                retRec.x = rec1.x;
                retRec.y = rec2.y;
                retRec.width = rec2.width - dxx;
                retRec.height = rec1.height - dyy;
            }
            else
            {
                retRec.x = rec1.x;
                retRec.y = rec1.y;
                retRec.width = rec2.width - dxx;
                retRec.height = rec2.height - dyy;
            }
        }

        if (rec1.width > rec2.width)
        {
            if (retRec.width >= rec2.width) retRec.width = rec2.width;
        }
        else
        {
            if (retRec.width >= rec1.width) retRec.width = rec1.width;
        }
        
        if (rec1.height > rec2.height)
        {
            if (retRec.height >= rec2.height) retRec.height = rec2.height;
        }
        else
        {
           if (retRec.height >= rec1.height) retRec.height = rec1.height;
        }
    }

    return retRec;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Cubic easing in-out 
// NOTE: Required for DrawLineBezier()
static float EaseCubicInOut(float t, float b, float c, float d) 
{ 
    if ((t /= 0.5*d) < 1)
        return 0.5*c*t*t*t + b;
    t -= 2;
    return 0.5*c*(t*t*t + 2) + b;
}

/* ??? textures */

/**********************************************************************************************
*
*   raylib.textures - Basic functions to load and draw Textures (2d)
*
*   CONFIGURATION:
*
*   #define SUPPORT_FILEFORMAT_BMP
*   #define SUPPORT_FILEFORMAT_PNG
*   #define SUPPORT_FILEFORMAT_TGA
*   #define SUPPORT_FILEFORMAT_JPG
*   #define SUPPORT_FILEFORMAT_GIF
*   #define SUPPORT_FILEFORMAT_PSD
*   #define SUPPORT_FILEFORMAT_HDR
*   #define SUPPORT_FILEFORMAT_DDS
*   #define SUPPORT_FILEFORMAT_PKM
*   #define SUPPORT_FILEFORMAT_KTX
*   #define SUPPORT_FILEFORMAT_PVR
*   #define SUPPORT_FILEFORMAT_ASTC
*       Selecte desired fileformats to be supported for image data loading. Some of those formats are 
*       supported by default, to remove support, just comment unrequired #define in this module
*
*   #define SUPPORT_IMAGE_MANIPULATION
*       Support multiple image editing functions to scale, adjust colors, flip, draw on images, crop...
*       If not defined only three image editing functions supported: ImageFormat(), ImageAlphaMask(), ImageToPOT()
*
*   #define SUPPORT_IMAGE_GENERATION
*       Support proedural image generation functionality (gradient, spot, perlin-noise, cellular)
*
*   DEPENDENCIES:
*       stb_image        - Multiple image formats loading (JPEG, PNG, BMP, TGA, PSD, GIF, PIC)
*                          NOTE: stb_image has been slightly modified to support Android platform.
*       stb_image_resize - Multiple image resize algorythms
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2018 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/


//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Nop...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
// It's lonely here...

//----------------------------------------------------------------------------------
// Other Modules Functions Declaration (required by text)
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
#if defined(SUPPORT_FILEFORMAT_DDS)
static Image LoadDDS(const char *fileName);   // Load DDS file
#endif
#if defined(SUPPORT_FILEFORMAT_PKM)
static Image LoadPKM(const char *fileName);   // Load PKM file
#endif
#if defined(SUPPORT_FILEFORMAT_KTX)
static Image LoadKTX(const char *fileName);   // Load KTX file
#endif
#if defined(SUPPORT_FILEFORMAT_PVR)
static Image LoadPVR(const char *fileName);   // Load PVR file
#endif
#if defined(SUPPORT_FILEFORMAT_ASTC)
static Image LoadASTC(const char *fileName);  // Load ASTC file
#endif

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Load image from file into CPU memory (RAM)
Image LoadImage(const char *fileName)
{
    Image image = { 0 };

    if ((IsFileExtension(fileName, ".png"))
#if defined(SUPPORT_FILEFORMAT_BMP)
        || (IsFileExtension(fileName, ".bmp"))
#endif
#if defined(SUPPORT_FILEFORMAT_TGA)
        || (IsFileExtension(fileName, ".tga"))
#endif
#if defined(SUPPORT_FILEFORMAT_JPG)
        || (IsFileExtension(fileName, ".jpg"))
#endif
#if defined(SUPPORT_FILEFORMAT_DDS)
        || (IsFileExtension(fileName, ".gif"))
#endif
#if defined(SUPPORT_FILEFORMAT_PSD)
        || (IsFileExtension(fileName, ".psd"))
#endif
       )
    {
        int imgWidth = 0;
        int imgHeight = 0;
        int imgBpp = 0;
        
        FILE *imFile = fopen(fileName, "rb");
        
        if (imFile != NULL)
        {
            // NOTE: Using stb_image to load images (Supports: BMP, TGA, PNG, JPG, ...)
            image.data = stbi_load_from_file(imFile, &imgWidth, &imgHeight, &imgBpp, 0);
            
            fclose(imFile);

            image.width = imgWidth;
            image.height = imgHeight;
            image.mipmaps = 1;

            if (imgBpp == 1) image.format = UNCOMPRESSED_GRAYSCALE;
            else if (imgBpp == 2) image.format = UNCOMPRESSED_GRAY_ALPHA;
            else if (imgBpp == 3) image.format = UNCOMPRESSED_R8G8B8;
            else if (imgBpp == 4) image.format = UNCOMPRESSED_R8G8B8A8;
        }
    }
#if defined(SUPPORT_FILEFORMAT_HDR)
    else if (IsFileExtension(fileName, ".hdr"))
    {
        int imgBpp = 0;
        
        FILE *imFile = fopen(fileName, "rb");
        
        stbi_set_flip_vertically_on_load(true);
        
        // Load 32 bit per channel floats data 
        image.data = stbi_loadf_from_file(imFile, &image.width, &image.height, &imgBpp, 0);
        
        stbi_set_flip_vertically_on_load(false);

        fclose(imFile);
        
        image.mipmaps = 1;
        
        if (imgBpp == 1) image.format = UNCOMPRESSED_R32;
        else if (imgBpp == 3) image.format = UNCOMPRESSED_R32G32B32;
        else if (imgBpp == 4) image.format = UNCOMPRESSED_R32G32B32A32;
        else 
        {
            TraceLog(LOG_WARNING, "[%s] Image fileformat not supported", fileName);
            UnloadImage(image);
        }
    }
#endif
#if defined(SUPPORT_FILEFORMAT_DDS)
    else if (IsFileExtension(fileName, ".dds")) image = LoadDDS(fileName);
#endif
#if defined(SUPPORT_FILEFORMAT_PKM)
    else if (IsFileExtension(fileName, ".pkm")) image = LoadPKM(fileName);
#endif
#if defined(SUPPORT_FILEFORMAT_KTX)
    else if (IsFileExtension(fileName, ".ktx")) image = LoadKTX(fileName);
#endif
#if defined(SUPPORT_FILEFORMAT_PVR)
    else if (IsFileExtension(fileName, ".pvr")) image = LoadPVR(fileName);
#endif
#if defined(SUPPORT_FILEFORMAT_ASTC)
    else if (IsFileExtension(fileName, ".astc")) image = LoadASTC(fileName);
#endif
    else TraceLog(LOG_WARNING, "[%s] Image fileformat not supported", fileName);

    if (image.data != NULL) TraceLog(LOG_INFO, "[%s] Image loaded successfully (%ix%i)", fileName, image.width, image.height);
    else TraceLog(LOG_WARNING, "[%s] Image could not be loaded", fileName);

    return image;
}

// Load image from Color array data (RGBA - 32bit)
// NOTE: Creates a copy of pixels data array
Image LoadImageEx(Color *pixels, int width, int height)
{
    Image image;
    image.data = NULL;
    image.width = width;
    image.height = height;
    image.mipmaps = 1;
    image.format = UNCOMPRESSED_R8G8B8A8;

    int k = 0;

    image.data = (unsigned char *)malloc(image.width*image.height*4*sizeof(unsigned char));

    for (int i = 0; i < image.width*image.height*4; i += 4)
    {
        ((unsigned char *)image.data)[i] = pixels[k].r;
        ((unsigned char *)image.data)[i + 1] = pixels[k].g;
        ((unsigned char *)image.data)[i + 2] = pixels[k].b;
        ((unsigned char *)image.data)[i + 3] = pixels[k].a;
        k++;
    }

    return image;
}

// Load image from raw data with parameters
// NOTE: This functions makes a copy of provided data
Image LoadImagePro(void *data, int width, int height, int format)
{
    Image srcImage = { 0 };

    srcImage.data = data;
    srcImage.width = width;
    srcImage.height = height;
    srcImage.mipmaps = 1;
    srcImage.format = format;

    Image dstImage = ImageCopy(srcImage);

    return dstImage;
}

// Load an image from RAW file data
Image LoadImageRaw(const char *fileName, int width, int height, int format, int headerSize)
{
    Image image = { 0 };

    FILE *rawFile = fopen(fileName, "rb");

    if (rawFile == NULL)
    {
        TraceLog(LOG_WARNING, "[%s] RAW image file could not be opened", fileName);
    }
    else
    {
        if (headerSize > 0) fseek(rawFile, headerSize, SEEK_SET);

        unsigned int size = GetPixelDataSize(width, height, format);
        
        image.data = malloc(size);      // Allocate required memory in bytes

        // NOTE: fread() returns num read elements instead of bytes, 
        // to get bytes we need to read (1 byte size, elements) instead of (x byte size, 1 element)
        int bytes = fread(image.data, 1, size, rawFile);

        // Check if data has been read successfully
        if (bytes < size)
        {
            TraceLog(LOG_WARNING, "[%s] RAW image data can not be read, wrong requested format or size", fileName);

            if (image.data != NULL) free(image.data);
        }
        else
        {
            image.width = width;
            image.height = height;
            image.mipmaps = 1;
            image.format = format;
        }

        fclose(rawFile);
    }

    return image;
}

// Load texture from file into GPU memory (VRAM)
Texture2D LoadTexture(const char *fileName)
{
    Texture2D texture = { 0 };

    Image image = LoadImage(fileName);

    if (image.data != NULL)
    {
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }
    else TraceLog(LOG_WARNING, "Texture could not be created");

    return texture;
}

// Load a texture from image data
// NOTE: image is not unloaded, it must be done manually
Texture2D LoadTextureFromImage(Image image)
{
    Texture2D texture = { 0 };

    texture.id = rlLoadTexture(image.data, image.width, image.height, image.format, image.mipmaps);

    texture.width = image.width;
    texture.height = image.height;
    texture.mipmaps = image.mipmaps;
    texture.format = image.format;

    return texture;
}

// Load texture for rendering (framebuffer)
RenderTexture2D LoadRenderTexture(int width, int height)
{
    RenderTexture2D target = rlLoadRenderTexture(width, height);

    return target;
}

// Unload image from CPU memory (RAM)
void UnloadImage(Image image)
{
    if (image.data != NULL) free(image.data);

    // NOTE: It becomes anoying every time a texture is loaded
    //TraceLog(LOG_INFO, "Unloaded image data");
}

// Unload texture from GPU memory (VRAM)
void UnloadTexture(Texture2D texture)
{
    if (texture.id > 0)
    {
        rlDeleteTextures(texture.id);

        TraceLog(LOG_INFO, "[TEX ID %i] Unloaded texture data from VRAM (GPU)", texture.id);
    }
}

// Unload render texture from GPU memory (VRAM)
void UnloadRenderTexture(RenderTexture2D target)
{
    if (target.id > 0) rlDeleteRenderTextures(target);
}

// Get pixel data from image in the form of Color struct array
// TODO: Support float pixel data retrieval
Color *GetImageData(Image image)
{
    Color *pixels = (Color *)malloc(image.width*image.height*sizeof(Color));

    for (int i = 0, k = 0; i < image.width*image.height; i++)
    {
        switch (image.format)
        {
            case UNCOMPRESSED_GRAYSCALE:
            {
                pixels[i].r = ((unsigned char *)image.data)[i];
                pixels[i].g = ((unsigned char *)image.data)[i];
                pixels[i].b = ((unsigned char *)image.data)[i];
                pixels[i].a = 255;
                
            } break;
            case UNCOMPRESSED_GRAY_ALPHA:
            {
                pixels[i].r = ((unsigned char *)image.data)[k];
                pixels[i].g = ((unsigned char *)image.data)[k];
                pixels[i].b = ((unsigned char *)image.data)[k];
                pixels[i].a = ((unsigned char *)image.data)[k + 1];

                k += 2;
            } break;
            case UNCOMPRESSED_R5G5B5A1:
            {
                unsigned short pixel = ((unsigned short *)image.data)[i];

                pixels[i].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)*(255/31));
                pixels[i].g = (unsigned char)((float)((pixel & 0b0000011111000000) >> 6)*(255/31));
                pixels[i].b = (unsigned char)((float)((pixel & 0b0000000000111110) >> 1)*(255/31));
                pixels[i].a = (unsigned char)((pixel & 0b0000000000000001)*255);

            } break;
            case UNCOMPRESSED_R5G6B5:
            {
                unsigned short pixel = ((unsigned short *)image.data)[i];

                pixels[i].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)*(255/31));
                pixels[i].g = (unsigned char)((float)((pixel & 0b0000011111100000) >> 5)*(255/63));
                pixels[i].b = (unsigned char)((float)(pixel & 0b0000000000011111)*(255/31));
                pixels[i].a = 255;

            } break;
            case UNCOMPRESSED_R4G4B4A4:
            {
                unsigned short pixel = ((unsigned short *)image.data)[i];

                pixels[i].r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12)*(255/15));
                pixels[i].g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8)*(255/15));
                pixels[i].b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4)*(255/15));
                pixels[i].a = (unsigned char)((float)(pixel & 0b0000000000001111)*(255/15));
                
            } break;
            case UNCOMPRESSED_R8G8B8A8:
            {
                pixels[i].r = ((unsigned char *)image.data)[k];
                pixels[i].g = ((unsigned char *)image.data)[k + 1];
                pixels[i].b = ((unsigned char *)image.data)[k + 2];
                pixels[i].a = ((unsigned char *)image.data)[k + 3];

                k += 4;
            } break;
            case UNCOMPRESSED_R8G8B8:
            {
                pixels[i].r = (unsigned char)((unsigned char *)image.data)[k];
                pixels[i].g = (unsigned char)((unsigned char *)image.data)[k + 1];
                pixels[i].b = (unsigned char)((unsigned char *)image.data)[k + 2];
                pixels[i].a = 255;

                k += 3;
            } break;
            default: TraceLog(LOG_WARNING, "Format not supported for pixel data retrieval"); break;
        }
    }

    return pixels;
}

// Get pixel data size in bytes (image or texture)
// NOTE: Size depends on pixel format
int GetPixelDataSize(int width, int height, int format)
{
    int dataSize = 0;       // Size in bytes
    int bpp = 0;            // Bits per pixel

    switch (format)
    {
        case UNCOMPRESSED_GRAYSCALE: bpp = 8; break;
        case UNCOMPRESSED_GRAY_ALPHA:
        case UNCOMPRESSED_R5G6B5:
        case UNCOMPRESSED_R5G5B5A1:
        case UNCOMPRESSED_R4G4B4A4: bpp = 16; break;
        case UNCOMPRESSED_R8G8B8A8: bpp = 32; break;
        case UNCOMPRESSED_R8G8B8: bpp = 24; break;
        case UNCOMPRESSED_R32: bpp = 32; break;
        case UNCOMPRESSED_R32G32B32: bpp = 32*3; break;
        case UNCOMPRESSED_R32G32B32A32: bpp = 32*4; break;
        case COMPRESSED_DXT1_RGB:
        case COMPRESSED_DXT1_RGBA: 
        case COMPRESSED_ETC1_RGB:
        case COMPRESSED_ETC2_RGB:
        case COMPRESSED_PVRT_RGB:
        case COMPRESSED_PVRT_RGBA: bpp = 4; break;
        case COMPRESSED_DXT3_RGBA:
        case COMPRESSED_DXT5_RGBA:
        case COMPRESSED_ETC2_EAC_RGBA:
        case COMPRESSED_ASTC_4x4_RGBA: bpp = 8; break;
        case COMPRESSED_ASTC_8x8_RGBA: bpp = 2; break;
        default: break;
    }

    dataSize = width*height*bpp/8;  // Total data size in bytes
    
    return dataSize;
}

// Get pixel data from GPU texture and return an Image
// NOTE: Compressed texture formats not supported
Image GetTextureData(Texture2D texture)
{
    Image image = { 0 };
    
    if (texture.format < 8)
    {
        image.data = rlReadTexturePixels(texture);

        if (image.data != NULL)
        {
            image.width = texture.width;
            image.height = texture.height;
            image.format = texture.format;
            image.mipmaps = 1;
            
            // NOTE: Data retrieved on OpenGL ES 2.0 should be RGBA
            // coming from FBO color buffer, but it seems original
            // texture format is retrieved on RPI... weird...
            //image.format = UNCOMPRESSED_R8G8B8A8;

            TraceLog(LOG_INFO, "Texture pixel data obtained successfully");
        }
        else TraceLog(LOG_WARNING, "Texture pixel data could not be obtained");
    }
    else TraceLog(LOG_WARNING, "Compressed texture data could not be obtained");

    return image;
}

// Update GPU texture with new data
// NOTE: pixels data must match texture.format
void UpdateTexture(Texture2D texture, const void *pixels)
{
    rlUpdateTexture(texture.id, texture.width, texture.height, texture.format, pixels);
}

// Save image to a PNG file
void SaveImageAs(const char *fileName, Image image)
{
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_RPI)
    // NOTE: Getting Color array as RGBA unsigned char values
    unsigned char *imgData = (unsigned char *)GetImageData(image);
    SavePNG(fileName, imgData, image.width, image.height, 4);
    free(imgData);

    TraceLog(LOG_INFO, "Image saved: %s", fileName);
#endif
}

// Copy an image to a new image
Image ImageCopy(Image image)
{
    Image newImage = { 0 };

    int width = image.width;
    int height = image.height;
    int size = 0;

    for (int i = 0; i < image.mipmaps; i++)
    {
        size += GetPixelDataSize(width, height, image.format);
        
        width /= 2;
        height /= 2;
        
        // Security check for NPOT textures
        if (width < 1) width = 1;
        if (height < 1) height = 1;
    }
    
    newImage.data = malloc(size);

    if (newImage.data != NULL)
    {
        // NOTE: Size must be provided in bytes
        memcpy(newImage.data, image.data, size);

        newImage.width = image.width;
        newImage.height = image.height;
        newImage.mipmaps = image.mipmaps;
        newImage.format = image.format;
    }

    return newImage;
}

// Convert image to POT (power-of-two)
// NOTE: It could be useful on OpenGL ES 2.0 (RPI, HTML5)
void ImageToPOT(Image *image, Color fillColor)
{
    Color *pixels = GetImageData(*image);   // Get pixels data

    // Calculate next power-of-two values
    // NOTE: Just add the required amount of pixels at the right and bottom sides of image...
    int potWidth = (int)powf(2, ceilf(logf((float)image->width)/logf(2)));
    int potHeight = (int)powf(2, ceilf(logf((float)image->height)/logf(2)));

    // Check if POT texture generation is required (if texture is not already POT)
    if ((potWidth != image->width) || (potHeight != image->height))
    {
        Color *pixelsPOT = NULL;

        // Generate POT array from NPOT data
        pixelsPOT = (Color *)malloc(potWidth*potHeight*sizeof(Color));

        for (int j = 0; j < potHeight; j++)
        {
            for (int i = 0; i < potWidth; i++)
            {
                if ((j < image->height) && (i < image->width)) pixelsPOT[j*potWidth + i] = pixels[j*image->width + i];
                else pixelsPOT[j*potWidth + i] = fillColor;
            }
        }

        TraceLog(LOG_WARNING, "Image converted to POT: (%ix%i) -> (%ix%i)", image->width, image->height, potWidth, potHeight);

        free(pixels);                       // Free pixels data
        free(image->data);                  // Free old image data

        int format = image->format;         // Store image data format to reconvert later

        // TODO: Image width and height changes... do we want to store new values or keep the old ones?
        // NOTE: Issues when using image.width and image.height for sprite animations...
        *image = LoadImageEx(pixelsPOT, potWidth, potHeight);

        free(pixelsPOT);                    // Free POT pixels data

        ImageFormat(image, format);  // Reconvert image to previous format
    }
}

// Convert image data to desired format
void ImageFormat(Image *image, int newFormat)
{
    if (image->format != newFormat)
    {
        if ((image->format < COMPRESSED_DXT1_RGB) && (newFormat < COMPRESSED_DXT1_RGB))
        {
            Color *pixels = GetImageData(*image);

            free(image->data);      // WARNING! We loose mipmaps data --> Regenerated at the end...

            image->format = newFormat;

            int k = 0;

            switch (image->format)
            {
                case UNCOMPRESSED_GRAYSCALE:
                {
                    image->data = (unsigned char *)malloc(image->width*image->height*sizeof(unsigned char));

                    for (int i = 0; i < image->width*image->height; i++)
                    {
                        ((unsigned char *)image->data)[i] = (unsigned char)((float)pixels[i].r*0.299f + (float)pixels[i].g*0.587f + (float)pixels[i].b*0.114f);
                    }

                } break;
                case UNCOMPRESSED_GRAY_ALPHA:
                {
                   image->data = (unsigned char *)malloc(image->width*image->height*2*sizeof(unsigned char));

                   for (int i = 0; i < image->width*image->height*2; i += 2, k++)
                    {
                        ((unsigned char *)image->data)[i] = (unsigned char)((float)pixels[k].r*0.299f + (float)pixels[k].g*0.587f + (float)pixels[k].b*0.114f);
                        ((unsigned char *)image->data)[i + 1] = pixels[k].a;
                    }

                } break;
                case UNCOMPRESSED_R5G6B5:
                {
                    image->data = (unsigned short *)malloc(image->width*image->height*sizeof(unsigned short));

                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;

                    for (int i = 0; i < image->width*image->height; i++)
                    {
                        r = (unsigned char)(round((float)pixels[i].r*31.0f/255));
                        g = (unsigned char)(round((float)pixels[i].g*63.0f/255));
                        b = (unsigned char)(round((float)pixels[i].b*31.0f/255));

                        ((unsigned short *)image->data)[i] = (unsigned short)r << 11 | (unsigned short)g << 5 | (unsigned short)b;
                    }

                } break;
                case UNCOMPRESSED_R8G8B8:
                {
                    image->data = (unsigned char *)malloc(image->width*image->height*3*sizeof(unsigned char));

                    for (int i = 0; i < image->width*image->height*3; i += 3, k++)
                    {
                        ((unsigned char *)image->data)[i] = pixels[k].r;
                        ((unsigned char *)image->data)[i + 1] = pixels[k].g;
                        ((unsigned char *)image->data)[i + 2] = pixels[k].b;
                    }
                } break;
                case UNCOMPRESSED_R5G5B5A1:
                {
                    #define ALPHA_THRESHOLD  50

                    image->data = (unsigned short *)malloc(image->width*image->height*sizeof(unsigned short));

                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;
                    unsigned char a = 0;

                    for (int i = 0; i < image->width*image->height; i++)
                    {
                        r = (unsigned char)(round((float)pixels[i].r*31.0f/255));
                        g = (unsigned char)(round((float)pixels[i].g*31.0f/255));
                        b = (unsigned char)(round((float)pixels[i].b*31.0f/255));
                        a = (pixels[i].a > ALPHA_THRESHOLD) ? 1 : 0;

                        ((unsigned short *)image->data)[i] = (unsigned short)r << 11 | (unsigned short)g << 6 | (unsigned short)b << 1 | (unsigned short)a;
                    }

                } break;
                case UNCOMPRESSED_R4G4B4A4:
                {
                    image->data = (unsigned short *)malloc(image->width*image->height*sizeof(unsigned short));

                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;
                    unsigned char a = 0;

                    for (int i = 0; i < image->width*image->height; i++)
                    {
                        r = (unsigned char)(round((float)pixels[i].r*15.0f/255));
                        g = (unsigned char)(round((float)pixels[i].g*15.0f/255));
                        b = (unsigned char)(round((float)pixels[i].b*15.0f/255));
                        a = (unsigned char)(round((float)pixels[i].a*15.0f/255));
                        
                        ((unsigned short *)image->data)[i] = (unsigned short)r << 12 | (unsigned short)g << 8 | (unsigned short)b << 4 | (unsigned short)a;
                    }

                } break;
                case UNCOMPRESSED_R8G8B8A8:
                {
                    image->data = (unsigned char *)malloc(image->width*image->height*4*sizeof(unsigned char));

                    for (int i = 0; i < image->width*image->height*3; i += 3, k++)
                    {
                        ((unsigned char *)image->data)[i] = pixels[k].r;
                        ((unsigned char *)image->data)[i + 1] = pixels[k].g;
                        ((unsigned char *)image->data)[i + 2] = pixels[k].b;
                        ((unsigned char *)image->data)[i + 3] = pixels[k].a;
                    }
                } break;
                case UNCOMPRESSED_R32:
                {
                    image->data = (float *)malloc(image->width*image->height*sizeof(float));
                    
                    for (int i = 0; i < image->width*image->height; i++)
                    {
                        ((float *)image->data)[i] = (float)((float)pixels[i].r*0.299f/255.0f + (float)pixels[i].g*0.587f/255.0f + (float)pixels[i].b*0.114f/255.0f);
                    }
                } break;
                case UNCOMPRESSED_R32G32B32:
                {
                    image->data = (float *)malloc(image->width*image->height*3*sizeof(float));

                    for (int i = 0; i < image->width*image->height*3; i += 3, k++)
                    {
                        ((float *)image->data)[i] = (float)pixels[k].r/255.0f;
                        ((float *)image->data)[i + 1] = (float)pixels[k].g/255.0f;
                        ((float *)image->data)[i + 2] = (float)pixels[k].b/255.0f;
                    }
                } break;
                case UNCOMPRESSED_R32G32B32A32:
                {
                    image->data = (float *)malloc(image->width*image->height*4*sizeof(float));

                    for (int i = 0; i < image->width*image->height*4; i += 4, k++)
                    {
                        ((float *)image->data)[i] = (float)pixels[k].r/255.0f;
                        ((float *)image->data)[i + 1] = (float)pixels[k].g/255.0f;
                        ((float *)image->data)[i + 2] = (float)pixels[k].b/255.0f;
                        ((float *)image->data)[i + 3] = (float)pixels[k].a/255.0f;
                    }
                } break;
                default: break;
            }

            free(pixels);
            
            // In case original image had mipmaps, generate mipmaps for formated image
            // NOTE: Original mipmaps are replaced by new ones, if custom mipmaps were used, they are lost
            if (image->mipmaps > 1) 
            {
                image->mipmaps = 1;
                ImageMipmaps(image);
            }
        }
        else TraceLog(LOG_WARNING, "Image data format is compressed, can not be converted");
    }
}

// Apply alpha mask to image
// NOTE 1: Returned image is GRAY_ALPHA (16bit) or RGBA (32bit)
// NOTE 2: alphaMask should be same size as image
void ImageAlphaMask(Image *image, Image alphaMask)
{
    if ((image->width != alphaMask.width) || (image->height != alphaMask.height))
    {
        TraceLog(LOG_WARNING, "Alpha mask must be same size as image");
    }
    else if (image->format >= COMPRESSED_DXT1_RGB)
    {
        TraceLog(LOG_WARNING, "Alpha mask can not be applied to compressed data formats");
    }
    else
    {
        // Force mask to be Grayscale
        Image mask = ImageCopy(alphaMask);
        if (mask.format != UNCOMPRESSED_GRAYSCALE) ImageFormat(&mask, UNCOMPRESSED_GRAYSCALE);

        // In case image is only grayscale, we just add alpha channel
        if (image->format == UNCOMPRESSED_GRAYSCALE)
        {
            ImageFormat(image, UNCOMPRESSED_GRAY_ALPHA);

            // Apply alpha mask to alpha channel
            for (int i = 0, k = 1; (i < mask.width*mask.height) || (i < image->width*image->height); i++, k += 2)
            {
                ((unsigned char *)image->data)[k] = ((unsigned char *)mask.data)[i];
            }
        }
        else
        {
            // Convert image to RGBA
            if (image->format != UNCOMPRESSED_R8G8B8A8) ImageFormat(image, UNCOMPRESSED_R8G8B8A8);

            // Apply alpha mask to alpha channel
            for (int i = 0, k = 3; (i < mask.width*mask.height) || (i < image->width*image->height); i++, k += 4)
            {
                ((unsigned char *)image->data)[k] = ((unsigned char *)mask.data)[i];
            }
        }

        UnloadImage(mask);
    }
}

// Clear alpha channel to desired color
// NOTE: Threshold defines the alpha limit, 0.0f to 1.0f
void ImageAlphaClear(Image *image, Color color, float threshold)
{
    Color *pixels = GetImageData(*image);
    
    for (int i = 0; i < image->width*image->height; i++) if (pixels[i].a <= (unsigned char)(threshold*255.0f)) pixels[i] = color;

    UnloadImage(*image);
    
    int prevFormat = image->format;
    *image = LoadImageEx(pixels, image->width, image->height);
    
    ImageFormat(image, prevFormat);
}

// Crop image depending on alpha value
void ImageAlphaCrop(Image *image, float threshold)
{
    Rectangle crop = { 0 };
    
    Color *pixels = GetImageData(*image);
    
    int minx = 0;
    int miny = 0;

    for (int i = 0; i < image->width*image->height; i++) 
    {
        if (pixels[i].a > (unsigned char)(threshold*255.0f))
        {
            minx = i%image->width;
            miny = -(-((i/image->width) + 1) + 1);

            if (crop.y == 0) crop.y = miny;

            if (crop.x == 0) crop.x = minx;
            else if (minx < crop.x) crop.x = minx;

            if (crop.width == 0) crop.width = minx; 
            else if (crop.width < minx) crop.width = minx;

            if (crop.height == 0) crop.height = miny;
            else if (crop.height < miny) crop.height = miny;
        }
    }
    
    crop.width -= (crop.x - 1);
    crop.height -= (crop.y - 1);
    
    TraceLog(LOG_INFO, "Crop rectangle: (%i, %i, %i, %i)", crop.x, crop.y, crop.width, crop.height);
    
    free(pixels);
    
    // NOTE: Added this weird check to avoid additional 1px crop to
    // image data that has already been cropped...
    if ((crop.x != 1) && 
        (crop.y != 1) && 
        (crop.width != image->width - 1) &&
        (crop.height != image->height - 1)) ImageCrop(image, crop);
}

// Premultiply alpha channel
void ImageAlphaPremultiply(Image *image)
{
    float alpha = 0.0f;
    Color *pixels = GetImageData(*image);
    
    for (int i = 0; i < image->width*image->height; i++) 
    {
        alpha = (float)pixels[i].a/255.0f;
        pixels[i].r = (unsigned char)((float)pixels[i].r*alpha);
        pixels[i].g = (unsigned char)((float)pixels[i].g*alpha);
        pixels[i].b = (unsigned char)((float)pixels[i].b*alpha);
    }

    UnloadImage(*image);
    
    int prevFormat = image->format;
    *image = LoadImageEx(pixels, image->width, image->height);
    
    ImageFormat(image, prevFormat);
}



#if defined(SUPPORT_IMAGE_MANIPULATION)
// Crop an image to area defined by a rectangle
// NOTE: Security checks are performed in case rectangle goes out of bounds
void ImageCrop(Image *image, Rectangle crop)
{
    // Security checks to make sure cropping rectangle is inside margins
    if ((crop.x + crop.width) > image->width)
    {
        crop.width = image->width - crop.x;
        TraceLog(LOG_WARNING, "Crop rectangle width out of bounds, rescaled crop width: %i", crop.width);
    }

    if ((crop.y + crop.height) > image->height)
    {
        crop.height = image->height - crop.y;
        TraceLog(LOG_WARNING, "Crop rectangle height out of bounds, rescaled crop height: %i", crop.height);
    }

    if ((crop.x < image->width) && (crop.y < image->height))
    {
        // Start the cropping process
        Color *pixels = GetImageData(*image);   // Get data as Color pixels array
        Color *cropPixels = (Color *)malloc(crop.width*crop.height*sizeof(Color));

        for (int j = crop.y; j < (crop.y + crop.height); j++)
        {
            for (int i = crop.x; i < (crop.x + crop.width); i++)
            {
                cropPixels[(j - crop.y)*crop.width + (i - crop.x)] = pixels[j*image->width + i];
            }
        }

        free(pixels);

        int format = image->format;

        UnloadImage(*image);

        *image = LoadImageEx(cropPixels, crop.width, crop.height);

        free(cropPixels);

        // Reformat 32bit RGBA image to original format
        ImageFormat(image, format);
    }
    else
    {
        TraceLog(LOG_WARNING, "Image can not be cropped, crop rectangle out of bounds");
    }
}

// Resize and image to new size
// NOTE: Uses stb default scaling filters (both bicubic):
// STBIR_DEFAULT_FILTER_UPSAMPLE    STBIR_FILTER_CATMULLROM
// STBIR_DEFAULT_FILTER_DOWNSAMPLE  STBIR_FILTER_MITCHELL   (high-quality Catmull-Rom)
void ImageResize(Image *image, int newWidth, int newHeight)
{
    // Get data as Color pixels array to work with it
    Color *pixels = GetImageData(*image);
    Color *output = (Color *)malloc(newWidth*newHeight*sizeof(Color));

    // NOTE: Color data is casted to (unsigned char *), there shouldn't been any problem...
    stbir_resize_uint8((unsigned char *)pixels, image->width, image->height, 0, (unsigned char *)output, newWidth, newHeight, 0, 4);

    int format = image->format;

    UnloadImage(*image);

    *image = LoadImageEx(output, newWidth, newHeight);
    ImageFormat(image, format);  // Reformat 32bit RGBA image to original format

    free(output);
    free(pixels);
}

// Resize and image to new size using Nearest-Neighbor scaling algorithm
void ImageResizeNN(Image *image,int newWidth,int newHeight)
{
    Color *pixels = GetImageData(*image);
    Color *output = (Color *)malloc(newWidth*newHeight*sizeof(Color));

    // EDIT: added +1 to account for an early rounding problem
    int xRatio = (int)((image->width << 16)/newWidth) + 1;
    int yRatio = (int)((image->height << 16)/newHeight) + 1;

    int x2, y2;
    for (int y = 0; y < newHeight; y++)
    {
        for (int x = 0; x < newWidth; x++)
        {
            x2 = ((x*xRatio) >> 16);
            y2 = ((y*yRatio) >> 16);

            output[(y*newWidth) + x] = pixels[(y2*image->width) + x2] ;
        }
    }

    int format = image->format;

    UnloadImage(*image);

    *image = LoadImageEx(output, newWidth, newHeight);
    ImageFormat(image, format);  // Reformat 32bit RGBA image to original format

    free(output);
    free(pixels);
}

// Generate all mipmap levels for a provided image
// NOTE 1: Supports POT and NPOT images
// NOTE 2: image.data is scaled to include mipmap levels
// NOTE 3: Mipmaps format is the same as base image
void ImageMipmaps(Image *image)
{
    int mipCount = 1;                   // Required mipmap levels count (including base level)
    int mipWidth = image->width;        // Base image width
    int mipHeight = image->height;      // Base image height
    int mipSize = GetPixelDataSize(mipWidth, mipHeight, image->format);  // Image data size (in bytes)

    // Count mipmap levels required
    while ((mipWidth != 1) || (mipHeight != 1))
    {
        if (mipWidth != 1) mipWidth /= 2;
        if (mipHeight != 1) mipHeight /= 2;
        
        // Security check for NPOT textures
        if (mipWidth < 1) mipWidth = 1;
        if (mipHeight < 1) mipHeight = 1;

        TraceLog(LOG_DEBUG, "Next mipmap level: %i x %i - current size %i", mipWidth, mipHeight, mipSize);

        mipCount++;
        mipSize += GetPixelDataSize(mipWidth, mipHeight, image->format);       // Add mipmap size (in bytes)
    }

    TraceLog(LOG_DEBUG, "Mipmaps available: %i - Mipmaps required: %i", image->mipmaps, mipCount);
    TraceLog(LOG_DEBUG, "Mipmaps total size required: %i", mipSize);
    TraceLog(LOG_DEBUG, "Image data memory start address: 0x%x", image->data);

    if (image->mipmaps < mipCount)
    {
        void *temp = realloc(image->data, mipSize);
        
        if (temp != NULL) 
        {
            image->data = temp;      // Assign new pointer (new size) to store mipmaps data
            TraceLog(LOG_DEBUG, "Image data memory point reallocated: 0x%x", temp);
        }
        else TraceLog(LOG_WARNING, "Mipmaps required memory could not be allocated");

        // Pointer to allocated memory point where store next mipmap level data
        unsigned char *nextmip = (unsigned char *)image->data + GetPixelDataSize(image->width, image->height, image->format);
        
        mipWidth = image->width/2;
        mipHeight = image->height/2;
        mipSize = GetPixelDataSize(mipWidth, mipHeight, image->format);
        Image imCopy = ImageCopy(*image);
        
        for (int i = 1; i < mipCount; i++)
        {
            TraceLog(LOG_DEBUG, "Gen mipmap level: %i (%i x %i) - size: %i - offset: 0x%x", i, mipWidth, mipHeight, mipSize, nextmip);
            
            ImageResize(&imCopy, mipWidth, mipHeight);  // Uses internally Mitchell cubic downscale filter

            memcpy(nextmip, imCopy.data, mipSize);
            nextmip += mipSize;
            image->mipmaps++;
            
            mipWidth /= 2;
            mipHeight /= 2;
            
            // Security check for NPOT textures
            if (mipWidth < 1) mipWidth = 1;
            if (mipHeight < 1) mipHeight = 1;
            
            mipSize = GetPixelDataSize(mipWidth, mipHeight, image->format);
        }

        UnloadImage(imCopy);
    }
    else TraceLog(LOG_WARNING, "Image mipmaps already available");
}

// Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
// NOTE: In case selected bpp do not represent an known 16bit format,
// dithered data is stored in the LSB part of the unsigned short
void ImageDither(Image *image, int rBpp, int gBpp, int bBpp, int aBpp)
{
    if (image->format >= COMPRESSED_DXT1_RGB)
    {
        TraceLog(LOG_WARNING, "Compressed data formats can not be dithered");
        return;
    }

    if ((rBpp+gBpp+bBpp+aBpp) > 16)
    {
        TraceLog(LOG_WARNING, "Unsupported dithering bpps (%ibpp), only 16bpp or lower modes supported", (rBpp+gBpp+bBpp+aBpp));
    }
    else
    {
        Color *pixels = GetImageData(*image);

        free(image->data);      // free old image data

        if ((image->format != UNCOMPRESSED_R8G8B8) && (image->format != UNCOMPRESSED_R8G8B8A8))
        {
            TraceLog(LOG_WARNING, "Image format is already 16bpp or lower, dithering could have no effect");
        }

        // Define new image format, check if desired bpp match internal known format
        if ((rBpp == 5) && (gBpp == 6) && (bBpp == 5) && (aBpp == 0)) image->format = UNCOMPRESSED_R5G6B5;
        else if ((rBpp == 5) && (gBpp == 5) && (bBpp == 5) && (aBpp == 1)) image->format = UNCOMPRESSED_R5G5B5A1;
        else if ((rBpp == 4) && (gBpp == 4) && (bBpp == 4) && (aBpp == 4)) image->format = UNCOMPRESSED_R4G4B4A4;
        else
        {
            image->format = 0;
            TraceLog(LOG_WARNING, "Unsupported dithered OpenGL internal format: %ibpp (R%iG%iB%iA%i)", (rBpp+gBpp+bBpp+aBpp), rBpp, gBpp, bBpp, aBpp);
        }

        // NOTE: We will store the dithered data as unsigned short (16bpp)
        image->data = (unsigned short *)malloc(image->width*image->height*sizeof(unsigned short));

        Color oldPixel = WHITE;
        Color newPixel = WHITE;

        int rError, gError, bError;
        unsigned short rPixel, gPixel, bPixel, aPixel;   // Used for 16bit pixel composition

        #define MIN(a,b) (((a)<(b))?(a):(b))

        for (int y = 0; y < image->height; y++)
        {
            for (int x = 0; x < image->width; x++)
            {
                oldPixel = pixels[y*image->width + x];

                // NOTE: New pixel obtained by bits truncate, it would be better to round values (check ImageFormat())
                newPixel.r = oldPixel.r >> (8 - rBpp);     // R bits
                newPixel.g = oldPixel.g >> (8 - gBpp);     // G bits
                newPixel.b = oldPixel.b >> (8 - bBpp);     // B bits
                newPixel.a = oldPixel.a >> (8 - aBpp);     // A bits (not used on dithering)

                // NOTE: Error must be computed between new and old pixel but using same number of bits!
                // We want to know how much color precision we have lost...
                rError = (int)oldPixel.r - (int)(newPixel.r << (8 - rBpp));
                gError = (int)oldPixel.g - (int)(newPixel.g << (8 - gBpp));
                bError = (int)oldPixel.b - (int)(newPixel.b << (8 - bBpp));

                pixels[y*image->width + x] = newPixel;

                // NOTE: Some cases are out of the array and should be ignored
                if (x < (image->width - 1))
                {
                    pixels[y*image->width + x+1].r = MIN((int)pixels[y*image->width + x+1].r + (int)((float)rError*7.0f/16), 0xff);
                    pixels[y*image->width + x+1].g = MIN((int)pixels[y*image->width + x+1].g + (int)((float)gError*7.0f/16), 0xff);
                    pixels[y*image->width + x+1].b = MIN((int)pixels[y*image->width + x+1].b + (int)((float)bError*7.0f/16), 0xff);
                }

                if ((x > 0) && (y < (image->height - 1)))
                {
                    pixels[(y+1)*image->width + x-1].r = MIN((int)pixels[(y+1)*image->width + x-1].r + (int)((float)rError*3.0f/16), 0xff);
                    pixels[(y+1)*image->width + x-1].g = MIN((int)pixels[(y+1)*image->width + x-1].g + (int)((float)gError*3.0f/16), 0xff);
                    pixels[(y+1)*image->width + x-1].b = MIN((int)pixels[(y+1)*image->width + x-1].b + (int)((float)bError*3.0f/16), 0xff);
                }

                if (y < (image->height - 1))
                {
                    pixels[(y+1)*image->width + x].r = MIN((int)pixels[(y+1)*image->width + x].r + (int)((float)rError*5.0f/16), 0xff);
                    pixels[(y+1)*image->width + x].g = MIN((int)pixels[(y+1)*image->width + x].g + (int)((float)gError*5.0f/16), 0xff);
                    pixels[(y+1)*image->width + x].b = MIN((int)pixels[(y+1)*image->width + x].b + (int)((float)bError*5.0f/16), 0xff);
                }

                if ((x < (image->width - 1)) && (y < (image->height - 1)))
                {
                    pixels[(y+1)*image->width + x+1].r = MIN((int)pixels[(y+1)*image->width + x+1].r + (int)((float)rError*1.0f/16), 0xff);
                    pixels[(y+1)*image->width + x+1].g = MIN((int)pixels[(y+1)*image->width + x+1].g + (int)((float)gError*1.0f/16), 0xff);
                    pixels[(y+1)*image->width + x+1].b = MIN((int)pixels[(y+1)*image->width + x+1].b + (int)((float)bError*1.0f/16), 0xff);
                }

                rPixel = (unsigned short)newPixel.r;
                gPixel = (unsigned short)newPixel.g;
                bPixel = (unsigned short)newPixel.b;
                aPixel = (unsigned short)newPixel.a;

                ((unsigned short *)image->data)[y*image->width + x] = (rPixel << (gBpp + bBpp + aBpp)) | (gPixel << (bBpp + aBpp)) | (bPixel << aBpp) | aPixel;
            }
        }

        free(pixels);
    }
}

// Draw an image (source) within an image (destination)
// TODO: Feel this function could be simplified...
void ImageDraw(Image *dst, Image src, Rectangle srcRec, Rectangle dstRec)
{
    bool cropRequired = false;

    // Security checks to avoid size and rectangle issues (out of bounds)
    // Check that srcRec is inside src image
    if (srcRec.x < 0) srcRec.x = 0;
    if (srcRec.y < 0) srcRec.y = 0;

    if ((srcRec.x + srcRec.width) > src.width)
    {
        srcRec.width = src.width - srcRec.x;
        TraceLog(LOG_WARNING, "Source rectangle width out of bounds, rescaled width: %i", srcRec.width);
    }

    if ((srcRec.y + srcRec.height) > src.height)
    {
        srcRec.height = src.height - srcRec.y;
        TraceLog(LOG_WARNING, "Source rectangle height out of bounds, rescaled height: %i", srcRec.height);
        cropRequired = true;
    }

    Image srcCopy = ImageCopy(src);     // Make a copy of source image to work with it
    ImageCrop(&srcCopy, srcRec);        // Crop source image to desired source rectangle

    // Check that dstRec is inside dst image
    // TODO: Allow negative position within destination with cropping
    if (dstRec.x < 0) dstRec.x = 0;
    if (dstRec.y < 0) dstRec.y = 0;

    // Scale source image in case destination rec size is different than source rec size
    if ((dstRec.width != srcRec.width) || (dstRec.height != srcRec.height))
    {
        ImageResize(&srcCopy, dstRec.width, dstRec.height);
    }

    if ((dstRec.x + dstRec.width) > dst->width)
    {
        dstRec.width = dst->width - dstRec.x;
        TraceLog(LOG_WARNING, "Destination rectangle width out of bounds, rescaled width: %i", dstRec.width);
        cropRequired = true;
    }

    if ((dstRec.y + dstRec.height) > dst->height)
    {
        dstRec.height = dst->height - dstRec.y;
        TraceLog(LOG_WARNING, "Destination rectangle height out of bounds, rescaled height: %i", dstRec.height);
        cropRequired = true;
    }

    if (cropRequired)
    {
        // Crop destination rectangle if out of bounds
        Rectangle crop = { 0, 0, dstRec.width, dstRec.height };
        ImageCrop(&srcCopy, crop);
    }

    // Get image data as Color pixels array to work with it
    Color *dstPixels = GetImageData(*dst);
    Color *srcPixels = GetImageData(srcCopy);

    UnloadImage(srcCopy);       // Source copy not required any more...

    Color srcCol, dstCol;

    // Blit pixels, copy source image into destination
    // TODO: Probably out-of-bounds blitting could be considered here instead of so much cropping...
    for (int j = dstRec.y; j < (dstRec.y + dstRec.height); j++)
    {
        for (int i = dstRec.x; i < (dstRec.x + dstRec.width); i++)
        {
            // Alpha blending implementation
            dstCol = dstPixels[j*dst->width + i];
            srcCol = srcPixels[(j - dstRec.y)*dstRec.width + (i - dstRec.x)];

            dstCol.r = ((srcCol.a*(srcCol.r - dstCol.r)) >> 8) + dstCol.r;
            dstCol.g = ((srcCol.a*(srcCol.g - dstCol.g)) >> 8) + dstCol.g;
            dstCol.b = ((srcCol.a*(srcCol.b - dstCol.b)) >> 8) + dstCol.b;
            dstCol.a = ((srcCol.a*(srcCol.a - dstCol.a)) >> 8) + dstCol.a;

            dstPixels[j*dst->width + i] = dstCol;

            // TODO: Support other blending options
        }
    }

    UnloadImage(*dst);  // NOTE: Only dst->data is unloaded

    *dst = LoadImageEx(dstPixels, dst->width, dst->height);
    ImageFormat(dst, dst->format);

    free(srcPixels);
    free(dstPixels);
}

// Create an image from text (default font)
Image ImageText(const char *text, int fontSize, Color color)
{
    int defaultFontSize = 10;   // Default Font chars height in pixel
    if (fontSize < defaultFontSize) fontSize = defaultFontSize;
    int spacing = fontSize/defaultFontSize;

    Image imText = ImageTextEx(GetDefaultFont(), text, (float)fontSize, spacing, color);

    return imText;
}

// Create an image from text (custom sprite font)
Image ImageTextEx(SpriteFont font, const char *text, float fontSize, int spacing, Color tint)
{
    int length = strlen(text);
    int posX = 0;

    Vector2 imSize = MeasureTextEx(font, text, font.baseSize, spacing);
    
    TraceLog(LOG_DEBUG, "Text Image size: %f, %f", imSize.x, imSize.y);

    // NOTE: glGetTexImage() not available in OpenGL ES
    Image imFont = GetTextureData(font.texture);
    
    ImageColorTint(&imFont, tint);                    // Apply color tint to font

    // Create image to store text
    Image imText = GenImageColor((int)imSize.x, (int)imSize.y, BLANK);

    for (int i = 0; i < length; i++)
    {
        CharInfo letter = font.chars[(int)text[i] - 32];
        
        ImageDraw(&imText, imFont, letter.rec, (Rectangle){ posX + letter.offsetX, 
                  letter.offsetY, letter.rec.width, letter.rec.height });

        if (letter.advanceX == 0) posX += letter.rec.width + spacing;
        else posX += letter.advanceX + spacing;
    }

    UnloadImage(imFont);

    // Scale image depending on text size
    if (fontSize > imSize.y)
    {
        float scaleFactor = fontSize/imSize.y;
        TraceLog(LOG_INFO, "Image text scaled by factor: %f", scaleFactor);

        // Using nearest-neighbor scaling algorithm for default font
        if (font.texture.id == GetDefaultFont().texture.id) ImageResizeNN(&imText, (int)(imSize.x*scaleFactor), (int)(imSize.y*scaleFactor));
        else ImageResize(&imText, (int)(imSize.x*scaleFactor), (int)(imSize.y*scaleFactor));
    }

    return imText;
}

// Draw text (default font) within an image (destination)
void ImageDrawText(Image *dst, Vector2 position, const char *text, int fontSize, Color color)
{
    // NOTE: For default font, sapcing is set to desired font size / default font size (10)
    ImageDrawTextEx(dst, position, GetDefaultFont(), text, (float)fontSize, fontSize/10, color);
}

// Draw text (custom sprite font) within an image (destination)
void ImageDrawTextEx(Image *dst, Vector2 position, SpriteFont font, const char *text, float fontSize, int spacing, Color color)
{
    Image imText = ImageTextEx(font, text, fontSize, spacing, color);

    Rectangle srcRec = { 0, 0, imText.width, imText.height };
    Rectangle dstRec = { (int)position.x, (int)position.y, imText.width, imText.height };

    ImageDraw(dst, imText, srcRec, dstRec);

    UnloadImage(imText);
}

// Flip image vertically
void ImageFlipVertical(Image *image)
{
    Color *srcPixels = GetImageData(*image);
    Color *dstPixels = (Color *)malloc(sizeof(Color)*image->width*image->height);

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            dstPixels[y*image->width + x] = srcPixels[(image->height - 1 - y)*image->width + x];
        }
    }

    Image processed = LoadImageEx(dstPixels, image->width, image->height);
    ImageFormat(&processed, image->format);
    UnloadImage(*image);

    free(srcPixels);
    free(dstPixels);

    image->data = processed.data;
}

// Flip image horizontally
void ImageFlipHorizontal(Image *image)
{
    Color *srcPixels = GetImageData(*image);
    Color *dstPixels = (Color *)malloc(sizeof(Color)*image->width*image->height);

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            dstPixels[y*image->width + x] = srcPixels[y*image->width + (image->width - 1 - x)];
        }
    }

    Image processed = LoadImageEx(dstPixels, image->width, image->height);
    ImageFormat(&processed, image->format);
    UnloadImage(*image);

    free(srcPixels);
    free(dstPixels);

    image->data = processed.data;
}

// Modify image color: tint
void ImageColorTint(Image *image, Color color)
{
    Color *pixels = GetImageData(*image);

    float cR = (float)color.r/255;
    float cG = (float)color.g/255;
    float cB = (float)color.b/255;
    float cA = (float)color.a/255;

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            unsigned char r = 255*((float)pixels[y*image->width + x].r/255*cR);
            unsigned char g = 255*((float)pixels[y*image->width + x].g/255*cG);
            unsigned char b = 255*((float)pixels[y*image->width + x].b/255*cB);
            unsigned char a = 255*((float)pixels[y*image->width + x].a/255*cA);

            pixels[y*image->width + x].r = r;
            pixels[y*image->width + x].g = g;
            pixels[y*image->width + x].b = b;
            pixels[y*image->width + x].a = a;
        }
    }

    Image processed = LoadImageEx(pixels, image->width, image->height);
    ImageFormat(&processed, image->format);
    UnloadImage(*image);
    free(pixels);

    image->data = processed.data;
}

// Modify image color: invert
void ImageColorInvert(Image *image)
{
    Color *pixels = GetImageData(*image);

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            pixels[y*image->width + x].r = 255 - pixels[y*image->width + x].r;
            pixels[y*image->width + x].g = 255 - pixels[y*image->width + x].g;
            pixels[y*image->width + x].b = 255 - pixels[y*image->width + x].b;
        }
    }

    Image processed = LoadImageEx(pixels, image->width, image->height);
    ImageFormat(&processed, image->format);
    UnloadImage(*image);
    free(pixels);

    image->data = processed.data;
}

// Modify image color: grayscale
void ImageColorGrayscale(Image *image)
{
    ImageFormat(image, UNCOMPRESSED_GRAYSCALE);
}

// Modify image color: contrast
// NOTE: Contrast values between -100 and 100
void ImageColorContrast(Image *image, float contrast)
{
    if (contrast < -100) contrast = -100;
    if (contrast > 100) contrast = 100;

    contrast = (100.0f + contrast)/100.0f;
    contrast *= contrast;

    Color *pixels = GetImageData(*image);

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            float pR = (float)pixels[y*image->width + x].r/255.0f;
            pR -= 0.5;
            pR *= contrast;
            pR += 0.5;
            pR *= 255;
            if (pR < 0) pR = 0;
            if (pR > 255) pR = 255;

            float pG = (float)pixels[y*image->width + x].g/255.0f;
            pG -= 0.5;
            pG *= contrast;
            pG += 0.5;
            pG *= 255;
            if (pG < 0) pG = 0;
            if (pG > 255) pG = 255;

            float pB = (float)pixels[y*image->width + x].b/255.0f;
            pB -= 0.5;
            pB *= contrast;
            pB += 0.5;
            pB *= 255;
            if (pB < 0) pB = 0;
            if (pB > 255) pB = 255;

            pixels[y*image->width + x].r = (unsigned char)pR;
            pixels[y*image->width + x].g = (unsigned char)pG;
            pixels[y*image->width + x].b = (unsigned char)pB;
        }
    }

    Image processed = LoadImageEx(pixels, image->width, image->height);
    ImageFormat(&processed, image->format);
    UnloadImage(*image);
    free(pixels);

    image->data = processed.data;
}

// Modify image color: brightness
// NOTE: Brightness values between -255 and 255
void ImageColorBrightness(Image *image, int brightness)
{
    if (brightness < -255) brightness = -255;
    if (brightness > 255) brightness = 255;

    Color *pixels = GetImageData(*image);

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            int cR = pixels[y*image->width + x].r + brightness;
            int cG = pixels[y*image->width + x].g + brightness;
            int cB = pixels[y*image->width + x].b + brightness;

            if (cR < 0) cR = 1;
            if (cR > 255) cR = 255;

            if (cG < 0) cG = 1;
            if (cG > 255) cG = 255;

            if (cB < 0) cB = 1;
            if (cB > 255) cB = 255;

            pixels[y*image->width + x].r = (unsigned char)cR;
            pixels[y*image->width + x].g = (unsigned char)cG;
            pixels[y*image->width + x].b = (unsigned char)cB;
        }
    }

    Image processed = LoadImageEx(pixels, image->width, image->height);
    ImageFormat(&processed, image->format);
    UnloadImage(*image);
    free(pixels);

    image->data = processed.data;
}
#endif      // SUPPORT_IMAGE_MANIPULATION

#if defined(SUPPORT_IMAGE_GENERATION)
// Generate image: plain color
Image GenImageColor(int width, int height, Color color)
{
    Color *pixels = (Color *)calloc(width*height, sizeof(Color));
    
    for (int i = 0; i < width*height; i++) pixels[i] = color;
    
    Image image = LoadImageEx(pixels, width, height);
    
    free(pixels);
    
    return image;
}

// Generate image: vertical gradient
Image GenImageGradientV(int width, int height, Color top, Color bottom)
{
    Color *pixels = (Color *)malloc(width*height*sizeof(Color));

    for (int j = 0; j < height; j++)
    {
        float factor = (float)j/(float)height;
        for (int i = 0; i < width; i++)
        {
            pixels[j*width + i].r = (int)((float)bottom.r*factor + (float)top.r*(1.f - factor));
            pixels[j*width + i].g = (int)((float)bottom.g*factor + (float)top.g*(1.f - factor));
            pixels[j*width + i].b = (int)((float)bottom.b*factor + (float)top.b*(1.f - factor));
            pixels[j*width + i].a = (int)((float)bottom.a*factor + (float)top.a*(1.f - factor));
        }
    }

    Image image = LoadImageEx(pixels, width, height);
    free(pixels);

    return image;
}

// Generate image: horizontal gradient
Image GenImageGradientH(int width, int height, Color left, Color right)
{
    Color *pixels = (Color *)malloc(width*height*sizeof(Color));

    for (int i = 0; i < width; i++)
    {
        float factor = (float)i/(float)width;
        for (int j = 0; j < height; j++)
        {
            pixels[j*width + i].r = (int)((float)right.r*factor + (float)left.r*(1.f - factor));
            pixels[j*width + i].g = (int)((float)right.g*factor + (float)left.g*(1.f - factor));
            pixels[j*width + i].b = (int)((float)right.b*factor + (float)left.b*(1.f - factor));
            pixels[j*width + i].a = (int)((float)right.a*factor + (float)left.a*(1.f - factor));
        }
    }

    Image image = LoadImageEx(pixels, width, height);
    free(pixels);

    return image;
}

// Generate image: radial gradient
Image GenImageGradientRadial(int width, int height, float density, Color inner, Color outer)
{
    Color *pixels = (Color *)malloc(width*height*sizeof(Color));
    float radius = (width < height) ? (float)width/2.0f : (float)height/2.0f;

    float centerX = (float)width/2.0f;
    float centerY = (float)height/2.0f;
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float dist = hypotf((float)x - centerX, (float)y - centerY);
            float factor = (dist - radius*density)/(radius*(1.0f - density));
            
            factor = fmax(factor, 0.f);
            factor = fmin(factor, 1.f); // dist can be bigger than radius so we have to check
            
            pixels[y*width + x].r = (int)((float)outer.r*factor + (float)inner.r*(1.0f - factor));
            pixels[y*width + x].g = (int)((float)outer.g*factor + (float)inner.g*(1.0f - factor));
            pixels[y*width + x].b = (int)((float)outer.b*factor + (float)inner.b*(1.0f - factor));
            pixels[y*width + x].a = (int)((float)outer.a*factor + (float)inner.a*(1.0f - factor));
        }
    }

    Image image = LoadImageEx(pixels, width, height);
    free(pixels);

    return image;
}

// Generate image: checked
Image GenImageChecked(int width, int height, int checksX, int checksY, Color col1, Color col2)
{
    Color *pixels = (Color *)malloc(width*height*sizeof(Color));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if ((x/checksX + y/checksY)%2 == 0) pixels[y*width + x] = col1;
            else pixels[y*width + x] = col2;
        }
    }

    Image image = LoadImageEx(pixels, width, height);
    free(pixels);

    return image;
}

// Generate image: white noise
Image GenImageWhiteNoise(int width, int height, float factor)
{
    Color *pixels = (Color *)malloc(width*height*sizeof(Color));

    for (int i = 0; i < width*height; i++)
    {
        if (GetRandomValue(0, 99) < (int)(factor*100.0f)) pixels[i] = WHITE;
        else pixels[i] = BLACK;
    }

    Image image = LoadImageEx(pixels, width, height);
    free(pixels);

    return image;
}

// Generate image: perlin noise
Image GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale)
{
    Color *pixels = (Color *)malloc(width*height*sizeof(Color));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float nx = (float)(x + offsetX)*scale/(float)width;
            float ny = (float)(y + offsetY)*scale/(float)height;
            
            // Typical values to start playing with:
            //   lacunarity = ~2.0   -- spacing between successive octaves (use exactly 2.0 for wrapping output)
            //   gain       =  0.5   -- relative weighting applied to each successive octave
            //   octaves    =  6     -- number of "octaves" of noise3() to sum

            // NOTE: We need to translate the data from [-1..1] to [0..1]
            float p = (stb_perlin_fbm_noise3(nx, ny, 1.0f, 2.0f, 0.5f, 6, 0, 0, 0) + 1.0f)/2.0f;
            
            int intensity = (int)(p*255.0f);
            pixels[y*width + x] = (Color){intensity, intensity, intensity, 255};
        }
    }

    Image image = LoadImageEx(pixels, width, height);
    free(pixels);

    return image;
}

// Generate image: cellular algorithm. Bigger tileSize means bigger cells
Image GenImageCellular(int width, int height, int tileSize)
{
    Color *pixels = (Color *)malloc(width*height*sizeof(Color));

    int seedsPerRow = width/tileSize;
    int seedsPerCol = height/tileSize;
    int seedsCount = seedsPerRow * seedsPerCol;

    Vector2 *seeds = (Vector2 *)malloc(seedsCount*sizeof(Vector2));

    for (int i = 0; i < seedsCount; i++)
    {
        int y = (i/seedsPerRow)*tileSize + GetRandomValue(0, tileSize - 1);
        int x = (i%seedsPerRow)*tileSize + GetRandomValue(0, tileSize - 1);
        seeds[i] = (Vector2){x, y};
    }

    for (int y = 0; y < height; y++)
    {
        int tileY = y/tileSize;
        
        for (int x = 0; x < width; x++)
        {
            int tileX = x/tileSize;

            float minDistance = strtod("Inf", NULL);

            // Check all adjacent tiles
            for (int i = -1; i < 2; i++)
            {
                if ((tileX + i < 0) || (tileX + i >= seedsPerRow)) continue;

                for (int j = -1; j < 2; j++)
                {
                    if ((tileY + j < 0) || (tileY + j >= seedsPerCol)) continue;

                    Vector2 neighborSeed = seeds[(tileY + j)*seedsPerRow + tileX + i];

                    float dist = hypot(x - (int)neighborSeed.x, y - (int)neighborSeed.y);
                    minDistance = fmin(minDistance, dist);
                }
            }

            // I made this up but it seems to give good results at all tile sizes
            int intensity = (int)(minDistance*256.0f/tileSize);
            if (intensity > 255) intensity = 255;

            pixels[y*width + x] = (Color){ intensity, intensity, intensity, 255 };
        }
    }
    
    free(seeds);

    Image image = LoadImageEx(pixels, width, height);
    free(pixels);

    return image;
}
#endif      // SUPPORT_IMAGE_GENERATION

// Generate GPU mipmaps for a texture
void GenTextureMipmaps(Texture2D *texture)
{
#if defined(PLATFORM_WEB)
    // Calculate next power-of-two values
    int potWidth = (int)powf(2, ceilf(logf((float)texture->width)/logf(2)));
    int potHeight = (int)powf(2, ceilf(logf((float)texture->height)/logf(2)));

    // Check if texture is POT
    if ((potWidth != texture->width) || (potHeight != texture->height))
    {
        TraceLog(LOG_WARNING, "Limited NPOT support, no mipmaps available for NPOT textures");
    }
    else rlGenerateMipmaps(texture);
#else
    rlGenerateMipmaps(texture);
#endif
}

// Set texture scaling filter mode
void SetTextureFilter(Texture2D texture, int filterMode)
{
    switch (filterMode)
    {
        case FILTER_POINT:
        {
            if (texture.mipmaps > 1)
            {
                // RL_FILTER_MIP_NEAREST - tex filter: POINT, mipmaps filter: POINT (sharp switching between mipmaps)
                rlTextureParameters(texture.id, RL_TEXTURE_MIN_FILTER, RL_FILTER_MIP_NEAREST);

                // RL_FILTER_NEAREST - tex filter: POINT (no filter), no mipmaps
                rlTextureParameters(texture.id, RL_TEXTURE_MAG_FILTER, RL_FILTER_NEAREST);
            }
            else
            {
                // RL_FILTER_NEAREST - tex filter: POINT (no filter), no mipmaps
                rlTextureParameters(texture.id, RL_TEXTURE_MIN_FILTER, RL_FILTER_NEAREST);
                rlTextureParameters(texture.id, RL_TEXTURE_MAG_FILTER, RL_FILTER_NEAREST);
            }
        } break;
        case FILTER_BILINEAR:
        {
            if (texture.mipmaps > 1)
            {
                // RL_FILTER_LINEAR_MIP_NEAREST - tex filter: BILINEAR, mipmaps filter: POINT (sharp switching between mipmaps)
                // Alternative: RL_FILTER_NEAREST_MIP_LINEAR - tex filter: POINT, mipmaps filter: BILINEAR (smooth transition between mipmaps)
                rlTextureParameters(texture.id, RL_TEXTURE_MIN_FILTER, RL_FILTER_LINEAR_MIP_NEAREST);

                // RL_FILTER_LINEAR - tex filter: BILINEAR, no mipmaps
                rlTextureParameters(texture.id, RL_TEXTURE_MAG_FILTER, RL_FILTER_LINEAR);
            }
            else
            {
                // RL_FILTER_LINEAR - tex filter: BILINEAR, no mipmaps
                rlTextureParameters(texture.id, RL_TEXTURE_MIN_FILTER, RL_FILTER_LINEAR);
                rlTextureParameters(texture.id, RL_TEXTURE_MAG_FILTER, RL_FILTER_LINEAR);
            }
        } break;
        case FILTER_TRILINEAR:
        {
            if (texture.mipmaps > 1)
            {
                // RL_FILTER_MIP_LINEAR - tex filter: BILINEAR, mipmaps filter: BILINEAR (smooth transition between mipmaps)
                rlTextureParameters(texture.id, RL_TEXTURE_MIN_FILTER, RL_FILTER_MIP_LINEAR);

                // RL_FILTER_LINEAR - tex filter: BILINEAR, no mipmaps
                rlTextureParameters(texture.id, RL_TEXTURE_MAG_FILTER, RL_FILTER_LINEAR);
            }
            else
            {
                TraceLog(LOG_WARNING, "[TEX ID %i] No mipmaps available for TRILINEAR texture filtering", texture.id);

                // RL_FILTER_LINEAR - tex filter: BILINEAR, no mipmaps
                rlTextureParameters(texture.id, RL_TEXTURE_MIN_FILTER, RL_FILTER_LINEAR);
                rlTextureParameters(texture.id, RL_TEXTURE_MAG_FILTER, RL_FILTER_LINEAR);
            }
        } break;
        case FILTER_ANISOTROPIC_4X: rlTextureParameters(texture.id, RL_TEXTURE_ANISOTROPIC_FILTER, 4); break;
        case FILTER_ANISOTROPIC_8X: rlTextureParameters(texture.id, RL_TEXTURE_ANISOTROPIC_FILTER, 8); break;
        case FILTER_ANISOTROPIC_16X: rlTextureParameters(texture.id, RL_TEXTURE_ANISOTROPIC_FILTER, 16); break;
        default: break;
    }
}

// Set texture wrapping mode
void SetTextureWrap(Texture2D texture, int wrapMode)
{
    switch (wrapMode)
    {
        case WRAP_REPEAT:
        {
            rlTextureParameters(texture.id, RL_TEXTURE_WRAP_S, RL_WRAP_REPEAT);
            rlTextureParameters(texture.id, RL_TEXTURE_WRAP_T, RL_WRAP_REPEAT);
        } break;
        case WRAP_CLAMP:
        {
            rlTextureParameters(texture.id, RL_TEXTURE_WRAP_S, RL_WRAP_CLAMP);
            rlTextureParameters(texture.id, RL_TEXTURE_WRAP_T, RL_WRAP_CLAMP);
        } break;
        case WRAP_MIRROR:
        {
            rlTextureParameters(texture.id, RL_TEXTURE_WRAP_S, RL_WRAP_CLAMP_MIRROR);
            rlTextureParameters(texture.id, RL_TEXTURE_WRAP_T, RL_WRAP_CLAMP_MIRROR);
        } break;
        default: break;
    }
}

// Draw a Texture2D
void DrawTexture(Texture2D texture, int posX, int posY, Color tint)
{
    DrawTextureEx(texture, (Vector2){ (float)posX, (float)posY }, 0.0f, 1.0f, tint);
}

// Draw a Texture2D with position defined as Vector2
void DrawTextureV(Texture2D texture, Vector2 position, Color tint)
{
    DrawTextureEx(texture, position, 0, 1.0f, tint);
}

// Draw a Texture2D with extended parameters
void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint)
{
    Rectangle sourceRec = { 0, 0, texture.width, texture.height };
    Rectangle destRec = { (int)position.x, (int)position.y, texture.width*scale, texture.height*scale };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(texture, sourceRec, destRec, origin, rotation, tint);
}

// Draw a part of a texture (defined by a rectangle)
void DrawTextureRec(Texture2D texture, Rectangle sourceRec, Vector2 position, Color tint)
{
    Rectangle destRec = { (int)position.x, (int)position.y, abs(sourceRec.width), abs(sourceRec.height) };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, tint);
}

// Draw a part of a texture (defined by a rectangle) with 'pro' parameters
// NOTE: origin is relative to destination rectangle size
void DrawTexturePro(Texture2D texture, Rectangle sourceRec, Rectangle destRec, Vector2 origin, float rotation, Color tint)
{
    // Check if texture is valid
    if (texture.id > 0)
    {
        if (sourceRec.width < 0) sourceRec.x -= sourceRec.width;
        if (sourceRec.height < 0) sourceRec.y -= sourceRec.height;

        rlEnableTexture(texture.id);

        rlPushMatrix();
            rlTranslatef((float)destRec.x, (float)destRec.y, 0);
            rlRotatef(rotation, 0, 0, 1);
            rlTranslatef(-origin.x, -origin.y, 0);

            rlBegin(RL_QUADS);
                rlColor4ub(tint.r, tint.g, tint.b, tint.a);
                rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer

                // Bottom-left corner for texture and quad
                rlTexCoord2f((float)sourceRec.x/texture.width, (float)sourceRec.y/texture.height);
                rlVertex2f(0.0f, 0.0f);

                // Bottom-right corner for texture and quad
                rlTexCoord2f((float)sourceRec.x/texture.width, (float)(sourceRec.y + sourceRec.height)/texture.height);
                rlVertex2f(0.0f, (float)destRec.height);

                // Top-right corner for texture and quad
                rlTexCoord2f((float)(sourceRec.x + sourceRec.width)/texture.width, (float)(sourceRec.y + sourceRec.height)/texture.height);
                rlVertex2f((float)destRec.width, (float)destRec.height);

                // Top-left corner for texture and quad
                rlTexCoord2f((float)(sourceRec.x + sourceRec.width)/texture.width, (float)sourceRec.y/texture.height);
                rlVertex2f((float)destRec.width, 0.0f);
            rlEnd();
        rlPopMatrix();

        rlDisableTexture();
    }
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

#if defined(SUPPORT_FILEFORMAT_DDS)
// Loading DDS image data (compressed or uncompressed)
static Image LoadDDS(const char *fileName)
{
    // Required extension:
    // GL_EXT_texture_compression_s3tc

    // Supported tokens (defined by extensions)
    // GL_COMPRESSED_RGB_S3TC_DXT1_EXT      0x83F0
    // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT     0x83F1
    // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT     0x83F2
    // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT     0x83F3

    #define FOURCC_DXT1 0x31545844  // Equivalent to "DXT1" in ASCII
    #define FOURCC_DXT3 0x33545844  // Equivalent to "DXT3" in ASCII
    #define FOURCC_DXT5 0x35545844  // Equivalent to "DXT5" in ASCII

    // DDS Pixel Format
    typedef struct {
        unsigned int size;
        unsigned int flags;
        unsigned int fourCC;
        unsigned int rgbBitCount;
        unsigned int rBitMask;
        unsigned int gBitMask;
        unsigned int bBitMask;
        unsigned int aBitMask;
    } DDSPixelFormat;

    // DDS Header (124 bytes)
    typedef struct {
        unsigned int size;
        unsigned int flags;
        unsigned int height;
        unsigned int width;
        unsigned int pitchOrLinearSize;
        unsigned int depth;
        unsigned int mipmapCount;
        unsigned int reserved1[11];
        DDSPixelFormat ddspf;
        unsigned int caps;
        unsigned int caps2;
        unsigned int caps3;
        unsigned int caps4;
        unsigned int reserved2;
    } DDSHeader;

    Image image = { 0 };

    FILE *ddsFile = fopen(fileName, "rb");

    if (ddsFile == NULL)
    {
        TraceLog(LOG_WARNING, "[%s] DDS file could not be opened", fileName);
    }
    else
    {
        // Verify the type of file
        char filecode[4];

        fread(filecode, 4, 1, ddsFile);

        if (strncmp(filecode, "DDS ", 4) != 0)
        {
            TraceLog(LOG_WARNING, "[%s] DDS file does not seem to be a valid image", fileName);
        }
        else
        {
            DDSHeader ddsHeader;

            // Get the image header
            fread(&ddsHeader, sizeof(DDSHeader), 1, ddsFile);

            TraceLog(LOG_DEBUG, "[%s] DDS file header size: %i", fileName, sizeof(DDSHeader));
            TraceLog(LOG_DEBUG, "[%s] DDS file pixel format size: %i", fileName, ddsHeader.ddspf.size);
            TraceLog(LOG_DEBUG, "[%s] DDS file pixel format flags: 0x%x", fileName, ddsHeader.ddspf.flags);
            TraceLog(LOG_DEBUG, "[%s] DDS file format: 0x%x", fileName, ddsHeader.ddspf.fourCC);
            TraceLog(LOG_DEBUG, "[%s] DDS file bit count: 0x%x", fileName, ddsHeader.ddspf.rgbBitCount);

            image.width = ddsHeader.width;
            image.height = ddsHeader.height;
            
            if (ddsHeader.mipmapCount == 0) image.mipmaps = 1;      // Parameter not used
            else image.mipmaps = ddsHeader.mipmapCount;

            if (ddsHeader.ddspf.rgbBitCount == 16)     // 16bit mode, no compressed
            {
                if (ddsHeader.ddspf.flags == 0x40)         // no alpha channel
                {
                    image.data = (unsigned short *)malloc(image.width*image.height*sizeof(unsigned short));
                    fread(image.data, image.width*image.height*sizeof(unsigned short), 1, ddsFile);

                    image.format = UNCOMPRESSED_R5G6B5;
                }
                else if (ddsHeader.ddspf.flags == 0x41)        // with alpha channel
                {
                    if (ddsHeader.ddspf.aBitMask == 0x8000)    // 1bit alpha
                    {
                        image.data = (unsigned short *)malloc(image.width*image.height*sizeof(unsigned short));
                        fread(image.data, image.width*image.height*sizeof(unsigned short), 1, ddsFile);

                        unsigned char alpha = 0;

                        // NOTE: Data comes as A1R5G5B5, it must be reordered to R5G5B5A1
                        for (int i = 0; i < image.width*image.height; i++)
                        {
                            alpha = ((unsigned short *)image.data)[i] >> 15;
                            ((unsigned short *)image.data)[i] = ((unsigned short *)image.data)[i] << 1;
                            ((unsigned short *)image.data)[i] += alpha;
                        }

                        image.format = UNCOMPRESSED_R5G5B5A1;
                    }
                    else if (ddsHeader.ddspf.aBitMask == 0xf000)   // 4bit alpha
                    {
                        image.data = (unsigned short *)malloc(image.width*image.height*sizeof(unsigned short));
                        fread(image.data, image.width*image.height*sizeof(unsigned short), 1, ddsFile);

                        unsigned char alpha = 0;

                        // NOTE: Data comes as A4R4G4B4, it must be reordered R4G4B4A4
                        for (int i = 0; i < image.width*image.height; i++)
                        {
                            alpha = ((unsigned short *)image.data)[i] >> 12;
                            ((unsigned short *)image.data)[i] = ((unsigned short *)image.data)[i] << 4;
                            ((unsigned short *)image.data)[i] += alpha;
                        }

                        image.format = UNCOMPRESSED_R4G4B4A4;
                    }
                }
            }
            if (ddsHeader.ddspf.flags == 0x40 && ddsHeader.ddspf.rgbBitCount == 24)   // DDS_RGB, no compressed
            {
                // NOTE: not sure if this case exists...
                image.data = (unsigned char *)malloc(image.width*image.height*3*sizeof(unsigned char));
                fread(image.data, image.width*image.height*3, 1, ddsFile);

                image.format = UNCOMPRESSED_R8G8B8;
            }
            else if (ddsHeader.ddspf.flags == 0x41 && ddsHeader.ddspf.rgbBitCount == 32) // DDS_RGBA, no compressed
            {
                image.data = (unsigned char *)malloc(image.width*image.height*4*sizeof(unsigned char));
                fread(image.data, image.width*image.height*4, 1, ddsFile);

                unsigned char blue = 0;

                // NOTE: Data comes as A8R8G8B8, it must be reordered R8G8B8A8 (view next comment)
                // DirecX understand ARGB as a 32bit DWORD but the actual memory byte alignment is BGRA
                // So, we must realign B8G8R8A8 to R8G8B8A8
                for (int i = 0; i < image.width*image.height*4; i += 4)
                {
                    blue = ((unsigned char *)image.data)[i];
                    ((unsigned char *)image.data)[i] = ((unsigned char *)image.data)[i + 2];
                    ((unsigned char *)image.data)[i + 2] = blue;
                }

                image.format = UNCOMPRESSED_R8G8B8A8;
            }
            else if (((ddsHeader.ddspf.flags == 0x04) || (ddsHeader.ddspf.flags == 0x05)) && (ddsHeader.ddspf.fourCC > 0)) // Compressed
            {
                int size;       // DDS image data size

                // Calculate data size, including all mipmaps
                if (ddsHeader.mipmapCount > 1) size = ddsHeader.pitchOrLinearSize*2;
                else size = ddsHeader.pitchOrLinearSize;

                TraceLog(LOG_DEBUG, "Pitch or linear size: %i", ddsHeader.pitchOrLinearSize);

                image.data = (unsigned char*)malloc(size*sizeof(unsigned char));

                fread(image.data, size, 1, ddsFile);

                switch (ddsHeader.ddspf.fourCC)
                {
                    case FOURCC_DXT1:
                    {
                        if (ddsHeader.ddspf.flags == 0x04) image.format = COMPRESSED_DXT1_RGB;
                        else image.format = COMPRESSED_DXT1_RGBA;
                    } break;
                    case FOURCC_DXT3: image.format = COMPRESSED_DXT3_RGBA; break;
                    case FOURCC_DXT5: image.format = COMPRESSED_DXT5_RGBA; break;
                    default: break;
                }
            }
        }

        fclose(ddsFile);    // Close file pointer
    }

    return image;
}
#endif

#if defined(SUPPORT_FILEFORMAT_PKM)
// Loading PKM image data (ETC1/ETC2 compression)
// NOTE: KTX is the standard Khronos Group compression format (ETC1/ETC2, mipmaps)
// PKM is a much simpler file format used mainly to contain a single ETC1/ETC2 compressed image (no mipmaps)
static Image LoadPKM(const char *fileName)
{
    // Required extensions:
    // GL_OES_compressed_ETC1_RGB8_texture  (ETC1) (OpenGL ES 2.0)
    // GL_ARB_ES3_compatibility  (ETC2/EAC) (OpenGL ES 3.0)

    // Supported tokens (defined by extensions)
    // GL_ETC1_RGB8_OES                 0x8D64
    // GL_COMPRESSED_RGB8_ETC2          0x9274
    // GL_COMPRESSED_RGBA8_ETC2_EAC     0x9278

    // PKM file (ETC1) Header (16 bytes)
    typedef struct {
        char id[4];                 // "PKM "
        char version[2];            // "10" or "20"
        unsigned short format;      // Data format (big-endian) (Check list below)
        unsigned short width;       // Texture width (big-endian) (origWidth rounded to multiple of 4)
        unsigned short height;      // Texture height (big-endian) (origHeight rounded to multiple of 4)
        unsigned short origWidth;   // Original width (big-endian)
        unsigned short origHeight;  // Original height (big-endian)
    } PKMHeader;

    // Formats list
    // version 10: format: 0=ETC1_RGB, [1=ETC1_RGBA, 2=ETC1_RGB_MIP, 3=ETC1_RGBA_MIP] (not used)
    // version 20: format: 0=ETC1_RGB, 1=ETC2_RGB, 2=ETC2_RGBA_OLD, 3=ETC2_RGBA, 4=ETC2_RGBA1, 5=ETC2_R, 6=ETC2_RG, 7=ETC2_SIGNED_R, 8=ETC2_SIGNED_R

    // NOTE: The extended width and height are the widths rounded up to a multiple of 4.
    // NOTE: ETC is always 4bit per pixel (64 bit for each 4x4 block of pixels)

    Image image = { 0 };

    FILE *pkmFile = fopen(fileName, "rb");

    if (pkmFile == NULL)
    {
        TraceLog(LOG_WARNING, "[%s] PKM file could not be opened", fileName);
    }
    else
    {
        PKMHeader pkmHeader;

        // Get the image header
        fread(&pkmHeader, sizeof(PKMHeader), 1, pkmFile);

        if (strncmp(pkmHeader.id, "PKM ", 4) != 0)
        {
            TraceLog(LOG_WARNING, "[%s] PKM file does not seem to be a valid image", fileName);
        }
        else
        {
            // NOTE: format, width and height come as big-endian, data must be swapped to little-endian
            pkmHeader.format = ((pkmHeader.format & 0x00FF) << 8) | ((pkmHeader.format & 0xFF00) >> 8);
            pkmHeader.width = ((pkmHeader.width & 0x00FF) << 8) | ((pkmHeader.width & 0xFF00) >> 8);
            pkmHeader.height = ((pkmHeader.height & 0x00FF) << 8) | ((pkmHeader.height & 0xFF00) >> 8);

            TraceLog(LOG_DEBUG, "PKM (ETC) image width: %i", pkmHeader.width);
            TraceLog(LOG_DEBUG, "PKM (ETC) image height: %i", pkmHeader.height);
            TraceLog(LOG_DEBUG, "PKM (ETC) image format: %i", pkmHeader.format);

            image.width = pkmHeader.width;
            image.height = pkmHeader.height;
            image.mipmaps = 1;

            int bpp = 4;
            if (pkmHeader.format == 3) bpp = 8;

            int size = image.width*image.height*bpp/8;  // Total data size in bytes

            image.data = (unsigned char*)malloc(size*sizeof(unsigned char));

            fread(image.data, size, 1, pkmFile);

            if (pkmHeader.format == 0) image.format = COMPRESSED_ETC1_RGB;
            else if (pkmHeader.format == 1) image.format = COMPRESSED_ETC2_RGB;
            else if (pkmHeader.format == 3) image.format = COMPRESSED_ETC2_EAC_RGBA;
        }

        fclose(pkmFile);    // Close file pointer
    }

    return image;
}
#endif

#if defined(SUPPORT_FILEFORMAT_KTX)
// Load KTX compressed image data (ETC1/ETC2 compression)
static Image LoadKTX(const char *fileName)
{
    // Required extensions:
    // GL_OES_compressed_ETC1_RGB8_texture  (ETC1)
    // GL_ARB_ES3_compatibility  (ETC2/EAC)

    // Supported tokens (defined by extensions)
    // GL_ETC1_RGB8_OES                 0x8D64
    // GL_COMPRESSED_RGB8_ETC2          0x9274
    // GL_COMPRESSED_RGBA8_ETC2_EAC     0x9278

    // KTX file Header (64 bytes)
    // https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
    typedef struct {
        char id[12];                        // Identifier: "«KTX 11»\r\n\x1A\n"
        unsigned int endianness;            // Little endian: 0x01 0x02 0x03 0x04
        unsigned int glType;                // For compressed textures, glType must equal 0
        unsigned int glTypeSize;            // For compressed texture data, usually 1
        unsigned int glFormat;              // For compressed textures is 0
        unsigned int glInternalFormat;      // Compressed internal format
        unsigned int glBaseInternalFormat;  // Same as glFormat (RGB, RGBA, ALPHA...)
        unsigned int width;                 // Texture image width in pixels
        unsigned int height;                // Texture image height in pixels
        unsigned int depth;                 // For 2D textures is 0
        unsigned int elements;              // Number of array elements, usually 0
        unsigned int faces;                 // Cubemap faces, for no-cubemap = 1
        unsigned int mipmapLevels;          // Non-mipmapped textures = 1
        unsigned int keyValueDataSize;      // Used to encode any arbitrary data...
    } KTXHeader;

    // NOTE: Before start of every mipmap data block, we have: unsigned int dataSize

    Image image = { 0 };

    FILE *ktxFile = fopen(fileName, "rb");

    if (ktxFile == NULL)
    {
        TraceLog(LOG_WARNING, "[%s] KTX image file could not be opened", fileName);
    }
    else
    {
        KTXHeader ktxHeader;

        // Get the image header
        fread(&ktxHeader, sizeof(KTXHeader), 1, ktxFile);

        if ((ktxHeader.id[1] != 'K') || (ktxHeader.id[2] != 'T') || (ktxHeader.id[3] != 'X') ||
            (ktxHeader.id[4] != ' ') || (ktxHeader.id[5] != '1') || (ktxHeader.id[6] != '1'))
        {
            TraceLog(LOG_WARNING, "[%s] KTX file does not seem to be a valid file", fileName);
        }
        else
        {
            image.width = ktxHeader.width;
            image.height = ktxHeader.height;
            image.mipmaps = ktxHeader.mipmapLevels;

            TraceLog(LOG_DEBUG, "KTX (ETC) image width: %i", ktxHeader.width);
            TraceLog(LOG_DEBUG, "KTX (ETC) image height: %i", ktxHeader.height);
            TraceLog(LOG_DEBUG, "KTX (ETC) image format: 0x%x", ktxHeader.glInternalFormat);

            unsigned char unused;

            if (ktxHeader.keyValueDataSize > 0)
            {
                for (int i = 0; i < ktxHeader.keyValueDataSize; i++) fread(&unused, sizeof(unsigned char), 1, ktxFile);
            }

            int dataSize;
            fread(&dataSize, sizeof(unsigned int), 1, ktxFile);

            image.data = (unsigned char*)malloc(dataSize*sizeof(unsigned char));

            fread(image.data, dataSize, 1, ktxFile);

            if (ktxHeader.glInternalFormat == 0x8D64) image.format = COMPRESSED_ETC1_RGB;
            else if (ktxHeader.glInternalFormat == 0x9274) image.format = COMPRESSED_ETC2_RGB;
            else if (ktxHeader.glInternalFormat == 0x9278) image.format = COMPRESSED_ETC2_EAC_RGBA;
        }

        fclose(ktxFile);    // Close file pointer
    }

    return image;
}
#endif

#if defined(SUPPORT_FILEFORMAT_PVR)
// Loading PVR image data (uncompressed or PVRT compression)
// NOTE: PVR v2 not supported, use PVR v3 instead
static Image LoadPVR(const char *fileName)
{
    // Required extension:
    // GL_IMG_texture_compression_pvrtc

    // Supported tokens (defined by extensions)
    // GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG       0x8C00
    // GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG      0x8C02

#if 0   // Not used...
    // PVR file v2 Header (52 bytes)
    typedef struct {
        unsigned int headerLength;
        unsigned int height;
        unsigned int width;
        unsigned int numMipmaps;
        unsigned int flags;
        unsigned int dataLength;
        unsigned int bpp;
        unsigned int bitmaskRed;
        unsigned int bitmaskGreen;
        unsigned int bitmaskBlue;
        unsigned int bitmaskAlpha;
        unsigned int pvrTag;
        unsigned int numSurfs;
    } PVRHeaderV2;
#endif

    // PVR file v3 Header (52 bytes)
    // NOTE: After it could be metadata (15 bytes?)
    typedef struct {
        char id[4];
        unsigned int flags;
        unsigned char channels[4];      // pixelFormat high part
        unsigned char channelDepth[4];  // pixelFormat low part
        unsigned int colourSpace;
        unsigned int channelType;
        unsigned int height;
        unsigned int width;
        unsigned int depth;
        unsigned int numSurfaces;
        unsigned int numFaces;
        unsigned int numMipmaps;
        unsigned int metaDataSize;
    } PVRHeaderV3;

#if 0   // Not used...
    // Metadata (usually 15 bytes)
    typedef struct {
        unsigned int devFOURCC;
        unsigned int key;
        unsigned int dataSize;      // Not used?
        unsigned char *data;        // Not used?
    } PVRMetadata;
#endif

    Image image = { 0 };

    FILE *pvrFile = fopen(fileName, "rb");

    if (pvrFile == NULL)
    {
        TraceLog(LOG_WARNING, "[%s] PVR file could not be opened", fileName);
    }
    else
    {
        // Check PVR image version
        unsigned char pvrVersion = 0;
        fread(&pvrVersion, sizeof(unsigned char), 1, pvrFile);
        fseek(pvrFile, 0, SEEK_SET);

        // Load different PVR data formats
        if (pvrVersion == 0x50)
        {
            PVRHeaderV3 pvrHeader;

            // Get PVR image header
            fread(&pvrHeader, sizeof(PVRHeaderV3), 1, pvrFile);

            if ((pvrHeader.id[0] != 'P') || (pvrHeader.id[1] != 'V') || (pvrHeader.id[2] != 'R') || (pvrHeader.id[3] != 3))
            {
                TraceLog(LOG_WARNING, "[%s] PVR file does not seem to be a valid image", fileName);
            }
            else
            {
                image.width = pvrHeader.width;
                image.height = pvrHeader.height;
                image.mipmaps = pvrHeader.numMipmaps;

                // Check data format
                if (((pvrHeader.channels[0] == 'l') && (pvrHeader.channels[1] == 0)) && (pvrHeader.channelDepth[0] == 8))
                    image.format = UNCOMPRESSED_GRAYSCALE;
                else if (((pvrHeader.channels[0] == 'l') && (pvrHeader.channels[1] == 'a')) && ((pvrHeader.channelDepth[0] == 8) && (pvrHeader.channelDepth[1] == 8)))
                    image.format = UNCOMPRESSED_GRAY_ALPHA;
                else if ((pvrHeader.channels[0] == 'r') && (pvrHeader.channels[1] == 'g') && (pvrHeader.channels[2] == 'b'))
                {
                    if (pvrHeader.channels[3] == 'a')
                    {
                        if ((pvrHeader.channelDepth[0] == 5) && (pvrHeader.channelDepth[1] == 5) && (pvrHeader.channelDepth[2] == 5) && (pvrHeader.channelDepth[3] == 1))
                            image.format = UNCOMPRESSED_R5G5B5A1;
                        else if ((pvrHeader.channelDepth[0] == 4) && (pvrHeader.channelDepth[1] == 4) && (pvrHeader.channelDepth[2] == 4) && (pvrHeader.channelDepth[3] == 4))
                            image.format = UNCOMPRESSED_R4G4B4A4;
                        else if ((pvrHeader.channelDepth[0] == 8) && (pvrHeader.channelDepth[1] == 8) && (pvrHeader.channelDepth[2] == 8) && (pvrHeader.channelDepth[3] == 8))
                            image.format = UNCOMPRESSED_R8G8B8A8;
                    }
                    else if (pvrHeader.channels[3] == 0)
                    {
                        if ((pvrHeader.channelDepth[0] == 5) && (pvrHeader.channelDepth[1] == 6) && (pvrHeader.channelDepth[2] == 5)) image.format = UNCOMPRESSED_R5G6B5;
                        else if ((pvrHeader.channelDepth[0] == 8) && (pvrHeader.channelDepth[1] == 8) && (pvrHeader.channelDepth[2] == 8)) image.format = UNCOMPRESSED_R8G8B8;
                    }
                }
                else if (pvrHeader.channels[0] == 2) image.format = COMPRESSED_PVRT_RGB;
                else if (pvrHeader.channels[0] == 3) image.format = COMPRESSED_PVRT_RGBA;

                // Skip meta data header
                unsigned char unused = 0;
                for (int i = 0; i < pvrHeader.metaDataSize; i++) fread(&unused, sizeof(unsigned char), 1, pvrFile);

                // Calculate data size (depends on format)
                int bpp = 0;

                switch (image.format)
                {
                    case UNCOMPRESSED_GRAYSCALE: bpp = 8; break;
                    case UNCOMPRESSED_GRAY_ALPHA:
                    case UNCOMPRESSED_R5G5B5A1:
                    case UNCOMPRESSED_R5G6B5:
                    case UNCOMPRESSED_R4G4B4A4: bpp = 16; break;
                    case UNCOMPRESSED_R8G8B8A8: bpp = 32; break;
                    case UNCOMPRESSED_R8G8B8: bpp = 24; break;
                    case COMPRESSED_PVRT_RGB:
                    case COMPRESSED_PVRT_RGBA: bpp = 4; break;
                    default: break;
                }

                int dataSize = image.width*image.height*bpp/8;  // Total data size in bytes
                image.data = (unsigned char*)malloc(dataSize*sizeof(unsigned char));

                // Read data from file
                fread(image.data, dataSize, 1, pvrFile);
            }
        }
        else if (pvrVersion == 52) TraceLog(LOG_INFO, "PVR v2 not supported, update your files to PVR v3");

        fclose(pvrFile);    // Close file pointer
    }

    return image;
}
#endif

#if defined(SUPPORT_FILEFORMAT_ASTC)
// Load ASTC compressed image data (ASTC compression)
static Image LoadASTC(const char *fileName)
{
    // Required extensions:
    // GL_KHR_texture_compression_astc_hdr
    // GL_KHR_texture_compression_astc_ldr

    // Supported tokens (defined by extensions)
    // GL_COMPRESSED_RGBA_ASTC_4x4_KHR      0x93b0
    // GL_COMPRESSED_RGBA_ASTC_8x8_KHR      0x93b7

    // ASTC file Header (16 bytes)
    typedef struct {
        unsigned char id[4];        // Signature: 0x13 0xAB 0xA1 0x5C
        unsigned char blockX;       // Block X dimensions
        unsigned char blockY;       // Block Y dimensions
        unsigned char blockZ;       // Block Z dimensions (1 for 2D images)
        unsigned char width[3];     // Image width in pixels (24bit value)
        unsigned char height[3];    // Image height in pixels (24bit value)
        unsigned char length[3];    // Image Z-size (1 for 2D images)
    } ASTCHeader;

    Image image = { 0 };

    FILE *astcFile = fopen(fileName, "rb");

    if (astcFile == NULL)
    {
        TraceLog(LOG_WARNING, "[%s] ASTC file could not be opened", fileName);
    }
    else
    {
        ASTCHeader astcHeader;

        // Get ASTC image header
        fread(&astcHeader, sizeof(ASTCHeader), 1, astcFile);

        if ((astcHeader.id[3] != 0x5c) || (astcHeader.id[2] != 0xa1) || (astcHeader.id[1] != 0xab) || (astcHeader.id[0] != 0x13))
        {
            TraceLog(LOG_WARNING, "[%s] ASTC file does not seem to be a valid image", fileName);
        }
        else
        {
            // NOTE: Assuming Little Endian (could it be wrong?)
            image.width = 0x00000000 | ((int)astcHeader.width[2] << 16) | ((int)astcHeader.width[1] << 8) | ((int)astcHeader.width[0]);
            image.height = 0x00000000 | ((int)astcHeader.height[2] << 16) | ((int)astcHeader.height[1] << 8) | ((int)astcHeader.height[0]);

            TraceLog(LOG_DEBUG, "ASTC image width: %i", image.width);
            TraceLog(LOG_DEBUG, "ASTC image height: %i", image.height);
            TraceLog(LOG_DEBUG, "ASTC image blocks: %ix%i", astcHeader.blockX, astcHeader.blockY);
            
            image.mipmaps = 1;      // NOTE: ASTC format only contains one mipmap level

            // NOTE: Each block is always stored in 128bit so we can calculate the bpp
            int bpp = 128/(astcHeader.blockX*astcHeader.blockY);

            // NOTE: Currently we only support 2 blocks configurations: 4x4 and 8x8
            if ((bpp == 8) || (bpp == 2))
            {
                int dataSize = image.width*image.height*bpp/8;  // Data size in bytes

                image.data = (unsigned char *)malloc(dataSize*sizeof(unsigned char));
                fread(image.data, dataSize, 1, astcFile);

                if (bpp == 8) image.format = COMPRESSED_ASTC_4x4_RGBA;
                else if (bpp == 2) image.format = COMPRESSED_ASTC_4x4_RGBA;
            }
            else TraceLog(LOG_WARNING, "[%s] ASTC block size configuration not supported", fileName);
        }

        fclose(astcFile);
    }

    return image;
}
#endif

