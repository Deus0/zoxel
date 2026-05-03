#include "parent.c"
#include "position2.c"
#include "anchor_size.c"
#include "list.c"
#include "grid.c"
#include "list_element.c"
#include "transform.c"

// TODO: We probably need a frame by frame, parent to child system, atm it just pushes it all at once, creates race issues

void define_systems_layouts2(ecs* world) {
    // NOTE: Anchor Size for stretching along canvas, must work before the positioning
    zox_system(
        AnchorSizeSystem,
        EcsOnLoad,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.AnchorSize,
        [in] hierarchys.ParentLink,
        [out] layouts2.LayoutSize
    );
    zox_system(
        LayoutParentPositionSystem,
        EcsOnLoad, // + 1,
        [in] LayoutPositionDirty,
        [in] LayoutPosition,
        [in] LayoutSize,
        [in] Anchor,
        [in] hierarchys.ParentLink,
        [out] CanvasPosition
    );
    zox_system(
        LayoutPosition2System,
        EcsOnLoad, // + 2,
        [in] layouts2.LayoutPositionDirty,
        [in] layouts2.CanvasPosition,
        [in] layouts2.CanvasLink,
        [out] transforms2.Position2
    );
    zox_system(
        LayoutTransform2System,
        EcsOnUpdate,
        [in] layouts2.LayoutPositionDirty,
        [in] transforms2.Position2,
        [in] transforms.Scale1D,
        [out] transforms.TransformMatrix
    );


    zox_system(
        GridSystem,
        EcsOnUpdate,
        [in] layouts2.GridDirty,
        [in] hierarchys.Children,
        [in] layouts2.LayoutSize,
        [in] layouts2.GridSize,
        [in] layouts2.GridPadding,
        [in] layouts2.GridMargins
    );


    zox_system(
        ListSystem,
        EcsOnUpdate,
        [in] layouts2.ListPositionDirty,
        [in] hierarchys.Children,
        [in] layouts2.LayoutSize,
        [in] layouts2.ListPadding,
        [in] layouts2.ListMargins,
        [in] layouts2.ListStart,
        [in] layouts2.ListAlignment
    );

    zox_system(
        ListElementPositionSystem,
        EcsOnUpdate,
        [in] hierarchys.ParentLink,
        [in] layouts2.LayoutSize,
        [in] layouts2.LayoutSizeDirty,
        [out] layouts2.LayoutPosition,
        [out] layouts2.LayoutPositionDirty
    );
}
