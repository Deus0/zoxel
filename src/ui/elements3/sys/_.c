#include "ui_trail.c"
#include "elementbar3.c"
#include "spawn_text.c"
#include "billboards.c"
#include "mesh.c"

void zox_systems_elements3D(ecs *world) {
    zox_system(
        Layout3MeshBeginSystem,
        zoxp_initialize,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices,
        [none] core.Initialize,
    );
    zox_system(
        Layout3MeshUpdateSystem,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices,
        [none] layouts.LayoutSizeUpdate,
        [none] !core.Initialize,
    );
    // multi threaded, zoxp_trails_sync (a barrier) has physics done first
    zox_system(
        trail_system,
        zoxp_trails,
        [in] UITrail,
        [out] transforms3.Position3D
    );
    zox_filter(
        billboard_cameras,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [none] cameras.Camera3
    );
    // multi threaded, zoxp_billboards_sync has the hierarchy done first
    zox_system_ctx(
        billboard_system,
        zoxp_billboards,
        billboard_cameras,
        [in] rendering.RenderDisabled,
        [in] transforms3.Position3D,
        [out] transforms3.Rotation3D,
        [none] cameras.ElementBillboard
    );
}
