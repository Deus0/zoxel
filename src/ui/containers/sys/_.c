#include "click.c"

void define_systems_ui_containers(ecs* world) {
    zox_system(
        DataFrameClickSystem,
        EcsOnUpdate,
        [in] elements.ClickState,
        [out] u.i.containers.DataLink,
        [none] u.i.containers.DataFrame
    );
}
