
// zoxel apps: handles os windows, using (sdl, glut) libraries
// TODO: Make a listener event for app booting (Settings)

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
#include "eve/_.c"
#include "sys/_.c"

void import_apps(ecs* world) {
    zox_module(apps);
    define_components_apps(world);
    zox_define_systems_apps(world);
    add_hook_terminal_command(process_arguments_apps);
    add_hook_spawn_prefabs(zox_prefabs_apps);
    add_hook_spawn_prefabs(zox_apps_events);
    if (is_on_phosh()) {
        zox_log("Phosh Detected. Disabling Decor.");
        disable_apps_decor = 1;
        fullscreen = 1;
    }
#ifdef zox_sdl
    zox_import_module(Sdl);
#elif zox_glut
    zox_import_module(Glut);
#endif
}
