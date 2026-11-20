/*
 *  Zoxel Graphics
 *
 *      - Imports one of the sub modules based on compiler
 *
 * */
byte zox_render_lines = 0;
#include "dat/opengl_mode.c"

#ifdef zox_opengl
    #include "opengl/_.c"
#elif zox_vulkan
    #include "vulkan/_.c"
#else
    #include "headless/_.c"
#endif
#ifndef zox_vulkan
// a render system, run now on this thread (vulkan can split parallel ones across threads, fun/workers.c)
static inline void zox_gpu_run_system(ecs* world, entity system, byte parallel) {
    (void) parallel;
    zox_run(world, system);
}
#endif
#include "fun/screenshot.c"
#include "fun/gif.c"

void import_graphics(ecs* world) {
    zox_module(graphics);
#ifdef zox_opengl
    zox_add_module(opengl);
#elif zox_vulkan
    zox_add_module(vulkan);
#else
    zox_add_module(headless);
#endif
}
