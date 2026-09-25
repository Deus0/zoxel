#include "icon.c"

void zox_events_items_ui(ecs* world) {
    zox_on_add(
        item_icon_label_event,
        [out] texts.TextData,
        [none] slots.SlotUser,
        [none] texts.Text,
        [none] slots.DataUpdate,
    );
    zox_on_add(
        item_icon_tooltip_event,
        [none] elements2.Icon,
        [none] slots.SlotUser,
        [none] interactions.Select,
    );

}
