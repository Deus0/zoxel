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
    zox_on_add(
        stat_dirty_labels_event,
        [none] stats.Stat,
        [none] slots.DataDirty,
    );
    stat_labels_query = ecs_query(world, {
        .expr = "[in] slots.DataLink, [inout] texts.TextData, [none] texts.Text, [none] elements2.Label, [none] ui_containers.DataUI",
        .cache_kind = EcsQueryCacheAuto
    });
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_stats_ui);
}
