#include "settings.c"

void zox_events_ai(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, ai_settings);
    zox_on_add(
        ai_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Update,
    );
}