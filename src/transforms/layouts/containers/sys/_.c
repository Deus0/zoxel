#include "list.c"
#include "grid.c"
#include "list_element.c"
zoxc_system_dirty(Grid);

void zox_systems_containers(ecs* world) {
    zoxd_system_dirty(Grid);
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
        [in] containers.GridSize,
        [in] containers.GridPadding,
        [in] containers.GridMargins,
        [none] containers.GridUpdate,
    );
    // TODO: We should really tag these? ListElement
    zox_system(
        ListElementPositionSystem,
        zoxp_update,
        [in] layouts.LayoutSize,
        [out] layouts.LayoutPosition,
        [none] layouts.LayoutSizeUpdate,
        // [none] layouts.LayoutPositionUpdate,
    );
}
