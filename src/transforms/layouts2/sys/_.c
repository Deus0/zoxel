#include "layout.c"
#include "position2.c"
#include "anchor_size.c"
#include "transform.c"
#include "canvas.c"

// TODO: We probably need a frame by frame, parent to child system, atm it just pushes it all at once, creates race issues
// NOTE: Anchor Size for stretching along canvas, must work before the positioning

void define_systems_layouts2(ecs* world) {
    zox_system(
        AnchorSizeSystem,
        EcsOnLoad,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.AnchorSize,
        [out] layouts2.LayoutSize
    );
    zox_system(
        LocalLayoutSystem,
        EcsOnLoad,
        [in] LayoutPositionDirty,
        [in] LayoutPosition,
        [in] LayoutSize,
        [in] Anchor,
        [out] CanvasPosition
    );
    zox_system(
        LayoutToCanvasSystem,
        EcsOnLoad,
        [in] layouts2.LayoutPositionDirty,
        [in] layouts2.CanvasLink,
        [in] layouts2.CanvasPosition,
        [out] transforms2.Position2
    );
    // Transform Layouts to Real Space
    zox_system(
        LayoutTransform2System,
        EcsOnUpdate,
        [in] layouts2.LayoutPositionDirty,
        [in] transforms2.Position2,
        [in] transforms.Scale1D,
        [out] transforms.TransformMatrix
    );
    zox_system(
        CanvasLinkSystem,
        EcsOnLoad,
        [out] layouts2.CanvasLink,
        // [none] layouts2.Layout
    );
}
