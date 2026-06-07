#ifndef zoxm_apps
#define zoxm_apps

// zoxel apps: handles os windows, using (sdl, glut) libraries
#include "com/_.c"
#include "set/_.c"

#ifdef zox_headless
    #include "headless/_.c"
#endif

#include "pre/_.c"

#ifdef zox_sdl
    #include "sdl/_.c"
#elif zox_glut
    #include "glut/_.c"
#elif zox_glfw
    #include "glfw/_.c"
#endif

#include "fun/_.c"

zox_begin_module(Apps) {
    define_components_apps(world);
    add_hook_terminal_command(process_arguments_apps);
    add_hook_files_load(load_files_settings);
    add_hook_spawn_prefabs(spawn_prefabs_apps);
    // add_hook_spawn_prefabs(initialize_settings_apps);
    initialize_settings_apps(world);
#ifdef zox_sdl
    zox_import_module(Sdl);
#elif zox_glut
    zox_import_module(Glut);
#endif
} zox_end_module(Apps);

#endif
