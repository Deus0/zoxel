#include "defines.c"
#include "includes.c"
#include "_/_.c"
#include "modules.c"

// Why is this here?
// event used by testers atm
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
    zox_log("+ Starting Zox Engine +");
#ifdef zox_android
    __android_log_print(ANDROID_LOG_INFO, "SDL", "Zoxel Android logging TEST");
    zox_log("Log Test 2");
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
    zox_add_module(zoxgame);
    zox_logv("Processing Terminal Commands");
    run_hook_terminal_command(world, argv, argc);
    zox_logv("Initialize Pathing");
    if (initialize_pathing(game_name) == EXIT_FAILURE) {
        zox_loge("Pathing Setup Failed.");
        return EXIT_FAILURE;
    }
    zox_logv("Initializing ECS Settings: FPS [%i]", target_fps);
    initialize_ecs_settings(world, target_fps, cores); // sets ecs threads
    // TODO We should really hide the library used here
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
    entity app;
    // app = spawn_app(world, game_name);
#ifdef zox_xr
    app = spawn_app(world, game_name);
    /*SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "0");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        zox_loge("[XR] SDL_Init failed: %s", SDL_GetError());
        return EXIT_FAILURE;
    }*/
    if (!xr_begin()) {
        zox_loge("[XR] xr_begin failed");
        return EXIT_FAILURE;
    }
#elif defined(zox_sdl)
    app = spawn_engine_app(world, game_name);
    if (app) {
        if (zox_init_glew() == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
        zox_logv("Setting App Icon [game.png]");
        spawn_window_icon(world, app, "game.bmp");
        // FIX: Actually load shaders -> spawns materials...
    }
#endif
#ifndef zox_headless
    zox_logv("Initializing Devices");
    spawn_devices_on_app(world, app);
    zox_logv("Initializing Rendering");
    initialize_rendering(render_backend);
    zox_logv("Loading (Processing) Shaders");
    load_files_shaders(world);
    process_shaders(world);
#else
    // TODO: Move this into headless module
    app = zox_new();
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
    // zox_logv("Running our Boot Hook");
    // run_hook_on_boot(world, app);
// #ifdef zox_xr
    /*entity e2 = spawn_test_vox_at(world, float3_zero, quaternion_identity);
    // zox_set(e2, GenerateModel, { zox_generate_model_run });
    zox_set_parent(world, e2, app);*/
    // Test cube
    // spawn_cube(world, prefab_cube, (float3) { 0, 0, -1 }, 0.25f);
// #endif
    zox_logv("Running Main Loop [%s]", game_name);
    main_loop(world);
    zox_logv("Ended Main Loop [%s]", game_name);
#ifdef zox_xr
    SDL_Quit();
#endif
    return EXIT_SUCCESS;
}
