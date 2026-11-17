// tst: xr_navigator, paused in a game on the simulator, the taskbar's a 3D row above the pause menu, and the grips
//  (LB, RB) step through its uis one at a time (huds/sys/navigator.c, huds/ins/taskbar.c spawn_taskbar3)
//  - the menu button pauses: the taskbar's a Canvas3, its pause frame lit (the pause menu open)
//  - RB: the frame after it lit, the pause menu's not (it closed: a hud, the action bar, stays open); LB: the pause
//    frame lit again, the menu back
//  - screenshot paused, the taskbar over the menu: tst/xr_navigator.ppm
//  - then RB round every ui: each opens 3D (a Canvas3 with 3D elements: spawn_window's 3D twins), a screenshot each
//    (tst/xr_window_<n>.ppm)
//  - run: bash bsh/tst.sh xr_navigator
int zox_tst_xr_nav_stage = 0;
int zox_tst_xr_nav_frames = 0;
byte zox_tst_xr_nav_taskbar3 = 0;
int zox_tst_xr_nav_paused_lit = -1;     // the lit frame's index paused, after RB, after LB
int zox_tst_xr_nav_rb_lit = -1;
int zox_tst_xr_nav_lb_lit = -1;
byte zox_tst_xr_nav_rb_closed = 0;      // after RB the pause frame unlit, the next lit
int zox_tst_xr_nav_frame_count = 0;
byte zox_tst_xr_nav_menu_back = 0;
int zox_tst_xr_nav_round = 0;           // the uis stepped to by RB round the taskbar
int zox_tst_xr_nav_round_3d = 0;        // of them, opened 3D
char zox_tst_xr_nav_flat[256] = "";     // the ones that weren't

// the ui open for the lit frame (not one closing)
static entity zox_tst_xr_nav_open_ui(ecs* world, entity canvas) {
    entity frames[navigator_frames_max];
    const int count = navigator_frames(world, canvas, frames);
    for (int i = 0; i < count; i++) {
        if (!zox_getv(frames[i], ActiveState)) {
            continue;
        }
        const entity id = zox_getv(frames[i], TaskbarWindowID);
        iter it = zox_children(world, canvas);
        entity found = 0;
        while (zox_children_next(it)) {
            for (int j = 0; j < it.count; j++) {
                if (zox_has_id(it.entities[j], id) && !zox_has(it.entities[j], UIClosing)) {
                    found = it.entities[j];
                }
            }
        }
        if (!found) {
            zox_log("[tst] xr_navigator: the lit frame's ui [%s] isn't open", zox_getn(id));
        }
        return found;
    }
    return 0;
}

static int zox_tst_xr_nav_elements3(ecs* world, entity e) {
    int count = 0;
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            count += zox_has(it.entities[j], Element3) + zox_tst_xr_nav_elements3(world, it.entities[j]);
        }
    }
    return count;
}

static inline void zox_tst_xr_nav_next() {
    zox_tst_xr_nav_stage++;
    zox_tst_xr_nav_frames = 0;
}

static byte zox_tst_xr_nav_is_lit(ecs* world, entity canvas, int index) {
    entity frames[navigator_frames_max];
    const int count = navigator_frames(world, canvas, frames);
    return index >= 0 && index < count && zox_getv(frames[index], ActiveState);
}

// the paused frame's place in the row (MenuPaused's), -1 none
static int zox_tst_xr_nav_pause_index(ecs* world, entity canvas) {
    entity frames[navigator_frames_max];
    const int count = navigator_frames(world, canvas, frames);
    zox_tst_xr_nav_frame_count = count;
    for (int i = 0; i < count; i++) {
        if (zox_getv(frames[i], TaskbarWindowID) == zox_id(MenuPaused)) {
            return i;
        }
    }
    return -1;
}

