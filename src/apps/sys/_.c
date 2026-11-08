// #include "boot.c"
#include "settings.c"
zoxc_system_dirty(WindowSize);

void zox_systems_apps(ecs* world) {
    zoxd_system_dirty(WindowSize);
    // NOTE: Needs mainthread for GPU settings
    /*zox_system_1(
        app_boot_system,
        zoxp_spawn,
        [none] apps.App,
        [none] core.Initialize,
    );*/
}
