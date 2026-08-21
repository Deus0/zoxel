#include "defines.c"
#include "includes.c"
#include "_/_.c"
#include "modules.c"

// Why is this here?
// event used by testers atm
typedef byte (*boot_zox)(ecs*, entity);
boot_zox boot_event;
// Includes our game source
#include zox_nexus_game

/*
 * Initialize Flecs
 * Initialize Flecs Modules
 * Spawn our Realm (Assets Manager)
 * Load all our Files
 * Spawn our Game Manager
 * Spawn our SDL Window (OpenGL)
 * Initialize Rendering
*/
int main(int argc, char* argv[]) {
    zox_logv("+ Starting Zox Engine +");
#ifdef zox_android
    __android_log_print(ANDROID_LOG_INFO, "SDL", "Zoxel Android logging TEST");
#endif
#ifndef zoxm_game
    zox_loge("[zoxm_game] not defined: game cannot load");
    return EXIT_FAILURE;
#endif
    byte cores = get_cpu_count();    // gets our cpu core count
    set_cpu_tier2(cores);
    zox_logv("Initializing Flecs");
    ecs* world = initialize_ecs(argc, argv);
    if (!world) {
        zox_loge("[initialize_ecs] failed");
        return EXIT_FAILURE;
    }
    zox_logv("Initializing Zox Engine");
    zox_add_module(zox);
    zox_logv("Initializing Game");
    zox_import_module(ZoxGame);
    zox_logv("Processing Terminal Commands");
    run_hook_terminal_command(world, argv, argc);
    zox_logv("Initialize Pathing");
    if (initialize_pathing(game_name) == EXIT_FAILURE) {
        zox_loge("Pathing Setup Failed.");
        return EXIT_FAILURE;
    }
    zox_logv("Initializing ECS Settings: FPS [%i]", target_fps);
    initialize_ecs_settings(world, target_fps, cores); // sets ecs threads
#ifdef zox_sdl
    if (initialize_sdl(game_name) == EXIT_FAILURE) {
        zox_loge("[initialize_sdl] failed");
        return EXIT_FAILURE;
    }
#endif
    zox_logv("Spawning Prefabs");
    run_hook_spawn_prefabs(world);
    zox_logv("Spawning Realm and Loading Assets");
    zox_logv("Spawning Game");
    entity game = spawn_game(world);
    zox_logv("Spawning App");
#ifdef zox_sdl
    entity app = spawn_engine_app(world, game_name);
    if (app) {
        if (zox_init_glew() == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
        zox_logv("Initializing Rendering");
        initialize_rendering(render_backend);
        zox_logv("Setting App Icon [game.png]");
        spawn_window_icon(world, app, "game.bmp");
        // FIX: Actually load shaders -> spawns materials...
        zox_logv("Loading (Processing) Shaders");
        load_files_shaders(world);
        process_shaders(world);
    }
#else
    // TODO: Move this into headless module
    entity app = zox_new();
    zox_add(app, App);
    zox_set_unique_name(app, "headless_app");
    main_app = app;
#endif
    if (app) {
        zox_set_parent(world, game, app);
        zox_setv(app, GameLink, game);
    }
    // Resource Loading
    run_hook_files_load(world);
    // Yet another Hook
    zox_logv("Running our Boot Hook");
    run_hook_on_boot(world, app);
    if (boot_event && boot_event(world, app) == EXIT_FAILURE) {
        zox_log_error("[boot_event] failed");
        dispose_zox(world);
        return EXIT_FAILURE;
    }
    zox_logv("Running Main Loop [%s]", game_name);
    main_loop(world);
    zox_logv("Ended Main Loop [%s]", game_name);
    return EXIT_SUCCESS;
}
