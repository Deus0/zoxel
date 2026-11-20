#include "touchui.c"
#include "auto_jump.c"
#include "actionbar.c"
#include "crosshair.c"
#include "game_start.c"
#include "game_end.c"
#include "dialogue_closed.c"

void zox_events_controllers(ecs* world) {
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_crosshair);
    add_to_PlayerStateEvent(player_event, player_state_actionbar);
    add_to_PlayerStateEvent(player_event, player_state_touch_ui);
    add_to_PlayerStateEvent(player_event, player_state_game_begin);
    add_to_PlayerStateEvent(player_event, player_state_game_end);
    zox_muter(prefab_app, InitializeEvent, app_event);
    add_to_InitializeEvent(app_event, auto_jump_settings);
    zox_on_add(
        auto_jump_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Update,
    );
    zox_observe(on_removed_dialogue_ui, EcsOnRemove, [none] dialogues.DialogueUI);
}
