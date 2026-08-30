#include "touchui.c"
#include "actionbar.c"
#include "crosshair.c"
#include "fader.c"
#include "game_start.c"
#include "game_end.c"

void zox_events_controllers(ecs* world) {
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_crosshair);
    add_to_PlayerStateEvent(player_event, player_state_actionbar);
    add_to_PlayerStateEvent(player_event, player_state_touch_ui);
    add_to_PlayerStateEvent(player_event, player_state_fader);
    add_to_PlayerStateEvent(player_event, player_state_game_begin);
    add_to_PlayerStateEvent(player_event, player_state_game_end);
}
