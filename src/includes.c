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
#if defined(zox_windows) && !defined(zox_vulkan)   // glew: opengl builds only
    #ifdef zox_logs
        // every glew gl call checks it's on the main thread (src/_/platforms/tst/gl_thread.c)
        void zox_gl_thread_check(const char* name);
        void zox_gl_log_last();
        #define GLEW_GET_FUN(x) (zox_gl_thread_check(#x), x)
    #endif
    #include <GL/glew.h>
    #ifdef zox_logs
        // gl 1.1's calls aren't glew's, checked here (a macro naming itself calls the real function)
        #define glDrawElements(...) (zox_gl_thread_check("glDrawElements"), glDrawElements(__VA_ARGS__))
        #define glDrawArrays(...) (zox_gl_thread_check("glDrawArrays"), glDrawArrays(__VA_ARGS__))
        #define glTexImage2D(...) (zox_gl_thread_check("glTexImage2D"), glTexImage2D(__VA_ARGS__))
        #define glTexSubImage2D(...) (zox_gl_thread_check("glTexSubImage2D"), glTexSubImage2D(__VA_ARGS__))
        #define glBindTexture(...) (zox_gl_thread_check("glBindTexture"), glBindTexture(__VA_ARGS__))
        #define glDeleteTextures(...) (zox_gl_thread_check("glDeleteTextures"), glDeleteTextures(__VA_ARGS__))
        #define glReadPixels(...) (zox_gl_thread_check("glReadPixels"), glReadPixels(__VA_ARGS__))
        #define glClear(...) (zox_gl_thread_check("glClear"), glClear(__VA_ARGS__))
        #define glViewport(...) (zox_gl_thread_check("glViewport"), glViewport(__VA_ARGS__))
    #endif
#endif
#ifdef zox_vulkan
    #include <vulkan/vulkan.h>
    #include <shaderc/shaderc.h>
#endif
// Windowing!
#ifdef zox_sdl
    // SDL Windowing!
    #ifdef zox_vulkan
        #ifdef zox_sdl3
            #include <SDL3/SDL_vulkan.h>
        #else
            #include <SDL_vulkan.h>
        #endif
    #endif
    #ifdef zox_sdl3
        #include <SDL3/SDL.h>
        #include <SDL3/SDL_opengl.h>
        #ifdef zox_sdl_images
            #include <SDL3/SDL_image.h>
        #endif
    #else
        #include <SDL.h>
        #include <SDL_opengl.h>
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