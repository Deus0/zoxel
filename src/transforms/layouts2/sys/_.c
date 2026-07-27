#include "anchor_size.c"
#include "new.c"
// TODO: We probably need a frame by frame, parent to child system, atm it just pushes it all at once, creates race issues
// NOTE: Anchor Size for stretching along canvas, must work before the positioning

void define_systems_layouts2(ecs* world) {
    zox_system(
        AnchorSizeSystem,
        zoxp_update,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.AnchorSize,
        [out] layouts2.LayoutSize
    );
    // For Canvas basically
    zox_system(
        LayoutSystem,
        zoxp_update,
        [in] layouts2.LayoutSize,
        [none] !transforms2.LocalPosition2
    );
}
