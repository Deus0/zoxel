#include "settings.c"

void zox_apps_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, zox_settings_apps);
    zox_on_add(
        zox_settings_dirty_apps,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Update,
    );
}