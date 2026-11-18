void add_player(
    ecs* world,
    entity e,
    entity player)
{
    zox_link(world, e, PlayerLinks, player);
    zox_link(world, player, GameLink, e);
}

entity dbg_player;

// tags a player for the game's mode, and adds it to the game
static inline void setup_player(
    ecs* world,
    entity game,
    entity e,
    byte zox_game_type)
{
    if (zox_game_type == zox_game_mode_3D) {
        zox_add(e, Player3);
    } else if (zox_game_type == zox_game_mode_2D) {
        zox_add(e, Player2);
    }
    add_player(world, game, e);
}

// local coop: a player joins with a device (a gamepad), no cameras or canvas until a game starts (huds)
//  - device 0 joins with none (--splitscreen without a gamepad, tests)
entity join_player(
    ecs* world,
    entity game,
    entity device)
{
    if (players_playing >= max_players || !zox_valid(game)) {
        return 0;
    }
    entity e = zox_ins_named(world, prefab_player);
    zox_set_parent(world, e, game);
    setup_player(world, game, e, zox_game_type);
    if (zox_valid(device)) {
        zox_set_parent(world, device, e);
        zox_remove(device, Disabled);
        zox_link(world, e, GamepadLink, device);
    }
    zox_setv(e, DeviceMode, zox_valid(device) ? zox_device_mode_gamepad : zox_device_mode_none);
    zox_players[players_playing++] = e;
    zox_log("+ [coop] player [%i] joined", players_playing);
    return e;
}

// a joined player leaves (not the first): its devices go back to the first player
void unjoin_player(
    ecs* world,
    entity e)
{
    int index = -1;
    for (int i = 1; i < players_playing; i++) {
        if (zox_players[i] == e) {
            index = i;
        }
    }
    if (index < 0) {
        return;
    }
    const entity first = zox_players[0];
    entity devices[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
    for (uint j = 0; j < length; j++) {
        zox_set_parent(world, devices[j], first);
        // enabled again when the first player switches to it
        if (zox_getv(first, DeviceMode) != zox_device_mode_gamepad) {
            zox_add(devices[j], Disabled);
        }
    }
    for (int i = index; i < players_playing - 1; i++) {
        zox_players[i] = zox_players[i + 1];
    }
    zox_players[--players_playing] = 0;
    zox_delete(e);
    zox_log("- [coop] player [%i] left", index + 1);
}

int spawn_players(
    ecs *world,
    entity app,
    entity game,
    byte zox_game_type)
{
    entity e = spawn_player(
        world,
        prefab_player,
        game,
        app);
    dbg_player = e;
    setup_player(world, game, e, zox_game_type);
    zox_players[0] = e;
    players_playing = 1;
    // --splitscreen: a second player joins at boot (the screen splits in game)
    if (is_split_screen) {
        join_player(world, game, 0);
    }
    return players_playing;
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
    spawn_players(
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