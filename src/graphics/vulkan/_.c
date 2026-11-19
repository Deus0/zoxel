/*
 * +--------------------------------------------------------------+
 * | Zox Module: Vulkan                                           |
 * |                                                              |
 * |  Device - Frames - Shaders - Pipelines - Draws              |
 * |                                                              |
 * |  Uses: Apps (the sdl window makes the surface)               |
 * |                                                              |
 * +--------------------------------------------------------------+
 *  The renderer's zox_gpu_* calls (the opengl module's) on vulkan. A frame's command buffer opens at the first
 *  call that draws and is submitted and presented at app_gpu_end_system. Calls not ported yet: fun/todo.c
 */
#include "dat/_.c"
#include "set/_.c"
#include "fun/_.c"
#ifdef zox_tests
    #include "tst/_.c"
#endif

void dispose_vulkan(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    vulkan_dispose();
}

void import_vulkan(ecs* world) {
    zox_module(vulkan);
    // no render textures yet (part 4 of doc/plans/vulkan.md): cameras draw straight to the screen, before the
    // camera prefabs are made
    zox_disable_post_processing = 1;
    zox_module_dispose(dispose_vulkan);
#ifdef zox_tests
    zox_tsts_vulkan();
#endif
}
