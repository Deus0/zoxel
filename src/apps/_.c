#ifndef zoxm_apps
#define zoxm_apps

// zoxel apps: handles os windows, using (sdl, glut) libraries
#include "com/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "fun/_.c"

#ifdef zox_sdl
    #include "sdl/_.c"
#elif zox_glut
    #include "glut/_.c"
#elif zox_glfw
    #include "glfw/_.c"
#else
    #include "headless/_.c"
#endif

zox_begin_module(Apps)
    define_components_apps(world);
    initialize_settings_apps(world);
    add_hook_terminal_command(process_arguments_apps);
    add_hook_files_load(load_files_settings);
    add_hook_spawn_prefabs(spawn_prefabs_apps);
#ifdef zox_sdl
        zox_import_module(Sdl);
#endif
zox_end_module(Apps)

#endif
