#include "click.c"
#include "icon.c"

void define_systems_ui_containers(ecs* world) {
    zox_system(
        DataFrameClickSystem,
        EcsOnUpdate,
        [in] elements.ClickState,
        [out] u.i.containers.DataLink,
        [none] u.i.containers.DataFrame
    );
    zox_system(
        SlotDataCleanSystem,
        EcsOnUpdate,
        [out] DataLink,
        [out] DataDirty,
        [none] Slot
    );
    zox_system(
        DataIconSystem,
        EcsOnUpdate,
        [in] SlotLink,
        [out] DataLink,
        [out] DataDirty,
        [none] elements2.Icon
    );
    zox_system(
        DataIconUpdateSystem,
        EcsOnUpdate,
        [in] DataDirty,
        [in] DataLink,
        [none] elements2.Icon
    );
}
