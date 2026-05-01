/*
 *  Zoxel Graphics
 *
 *      - Imports one of the sub modules based on compiler
 *
 * */
#ifndef zoxm_graphics
#define zoxm_graphics

#ifdef zox_sdl
    #include "opengl/_.c"
    #include "vulkan/_.c"
#else
    #include "headless/_.c"
#endif

zox_begin_module(Graphics)
#ifndef zox_sdl
    zox_import_module(Headless);
#endif
zox_end_module(Headless)

#endif
