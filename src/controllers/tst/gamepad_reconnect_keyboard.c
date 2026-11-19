// tst: gamepad_reconnect_keyboard, one player: its gamepad unplugged, the keyboard used, the gamepad plugged back in
//  - the player on a gamepad, unplugged (on_gamepad_disconnected), then on the keyboard (as a key or the mouse
//      switches it, device_switch_system)
//  - plugged back in (a new gamepad, the app's): it's the player's (reclaim_gamepads), and a press of A takes the
//      player back onto it (it stayed the app's: the player had a device, the keyboard, so it wasn't given one)
//  - run: bash bsh/tst.sh gamepad_reconnect_keyboard
int zox_tst_rk_stage = 0;
double zox_tst_rk_time = 0;
byte zox_tst_rk_was_gamepad = 0;

static inline void zox_tst_rk_next() {
    zox_tst_rk_stage++;
    zox_tst_rk_time = zox_current_time;
}

byte zox_tst_gamepad_reconnect_keyboard_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity player = zox_players[0];
    const double t = zox_current_time - zox_tst_rk_time;
    zox_tst_progress(zox_tst_rk_stage * 100 + t * 10);
    zox_tst_pad_stick = float2_zero;
    zox_tst_pad_held[zox_btn_a] = 0;
    switch (zox_tst_rk_stage) {
        case 0:
            zox_tst_pad = zox_tst_spawn_pad(world, player);
            zox_tst_rk_next();
            return 0;
        case 1:
            zox_tst_waiting = "the player on its gamepad";
            zox_tst_pad_stick = (float2) { 0, t < 0.2 ? 0.5f : 0 };
            if (t < 0.5) {
                return 0;
            }
            zox_tst_rk_was_gamepad = zox_getv(player, DeviceMode) == zox_device_mode_gamepad;
            on_gamepad_disconnected(world, zox_tst_pad);
            zox_tst_pad = 0;
            zox_tst_rk_next();
            return 0;
        case 2:
            zox_tst_waiting = "unplugged, then on the keyboard";
            if (t < 0.3) {
                return 0;
            }
            // the keyboard used (device_switch_system would, a key down): its mode, not lost any more
            zox_setv(player, DeviceMode, zox_device_mode_keyboardmouse);
            zox_remove(player, DeviceLost);
            zox_tst_rk_next();
            return 0;
        case 3:
            if (t < 0.3) {
                return 0;
            }
            zox_tst_pad = zox_tst_spawn_pad(world, main_app);
            zox_tst_rk_next();
            return 0;
        case 4:
            zox_tst_waiting = "plugged in, A pressed";
            zox_tst_pad_held[zox_btn_a] = t > 0.3 && t < 0.5;
            return t > 1.0;
    }
    return 1;
}

void zox_tst_gamepad_reconnect_keyboard_check(ecs* world) {
    const entity player = zox_players[0];
    const entity pad = zox_tst_pad;
    zox_log("[tst] gamepad_reconnect_keyboard: the gamepad's [%s], the player's mode [%i]",
        zox_valid(pad) ? zox_get_name(zox_get_parent(world, pad)) : "gone", zox_getv(player, DeviceMode));
    zox_tst_check(zox_tst_rk_was_gamepad, "the player never used its gamepad");
    zox_tst_check(zox_valid(pad) && zox_get_parent(world, pad) == player, "the plugged in gamepad isn't the player's");
    zox_tst_check(zox_getv(player, DeviceMode) == zox_device_mode_gamepad, "pressing A, the player isn't on its gamepad again [%i]",
        zox_getv(player, DeviceMode));
}

void zox_tsts_gamepad_reconnect_keyboard(ecs* world) {
    zox_tst_add("gamepad_reconnect_keyboard", zox_tst_gamepad_reconnect_keyboard_ready, zox_tst_gamepad_reconnect_keyboard_check, 120);
}
