#include "texture_size.c"
#include "texture_size_generate.c"
#include "mesh.c"
#include "canvas_resize.c"
#include "element_begin.c"
#include "texture_dirty_begin.c"
#include "texture_gpu_begin.c"
#include "element_renderer.c"

void zox_define_systems_elements(ecs *world) {
    zox_system(
        CanvasResizeSystem,
        EcsOnUpdate,
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
        EcsPreUpdate,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.LayoutSize,
        [out] rendering.TextureSize,
        [none] textures.FixToLayout
    );
    zox_system(
        TextureSizeGenerateSystem,
        EcsPreUpdate,
        [in] layouts2.LayoutSizeDirty,
        [out] textures.GenerateTexture
    );
    zox_system(
        LayoutMeshSystem,
        EcsPostUpdate,
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
        [in] transforms.Scale1D,
        [in] layouts2.Layer2D,
        [in] rendering.RenderDisabled,
        [in] rendering.Brightness,
        [in] rendering.Alpha,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.TextureGPULink,
        [none] ElementRender,
        [none] !cameras.RenderTexture
    );
    add_system_process_counter(world, zox_id(ElementRenderSystem));
    // NOTE: Mainthread due to gpu components
    zox_system_1(
        ElementBeginSystem,
        EcsPostLoad,
        [in] core.InitializeEntity,
        [in] layouts2.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshDirty,
        [out] rendering.MeshVertices2D,
        [out] rendering.MeshGPULink,
        [out] rendering.UvsGPULink,
        [none] Element
    );
    zox_system_1(
        TextureDirtyBeginSystem,
        EcsPostLoad,
        [in] core.InitializeEntity,
        [out] rendering.TextureDirty
    );
    zox_system_1(
        TextureGpuBeginSystem,
        EcsPostLoad,
        [in] core.InitializeEntity,
        [out] rendering.TextureGPULink
    );
}
