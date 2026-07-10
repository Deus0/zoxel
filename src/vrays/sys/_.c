#include "chunk3_raycast.c"
#include "raycast_gizmo.c"

void define_systems_vrays(ecs* world) {
    zox_system(
        Chunk3RaycastSystem,
        zoxp_update,
        [in] terrains.TerrainLink,
        [in] vrays.RaycastRange,
        [out] vrays.RaycastVoxelData
    );
    zox_system_1(
        RaycastGizmoSystem,
        zoxp_mainthread, // EcsPreStore,
        [in] vrays.RaycastVoxelData,
        [in] vrays.RaycastType,
        [out] vrays.GizmoLink
    );
}
