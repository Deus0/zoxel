#include "icon.c"
#include "tracker.c"

// event on quest icon clicked?

void zox_events_quests_ui(ecs* world) {
    zox_on_add(
        quest_icon_label_event,
        [out] texts.TextData,
        [none] texts.Text,
        [none] elements2.Label,
        [none] slots.DataUpdate,
    );
    zox_on_add(
        quest_icon_tooltip_event,
        [in] slots.DataLink,
        [none] quests_ui.QuestIcon,
        [none] slots.SlotUser,
        [none] interactions.Select, // event
    );
    // icon clicked, add tracking
    zox_on_add(
        quest_icon_click_event,
        [in] slots.DataLink,
        [none] quests_ui.QuestIcon,
        [none] slots.SlotUser,
        [none] interactions.Click, // event
    );
    zox_on_add(
        quest_tracker_updated,
        [none] quests.Quest,
        [none] core.Dirty,
    );
    // for now...
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_quest_tracker);
}
