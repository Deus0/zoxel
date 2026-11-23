// tst: coop_pause, split screen: one pause menu over the whole window for both players (huds/fun/pause_canvas.c)
//  - the second player's gamepad presses Start: the game pauses, one pause menu on the shared canvas (as wide as the
//    window), none on either player's canvas
//  - its gamepad's selection goes into it (it has no window of its own open), A on its first button (Wander) resumes
//    the game for both, the shared canvas gone
//  - paused both players link the shared canvas too (CanvasLink, many): their first link is still their own canvas
//  - the shared canvas is given low ids if it can (zox_tst_recycle_low_ids), as in a long game: sorted before the
//    players' own (deferred frees: it often takes others first, logged)
//  - each player's own canvas darkens paused and fades back resumed (the fades went to the shared canvas: one stayed dark)
//  - a screenshot paused: tst/coop_pause.ppm
//  - run: bash bsh/tst.sh coop_pause
int zox_tst_cp_stage = 0;
int zox_tst_cp_frames = 0;
int zox_tst_cp_menus = -1, zox_tst_cp_own_menus = -1;
int2 zox_tst_cp_canvas_size, zox_tst_cp_window_size;
byte zox_tst_cp_selected = 0, zox_tst_cp_resumed = 0;
float zox_tst_cp_dark[2] = { -1, -1 }, zox_tst_cp_light[2] = { -1, -1 };    // each own overlay's alpha, paused and resumed
double zox_tst_cp_resumed_time = 0;
int zox_tst_cp_recycled = 0;
entity zox_tst_cp_shared_id = 0, zox_tst_cp_own_id = 0;
int zox_tst_cp_own_first = -1;     // players whose first CanvasLink is their own canvas, paused (the shared one's linked too)

// a player's own canvas's overlay alpha (player_own_canvas)
static inline float zox_tst_cp_overlay(ecs* world, byte i) {
    const entity overlay = zox_get_child_by_id(world, player_own_canvas(world, zox_players[i]), zox_id(CanvasOverlay));
    return zox_valid(overlay) ? zox_getv(overlay, Alpha) : -1;
}

static inline int zox_tst_cp_count_menus(ecs* world, entity canvas) {
    int count = 0;
    if (!zox_valid(canvas)) {
        return 0;
    }
    iter it = zox_children(world, canvas);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            count += zox_has(it.entities[i], MenuPaused) && !zox_has(it.entities[i], UIClosing);
        }
    }
    return count;
}

byte zox_tst_coop_pause_ready(ecs* world) {
    // (paused, they're not playing)
    if (!zox_tst_cp_stage && !zox_tst_coop_playing(world)) {
        return 0;
    }
    const entity player = zox_players[1];
    const entity game = zox_get_link(world, player, GameLink);
    const int f = ++zox_tst_cp_frames;
    zox_tst_progress(zox_tst_cp_stage * 1000 + f);
    memset(zox_tst_pad_held, 0, sizeof(zox_tst_pad_held));
    zox_tst_pad_stick = float2_zero;
    switch (zox_tst_cp_stage) {
        case 0:
            // the second player on its gamepad (in coop a player keeps its device, so set as it joined with it)
            zox_tst_pad = zox_tst_spawn_pad(world, player);
            zox_setv(player, DeviceMode, zox_device_mode_gamepad);
            zox_set_link(world, player, GamepadLink, zox_tst_pad);
            zox_remove(zox_tst_pad, Disabled);
            // the shared camera and canvas take these as the game pauses
            zox_tst_cp_recycled = zox_tst_recycle_low_ids(world, player_own_canvas(world, zox_players[0]), 2);
            zox_tst_cp_stage = 1;
            zox_tst_cp_frames = 0;
            return 0;
        case 1:
            zox_tst_waiting = "Start pausing, the one pause menu";
            zox_tst_pad_held[zox_btn_start] = f >= 10 && f < 14;
            if (f < 40 || zox_getv(game, GameState) != zox_game_state_paused || !zox_valid(shared_canvas) ||
                !zox_tst_cp_count_menus(world, shared_canvas)) {
                return f > 600;
            }
            {
                const entity target = zox_getv(zox_tst_pad, Target);
                const entity menu = zox_get_child_by_id(world, shared_canvas, zox_id(MenuPaused));
                zox_tst_cp_selected = zox_valid(target) && zox_get_parent_by_id(world, target, zox_id(Window)) == menu;
                if (!zox_tst_cp_selected && f < 200) {
                    return 0;
                }
            }
            zox_tst_cp_menus = zox_tst_cp_count_menus(world, shared_canvas);
            zox_tst_cp_own_menus = zox_tst_cp_count_menus(world, zox_get_link(world, zox_players[0], CanvasLink)) +
                zox_tst_cp_count_menus(world, zox_get_link(world, player, CanvasLink));
            zox_tst_cp_canvas_size = zox_getv(shared_canvas, LayoutSize);
            zox_tst_cp_own_first = 0;
            for (int k = 0; k < 2; k++) {
                const entity c = zox_get_link(world, zox_players[k], CanvasLink);
                zox_tst_cp_own_first += zox_valid(c) && zox_get_link(world, c, PlayerLink) == zox_players[k];
            }
            zox_tst_cp_window_size = zox_getv(main_app, WindowSize);
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("coop_pause.ppm"));
            zox_tst_cp_stage = 2;
            zox_tst_cp_frames = 0;
            return 0;
        case 2:
            zox_tst_waiting = "A on Wander, resuming";
            zox_tst_pad_held[zox_btn_a] = f >= 10 && f < 14;
            if (f == 9) {
                for (byte i = 0; i < 2; i++) {
                    zox_tst_cp_dark[i] = zox_tst_cp_overlay(world, i);
                }
                zox_tst_cp_shared_id = shared_canvas;
                zox_tst_cp_own_id = player_own_canvas(world, zox_players[0]);
            }
            if (f > 40 && zox_getv(game, GameState) == zox_game_state_playing && !zox_valid(shared_canvas)) {
                zox_tst_cp_resumed = 1;
                zox_tst_cp_resumed_time = zox_current_time;
                zox_tst_cp_stage = 3;
                return 0;
            }
            return f > 600;
        case 3:
            zox_tst_waiting = "both screens faded back";
            if (zox_current_time - zox_tst_cp_resumed_time < pause_fade_time + 0.5) {
                return 0;
            }
            for (byte i = 0; i < 2; i++) {
                zox_tst_cp_light[i] = zox_tst_cp_overlay(world, i);
            }
            return 1;
    }
    return 1;
}

