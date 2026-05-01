// OpenGL Wrapper for Zoxel
#ifndef zoxm_opengl
#define zoxm_opengl

byte is_log_opengl = 0;

#define zox_log_opengl(...) if (is_log_opengl) { zox_log(__VA_ARGS__); }


#ifdef zox_gles2
    #define zox_disable_instancing
    #define zox_disable_ubos
    #define zox_disable_compute
    #define zox_disable_geometry_shaders
#endif

#include "dat/_.c"
#include "fun/_.c"
#include "tst/_.c"

#endif
