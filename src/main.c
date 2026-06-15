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
#ifndef zoxm_game
    zox_log_error("[zoxm_game] not defined: game cannot load");
    return EXIT_FAILURE;
#endif
    zox_logv("Get System Info");
    byte cores = get_cpu_count();    // gets our cpu core count
    set_cpu_tier2(cores);
    zox_logv("Initializing Flecs");
    ecs* world = initialize_ecs(argc, argv);
    if (!world) {
        zox_log_error("[initialize_ecs] failed");
        return EXIT_FAILURE;
    }
    initialize_flecs_profiler(world);
    zox_logv("Initializing Zox Engine");
    zox_import_module(Zox);
    zox_logv("Initializing Game");
    zox_import_module(ZoxGame);
    zox_logv("Processing Terminal Commands");
    run_hook_terminal_command(world, argv, argc);
    zox_logv("Initialize Pathing");
    if (initialize_pathing(game_name) == EXIT_FAILURE) {
        zox_log_error("Pathing Setup Failed.");
        return EXIT_FAILURE;
    }
    zox_logv("Initializing ECS Settings: FPS [%i]", (int) target_fps);
    initialize_ecs_settings(world, target_fps, cores); // sets ecs threads
#ifdef zox_sdl
    zox_logv("Initializing Video");
    if (initialize_video() == EXIT_FAILURE) {
        zox_log_error("[initialize_video] failed");
        return EXIT_FAILURE;
    }
#endif
    // zox_logv("Initializing Sounds");
    // initialize_sounds();
    zox_logv("Spawning Prefabs");
    run_hook_spawn_prefabs(world);
    // TODO: attach Loaded Files to Realm
    zox_logv("Spawning Realm and Loading Assets");
    // TODO: Menu Realm
    // entity realm = spawn_realm(world, prefab_realm);
    // run_hook_files_load(world);
    zox_logv("Spawning Game");
    entity game = spawn_game(world);
    // spawn app (creates our opengl context too)
    zox_logv("Spawning App");
#ifdef zox_sdl
    entity app = spawn_engine_app(world);
    if (app) {
        zox_logv("Initializing Glew");
        if (zox_init_glew() == EXIT_FAILURE) {
            zox_log_error("[initialize_rendering] failed at [zox_init_glew]");
            return EXIT_FAILURE;
        }
        zox_set_parent(world, game, app);
        zox_set(app, GameLink, { game });
        zox_logv("Setting VSync");
        on_set_vsync(vsync);
        zox_logv("Initializing Rendering");
        initialize_rendering(render_backend);
        zox_logv("Setting App Icon [game.png]");
        // spawn_window_icon(world, app, "game.png");
        spawn_window_icon(world, app, "game.bmp");
        // FIX: Actually load shaders -> spawns materials...
        zox_logv("Loading (Processing) Shaders");
        load_files_shaders(world);
        process_shaders(world);
    }
#else
    entity app = 0;
#endif
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
