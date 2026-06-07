/*
 *  Zoxel Graphics
 *
 *      - Imports one of the sub modules based on compiler
 *
 * */
#ifndef zoxm_graphics
#define zoxm_graphics

#ifdef zox_opengl
    #include "opengl/_.c"
#elif zox_vulkan
    #include "vulkan/_.c"
#else
    #include "headless/_.c"
#endif

zox_begin_module(Graphics) {
#ifdef zox_opengl
    zox_import_module(Opengl);
#elif zox_vulkan
    zox_import_module(Vulkan);
#else
    zox_import_module(Headless);
#endif
} zox_end_module(Headless);

#endif
