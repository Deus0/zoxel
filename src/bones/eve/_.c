#include "settings.c"

void zox_events_bones(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, zox_settings_bones);
    zox_on_add(
        zox_settings_dirty_bones,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Update,
    );
}