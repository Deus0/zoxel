#include "chunk3_raycast.c"
#include "raycast_gizmo.c"

void zox_systems_vrays(ecs* world) {
    zox_system(
        Chunk3RaycastSystem,
        zoxp_update,
        [in] vrays.RaycastRange,
        [out] vrays.RaycastVoxelData
    );
    zox_system_1(
        RaycastGizmoSystem,
        zoxp_spawn,
        [in] vrays.RaycastVoxelData,
        [in] vrays.RaycastType,
        [none] vrays.GizmoOwner,
    );
}
