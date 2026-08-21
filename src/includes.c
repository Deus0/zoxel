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
#ifdef zox_logs
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
// Graphics Libraries
#ifdef zox_gles2
    #include <GLES2/gl2.h>
#endif
#ifdef zox_windows
    #include <GL/glew.h>
#endif
#ifdef zox_vulkan
    #include <vulkan/vulkan.h>
    #include <vulkan/vulkan_wayland.h>
#endif
// Windowing!
#ifdef zox_sdl
    // SDL Windowing!
    #ifdef zox_vulkan
        #ifdef zox_sdl3
            #include <SDL2/SDL_vulkan.h>
        #else
            #include <SDL3/SDL_vulkan.h>
        #endif
    #endif
    #ifdef zox_sdl3
        #include <SDL3/SDL.h>
        #include <SDL3/SDL_opengl.h>
        #ifdef zox_sdl_mixer
            #include <SDL3_mixer/SDL_mixer.h>
        #endif
        #ifdef zox_sdl_images
            #include <SDL3/SDL_image.h>
        #endif
    #else
        #include <SDL.h>
        #include <SDL_opengl.h>
        #ifdef zox_sdl_mixer
            #include <SDL_mixer.h>
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
#elif zox_glut
    // Glut Windowing!
    #include <GL/freeglut.h>
#elif zox_glfw
    // TODO: GLFW?
#endif