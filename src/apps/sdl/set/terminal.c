void process_terminal_sdl(ecs *world, char* args[], int count) {
#ifdef zox_verbose
    is_verbose = 1;
    zox_logv("setting enabled [verbose]");
#endif
    for (int i = 1; i < count; i++) {
        if (!strcmp(args[i], "--opengles") || !strcmp(args[i], "--es")) {
            opengl_mode = zox_opengl_es;
            zox_logv("+ opengl_mode -> [opengl_es]");
        } else if (strcmp(args[i], "--openglcore") == 0) {
            opengl_mode = zox_opengl_core;
            zox_logv("+ opengl_mode -> [opengl_core]");
        } else if (strcmp(args[i], "--verbose") == 0) {
            is_verbose = 1;
            zox_logv("setting enabled [verbose]");
        }
    }
}
