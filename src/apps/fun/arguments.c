void process_arguments_apps(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "-z") == 0 || strcmp(args[i], "--headless") == 0) {
            headless = 1;
        } else if (strcmp(args[i], "-w") == 0 || strcmp(args[i], "--windowed") == 0) {
            fullscreen = 0;
        } else if (strcmp(args[i], "-f") == 0 || strcmp(args[i], "--fullscreen") == 0) {
            fullscreen = 1;
            fullscreen_override = 1;
        } else if (strcmp(args[i], "-g") == 0 || strcmp(args[i], "--halfscreen") == 0) {
            halfscreen = 1;
        } else if (strcmp(args[i], "-s") == 0 || strcmp(args[i], "--splitscreen") == 0) {
            is_split_screen = 1;
        } else if (strcmp(args[i], "--vsync") == 0 || strcmp(args[i], "--enablevsync") == 0) {
            vsync = 1;
            zox_log("Enabled [vsync]");
        } else if (strcmp(args[i], "--disablevsync") == 0 || strcmp(args[i], "--novsync") == 0) {
            vsync = 0;
            zox_log("Disabled [vsync]");
        } else if (strcmp(args[i], "--background") == 0) {
            background = 1;
            fullscreen = 0;
            maximized = 0;
            nosounds = 1;
            // silent: tests turn sounds on to check them (music_plays), the device opened before args
#ifdef zox_sdl
            if (audio_enabled) {
                zox_dispose_audio();
                audio_enabled = 0;
            }
#endif
            zox_log("+ setting enabled [background]");
        } else if (strcmp(args[i], "--sounds") == 0) {
            // after --background, i.e. tests that listen (bash bsh/tst.sh <name> --sounds)
            nosounds = 0;
#ifdef zox_sdl
            if (!audio_enabled) {
                initialize_sounds();
            }
#endif
            zox_log("+ setting enabled [sounds]");
        } else if (strcmp(args[i], "--nodecor") == 0) {
            disable_apps_decor = 1;
        } else if (strcmp(args[i], "--1080p") == 0) {
            window_size_override = (int2) { 1920, 1080 };
            fullscreen = 0;
            maximized = 0;
        } else if (strcmp(args[i], "--nohud") == 0) {
            nohud = 1;
        }
#ifdef zox_sdl
        else if (strcmp(args[i], "--gles") == 0) {
            // skips the core probe, i.e. tests on mesa use the same es path as mali
            opengl_mode = zox_opengl_es;
            zox_log("+ setting enabled [gles]");
        }
#endif
    }
}