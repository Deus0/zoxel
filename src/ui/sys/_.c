#include "texture_size.c"
#include "texture_size_generate.c"
#include "mesh.c"
#include "canvas_resize.c"
#include "element_begin.c"
#include "element_renderer.c"
#include "render_transform.c"

void zox_define_systems_elements(ecs *world) {
    zox_system(
        CanvasResizeSystem,
        zoxp_update,
        [in] cameras.ScreenToCanvas,
        [in] apps.AppLink,
        [out] layouts2.LayoutPosition,
        [out] layouts2.LayoutSize,
        [out] layouts2.LayoutPositionDirty,
        [out] layouts2.LayoutSizeDirty,
        [none] layouts2.Canvas
    );
    zox_system(
        TextureSizeSystem,
        zoxp_update,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.LayoutSize,
        [out] rendering.TextureSize,
        [none] textures.FixToLayout
    );
    zox_system(
        TextureSizeGenerateSystem,
        zoxp_update,
        [in] layouts2.LayoutSizeDirty,
        [out] textures.GenerateTexture
    );
    zox_system(
        LayoutMeshSystem,
        zoxp_update,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        [out] rendering.MeshDirty
    );
    // all ui
    zox_render2D_system(
        ElementRenderSystem,
        [in] transforms2.Position2,
        [in] transforms2.Rotation2D,
        [in] transforms.Scale1,
        [in] layouts2.Layer2D,
        [in] rendering.RenderDisabled,
        [in] rendering.Brightness,
        [in] rendering.Alpha,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.TextureGPULink,
        [none] ElementRender,
        [none] !transforms.TransformMatrix
    );
    add_system_process_counter(world, zox_id(ElementRenderSystem));
    // Render using Matrix instead of Position2 etc
    zox_render2D_system(
        ElementRenderMatrixSystem,
        [in] transforms.TransformMatrix,
        [in] layouts2.Layer2D,
        [in] rendering.RenderDisabled,
        [in] rendering.Brightness,
        [in] rendering.Alpha,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.TextureGPULink,
        [none] ElementRender
    );
    // NOTE: Mainthread due to gpu components
    /*zox_system_1(
        ElementBeginSystem,
        zoxp_mainthread,
        [in] core.InitializeEntity,
        [in] layouts2.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        [out] rendering.MeshDirty,
        [none] Element
    );*/
    /*zox_system_1(
        TextureDirtyBeginSystem,
        zoxp_mainthread,
        [in] core.InitializeEntity,
        [out] rendering.TextureDirty
    );
    zox_system_1(
        TextureGpuBeginSystem,
        zoxp_mainthread,
        [in] core.InitializeEntity,
        [out] rendering.TextureGPULink
    );*/
}
