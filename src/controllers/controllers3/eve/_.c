#include "touchui.c"
#include "auto_jump.c"
#include "actionbar.c"
#include "crosshair.c"
#include "game_start.c"
#include "game_end.c"
#include "dialogue_closed.c"

// the player state changed (its core Dirty)
void player_state_controllers_event(iter* it) {
    player_state_each(it, player_state_crosshair);
    player_state_each(it, player_state_actionbar);
    player_state_each(it, player_state_touch_ui);
    player_state_each(it, player_state_game_begin);
    player_state_each(it, player_state_game_end);
}

void zox_events_controllers(ecs* world) {
    zox_on_add(player_state_controllers_event, [in] players.PlayerState, [none] players.Player, [none] core.Dirty);
    zox_muter(prefab_app, InitializeEvent, app_event);
    add_to_InitializeEvent(app_event, auto_jump_settings);
    zox_on_add(
        auto_jump_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Dirty,
    );
    zox_observe(on_removed_dialogue_ui, EcsOnRemove, [none] dialogues.DialogueUI);
}