byte zox_tst_xr_navigator_ready(ecs* world) {
    if (zox_tst_xr_play_stage < 3) {
        zox_tst_xr_play_ready(world);
        return 0;
    }
    const entity player = zox_players[0];
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const int f = ++zox_tst_xr_nav_frames;
    zox_tst_progress(zox_tst_xr_nav_stage * 1000 + f);
    xr_sim_script_start = 0;
    xr_sim_script_lb = 0;
    xr_sim_script_rb = 0;
    switch (zox_tst_xr_nav_stage) {
        case 0:
            zox_tst_waiting = "paused, the taskbar up";
            xr_sim_script_start = f >= 20 && f < 24;
            {
                const entity taskbar = zox_get_child_by_id(world, canvas, zox_id(Taskbar));
                if (f > 40 && zox_valid(taskbar) && zox_getv(player, PlayerState) == zox_player_state_paused &&
                    zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuPaused)))) {
                    zox_tst_xr_nav_taskbar3 = zox_has(taskbar, Canvas3);
                    const int pause = zox_tst_xr_nav_pause_index(world, canvas);
                    zox_tst_xr_nav_paused_lit = zox_tst_xr_nav_is_lit(world, canvas, pause) ? pause : -1;
                    zox_tst_xr_nav_next();
                }
            }
            return f > 600;
        case 1:
            zox_tst_waiting = "the screenshot paused";
            if (f == 60) {
                remove(zox_tst_path("xr_navigator.ppm"));
                snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("xr_navigator.ppm"));
            }
            if (f > 60 && !zox_screenshot_request[0]) {
                zox_tst_xr_nav_next();
            }
            return 0;
        case 2:
            zox_tst_waiting = "RB";
            xr_sim_script_rb = f >= 2 && f < 6;
            if (f == 30) {
                const int pause = zox_tst_xr_nav_paused_lit;
                const int next = (pause + 1) % zox_tst_xr_nav_frame_count;
                zox_tst_xr_nav_rb_lit = zox_tst_xr_nav_is_lit(world, canvas, next) ? next : -1;
                zox_tst_xr_nav_rb_closed = !zox_tst_xr_nav_is_lit(world, canvas, pause);
                zox_tst_xr_nav_next();
            }
            return 0;
        case 3:
            zox_tst_waiting = "LB";
            xr_sim_script_lb = f >= 2 && f < 6;
            if (f == 30) {
                zox_tst_xr_nav_lb_lit = zox_tst_xr_nav_is_lit(world, canvas, zox_tst_xr_nav_paused_lit) ? zox_tst_xr_nav_paused_lit : -1;
                const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuPaused));
                zox_tst_xr_nav_menu_back = zox_valid(menu) && !zox_has(menu, UIClosing);
                zox_tst_xr_nav_next();
            }
            return 0;
        case 4:
            zox_tst_waiting = "RB round every ui";
            {
                const int step = (f - 1) / 60, at = (f - 1) % 60;
                if (step >= zox_tst_xr_nav_frame_count) {
                    return 1;
                }
                xr_sim_script_rb = at >= 2 && at < 6;
                if (at == 40) {
                    const entity ui = zox_tst_xr_nav_open_ui(world, canvas);
                    zox_tst_xr_nav_round++;
                    if (zox_valid(ui) && zox_has(ui, Canvas3) && zox_tst_xr_nav_elements3(world, ui) > 1) {
                        zox_tst_xr_nav_round_3d++;
                    } else {
                        const size_t used = strlen(zox_tst_xr_nav_flat);
                        snprintf(zox_tst_xr_nav_flat + used, sizeof(zox_tst_xr_nav_flat) - used, " [%s]", zox_valid(ui) ? zox_getn(ui) : "none");
                    }
                    char name[64];
                    snprintf(name, sizeof(name), "xr_window_%i.ppm", step);
                    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(name));
                }
            }
            return 0;
    }
    return 0;
}

void zox_tst_xr_navigator_check(ecs* world) {
    (void) world;
    zox_log("[tst] xr_navigator: 3D taskbar [%i] of [%i] frames, lit paused [%i], after RB [%i] (the pause one closed [%i]), after LB [%i], the pause menu back [%i]",
        zox_tst_xr_nav_taskbar3, zox_tst_xr_nav_frame_count, zox_tst_xr_nav_paused_lit, zox_tst_xr_nav_rb_lit,
        zox_tst_xr_nav_rb_closed, zox_tst_xr_nav_lb_lit, zox_tst_xr_nav_menu_back);
    const int count = zox_tst_xr_nav_frame_count;
    zox_tst_check(zox_tst_xr_nav_taskbar3, "the taskbar isn't 3D paused");
    zox_tst_check(count > 1, "the taskbar has [%i] frames", count);
    zox_tst_check(zox_tst_xr_nav_paused_lit >= 0, "paused, no taskbar frame lit (the pause menu's)");
    if (count > 1 && zox_tst_xr_nav_paused_lit >= 0) {
        zox_tst_check(zox_tst_xr_nav_rb_lit == (zox_tst_xr_nav_paused_lit + 1) % count, "RB lit [%i], not the next", zox_tst_xr_nav_rb_lit);
    }
    zox_tst_check(zox_tst_xr_nav_rb_closed, "after RB the pause menu's frame is still lit");
    zox_tst_check(zox_tst_xr_nav_lb_lit == zox_tst_xr_nav_paused_lit, "LB lit [%i], not back to the pause menu's", zox_tst_xr_nav_lb_lit);
    zox_tst_check(zox_tst_xr_nav_menu_back, "LB didn't bring the pause menu back");
    zox_log("[tst] xr_navigator: round the taskbar, [%i] of [%i] uis 3D, not:%s", zox_tst_xr_nav_round_3d, zox_tst_xr_nav_round, zox_tst_xr_nav_flat);
    zox_tst_check(zox_tst_xr_nav_round == count && zox_tst_xr_nav_round_3d == count, "[%i] of [%i] uis opened 3D, not:%s",
        zox_tst_xr_nav_round_3d, count, zox_tst_xr_nav_flat);
}

void zox_tsts_xr_navigator(ecs* world) {
    zox_tst_add("xr_navigator", zox_tst_xr_navigator_ready, zox_tst_xr_navigator_check, 240);
    zox_tst_set_setup("xr_navigator", zox_tst_xr_sim_setup, 1);
}
