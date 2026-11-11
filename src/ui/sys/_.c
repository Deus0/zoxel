#include "texture_size.c"
#include "texture_size_generate.c"
#include "mesh.c"
#include "canvas_resize.c"
#include "element_begin.c"
#include "settings.c"

void zox_systems_elements(ecs *world) {
    // main thread, it spawns the setting
    zox_system_1(
        UISettingsSystem,
        zoxp_spawn,
        [in] settings.LoadSettings
    );
    zox_system(
        UISettingsDirtySystem,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
    zox_system(
        CanvasResizeSystem,
        zoxp_update,
        [in] cameras.ScreenToCanvas,
        [out] layouts.LayoutPosition,
        [out] layouts.LayoutSize,
        [none] layouts.Canvas
    );
    zox_system(
        TextureSizeSystem,
        zoxp_update,
        [in] layouts.LayoutSize,
        [out] rendering.TextureSize,
        [none] textures.FixToLayout,
        [none] layouts.LayoutSizeUpdate,
    );
    zox_system(
        TextureSizeGenerateSystem,
        zoxp_update,
        [out] textures.GenerateTexture,
        [none] layouts.LayoutSizeUpdate,
    );
    zox_system(
        LayoutMeshBeginSystem,
        zoxp_initialize,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        [none] core.Initialize,
    );
    zox_system(
        LayoutMeshUpdateSystem,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        [none] layouts.LayoutSizeUpdate,
    );
}
