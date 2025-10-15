#include "chunk3_raycast.c"
#include "raycast_gizmo.c"

void define_systems_vrays(ecs* world) {
    zox_system(
        Chunk3RaycastSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [in] terrain.TerrainLink,
        [in] vrays.RaycastRange,
        [out] vrays.RaycastVoxelData
    );
    zox_system_1(
        RaycastGizmoSystem,
        EcsPreStore,
        [in] vrays.RaycastVoxelData,
        [out] vrays.GizmoLink
    );
}
