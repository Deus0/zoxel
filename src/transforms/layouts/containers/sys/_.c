#include "list.c"
#include "grid.c"
#include "list_element.c"
zoxc_system_dirty(Grid);
zoxc_system_dirty(List);
zoxc_system_dirty(ListPosition);

void zox_systems_containers(ecs* world) {
    zoxd_system_dirty(Grid);
    zoxd_system_dirty(List);
    zoxd_system_dirty(ListPosition);
    zox_system(
        list_system,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] containers.ListPadding,
        [in] containers.ListMargins,
        [in] containers.ListStart,
        [in] containers.ListAlignment,
        [none] containers.ListPositionUpdate,
    );
    zox_system(
        grid_system,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] containers.GridSize,
        [in] containers.GridPadding,
        [in] containers.GridMargins,
        [none] containers.GridUpdate,
    );
    // TODO: We should really tag these? ListElement
    zox_system(
        list_element_position_system,
        zoxp_update,
        [in] layouts.LayoutSize,
        [out] layouts.LayoutPosition,
        [none] layouts.LayoutSizeUpdate,
        // [none] layouts.LayoutPositionUpdate,
    );
}
