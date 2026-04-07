#include "ui_trail.c"
#include "element_mesh3.c"
#include "element3_renderer.c"
#include "elementbar3.c"
#include "resize.c"

void define_systems_elements3D(ecs *world) {
    zox_system(
        UITrailSystem,
        zox_transforms_stage,
        [in] elements.UIHolderLink,
        [in] UITrail,
        [out] transforms3.Position3D
    );
    zox_system(
        Elementbar3DSystem,
        EcsOnUpdate,
        [in] elements.ElementBar,
        [in] elements.ElementBarSize,
        [in] hierarchys.Children,
        [none] rendering.MeshVertices
    );
    zox_system_1(
        Text3DResizeSystem,
        EcsOnUpdate,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] zigels.FontOutlineColor,
        [in] zigels.FontFillColor,
        [in] zigels.FontThickness,
        [in] rendering.RenderDisabled,
        [in] Text3DScale,
        [in] texts.TextFontSize,
        [out] hierarchys.Children,
        [none] texts.Zext,
        [none] Text3D
    );
    if (!headless) {
        zox_system_1(
            Element3DMeshSystem,
            zoxp_mainthread,
            [out] elements.InitializeElement,
            [out] rendering.MeshDirty,
            [out] rendering.MeshGPULink,
            [out] rendering.UvsGPULink,
            [out] rendering.ColorsGPULink,
            [out] rendering.TextureGPULink,
            [none] Element3D
        );
        zox_render3D_plus_system(
            Element3DRenderSystem,
            [in] transforms.TransformMatrix,
            [in] rendering.MeshGPULink,
            [in] rendering.UvsGPULink,
            [in] rendering.ColorsGPULink,
            [in] rendering.MeshIndicies,
            [in] rendering.TextureGPULink,
            [in] rendering.RenderDisabled,
            [none] rendering.SingleMaterial
        );
    }
}
