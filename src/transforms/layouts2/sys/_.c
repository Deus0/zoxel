#include "parent.c"
#include "position2.c"
#include "anchor_size.c"
#include "list.c"

zox_increment_system_with_reset(LayoutPositionDirty, zox_dirty_end);
zox_increment_system_with_reset(LayoutSizeDirty, zox_dirty_end);
zox_increment_system_with_reset(ListDirty, zox_dirty_end);

void define_systems_layouts2(ecs* world) {
    zoxd_system_increment(LayoutPositionDirty);
    zoxd_system_increment(LayoutSizeDirty);
    zoxd_system_increment(ListDirty);
    zox_system(
        LayoutParentPositionSystem,
        EcsOnLoad,
        [in] LayoutPositionDirty,
        [in] LayoutPosition,
        [in] LayoutSize,
        [in] Anchor,
        [in] hierarchys.ParentLink,
        [out] CanvasPosition
    );
    zox_system(
        LayoutPosition2System,
        EcsOnLoad,
        [in] LayoutPositionDirty,
        [in] CanvasPosition,
        [in] CanvasLink,
        [out] transforms2.Position2
    );
    zox_system(
        AnchorSizeSystem,
        EcsOnLoad,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.AnchorSize,
        [in] hierarchys.ParentLink,
        [out] layouts2.LayoutSize
    );

    zox_system(
        ListSystem,
        EcsOnUpdate,
        [in] layouts2.ListDirty,
        [in] hierarchys.Children,
        [in] layouts2.LayoutSize,
        [in] layouts2.ListPadding,
        [in] layouts2.ListMargins,
        [in] layouts2.ListStart
    );
}