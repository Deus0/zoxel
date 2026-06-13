#if !defined(zoxm_players)
#define zoxm_players

// todo: move based functionality into here
#include "sta/_.c"
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

// Game now effects all players
void game_state_players(ecs *world, entity game, byte last_state, byte state) {
    byte dbg_log = 0;
    zox_geter(game, PlayerLinks, players);
    for (int i = 0; i < players->length; i++) {
        entity player = players->value[i];
        if (state == zox_game_state_play_begin) {
            if (dbg_log) {
                zox_log("Game Setting player to Loading");
            }
            zox_set(player, PlayerState, { zox_player_state_loading });
        } else if (state == zox_game_start) {
            zox_set(player, PlayerState, { zox_player_state_main_menu });
        } else if (state == zox_game_state_paused) {
            if (dbg_log) {
                zox_log("Game Setting player to Paused");
            }
            zox_set(player, PlayerState, { zox_player_state_pause_begin });
        } else if (last_state == zox_game_state_paused && state == zox_game_state_playing) {
            if (dbg_log) {
                zox_log("Game Setting player to Resume");
            }
            zox_set(player, PlayerState, { zox_player_state_resume_begin });
        } else {
            continue;
        }
        zox_set(player, PlayerStateDirty, { zox_dirty_trigger });
    }
}


zox_begin_module(Players) {
    zox_define_components_players(world);
    zox_define_systems_players(world);
    add_hook_on_boot(on_boot_players);
    add_hook_spawn_prefabs(spawn_prefabs_players);
    add_to_event_game_state((zox_game_event) { &game_state_players });
} zox_end_module(Players);

#endif
