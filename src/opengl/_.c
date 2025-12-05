// OpenGL Wrapper for Zoxel
#ifndef zoxm_opengl
#define zoxm_opengl

byte zox_use_post_processing = 1;
#include "dat/_.c"
#include "fun/_.c"
#include "tst/_.c"

// uses modules: App, SDL, Rendering
entity spawn_engine_app(ecs* world) {
    if (!headless) {
        zox_logv("Initializing SDL Video");
        if (initialize_sdl_video() == EXIT_FAILURE) {
            zox_log_error("[initialize_sdl_video] failed");
            dispose_zox(world);
            return 0;
        }

        // Window creates and binds OpenGL Context too!
        zox_logv("Spawning SDL Window");
        const char* window_name;
#ifdef zox_game
        window_name = game_name;
#else
        window_name = "unknown";
#endif
        entity app = spawn_app_sdl_opengl(
            world,
            window_name,
            fullscreen,
            maximized,
            monitor
        );
        if (!app) {
            zox_log_error("[engine_spawn_window] failed");
            return 0;
        }

        // Link to a global
        main_app = app;

        zox_logv("Initializing Glew");
        if (zox_init_glew() == EXIT_FAILURE) {
            zox_log_error("[initialize_rendering] failed at [zox_init_glew]");
            return app;
        }

        return app;
    } else  {
        zox_logv("Headless Mode Enabled.");
        return 0;
    }
}

#endif
