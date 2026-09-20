#include "click.c"
#include "icon.c"
#include "label.c"
#include "clear.c"
#include "state.c"

void zox_systems_ui_containers(ecs* world) {
    zox_system(
        DataFrameClickSystem,
        zoxp_update,
        [in] interactions.ClickState,
        [out] slots.DataLink,
        [none] slots.SlotUser,
        [none] ui_containers.DataFrame
    );
    zox_system(
        SlotDataCleanSystem,
        zoxp_update,
        [out] slots.DataLink,
        [none] slots.Slot
    );
    zox_system(
        DataIconSystem,
        zoxp_update,
        [out] slots.DataLink,
        [none] slots.SlotUser,
        [none] elements2.Icon
    );
    zox_system(
        DataIconUpdateSystem,
        zoxp_update,
        [in] slots.DataLink,
        [none] elements2.Icon,
        [none] slots.DataUpdate,
    );
    zox_system(
        IconLabelClearSystem,
        zoxp_update,
        [out] texts.TextData,
        [none] texts.Text,
        [none] elements2.Label,
        [none] slots.DataUpdate,
    );
    zox_system(
        data_dirty_system,
        zoxp_remove,
        [none] slots.DataDirty,
    );
    zox_system(
        data_update_system,
        zoxp_remove,
        [none] slots.DataUpdate,
    );
    zox_system(
        slot_ui_update_system,
        zoxp_remove,
        [none] slots.SlotUser,
    );
    zox_system(
        data_ui_update_system,
        zoxp_remove,
        [none] slots.DataLink,
    );
}
