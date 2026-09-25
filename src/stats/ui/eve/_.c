#include "icon.c"
#include "panel.c"

void zox_events_stats_ui(ecs* world) {
    zox_on_add(
        stat_icon_label_event,
        [out] texts.TextData,
        // [none] slots.SlotUser,
        [none] texts.Text,
        [none] elements2.Label,
        [none] slots.DataUpdate,
    );
    zox_on_add(
        stat_icon_tooltip_event,
        [in] slots.DataLink,
        [none] slots.SlotUser,
        [none] interactions.Select,
    );
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_stats_ui);
}
