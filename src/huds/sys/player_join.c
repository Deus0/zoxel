// local coop on the main menu: players join with a gamepad (A) and leave with it (B)
//  - a gamepad joins when it isn't the first player's (its GamepadLink while it uses a gamepad), or it's no one's
//      (a gamepad plugged in later is the app's)
//  - the join ui: each player at the bottom centre, P1 P2.. next to their device's icon, and a hint to join
//      while a gamepad can
//  - their screens spawn as a game starts (split_screen.c)
int player_join_ui_shown = -1;  // the players and devices the join ui shows, -1 none

// the gamepad's button of a type was pressed this frame
static inline byte gamepad_pressed(ecs* world, entity gamepad, byte type) {
    byte pressed = 0;
    iter it2 = zox_children(world, gamepad);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count && !pressed; j++) {
            const entity e = it2.entities[j];
            pressed = zox_has(e, ZeviceButton) && zox_has(e, DeviceButtonType) &&
                zox_getv(e, DeviceButtonType) == type &&
                devices_get_pressed_this_frame(zox_getv(e, ZeviceButton));
        }
    }
    return pressed;
}

// the gamepads that can join: the app's, and the first player's but the one it uses
static inline uint get_joinable_gamepads(ecs* world, entity first, entity* gamepads, uint capacity) {
    uint count = 0;
    const entity owners[2] = { main_app, first };
    const entity used = zox_getv(first, DeviceMode) == zox_device_mode_gamepad ?
        zox_get_link(world, first, GamepadLink) : 0;
    for (int o = 0; o < 2; o++) {
        if (!zox_valid(owners[o])) {
            continue;
        }
        iter it2 = zox_children(world, owners[o]);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count && count < capacity; j++) {
                const entity e = it2.entities[j];
                if (zox_has(e, Gamepad) && e != used) {
                    gamepads[count++] = e;
                }
            }
        }
    }
    return count;
}

static inline const char* device_mode_texture(byte mode) {
    if (mode == zox_device_mode_gamepad) {
        return "device_gamepad";
    } else if (mode == zox_device_mode_keyboardmouse) {
        return "device_keyboard";
    } else if (mode == zox_device_mode_touchscreen) {
        return "device_touchscreen";
    }
    return "device_none";
}

void despawn_player_join_ui(ecs* world, entity canvas) {
    player_join_ui_shown = -1;
    if (!zox_valid(canvas)) {
        return;
    }
    iter it2 = zox_children(world, canvas);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            if (zox_has(it2.entities[j], PlayerJoinUI)) {
                zox_delete(it2.entities[j]);
            }
        }
    }
}

void spawn_player_join_ui(ecs* world, entity canvas, byte can_join) {
    const float2 anchor = (float2) { 0.5f, 0 };
    const int slot_width = 40 * ui_scale;
    const int icon_size = 12 * ui_scale;
    const int y = 14 * ui_scale;
    const byte font_size = 8 * ui_scale;
    const byte2 padding = (byte2) { 4 * ui_scale, 2 * ui_scale };
    const int slots = players_playing + (can_join ? 1 : 0);
    for (int i = 0; i < slots; i++) {
        const int x = (int) ((i - (slots - 1) * 0.5f) * slot_width);
        char text[16];
        if (i < players_playing) {
            snprintf(text, sizeof(text), "P%i", i + 1);
            const int2 icon_position = (int2) { x - icon_size / 2 - 2 * ui_scale, y };
            entity icon = spawn_ui(world, prefab_element, canvas, anchor, icon_position,
                int2_single(icon_size), int2_single(icon_size));
            zox_set_unique_name(icon, "player_join_icon");
            zox_add(icon, PlayerJoinUI);
            zox_setv(icon, Layer, max_layers2D - 4);
            clone_texture_file_to_entity(world, icon, (char*) device_mode_texture(zox_getv(zox_players[i], DeviceMode)));
        } else {
            snprintf(text, sizeof(text), "A join");
        }
        const int2 label_position = (int2) { i < players_playing ? x + 8 * ui_scale : x, y };
        entity label = spawn_label(world, prefab_label2t, canvas, label_position, anchor, padding, text,
            font_size, zox_alignment_centre, 0, button_fill, button_outline, button_font_fill, button_font_outline);
        zox_set_unique_name(label, "player_join_label");
        zox_add(label, PlayerJoinUI);
    }
}

void player_join_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        // the first player runs it, for every player
        if (e != zox_players[0]) {
            continue;
        }
        const entity canvas = zox_get_link(world, e, CanvasLink);
        const entity game = zox_get_link(world, e, GameLink);
        const byte on_main_menu = state->value == zox_player_state_main_menu && zox_valid(canvas) &&
            zox_valid(game) && zox_getv(game, GameState) == zox_game_none &&
            zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuMain)));
        if (!on_main_menu) {
            if (player_join_ui_shown != -1) {
                despawn_player_join_ui(world, canvas);
            }
            continue;
        }
        // joining and leaving
        entity gamepads[16];
        uint gamepads_count = get_joinable_gamepads(world, e, gamepads, 16);
        for (uint j = 0; j < gamepads_count; j++) {
            if (gamepad_pressed(world, gamepads[j], zox_btn_a)) {
                join_player(world, game, gamepads[j]);
            }
        }
        for (int j = players_playing - 1; j >= 1; j--) {
            const entity player = zox_players[j];
            const entity gamepad = zox_get_link(world, player, GamepadLink);
            if (zox_valid(gamepad) && gamepad_pressed(world, gamepad, zox_btn_b)) {
                unjoin_player(world, player);
            }
        }
        // the join ui, again when its players or their devices change
        gamepads_count = get_joinable_gamepads(world, e, gamepads, 16);
        const byte can_join = gamepads_count > 0 && players_playing < max_players;
        int shown = can_join;
        for (int j = 0; j < players_playing; j++) {
            shown = shown * 7 + 1 + zox_getv(zox_players[j], DeviceMode);
        }
        if (shown != player_join_ui_shown) {
            despawn_player_join_ui(world, canvas);
            spawn_player_join_ui(world, canvas, can_join);
            player_join_ui_shown = shown;
        }
    }
    zox_sys_on_end();
} zoxd_system(player_join_system);
