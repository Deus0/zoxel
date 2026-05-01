#ifndef zoxm_vulkan
#define zoxm_vulkan

#include "fun/vulkan_check.c"
#ifdef zox_vulkan
    #include "fun/vulkan_setup.c"
#endif

byte initialize_vulkan() {
    // load shaders, vulkan load shader
    return EXIT_SUCCESS;
}

byte check_vulkan_suppport() {
    const byte vulkan_supported = is_vulkan_supported();
    if (vulkan_supported) {
        zox_log(" > vulkan is supported");
    } else {
        zox_log(" ! vulkan not supported\n");
    }
    return vulkan_supported;
}

void dispose_vulkan(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_vulkan_globals();
}

zox_begin_module(Vulkan)
    zox_module_dispose(dispose_vulkan);
zox_end_module(Vulkan)

#endif