void zox_tst_coop_pause_check(ecs* world) {
    (void) world;
    zox_log("[tst] coop_pause: [%i] pause menus shared, [%i] on the players' canvases, shared canvas [%ix%i] of the window [%ix%i], selected [%i], resumed [%i]",
        zox_tst_cp_menus, zox_tst_cp_own_menus, zox_tst_cp_canvas_size.x, zox_tst_cp_canvas_size.y, zox_tst_cp_window_size.x,
        zox_tst_cp_window_size.y, zox_tst_cp_selected, zox_tst_cp_resumed);
    zox_tst_check(zox_tst_cp_menus == 1, "[%i] pause menus on the shared canvas, not one", zox_tst_cp_menus);
    zox_tst_check(zox_tst_cp_own_menus == 0, "[%i] pause menus on the players' own canvases", zox_tst_cp_own_menus);
    zox_tst_check(zox_tst_cp_canvas_size.x == zox_tst_cp_window_size.x && zox_tst_cp_canvas_size.y == zox_tst_cp_window_size.y,
        "the shared canvas is [%ix%i], not the window's [%ix%i]", zox_tst_cp_canvas_size.x, zox_tst_cp_canvas_size.y,
        zox_tst_cp_window_size.x, zox_tst_cp_window_size.y);
    zox_tst_check(zox_tst_cp_own_first == 2, "paused, [%i] of 2 players' canvas link (the first) is their own", zox_tst_cp_own_first);
    zox_tst_check(zox_tst_cp_selected, "the second player's gamepad didn't select in the shared pause menu");
    zox_tst_check(zox_tst_cp_resumed, "A on Wander didn't resume the game");
    zox_log("[tst] coop_pause: shared canvas [%lu], player 1's own [%lu]", zox_tst_cp_shared_id, zox_tst_cp_own_id);
    // the low ids are freed (deferred) frames before the pause, entities deleted since are reused first, so the shared
    //  canvas only sometimes gets one: logged, the players' first link (own_first, above) is what's checked either way
    if ((uint) zox_tst_cp_shared_id >= (uint) zox_tst_cp_own_id) {
        zox_log("[tst] coop_pause: the shared canvas [%lu] didn't take a low id (under [%lu]), its order isn't tested this run",
            zox_tst_cp_shared_id, zox_tst_cp_own_id);
    }
    for (byte i = 0; i < 2; i++) {
        zox_log("[tst] coop_pause: player [%i]'s overlay paused [%.2f] resumed [%.2f]", i + 1, zox_tst_cp_dark[i], zox_tst_cp_light[i]);
        zox_tst_check(zox_tst_cp_dark[i] > 0, "paused, player [%i]'s screen isn't darkened [%.2f]", i + 1, zox_tst_cp_dark[i]);
        zox_tst_check(zox_tst_cp_light[i] == 0, "resumed, player [%i]'s screen is still dark [%.2f]", i + 1, zox_tst_cp_light[i]);
    }
}
