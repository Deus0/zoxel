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
#else
    #include "headless/_.c"
#endif
#ifdef zox_vulkan
    #include "vulkan/_.c"
#endif


zox_begin_module(Graphics)
#ifndef zox_sdl
    zox_import_module(Headless);
#endif

#ifdef zox_sdl // zox_opengl
    zox_import_module(Opengl);
#endif

#ifdef zox_vulkan
    zox_import_module(Vulkan);
#endif
zox_end_module(Headless)

#endif
