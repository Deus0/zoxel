#ifndef zoxm_rendering
#define zoxm_rendering

byte zox_new_ui_renderer = 0;
ushort max_layers2D = 254;
#include "com/_.c"
#include "dat/_.c"
#include "set/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "shd/_.c"
#include "dbg/_.c"
#include "basics2/_.c"
#include "basics3/_.c"
#include "cameras/_.c"

byte initialize_rendering(byte render_backend) {
    if (test_graphics() == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    if (render_backend == zox_render_backend_headless) {
        return EXIT_SUCCESS;
    } else if (render_backend == zox_render_backend_opengl) {
        rendering_initialized = 1;
        return EXIT_SUCCESS;
    }
#ifdef zox_vulkan
    else if (render_backend == zox_render_backend_vulkan) {
        rendering_initialized = 1;
        return initialize_vulkan(); // SDL_WINDOW_VULKAN
    }
#endif
    else {
        zox_log_error("! unknown render_backend")
        return EXIT_FAILURE;
    }
}

void viewport_clear(ecs *world) {
    zox_gpu_set_clear_color(color_to_float4(viewport_clear_color));
    zox_gpu_clear_viewport();
}

zox_begin_module(Rendering) {
    initialize_render_loop();
    initialize_hook_load_shader();
    initialize_gpu_systems();
    // ecs defines
    define_components_rendering(world);
    define_systems_rendering(world);
    // hooks
    add_hook_spawn_prefabs(spawn_prefabs_rendering);
    add_hook_terminal_command(process_arguments_rendering);
    zox_module_dispose(on_module_dispose_rendering)
    // prefab spawning
    zox_import_module(Shaders);
    zox_import_module(Rendering2);
    zox_import_module(Rendering3);
    zox_import_module(RenderingCameras);
    add_to_update_loop(viewport_clear);
} zox_end_module(Rendering);

#endif
