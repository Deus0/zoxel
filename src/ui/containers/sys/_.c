#include "click.c"
#include "icon.c"
#include "label.c"

void zox_define_systems_ui_containers(ecs* world) {
    zox_system(
        DataFrameClickSystem,
        EcsOnUpdate,
        [in] elements.ClickState,
        [in] slots.SlotLink,
        [out] u.i.containers.DataLink,
        [out] u.i.containers.DataDirty,
        [none] u.i.containers.DataFrame
    );
    zox_system(
        SlotDataCleanSystem,
        EcsOnUpdate,
        [out] DataLink,
        [out] DataDirty,
        [none] slots.Slot
    );
    zox_system(
        DataIconSystem,
        EcsOnUpdate,
        [in] slots.SlotLink,
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
    zox_system(
        DataFrameLabelSystem,
        EcsOnUpdate,
        [in] slots.SlotLink,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] elements2.Label
    );
}
