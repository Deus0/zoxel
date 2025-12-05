#ifndef zox_game
    #define zox_game zoxel
#endif
#define str_macro(x) #x
#define inc_nexus_game(x) str_macro(nexus/x/_.c)
#define zox_nexus_game inc_nexus_game(zox_game)
#define zox_glitch_fix_hierarchy_labels
#define zox_set_camera_firstperson
#define zoxel_time_main_loop_cutoff 33.33f
// release defines
#ifndef zox_debug
    #define zox_disable_names
    #define zox_disable_logs
#endif

#include "includes.c"

// engine modules
#include "_/logs/_.c"
#include "_/collections/_.c"
#include "_/maths/_.c"
#include "flecs/_.c"

#include "modules.c"

// event used by testers atm
typedef byte (*boot_zox)(ecs*, entity);
boot_zox boot_event;

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
byte run_engine(int argc, char* argv[]) {
#ifndef zoxm_game
    zox_log_error("[zoxm_game] not defined: game cannot load");
    return EXIT_FAILURE;
#endif

    zox_logv("Initializing Flecs");
    fetch_pc_info();    // gets our cpu core count
    ecs *world = initialize_ecs(argc, argv, cpu_core_count);
    if (!world) {
        zox_log_error("[initialize_ecs] failed");
        return EXIT_FAILURE;
    }

    zox_logv("Initializing Zox Engine");
    zox_import_module(Zox);

    zox_logv("Initializing Game");
    zox_import_module(ZoxGame);

    zox_logv("Processing Terminal Commands");
    run_hook_terminal_command(world, argv, argc);

    zox_logv("Initialize Pathing");
    if (initialize_pathing() == EXIT_FAILURE) {
        zox_log_error("Pathing Setup Failed.");
        return EXIT_FAILURE;
    }

    zox_logv("Initializing ECS Settings: FPS [%i]", (int) target_fps);
    initialize_ecs_settings(world, target_fps); // sets ecs threads

    if (!nosounds) {
        zox_logv("Initializing Sounds");
        initialize_sounds();                       // starts sdl mixer etc
    } else {
        zox_logw("Sounds are disabled");
    }

    zox_logv("Spawning Prefabs");
    run_hook_spawn_prefabs(world);

    zox_logv("Spawning Realm");
    const entity realm = spawn_realm(world, prefab_realm);

    // TODO: attach Loaded Files to Realm
    zox_logv("Loading Files");
    run_hook_files_load(world);

    zox_logv("Spawning Game");
    const entity game = spawn_game(world, realm);

    // spawn app (creates our opengl context too)
    zox_logv("Spawning App");
    const entity app = spawn_engine_app(world);

    if (app) {
        zox_set(app, RealmLink, { realm });
        zox_set(app, GameLink, { game });

        zox_logv("Initializing Rendering");
        initialize_rendering(render_backend);

        zox_logv("Initializing Window Stuff");
        spawn_window_icon(world, app, "game.png");
        set_vsync(vsync);

        zox_logv("Loading Shaders");
        load_files_shaders(world);

        zox_logv("Processing Shaders");
        process_shaders(world);
    }

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

int main(int argc, char* argv[]) {
    return run_engine(argc, argv);
}

#ifdef zox_windows

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    int argc = 0;
    wchar_t** argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
    // Array to hold arguments
    static char argv_storage[max_args][max_arg_len];
    static char* argv[max_args];
    for (int i = 0; i < argc && i < max_args; i++) {
        // Convert wide char string to char string
        wcstombs(argv_storage[i], argvW[i], max_arg_len);
        argv_storage[i][max_arg_len - 1] = '\0'; // Ensure null termination
        argv[i] = argv_storage[i];
    }
    LocalFree(argvW); // Free memory allocated by CommandLineToArgvW
    return main(argc, argv);
}

#endif
