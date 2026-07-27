#include "texture_size.c"
#include "texture_size_generate.c"
#include "mesh.c"
#include "canvas_resize.c"
#include "element_begin.c"

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
        LayoutMeshBeginSystem,
        zoxp_update,
        [in] core.Initialize,
        [in] layouts2.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        [out] rendering.MeshDirty,
    );
    zox_system(
        LayoutMeshUpdateSystem,
        zoxp_update,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        [out] rendering.MeshDirty
    );
}
