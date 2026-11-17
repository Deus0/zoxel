// tst: xr_navigator, paused in a game on the simulator, the taskbar's a 3D row above the pause menu, and the grips
//  (LB, RB) step through its uis one at a time (huds/sys/navigator.c, huds/ins/taskbar.c spawn_taskbar3)
//  - the menu button pauses: the taskbar's a Canvas3, its pause frame lit (the pause menu open)
//  - RB: the frame after it lit, only it, the pause menu closing; LB: the pause frame lit again, the menu back
//  - screenshot paused, the taskbar over the menu: tst/xr_navigator.ppm
//  - run: bash bsh/tst.sh xr_navigator
int zox_tst_xr_nav_stage = 0;
int zox_tst_xr_nav_frames = 0;
byte zox_tst_xr_nav_taskbar3 = 0;
int zox_tst_xr_nav_paused_lit = -1;     // the lit frame's index paused, after RB, after LB
int zox_tst_xr_nav_rb_lit = -1;
int zox_tst_xr_nav_lb_lit = -1;
int zox_tst_xr_nav_rb_count = 0;        // how many were lit after RB
int zox_tst_xr_nav_frame_count = 0;
byte zox_tst_xr_nav_menu_back = 0;

static inline void zox_tst_xr_nav_next() {
    zox_tst_xr_nav_stage++;
    zox_tst_xr_nav_frames = 0;
}

// the lit frame's place in the row (-1 none), and how many are lit
static int zox_tst_xr_nav_lit(ecs* world, entity canvas, int* lit_count) {
    entity frames[navigator_frames_max];
    const int count = navigator_frames(world, canvas, frames);
    zox_tst_xr_nav_frame_count = count;
    int lit = -1;
    *lit_count = 0;
    for (int i = 0; i < count; i++) {
        if (zox_getv(frames[i], ActiveState)) {
            (*lit_count)++;
            if (lit < 0) {
                lit = i;
            }
        }
    }
    return lit;
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
    int lit_count = 0;
    switch (zox_tst_xr_nav_stage) {
        case 0:
            zox_tst_waiting = "paused, the taskbar up";
            xr_sim_script_start = f >= 20 && f < 24;
            {
                const entity taskbar = zox_get_child_by_id(world, canvas, zox_id(Taskbar));
                if (f > 40 && zox_valid(taskbar) && zox_getv(player, PlayerState) == zox_player_state_paused &&
                    zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuPaused)))) {
                    zox_tst_xr_nav_taskbar3 = zox_has(taskbar, Canvas3);
                    zox_tst_xr_nav_paused_lit = zox_tst_xr_nav_lit(world, canvas, &lit_count);
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
                zox_tst_xr_nav_rb_lit = zox_tst_xr_nav_lit(world, canvas, &zox_tst_xr_nav_rb_count);
                zox_tst_xr_nav_next();
            }
            return 0;
        case 3:
            zox_tst_waiting = "LB";
            xr_sim_script_lb = f >= 2 && f < 6;
            if (f == 30) {
                zox_tst_xr_nav_lb_lit = zox_tst_xr_nav_lit(world, canvas, &lit_count);
                const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuPaused));
                zox_tst_xr_nav_menu_back = zox_valid(menu) && !zox_has(menu, UIClosing);
                return 1;
            }
            return 0;
    }
    return 0;
}

void zox_tst_xr_navigator_check(ecs* world) {
    (void) world;
    zox_log("[tst] xr_navigator: 3D taskbar [%i] of [%i] frames, lit paused [%i], after RB [%i] (lit [%i]), after LB [%i], the pause menu back [%i]",
        zox_tst_xr_nav_taskbar3, zox_tst_xr_nav_frame_count, zox_tst_xr_nav_paused_lit, zox_tst_xr_nav_rb_lit,
        zox_tst_xr_nav_rb_count, zox_tst_xr_nav_lb_lit, zox_tst_xr_nav_menu_back);
    const int count = zox_tst_xr_nav_frame_count;
    zox_tst_check(zox_tst_xr_nav_taskbar3, "the taskbar isn't 3D paused");
    zox_tst_check(count > 1, "the taskbar has [%i] frames", count);
    zox_tst_check(zox_tst_xr_nav_paused_lit >= 0, "paused, no taskbar frame lit (the pause menu's)");
    if (count > 1 && zox_tst_xr_nav_paused_lit >= 0) {
        zox_tst_check(zox_tst_xr_nav_rb_lit == (zox_tst_xr_nav_paused_lit + 1) % count, "RB lit [%i], not the next", zox_tst_xr_nav_rb_lit);
    }
    zox_tst_check(zox_tst_xr_nav_rb_count == 1, "after RB [%i] uis open, not one", zox_tst_xr_nav_rb_count);
    zox_tst_check(zox_tst_xr_nav_lb_lit == zox_tst_xr_nav_paused_lit, "LB lit [%i], not back to the pause menu's", zox_tst_xr_nav_lb_lit);
    zox_tst_check(zox_tst_xr_nav_menu_back, "LB didn't bring the pause menu back");
}

void zox_tsts_xr_navigator(ecs* world) {
    zox_tst_add("xr_navigator", zox_tst_xr_navigator_ready, zox_tst_xr_navigator_check, 120);
    zox_tst_set_setup("xr_navigator", zox_tst_xr_sim_setup, 1);
}
