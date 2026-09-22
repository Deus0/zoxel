#include "icon.c"
#include "tooltip.c"

void zox_events_quests_ui(ecs* world) {
    zox_on_add(
        quest_icon_label_event,
        [out] texts.TextData,
        // [none] slots.SlotUser,
        [none] texts.Text,
        [none] elements2.Label,
        [none] slots.DataUpdate,
    );
    zox_on_add(
        quest_icon_tooltip_event,
        [in] slots.DataLink,
        [none] slots.SlotUser,
        [none] interactions.Select,
    );
}
