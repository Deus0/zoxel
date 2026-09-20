/*
 * +----------------------------------------------------------+
 * | Zox Module: _                   -The Lowest of the Lows- |
 * |                                                          |
 * |  Flecs - Maths - Data Types - Strings                    |
 * |                                                          |
 * +----------------------------------------------------------+
 *
 *  Notes
 *
 *      - "_" Shall only contain definitions
 *
 *  TODO
 *
 *      - Remove the number of sub folders
 *      - Compile Modules seperately
 *
 */
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
#include "strings/_.c"
#include "pathing/_.c"
#include "types/_.c"
#include "sta/_.c"
#include "octrees/_.c"
#include "windows/_.c"

// Globals
byte zox_disable_process_skips = 0;

// Datasets
zoxel_dynamic_array(plane);
zox_hashmap(byte3_hashmap, entity, 0, byte3, uint, get_byte3_hash)
zox_hashmap(int_hashmap, entity, 0, int, uint,  get_int_hash)
zox_hashmap(int2_hashmap, entity, 0, int2, uint, get_int2_hash)
zox_hashmap(int3_hashmap, entity, 0, int3, uint, get_int3_hash)

// Hooks
zox_hook(files_load, (ecs* world), (world))
zox_hook(spawn_prefabs, (ecs* world), (world))
zox_hook(on_boot, (ecs* world, entity app), (world, app))

// sets up resources path per platform - during preload stage
static inline byte initialize_pathing(const char* game_name) {
    byte pathing_success = EXIT_FAILURE;
#ifdef zox_web
    pathing_success = initialize_pathing_web(
        // game_name,
        &data_path,
        &resources_path,
        &resources_path_game);
#elifdef zox_android
    pathing_success = initialize_pathing_android();
    if (pathing_success == EXIT_SUCCESS) {
        decompress_android_resources(resources_path);
    }
#else
    pathing_success = initialize_pathing_native(
        game_name,
        &data_path,
        &resources_path,
        &resources_path_game);
#endif
    zox_logv("Threads Support [%s]",
        supports_threads() ?
            "YES" :
            "NO");
    return pathing_success;
}

void module_dispose_zox(ecs *world, void* ctx) {
    dispose_hook_terminal_command();
    dispose_game_store();
    dispose_hook_files_load();
    dispose_hook_on_boot();
    dispose_hook_spawn_prefabs();
    dispose_component_ids();
}

void process_arguments_flecs(ecs *world, char* args[], int count) {
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

// Initializes our engine hooks
void initialize_zox(ecs* world) {
    zox_module_dispose(module_dispose_zox);
    clear_logs();
    initialize_hook_terminal_command();
    initialize_hook_files_load();
    initialize_hook_spawn_prefabs();
    initialize_hook_on_boot();
    initialize_component_ids();
    add_hook_terminal_command(process_arguments_flecs);
    add_hook_on_boot(on_boot_game_store);
    set_noise_seed(get_unique_time_seed());
/*#if zox_web
    add_to_update_loop(update_web_canvas);
#endif*/
}
