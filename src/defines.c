// #### INCLUDES ####
#define _POSIX_C_SOURCE 200809L
// Platforms
#ifdef zox_windows
    #define max_args 64
    #define max_arg_len 256
    #define WIN32_LEAN_AND_MEAN
#endif
// OpenGL
#define GL_GLEXT_PROTOTYPES
// Game
#ifndef zox_game
    #define zox_game zoxel
#endif
#define str_macro(x) #x
#define inc_nexus_game(x) str_macro(../gam/x/_.c)
#define zox_nexus_game inc_nexus_game(zox_game)

#ifndef zox_names
    #if defined(zox_logs) || defined(zox_profiler)
        #define zox_names
    #endif
#endif
#ifdef zox_debug
    #define zox_debug_settings
    // #define zox_disable_logs
#endif

// Temp
// #define zox_time_systems

// Flecs
#define FLECS_CUSTOM_BUILD
#define FLECS_MODULE
#define FLECS_SYSTEM
#define FLECS_PIPELINE
// #define FLECS_PROFILER
#ifdef flecs_profiler
    #define FLECS_STATS
    #define FLECS_REST
    #define FLECS_HTTP
    #define FLECS_METRICS
#endif
