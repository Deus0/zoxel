#include "anchor_size.c"
#include "layout2.c"
#include "layout3.c"
// TODO: We probably need a frame by frame, parent to child system, atm it just pushes it all at once, creates race issues
// NOTE: Anchor Size for stretching along canvas, must work before the positioning

void define_systems_layouts2(ecs* world) {
    /*zox_system(
        AnchorSizeSystem,
        zoxp_update,
        [in] layouts.LayoutSizeDirty,
        [in] layouts.AnchorSize,
        [out] layouts.LayoutSize
    );*/
    // For Canvas basically
    zox_system(
        LayoutSystem,
        zoxp_transforms - 1,
        [in] layouts.LayoutSize,
        [none] layouts.Canvas,
        [none] !transforms2.LocalPosition2,
        [none] !layouts.Canvas3,
    );
    zox_system(
        Layout3System,
        zoxp_transforms - 1,
        [in] layouts.LayoutSize,
        [none] layouts.Canvas3,
        [none] !transforms3.LocalPosition3D
    );
}
