#include "mesh_clone_sync.c"
#include "settings.c"

void zox_events_rendering3(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, app_initialize_rendering3);
    zox_on_add(
        rendering3_settings_observer_callback,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Update,
    );
    // when a cloned mesh is rebuilt, copy its data into its clones
    zox_on_add(
        mesh_clone_sync_event,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices,
        [in] rendering.MeshColorRGBs,
        [none] rendering.MeshDirty,
        [none] rendering3.MeshCloned,
    );
}
