#include "texture_size.c"
#include "texture_size_generate.c"
#include "mesh.c"
#include "canvas_resize.c"
#include "element_begin.c"
#include "settings.c"
#include "auto_hide.c"

void zox_systems_elements(ecs *world) {
    zox_system_1(
        auto_hide_system,
        zoxp_spawn,
        [in] AutoHide,
        [in] AutoHideTime
    );
    // main thread, it spawns the setting
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
    // its settings: spawned on the app, applied when one changes (needs prefab_app)
    add_hook_spawn_prefabs(ui_settings_events);
}
