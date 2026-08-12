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
        entity e = spawn_player(world, prefab_player, game);
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
                zox_set(e, DeviceMode, { zox_device_mode_keyboardmouse });
            } else if (i == 1) {
                zox_set(e, DeviceMode, { zox_device_mode_gamepad });
            }
            zox_set(e, DeviceModeDirty, { 1 });
        }
    }
    return players;
}

void spawn_connected_devices(ecs *world, entity e) {
    local_keyboard = spawn_keyboard(world, prefab_keyboard);
    zox_set_parent(world, local_keyboard, e);
    local_mouse = spawn_mouse(world);
    zox_set(local_mouse, AppLink, { e });
    zox_set_parent(world, local_mouse, e);
    local_touchscreen = spawn_touchscreen(world, prefab_touchscreen);
    zox_set(local_touchscreen, AppLink, { e });
    zox_set_parent(world, local_touchscreen, e);
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
    entity game = zox_getv(app, GameLink);
    spawn_connected_devices(world, app);
    players_playing = spawn_players(world, game, zox_game_type);
}
