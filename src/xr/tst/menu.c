// tst: xr_menu, the XR controllers drive the 3D menus as a player would on the simulator (--xr-sim)
//  - the start menu is a 3D label; A on the right controller (scripted) switches the device mode to the
//      controllers (gamepad) and opens the main menu, a 3D list (Canvas3) among the canvas's children
//  - navigation selects its first button; the right stick down (the navigating hand) moves it to the next
//  - screenshots it (tst/xr_menu.ppm): the panel ahead in both eyes
//  - run: bash bsh/tst.sh xr_menu
int zox_tst_xr_menu_stage = 0;
int zox_tst_xr_menu_frames = 0;
entity zox_tst_xr_menu_first = 0;
entity zox_tst_xr_menu_second = 0;
byte zox_tst_xr_menu_opened = 0;

static inline void zox_tst_xr_menu_next() {
    zox_tst_xr_menu_stage++;
    zox_tst_xr_menu_frames = 0;
}

static inline entity zox_tst_xr_menu_target(ecs* world) {
    const entity right = xr_controllers[xr_hand_right];
    return zox_valid(right) ? zox_getv(right, EntityTarget) : 0;
}

byte zox_tst_xr_menu_ready(ecs* world) {
    const entity player = zox_players[0];
    if (!zox_valid(xr_camera) || !zox_valid(player)) {
        zox_tst_waiting = "the xr camera";
        return 0;
    }
    const entity canvas = zox_get_link(world, player, CanvasLink);
    zox_tst_xr_menu_frames++;
    switch (zox_tst_xr_menu_stage) {
        case 0:
            zox_tst_waiting = "the start menu";
            if (zox_valid(zox_get_link(world, player, MenuStart)) && zox_tst_xr_menu_frames > 30) {
                xr_sim_head_euler = float3_zero;
                xr_sim_script_a = 1;
                zox_tst_xr_menu_next();
            }
            return 0;
        case 1:
            zox_tst_waiting = "A to open the main menu";
            if (zox_tst_xr_menu_frames == 3) {
                xr_sim_script_a = 0;
            }
            if (zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuMain)))) {
                zox_tst_xr_menu_opened = 1;
                xr_sim_script_a = 0;
                zox_tst_xr_menu_next();
            }
            return 0;
        case 2:
            zox_tst_waiting = "navigation to select the first button";
            zox_tst_xr_menu_first = zox_tst_xr_menu_target(world);
            if (zox_valid(zox_tst_xr_menu_first) && zox_tst_xr_menu_frames > 10) {
                xr_sim_script_stick = (float2) { 0, -1 };
                zox_tst_xr_menu_next();
            }
            return 0;
        case 3:
            zox_tst_waiting = "the stick to select the next button";
            zox_tst_xr_menu_second = zox_tst_xr_menu_target(world);
            if (zox_tst_xr_menu_second != zox_tst_xr_menu_first || zox_tst_xr_menu_frames > 120) {
                xr_sim_script_stick = float2_zero;
                zox_tst_xr_menu_next();
            }
            return 0;
        case 4:
            zox_tst_waiting = "the screenshot";
            if (zox_tst_xr_menu_frames == 60) {
                remove(zox_tst_path("xr_menu.ppm"));
                snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("xr_menu.ppm"));
            }
            return zox_tst_xr_menu_frames > 60 && !zox_screenshot_request[0];
    }
    return 0;
}

void zox_tst_xr_menu_check(ecs* world) {
    const entity player = zox_players[0];
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuMain));
    zox_tst_check(zox_tst_xr_menu_opened, "A didn't open the main menu");
    zox_tst_check(zox_valid(menu) && zox_has(menu, Canvas3), "the main menu isn't a 3D list");
    zox_tst_check(zox_getv(player, DeviceMode) == zox_device_mode_gamepad, "the controllers didn't take the device mode [%i]", zox_getv(player, DeviceMode));
    zox_tst_check(zox_valid(zox_tst_xr_menu_first) && zox_has(zox_tst_xr_menu_first, Element3), "no 3D button selected");
    zox_tst_check(zox_valid(zox_tst_xr_menu_second) && zox_tst_xr_menu_second != zox_tst_xr_menu_first, "the stick didn't move the selection");
    zox_tst_check(zox_get_link(world, player, MenuStart) == 0 || !zox_valid(zox_get_link(world, player, MenuStart)), "the start menu stayed open");
}

