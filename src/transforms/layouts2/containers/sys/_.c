#include "list.c"
#include "grid.c"
#include "list_element.c"

void define_systems_containers(ecs* world) {
    zox_system(
        ListSystem,
        zoxp_update,
        [in] layouts2.LayoutSize,
        [in] containers.ListPositionDirty,
        [in] containers.ListPadding,
        [in] containers.ListMargins,
        [in] containers.ListStart,
        [in] containers.ListAlignment
    );
    zox_system(
        GridSystem,
        zoxp_update,
        [in] layouts2.LayoutSize,
        [in] containers.GridDirty,
        [in] containers.GridSize,
        [in] containers.GridPadding,
        [in] containers.GridMargins
    );
    // TODO: We should really tag these? ListElement
    zox_system(
        ListElementPositionSystem,
        zoxp_update,
        [in] layouts2.LayoutSize,
        [in] layouts2.LayoutSizeDirty,
        [out] layouts2.LayoutPosition,
        [out] layouts2.LayoutPositionDirty
    );
}
