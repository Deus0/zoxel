#include "click.c"
#include "icon.c"
#include "label.c"

void zox_define_systems_ui_containers(ecs* world) {
    zox_system(
        DataFrameClickSystem,
        EcsOnUpdate,
        [in] interaction.ClickState,
        [in] slots.SlotLink,
        [out] slots.DataLink,
        [out] slots.DataDirty,
        [none] u.i.containers.DataFrame
    );
    zox_system(
        SlotDataCleanSystem,
        EcsOnUpdate,
        [out] slots.DataLink,
        [out] slots.DataDirty,
        [none] slots.Slot
    );
    zox_system(
        DataIconSystem,
        EcsOnUpdate,
        [in] slots.SlotLink,
        [out] slots.DataLink,
        [out] slots.DataDirty,
        [none] elements2.Icon
    );
    zox_system(
        DataIconUpdateSystem,
        EcsOnUpdate,
        [in] slots.DataDirty,
        [in] slots.DataLink,
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
