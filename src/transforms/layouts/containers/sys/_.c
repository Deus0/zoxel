#include "list.c"
#include "grid.c"
#include "list_element.c"

void zox_systems_containers(ecs* world) {
    zox_system(
        ListSystem,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] containers.ListPositionDirty,
        [in] containers.ListPadding,
        [in] containers.ListMargins,
        [in] containers.ListStart,
        [in] containers.ListAlignment
    );
    zox_system(
        GridSystem,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] containers.GridDirty,
        [in] containers.GridSize,
        [in] containers.GridPadding,
        [in] containers.GridMargins
    );
    // TODO: We should really tag these? ListElement
    zox_system(
        ListElementPositionSystem,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] layouts.LayoutSizeDirty,
        [out] layouts.LayoutPosition,
        [out] layouts.LayoutPositionDirty
    );
}
