// tst: window_toggle, windows pop in and out (ui/windows/ins/window.c, ui/fun/pop.c, toggle.c), the id window
//  - toggled open it pops in (its scale from 0 to 1), it has a close button
//  - toggled again it pops out (UIClosing) and is deleted once it has
//  - toggled while popping out, a new one opens (a closing one reads as closed)
//  - run: bash bsh/tst.sh window_toggle
byte zox_tst_wt_stage = 0;
double zox_tst_wt_time = 0;
entity zox_tst_wt_window = 0;
float zox_tst_wt_scale_min = 1, zox_tst_wt_scale_max = 0;
byte zox_tst_wt_close_button = 0, zox_tst_wt_closing = 0, zox_tst_wt_deleted = 0, zox_tst_wt_reopened = 0;

static inline entity zox_tst_wt_toggle(ecs* world) {
    return toggle_ui_with_id(world, spawn_player_menu_id, zox_id(MenuId), dbg_player);
}

byte zox_tst_window_toggle_ready(ecs* world) {
    if (!zox_tst_wt_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_wt_window = zox_tst_wt_toggle(world);
        zox_tst_wt_stage = 1;
        zox_tst_wt_time = zox_current_time;
        return 0;
    }
    zox_tst_progress(zox_tst_wt_stage);
    zox_tst_waiting = "the window popping in and out";
    const double passed = zox_current_time - zox_tst_wt_time;
    if (zox_tst_wt_stage == 1) {
        // popping in
        if (zox_valid(zox_tst_wt_window) && zox_has(zox_tst_wt_window, LocalScale1)) {
            const float scale = zox_getv(zox_tst_wt_window, LocalScale1);
            zox_tst_wt_scale_min = float_min(zox_tst_wt_scale_min, scale);
            zox_tst_wt_scale_max = float_max(zox_tst_wt_scale_max, scale);
        }
        if (passed < 1) {
            return 0;
        }
        zox_tst_wt_close_button = zox_valid(zox_get_child_by_id_recursive(world, zox_tst_wt_window, zox_id(CloseButton)));
        zox_tst_wt_toggle(world);
        zox_tst_wt_closing = zox_has(zox_tst_wt_window, UIClosing);
        // toggled again while it pops out: a new one
        const entity reopened = zox_tst_wt_toggle(world);
        zox_tst_wt_reopened = zox_valid(reopened) && reopened != zox_tst_wt_window;
        zox_tst_wt_stage = 2;
        zox_tst_wt_time = zox_current_time;
        return 0;
    }
    if (passed < 1) {
        return 0;
    }
    zox_tst_wt_deleted = !zox_valid(zox_tst_wt_window);
    return 1;
}

void zox_tst_window_toggle_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] window_toggle its scale [%.2f to %.2f]", zox_tst_wt_scale_min, zox_tst_wt_scale_max);
    zox_tst_check(zox_valid(zox_tst_wt_window) || zox_tst_wt_deleted, "no window opened");
    zox_tst_check(zox_tst_wt_scale_min < 0.5f && zox_tst_wt_scale_max > 0.99f, "it didn't pop in, its scale [%.2f to %.2f]",
        zox_tst_wt_scale_min, zox_tst_wt_scale_max);
    zox_tst_check(zox_tst_wt_close_button, "it has no close button");
    zox_tst_check(zox_tst_wt_closing, "toggled again, it didn't pop out");
    zox_tst_check(zox_tst_wt_deleted, "it wasn't deleted after popping out");
    zox_tst_check(zox_tst_wt_reopened, "toggled while popping out, no new one opened");
}
