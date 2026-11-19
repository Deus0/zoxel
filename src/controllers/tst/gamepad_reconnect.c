// tst: coop_gamepad_reconnect, split screen: the second player's gamepad unplugged and plugged back in, in game
//  - unplugged (on_gamepad_disconnected, as SDL's removed event): the player has no device
//  - plugged in (a new gamepad, the app's, as SDL's added event): it's the second player's again (reclaim_gamepads),
//      the first player's devices left be, and its stick walks the second player's character
//  - the disconnect screen on its screen while unplugged (huds/eve/disconnect.c), gone plugged in
//  - it went to the app, only the main menu's joining took one: the player had no way back
//  - run: bash bsh/tst.sh coop_gamepad_reconnect
int zox_tst_reconnect_stage = 0;
double zox_tst_reconnect_time = 0;
byte zox_tst_reconnect_lost = 0;        // the player had no device, unplugged
byte zox_tst_reconnect_first_mode = 0;
byte zox_tst_reconnect_screen = 0;     // the disconnect screen, unplugged
byte zox_tst_reconnect_screen_first = 0;  // on the first player's screen (it shouldn't be)  // the first player's device mode, before
float3 zox_tst_reconnect_start;
float zox_tst_reconnect_walked = 0;

static inline void zox_tst_reconnect_next() {
    zox_tst_reconnect_stage++;
    zox_tst_reconnect_time = zox_current_time;
}

byte zox_tst_coop_gamepad_reconnect_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    if (players_playing < 2 || !zox_valid(zox_get_link(world, zox_players[1], CharacterLink))) {
        zox_tst_waiting = "the second player's character";
        return 0;
    }
    const entity player = zox_players[1];
    const double t = zox_current_time - zox_tst_reconnect_time;
    zox_tst_progress(zox_tst_reconnect_stage * 100 + t * 10);
    zox_tst_pad_stick = float2_zero;
    switch (zox_tst_reconnect_stage) {
        case 0:
            zox_tst_waiting = "the second player's gamepad";
            zox_tst_pad = zox_tst_spawn_pad(world, player);
            zox_tst_reconnect_first_mode = zox_getv(zox_players[0], DeviceMode);
            zox_tst_reconnect_next();
            return 0;
        case 1:
            // touched, so it's the player's device
            zox_tst_pad_stick = (float2) { 0, t < 0.1 ? 0.05f : 0 };
            if (t > 0.5) {
                on_gamepad_disconnected(world, zox_tst_pad);
                zox_tst_pad = 0;
                zox_tst_reconnect_next();
            }
            return 0;
        case 2:
            zox_tst_waiting = "unplugged";
            if (t < 0.3) {
                return 0;
            }
            static byte shot = 0;
            if (!shot && getenv("zox_tst_shot")) {
                shot = 1;
                snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("disconnect.ppm"));
            }
            if (t < 0.5) {
                return 0;
            }
            zox_tst_reconnect_lost = zox_getv(player, DeviceMode) == zox_device_mode_none;
            zox_tst_reconnect_screen = zox_valid(zox_get_child_by_id(world, zox_get_link(world, player, CanvasLink), zox_id(DisconnectUI)));
            zox_tst_reconnect_screen_first = zox_valid(zox_get_child_by_id(world, zox_get_link(world, zox_players[0], CanvasLink),
                zox_id(DisconnectUI)));
            // plugged back in: a new gamepad, the app's
            zox_tst_pad = zox_tst_spawn_pad(world, main_app);
            zox_tst_reconnect_next();
            return 0;
        case 3:
            zox_tst_waiting = "plugged in";
            if (t < 0.3) {
                return 0;
            }
            zox_tst_reconnect_start = zox_getv(zox_get_link(world, player, CharacterLink), Position3D);
            zox_tst_reconnect_next();
            return 0;
        case 4:
            zox_tst_waiting = "walking with it";
            zox_tst_pad_stick = (float2) { 0, 1 };
            if (t < 1.0) {
                return 0;
            }
            {
                const float3 at = zox_getv(zox_get_link(world, player, CharacterLink), Position3D);
                const float dx = at.x - zox_tst_reconnect_start.x, dz = at.z - zox_tst_reconnect_start.z;
                zox_tst_reconnect_walked = sqrtf(dx * dx + dz * dz);
            }
            zox_tst_pad_stick = float2_zero;
            return 1;
    }
    return 1;
}

void zox_tst_coop_gamepad_reconnect_check(ecs* world) {
    const entity player = zox_players[1];
    const entity pad = zox_tst_pad;
    zox_log("[tst] unplugged, no device [%i]; plugged in, the gamepad's [%s], the player's mode [%i], walked [%.2f]",
        zox_tst_reconnect_lost, zox_valid(pad) ? zox_get_name(zox_get_parent(world, pad)) : "gone",
        zox_getv(player, DeviceMode), zox_tst_reconnect_walked);
    zox_tst_check(zox_tst_reconnect_lost, "unplugged, the second player still had a device");
    zox_tst_check(zox_tst_reconnect_screen, "unplugged, no disconnect screen on the second player's screen");
    zox_tst_check(!zox_tst_reconnect_screen_first, "unplugged, a disconnect screen on the first player's screen");
    zox_tst_check(!zox_valid(zox_get_child_by_id(world, zox_get_link(world, player, CanvasLink), zox_id(DisconnectUI))),
        "plugged back in, the disconnect screen stayed");
    zox_tst_check(zox_valid(pad) && zox_get_parent(world, pad) == player, "the plugged in gamepad isn't the second player's");
    zox_tst_check(zox_get_link(world, player, GamepadLink) == pad, "the second player isn't linked to the plugged in gamepad");
    zox_tst_check(zox_getv(player, DeviceMode) == zox_device_mode_gamepad, "the second player isn't using a gamepad again");
    zox_tst_check(zox_getv(zox_players[0], DeviceMode) == zox_tst_reconnect_first_mode, "the first player's device mode changed");
    zox_tst_check(zox_tst_reconnect_walked > 0.5f, "its stick walked the second player [%.2f]", zox_tst_reconnect_walked);
}
