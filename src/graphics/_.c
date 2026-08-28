/*
 *  Zoxel Graphics
 *
 *      - Imports one of the sub modules based on compiler
 *
 * */
byte zox_render_lines = 0;

#ifdef zox_opengl
    #include "opengl/_.c"
#elif zox_vulkan
    #include "vulkan/_.c"
#else
    #include "headless/_.c"
#endif

void import_graphics(ecs* world) {
    zox_module(graphics);
#ifdef zox_opengl
    zox_import_module(Opengl);
#elif zox_vulkan
    zox_import_module(Vulkan);
#else
    zox_import_module(Headless);
#endif
}