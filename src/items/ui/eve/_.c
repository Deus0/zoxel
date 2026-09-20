#include "icon.c"
#include "tooltip.c"

void zox_events_items_ui(ecs* world) {
    zox_on_add(
        icon_label_event,
        // [in] slots.SlotLink,
        [out] texts.TextData,
        [none] slots.SlotUser,
        [none] texts.Text,
        [none] elements2.Label,
        [none] slots.DataUpdate,
    );
    zox_on_add(
        icon_tooltip_event,
        [in] slots.DataLink,
        [none] slots.SlotUser,
        [none] interactions.Select,
    );

}
