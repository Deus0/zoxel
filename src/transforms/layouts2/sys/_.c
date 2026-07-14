#include "layout.c"
#include "position2.c"
#include "anchor_size.c"
#include "canvas.c"
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
    zox_system(
        LocalLayoutSystem,
        zoxp_update,
        [in] LayoutPositionDirty,
        [in] LayoutPosition,
        [in] LayoutSize,
        [in] Anchor,
        [out] CanvasPosition
    );
    zox_system(
        LayoutToCanvasSystem,
        zoxp_update,
        [in] layouts2.LayoutPositionDirty,
        [in] layouts2.CanvasPosition,
        [out] transforms2.Position2
    );
    zox_system(
        CanvasLinkSystem,
        zoxp_update,
        [out] layouts2.CanvasLink,
        [none] layouts2.Layout
    );
}
