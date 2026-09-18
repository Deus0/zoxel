#include "click.c"
#include "icon.c"
#include "label.c"
#include "clear.c"

void zox_systems_ui_containers(ecs* world) {
    zox_system(
        DataFrameClickSystem,
        zoxp_update,
        [in] interaction.ClickState,
        [in] slots.SlotLink,
        [out] slots.DataLink,
        [out] slots.DataDirty,
        [none] u.i.containers.DataFrame
    );
    zox_system(
        SlotDataCleanSystem,
        zoxp_update,
        [out] slots.DataLink,
        [out] slots.DataDirty,
        [none] slots.Slot
    );
    zox_system(
        DataIconSystem,
        zoxp_update,
        [in] slots.SlotLink,
        [out] slots.DataLink,
        [out] slots.DataDirty,
        [none] elements2.Icon
    );
    zox_system(
        DataIconUpdateSystem,
        zoxp_update,
        [in] slots.DataDirty,
        [in] slots.DataLink,
        [none] elements2.Icon
    );
    zox_system(
        IconLabelClearSystem,
        zoxp_update,
        [in] slots.DataDirty,
        [in] slots.SlotLink,
        [out] texts.TextData,
        [none] texts.Text,
        [none] elements2.Label
    );
}
