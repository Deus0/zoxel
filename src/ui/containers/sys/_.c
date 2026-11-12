#include "click.c"
#include "right_click.c"
#include "icon.c"
#include "label.c"
#include "clear.c"
#include "state.c"

void zox_systems_ui_containers(ecs* world) {
    zox_system(
        data_ui_click_system,
        zoxp_update,
        // [in] interactions.ClickState,
        [none] interactions.Click,
        [none] slots.SlotUser,
        [none] ui_containers.DataUI,
    );
    // main thread, splitting a stack spawns an item
    zox_system_1(
        data_ui_right_click_system,
        zoxp_update,
        // [in] interactions.ClickState,
        [none] interactions.RightClick,
        [none] slots.SlotUser,
        [none] ui_containers.DataUI,
    );
    zox_system(
        data_icon_system,
        zoxp_update,
        [none] slots.SlotUser,
        [none] elements2.Icon
    );
    zox_system(
        data_icon_update_system,
        zoxp_update,
        [none] elements2.Icon,
        [none] ui_containers.DataUI,
        [none] slots.DataUpdate,
    );
    /*zox_system(
        icon_label_clear_system,
        zoxp_update,
        [out] texts.TextData,
        [none] texts.Text,
        [none] elements2.Label,
        [none] slots.DataUpdate,
    );*/
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
        [none] ui_containers.DataUI,
    );
}
