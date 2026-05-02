void add_player(ecs *world, entity e, entity player) {
    zox_muter(e, PlayerLinks, players);
    add_to_PlayerLinks(players, player);
    zox_set(player, GameLink, { e });
}

entity dbg_player;

int spawn_players(ecs *world, entity game, byte zox_game_type) {
    int players = 0;
    if (is_split_screen) {
        players = 2;
        auto_switch_device = 0;
    } else {
        players = 1;
    }

    for (int i = 0; i < players; i++) {
        entity e = spawn_player(world, prefab_player);

        if (i == 0) {
            dbg_player = e;
        }

        if (zox_game_type == zox_game_mode_3D) {
            zox_add_tag(e, Player3);
            // zox_log("Added Player3 [%i]", zox_game_type);
        } else if (zox_game_type == zox_game_mode_2D) {
            zox_add_tag(e, Player2);
            // zox_log("Added Player2 [%i]", zox_game_type);
        }

        add_player(world, game, e);

        zox_players[i] = e;
        if (players == 2) {
            if (i == 0) {
                zox_set(e, DeviceModeDirty, { zox_device_mode_keyboardmouse });
            } else if (i == 1) {
                zox_set(e, DeviceModeDirty, { zox_device_mode_gamepad });
            }
        }
    }
    return players;
}

void spawn_connected_devices(ecs *world, entity e) {
    local_keyboard = spawn_keyboard(world, prefab_keyboard);
    local_mouse = spawn_mouse(world);
    local_touchscreen = spawn_touchscreen(world, prefab_touchscreen);
    zox_set(local_mouse, AppLink, { e });
    zox_set(local_touchscreen, AppLink, { e });
}

void on_boot_players(ecs *world, entity app) {
    if (!zox_valid(app)) {
        zox_logw("App is invalid.");
        return;
    }
    if (!zox_is_players) {
        zox_logw("Players are not present.");
        return;
    }

    zox_geter_value(app, GameLink, entity, game);
    spawn_connected_devices(world, app);
    players_playing = spawn_players(world, game, zox_game_type);
}

// Game now effects all players
void game_state_players(ecs *world, entity game, byte last_state, byte state) {

    zox_geter(game, PlayerLinks, players);

    for (int i = 0; i < players->length; i++) {
        entity player = players->value[i];
        if (state == zox_game_playing_start) {
            zox_set(player, PlayerState, { zox_player_state_loading });
        } else if (state == zox_game_start) {
            zox_set(player, PlayerState, { zox_player_state_main_menu });
        } else if (state == zox_game_paused) {
            zox_set(player, PlayerState, { zox_player_state_paused });
        } else if (last_state == zox_game_paused && state == zox_game_playing) {
            zox_set(player, PlayerState, { zox_player_state_resuming });
        } else {
            continue;
        }
        zox_set(player, PlayerStateDirty, { zox_dirty_trigger });
    }
}