void zox_tsts_xr_menu(ecs* world) {
    zox_tst_add("xr_menu", zox_tst_xr_menu_ready, zox_tst_xr_menu_check, 90);
    zox_tst_set_setup("xr_menu", zox_tst_xr_sim_setup, 1);
}

// tst: xr_play, into a game with XR on the simulator, by the controllers alone
//  - A on the start menu, A on the main menu's first button (New Game: tests have no saves), in with the
//      defaults (no realm menu, 3D huds have no text input)
//  - playing: the game state, the xr camera still the one rendering, both eyes drawing (tst/xr_play.ppm)
//  - run: bash bsh/tst.sh xr_play
int zox_tst_xr_play_stage = 0;
int zox_tst_xr_play_frames = 0;
byte zox_tst_xr_play_playing = 0;

static inline void zox_tst_xr_play_next() {
    zox_tst_xr_play_stage++;
    zox_tst_xr_play_frames = 0;
}

byte zox_tst_xr_play_ready(ecs* world) {
    const entity player = zox_players[0];
    if (!zox_valid(xr_camera) || !zox_valid(player)) {
        zox_tst_waiting = "the xr camera";
        return 0;
    }
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const entity game = zox_get_link(world, player, GameLink);
    zox_tst_xr_play_frames++;
    // A held for a few frames, then let go (a click is its release)
    xr_sim_script_a = zox_tst_xr_play_frames >= 20 && zox_tst_xr_play_frames < 24 && zox_tst_xr_play_stage < 2;
    switch (zox_tst_xr_play_stage) {
        case 0:
            zox_tst_waiting = "the start menu";
            if (zox_tst_xr_play_frames >= 30 && !zox_valid(zox_get_link(world, player, MenuStart))) {
                zox_tst_xr_play_next();
            }
            return 0;
        case 1:
            zox_tst_waiting = "the main menu, then new game";
            if (zox_tst_xr_play_frames >= 30 && !zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuMain)))) {
                zox_tst_xr_play_next();
            }
            if (zox_tst_xr_play_frames >= 60) {
                zox_tst_xr_play_frames = 0;     // pressed again, if the selection wasn't there yet
            }
            return 0;
        case 2:
            zox_tst_waiting = "playing";
            if (zox_valid(game) && zox_getv(game, GameState) == zox_game_state_playing) {
                zox_tst_xr_play_playing = 1;
                zox_tst_xr_play_next();
            }
            return 0;
        case 3:
            zox_tst_waiting = "the screenshot";
            if (zox_tst_xr_play_frames == 120) {
                remove(zox_tst_path("xr_play.ppm"));
                snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("xr_play.ppm"));
            }
            return zox_tst_xr_play_frames > 120 && !zox_screenshot_request[0];
    }
    return 0;
}

void zox_tst_xr_play_check(ecs* world) {
    zox_tst_check(zox_tst_xr_play_playing, "never playing");
    zox_tst_check(main_cameras[0] == xr_camera, "the main camera isn't the xr camera");
    int2 size;
    byte* pixels = zox_tst_load_ppm(zox_tst_path("xr_play.ppm"), &size);
    zox_tst_check(pixels != NULL, "no screenshot tst/xr_play.ppm");
    if (!pixels) {
        return;
    }
    const int half = size.x / 2;
    int drawn[2] = { 0, 0 };
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < half; x++) {
            for (int i = 0; i < 2; i++) {
                const byte* p = zox_tst_ppm_pixel(pixels, size, x + i * half, y);
                drawn[i] += abs(p[0] - xr_clear_color.r) + abs(p[1] - xr_clear_color.g) + abs(p[2] - xr_clear_color.b) > 12;
            }
        }
    }
    const float total = (float) half * size.y;
    zox_log("[tst] [metric] xr_play drawn [%.2f] [%.2f]", drawn[0] / total, drawn[1] / total);
    zox_tst_check(drawn[0] > total * 0.3f && drawn[1] > total * 0.3f, "an eye drew little in game [%.2f] [%.2f]", drawn[0] / total, drawn[1] / total);
    free(pixels);
}

void zox_tsts_xr_play(ecs* world) {
    zox_tst_add("xr_play", zox_tst_xr_play_ready, zox_tst_xr_play_check, 120);
    zox_tst_set_setup("xr_play", zox_tst_xr_sim_setup, 1);
}
