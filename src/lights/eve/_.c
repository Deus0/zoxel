// we should just add dirty hook to the settings entities
#include "settings.c"

void zox_events_lights(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, app_initialize_lights);
    zox_on_add(
        lights_settings_observer_callback,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Update,
    );
}
