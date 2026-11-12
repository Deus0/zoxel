#include "anchor_size.c"
#include "layout2.c"
#include "layout3.c"
zoxc_system_dirty(LayoutPosition);
zoxc_system_dirty(LayoutSize);
// TODO: We probably need a frame by frame, parent to child system, atm it just pushes it all at once, creates race issues
// NOTE: Anchor Size for stretching along canvas, must work before the positioning

void zox_systems_layouts2(ecs* world) {
    zoxd_system_dirty(LayoutPosition);
    zoxd_system_dirty(LayoutSize);
    /*zox_system(
        layout_position_dirty_system,
        zoxp_remove,
        [none] layouts.LayoutPositionDirty,
    );
    zox_system(
        layout_position_update_system,
        zoxp_remove,
        [none] layouts.LayoutPositionUpdate,
    );*/
    // NOTE: Used atm for render texture
    zox_system(
        AnchorSizeSystem,
        zoxp_update,
        [in] layouts.AnchorSize,
        [out] layouts.LayoutSize,
        [none] layouts.LayoutSizeUpdate,
    );
    // For Canvas basically
    zox_system(
        LayoutSystem,
        zoxp_layouts,
        [in] layouts.LayoutSize,
        [none] layouts.Canvas,
        [none] !transforms2.LocalPosition2,
        [none] !layouts.Canvas3,
    );
    // multi threaded: each canvas3 root writes its own children, zoxp_layouts_sync has trails done first
    zox_system(
        Layout3System,
        zoxp_layouts,
        [in] layouts.LayoutSize,
        [none] layouts.Canvas3,
        [none] transforms3.Position3D,  // for our syncing
        [none] !transforms3.LocalPosition3D,
    );
}
