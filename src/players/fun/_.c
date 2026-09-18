void add_player(
    ecs* world,
    entity e,
    entity player)
{
    zox_muter(e, PlayerLinks, players);
    add_to_PlayerLinks(players, player);
    zox_link(world, player, GameLink, e);
}

entity dbg_player;

int spawn_players(
    ecs *world,
    entity app,
    entity game,
    byte zox_game_type)
{
    int players = 0;
    if (is_split_screen) {
        players = 2;
        auto_switch_device = 0;
    } else {
        players = 1;
    }
    for (int i = 0; i < players; i++) {
        entity e = spawn_player(
            world,
            prefab_player,
            game,
            app);
        if (i == 0) {
            dbg_player = e;
        }
        if (zox_game_type == zox_game_mode_3D) {
            zox_add(e, Player3);
            // zox_log("Added Player3 [%i]", zox_game_type);
        } else if (zox_game_type == zox_game_mode_2D) {
            zox_add(e, Player2);
            // zox_log("Added Player2 [%i]", zox_game_type);
        }
        add_player(world, game, e);
        zox_players[i] = e;
        if (players == 2) {
            if (i == 0) {
                zox_setv(e, DeviceMode, zox_device_mode_keyboardmouse);
            } else if (i == 1) {
                zox_setv(e, DeviceMode, zox_device_mode_gamepad);
            }
            zox_setv(e, DeviceModeDirty, 1);
        }
    }
    return players;
}

void on_boot_players(ecs* world, entity app) {
    if (!zox_valid(app)) {
        zox_logw("App is invalid.");
        return;
    }
    if (!zox_is_players) {
        zox_logw("Players are not present.");
        return;
    }
    entity game = zox_get_link(world, app, GameLink);
    players_playing = spawn_players(
        world,
        app,
        game,
        zox_game_type);
}


void spawn_devices_on_app(ecs* world, entity app) {
    local_keyboard = spawn_keyboard(
        world,
        prefab_keyboard);
    local_mouse = spawn_mouse(world);
    local_touchscreen = spawn_touchscreen(
        world,
        prefab_touchscreen);
    zox_set_parent(world, local_keyboard, app);
    zox_set_parent(world, local_mouse, app);
    zox_set_parent(world, local_touchscreen, app);
    zox_link(world, local_keyboard, AppLink, app);
    zox_link(world, local_mouse, AppLink, app);
    zox_link(world, local_touchscreen, AppLink, app);
    initialize_sdl_gamepads(world, app);
}