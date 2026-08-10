// core includes
#include <signal.h>     // Detecting Cancel in Terminal
#include <stdlib.h>     // for malloc & free
#include <stdio.h>      // just for sprintf and perror
#include <stdint.h>     // Integers
#include <float.h>      // Floats
#include <stddef.h>     // offsetof
#include <string.h>     // Memory Copies and Strlen!
#include <pthread.h>    // Threadlocks - pthread_rwlock_init
#include <math.h>       // For Maths
#include <time.h>       // For Time
#include <limits.h>     // UINT_MAX
// # Pathing #
// // for DIR, readdir, closedir etc
#include <dirent.h>
// for pathing - can we do without?
#include <sys/stat.h>   // pathing
// used in pathing, doing funky stuff??
#include <unistd.h>
// also used in pathing - ENOENT
// and networking - EWOULDBLOCK
#include <errno.h>
//! Included Libraries for App
#ifndef zox_disable_logs
    #include <stdarg.h>
#endif
#ifdef flecssource
  #include "flecs.h"
#else
  #include <flecs.h>
#endif
// Networking
#include <fcntl.h>  // F_SETFL etc
#ifdef zox_windows
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif
// Platforms
#ifdef zox_windows
    #include <windows.h>
    #include <shellapi.h>
#endif
#ifdef zox_android
    #include <android/log.h>
    #include <android/asset_manager.h>
    #include <android/asset_manager_jni.h>
    #include <sys/types.h>
#endif
#ifdef zox_web
    #include <emscripten.h>
#endif
// SDL2 & OpenGL
// TODO: if zox_opengl we import Glew
#ifdef zox_sdl
#ifdef zox_windows
    #ifdef sdlsource
        #include "GL/glew.h"
    #else
        #include <GL/glew.h>
    #endif
#endif
// Glut Lib!
#elif zox_glut
    #include <GL/freeglut.h>
#endif
#ifdef zox_gles2
    #include <GLES2/gl2.h>
#endif
#ifdef zox_vulkan
    #include <SDL2/SDL_vulkan.h>
    #include <vulkan/vulkan.h>
    #include <vulkan/vulkan_wayland.h>
#endif
#ifdef zox_sdl3
    #include <SDL3/SDL.h>
    #include <SDL3/SDL_opengl.h>
#else
    #ifdef sdlsource
        #include "SDL.h"
        #include "SDL_opengl.h"
    #else
        #include <SDL2/SDL.h>
        #include <SDL2/SDL_opengl.h>
    #endif
#endif
#ifdef zox_android
    #ifdef zox_sdl3
        #include <SDL3/SDL_system.h>
        #include <SDL3/SDL_main.h>
    #else
        #include <SDL2/SDL_system.h>
    #endif
#endif
#ifdef zox_sdl_images
    #ifdef sdlsource
        #include "SDL_image.h"
    #else
        #include <SDL2/SDL_image.h>
    #endif
#endif
#ifdef zox_sdl_mixer
    #ifdef sdlsource
        #include "SDL_mixer.h"
    #else
        #include <SDL2/SDL_mixer.h>
    #endif
#endif
