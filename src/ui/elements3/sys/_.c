#include "ui_trail.c"
#include "elementbar3.c"
#include "spawn_text.c"
#include "billboards.c"
#include "mesh.c"

void define_systems_elements3D(ecs *world) {
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
        [in] layouts.LayoutSizeDirty,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices,
        [none] !core.Initialize,
    );
    // m ainthread to sync point it
    zox_system_1(
        trail_system,
        zoxp_trails,
        [in] ui.UIHolderLink,
        [in] UITrail,
        [out] transforms3.Position3D
    );
    zox_filter(
        billboard_cameras,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [none] cameras.Camera3
    );
    // to sync it
    zox_system_ctx_1(
        billboard_system,
        zoxp_billboards,
        billboard_cameras,
        [in] rendering.RenderDisabled,
        [in] transforms3.Position3D,
        [out] transforms3.Rotation3D,
        [none] cameras.ElementBillboard
    );
}
