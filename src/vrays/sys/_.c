#include "chunk3_raycast.c"
#include "raycast_gizmo.c"

void zox_systems_vrays(ecs* world) {
    zox_system(
        chunk3_raycast_system,
        zoxp_update,
        [in] vrays.RaycastRange,
        [out] vrays.RaycastVoxelData
    );
    zox_system_1(
        raycast_gizmo_system,
        zoxp_spawn,
        [in] vrays.RaycastVoxelData,
        [in] vrays.RaycastType,
        [none] vrays.GizmoOwner,
    );
}
