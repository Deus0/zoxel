#include "ui_trail.c"
#include "render.c"
#include "elementbar3.c"
#include "spawn_text.c"
#include "billboards.c"

void define_systems_elements3D(ecs *world) {
    zox_system(
        Elementbar3DSystem,
        zoxp_update,
        [in] rendering.RenderDisabled,
        [in] elements.ElementBar,
        [in] elements.ElementBarSize,
        [none] rendering.MeshVertices
    );
    zox_system_1(
        Text3DResizeSystem,
        zoxp_mainthread,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] zigels.FontOutlineColor,
        [in] zigels.FontFillColor,
        [in] zigels.FontThickness,
        [in] rendering.RenderDisabled,
        [in] Text3DScale,
        [in] texts.TextFontSize,
        [none] texts.Zext,
        [none] Text3D
    );
    zox_render3_system(
        2,
        Element3RenderSystem,
        [in] rendering.RenderDisabled,
        [in] transforms.TransformMatrix,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.TextureGPULink,
        [in] rendering.MeshIndicies,
        [none] rendering.SingleMaterial
    );
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
    zox_system_ctx(
        BillboardSystem,
        zoxp_update,
        billboard_cameras,
        [in] rendering.RenderDisabled,
        [in] transforms3.Position3D,
        [out] transforms3.Rotation3D,
        [none] cameras.ElementBillboard
    );
    add_system_process_counter(world, zox_id(BillboardSystem));
}
