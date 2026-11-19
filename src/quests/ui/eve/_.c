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
        [none] quests_ui.QuestIcon,
        [none] slots.SlotUser,
        [none] interactions.Select, // event
    );
    // icon clicked, add tracking
    zox_on_add(
        quest_icon_click_event,
        [none] quests_ui.QuestIcon,
        [none] slots.SlotUser,
        [none] interactions.Click, // event
    );
    // the tracked quest's icon shows it, once filled
    zox_on_add(
        quest_icon_tracked_event,
        [none] quests_ui.QuestIcon,
        [none] slots.SlotUser,
        [none] slots.DataUpdate,
    );
    // on Update, the frame after Dirty (dirty_system): a quest just given is whole then, its tracker linked (on its
    //  Dirty the link wasn't there yet, the tracker kept the ??? read as it was given)
    zox_on_add(
        quest_tracker_updated,
        [none] quests.Quest,
        [none] core.Update,
    );
    // and on Dirty: dirtied two frames running its Update stays (update_system), no add the second time
    zox_on_add(
        quest_tracker_updated,
        [none] quests.Quest,
        [none] core.Dirty,
    );
    // for now...
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_quest_tracker);
}
