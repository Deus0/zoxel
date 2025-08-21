#include "parent.c"
#include "position2.c"
zox_increment_system_with_reset(LayoutPositionDirty, zox_dirty_end);
zox_increment_system_with_reset(LayoutSizeDirty, zox_dirty_end);

void define_systems_layouts2(ecs* world) {
    zoxd_system_increment(LayoutPositionDirty);
    zoxd_system_increment(LayoutSizeDirty);
    zox_system(
        LayoutParentPositionSystem,
        EcsOnUpdate,
        [in] LayoutPositionDirty,
        [in] PixelPosition,
        [in] PixelSize,
        [in] Anchor,
        [in] hierarchys.ParentLink,
        [out] CanvasPosition
    );
    zox_system(
        LayoutPosition2System,
        EcsOnUpdate,
        [in] LayoutPositionDirty,
        [in] CanvasPosition,
        [in] CanvasLink,
        [out] transforms2.Position2
    );
}