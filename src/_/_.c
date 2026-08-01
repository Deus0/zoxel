/*
 *  Zox Core
 *
 *      - Maths, Generics, Etc
 *
 *  The Lowest of the Lows
 */
#ifndef zoxm_core
#define zoxm_core

// #define zox_disable_initialize_removal

#ifdef zox_debug
    #ifndef zox_safety_checks
        #define zox_safety_checks
    #endif
#endif

#include "logs/_.c"
#include "collections/_.c"
#include "maths/_.c"
#include "flecs/_.c"
#include "platforms/defines.c"
#include "platforms/_.c"
#include "terminals/_.c"
#include "pathing/_.c"
#include "sta/_.c"
#include "hok/_.c"
#include "windows/_.c"
#include "types/_.c"
#include "octrees/_.c"
#include "strings/_.c"
#include "com/_.c"
#include "timing/_.c"
#include "sys/_.c"
#include "settings/_.c"

byte zox_disable_process_skips = 0;

void module_dispose_core(ecs *world, void *ctx) {
    dispose_hook_terminal_command();
    dispose_game_store();
    dispose_hook_files_load();
    dispose_hook_on_boot();
    dispose_hook_spawn_prefabs();
    dispose_component_ids();
}

void process_arguments_core(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (!strcmp(args[i], "--fps")) {
            target_fps = (byte) (atoi(args[i + 1]));
            i++;
            zox_logv("Target FPS [%i]", target_fps);
        } else if (!strcmp(args[i], "--singlethread")) {
            zox_disable_threads = 1;
            zox_logv("Threading Disabled");
        } else if (!strcmp(args[i], "-p") || !strcmp(args[i], "--profiler")) {
            profiler = 1;
            zox_logv("Profiler Enabled");
        }
    }
}

// sets up resources path per platform - during preload stage
byte initialize_pathing(const char* game_name) {
    byte pathing_success = EXIT_FAILURE;
#ifdef zox_android
    pathing_success = initialize_pathing_android();
#else
    pathing_success = initialize_pathing_native(game_name);
#endif
    zox_logv("Threads Support [%s]", supports_threads() ? "YES" : "NO");
    return pathing_success;
}

zox_begin_module(Core) {
    clear_logs();
    initialize_update_loop();
    initialize_post_update_loop();
    initialize_hook_terminal_command();
    initialize_hook_files_load();
    initialize_hook_spawn_prefabs();
    initialize_hook_on_boot();
    initialize_component_ids();
    // hooks
    add_hook_terminal_command(process_arguments_core);
    add_hook_on_boot(on_boot_game_store);
    set_noise_seed(get_unique_time_seed());
    // Headless UI
    zox_module_dispose(module_dispose_core);
#if zox_web
    add_to_update_loop(update_web_canvas);
#endif
    // components
    zox_define_components_core(world);
    zox_define_systems_core(world);
    // sub modules
    zox_import_module(Timing);
    zox_import_module(Settings);
} zox_end_module(Core);

#endif
