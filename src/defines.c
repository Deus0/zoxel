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
// Flecs
#define FLECS_CUSTOM_BUILD
#define FLECS_MODULE
#define FLECS_SYSTEM
#define FLECS_PIPELINE
// Game
#ifndef zox_game
    #define zox_game zoxel
#endif
#define str_macro(x) #x
#define inc_nexus_game(x) str_macro(../gam/x/_.c)
#define zox_nexus_game inc_nexus_game(zox_game)
// Random
#define zox_set_camera_firstperson
#define zoxel_time_main_loop_cutoff 33.33f
// Release
#ifndef zox_debug
    #define zox_disable_system_timings
    #define zox_disable_names
    #define zox_disable_logs
#endif
