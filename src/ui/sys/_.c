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
        [out] layouts.LayoutPosition,
        [out] layouts.LayoutSize,
        //[out] layouts.LayoutPositionDirty,
        //[out] layouts.LayoutSizeDirty,
        [none] layouts.Canvas
    );
    zox_system(
        TextureSizeSystem,
        zoxp_update,
        [in] layouts.LayoutSizeDirty,
        [in] layouts.LayoutSize,
        [out] rendering.TextureSize,
        [none] textures.FixToLayout
    );
    zox_system(
        TextureSizeGenerateSystem,
        zoxp_update,
        [in] layouts.LayoutSizeDirty,
        [out] textures.GenerateTexture
    );
    zox_system(
        LayoutMeshBeginSystem,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        // [out] rendering.MeshDirty,
        [none] core.Initialize,
    );
    zox_system(
        LayoutMeshUpdateSystem,
        zoxp_update,
        [in] layouts.LayoutSizeDirty,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        // [out] rendering.MeshDirty
    );
}
