#include "boot.c"
#include "settings.c"

void zox_systems_apps(ecs *world) {
    // NOTE: Needs mainthread for GPU settings
    zox_system_1(
        app_boot_system,
        zoxp_spawn,
        [none] apps.App,
        [none] core.Initialize,
    );
}
