#include "texture_size.c"
#include "texture_size_generate.c"
#include "mesh.c"
#include "canvas_resize.c"
#include "element_begin.c"
#include "settings.c"

void zox_systems_elements(ecs *world) {
    // main thread, it spawns the setting
    zox_system_1(
        ui_settings_system,
        zoxp_spawn,
        [in] settings.LoadSettings
    );
    zox_system(
        ui_settings_dirty_system,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
    zox_system(
        canvas_resize_system,
        zoxp_update,
        [in] cameras.ScreenToCanvas,
        [out] layouts.LayoutPosition,
        [out] layouts.LayoutSize,
        [none] layouts.Canvas
    );
    zox_system(
        texture_size_system,
        zoxp_update,
        [in] layouts.LayoutSize,
        [out] rendering.TextureSize,
        [none] textures.FixToLayout,
        [none] layouts.LayoutSizeUpdate,
    );
    zox_system(
        texture_size_generate_system,
        zoxp_update,
        [out] textures.GenerateTexture,
        [none] layouts.LayoutSizeUpdate,
    );
    zox_system(
        layout_mesh_begin_system,
        zoxp_initialize,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        [none] core.Initialize,
    );
    zox_system(
        layout_mesh_update_system,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        [none] layouts.LayoutSizeUpdate,
    );
}
