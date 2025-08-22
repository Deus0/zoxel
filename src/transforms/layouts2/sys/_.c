#include "parent.c"
#include "position2.c"
#include "anchor_size.c"
// #include "old.c"
zox_increment_system_with_reset(LayoutPositionDirty, zox_dirty_end);
zox_increment_system_with_reset(LayoutSizeDirty, zox_dirty_end);

void define_systems_layouts2(ecs* world) {
    zoxd_system_increment(LayoutPositionDirty);
    zoxd_system_increment(LayoutSizeDirty);
    zox_system(
        LayoutParentPositionSystem,
        EcsOnLoad,
        [in] LayoutPositionDirty,
        [in] PixelPosition,
        [in] PixelSize,
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
        [out] layouts2.PixelSize
    );
    /*zox_system(
        ElementPositionSystem,
        EcsOnLoad,
        [in] layouts2.PixelPosition,
        [in] layouts2.PixelSize,
        [in] hierarchys.ParentLink,
        [in] layouts2.Anchor,
        [in] layouts2.CanvasLink,
        [out] transforms2.Position2,
        [out] layouts2.CanvasPosition
    );*/
}