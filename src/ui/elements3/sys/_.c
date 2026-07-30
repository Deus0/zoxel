#include "ui_trail.c"
#include "elementbar3.c"
#include "spawn_text.c"
#include "billboards.c"
#include "mesh.c"

void define_systems_elements3D(ecs *world) {
    zox_system(
        Layout3MeshBeginSystem,
        zoxp_update,
        [in] core.Initialize,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices,
        [out] rendering.MeshDirty,
    );
    zox_system(
        Layout3MeshUpdateSystem,
        zoxp_update,
        [in] layouts.LayoutSizeDirty,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices,
        [out] rendering.MeshDirty,
        [none] !core.Initialize,
    );
    zox_system(
        Elementbar3DSystem,
        zoxp_update,
        [in] rendering.RenderDisabled,
        [in] elements.ElementBar,
        [in] elements.ElementBarSize,
        [none] rendering.MeshVertices
    );
    /*zox_system_1(
        Text3DResizeSystem,
        zoxp_mainthread,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] zigels.FontOutlineColor,
        [in] zigels.FontFillColor,
        [in] zigels.FontThickness,
        [in] zigels.FontOutlineThickness,
        [in] rendering.RenderDisabled,
        [in] Text3DScale,
        [in] texts.TextFontSize,
        [none] texts.Zext,
        [none] Text3D
    );*/
    zox_system(
        UITrailSystem,
        zoxp_update,
        [in] elements.UIHolderLink,
        [in] UITrail,
        [out] transforms3.Position3D
    );
    zox_filter(
        billboard_cameras,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [none] cameras.Camera3
    );
#ifdef zox_debug_billboard_system
    zox_system_ctx_1(
#else
    zox_system_ctx(
#endif
        BillboardSystem,
        zoxp_update,
        billboard_cameras,
        [in] rendering.RenderDisabled,
        [in] transforms3.Position3D,
        [out] transforms3.Rotation3D,
        [none] cameras.ElementBillboard
    );
}
