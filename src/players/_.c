#if !defined(zoxm_players)
#define zoxm_players

// todo: move based functionality into here
#include "sta/_.c"
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

// int spawn_players(ecs_world_t *world, const ecs_entity_t game);

void add_player(ecs_world_t *world, const ecs_entity_t e, const ecs_entity_t player) {
    zox_muter(e, PlayerLinks, players)
    //byte length = players->length + 1;
    //resize_PlayerLinks(players, length);
    //players->value[length - 1] = player;
    add_to_PlayerLinks(players, player);
    zox_set(player, GameLink, { e })
}

int spawn_players(ecs_world_t *world, const ecs_entity_t game) {
    int players = 0;
    if (headless) {
        return players;   // no players in headless mode
    }
    if (is_split_screen) {
        players = 2;
        auto_switch_device = 0;
    }
    else {
        players = 1;
    }
    for (int i = 0; i < players; i++) {
        const entity e = spawn_player(world, prefab_player);
        add_player(world, game, e);
        zox_players[i] = e;
        if (players == 2) {
            if (i == 0) {
                zox_set(e, DeviceModeDirty, { zox_device_mode_keyboardmouse })
            } else if (i == 1) {
                zox_set(e, DeviceModeDirty, { zox_device_mode_gamepad })
            }
        }
        // zox_set(e, CameraLink, { main_cameras[i] });
    }
    return players;
}

void on_boot_players(ecs_world_t *world, ecs_entity_t app) {
    if (!headless && zox_is_players) {
        zox_geter_value(app, GameLink, ecs_entity_t, game)
        spawn_connected_devices(world, app);
        players_playing = spawn_players(world, game);
    }
}

void game_state_players(
    ecs *world,
    const entity game,
    const byte last_state,
    const byte state
) {
    zox_geter(game, PlayerLinks, players);
    for (int i = 0; i < players->length; i++) {
        const entity player = players->value[i];
        if (state == zox_game_playing_start) {
            zox_log("zox_player_state_loading");
            zox_set(player, PlayerState, { zox_player_state_loading });
        } else if (state == zox_game_start) {
            zox_set(player, PlayerState, { zox_player_state_main_menu });
        } else if (state == zox_game_paused) {
            zox_set(player, PlayerState, { zox_player_state_paused });
        } else if (last_state == zox_game_paused && state == zox_game_playing) {
            zox_set(player, PlayerState, { zox_game_playing });
        } else {
            continue;
        }
        zox_set(player, PlayerStateDirty, { zox_dirty_trigger });
    }
}

zox_begin_module(Players)
    define_components_players(world);
    define_systems_players(world);
    add_hook_on_boot(on_boot_players);
    add_hook_spawn_prefabs(spawn_prefabs_players);
    add_to_event_game_state((zox_game_event) { &game_state_players });
zox_end_module(Players)

#endif
