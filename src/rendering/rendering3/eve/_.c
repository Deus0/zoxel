#include "mesh_clone_sync.c"

void zox_events_rendering3(ecs* world) {
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
