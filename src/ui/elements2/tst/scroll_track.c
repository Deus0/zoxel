// tst: scroll_track, pressing a scrollbar's track jumps its handle there and scrolls the list (scrollbar_track_press_system)
static byte zox_tst_scroll_track_ready(ecs* world) {
    static entity canvas, row, bar, handle;
    static int step;
    static int2 point;
    static float row_x;
    zox_tst_progress(step);
    if (step == 0) {
        zox_tst_waiting = "the scroll fixture canvas";
        canvas = zox_tst_ui_fixture_canvas(world);
        if (!canvas) {
            return 0;
        }
        entity rows[2];
        const entity view = zox_tst_scroll_mask_list(world, canvas, 0, "This row is much wider than the scroll area", rows);
        row = rows[0];
        bar = zox_get_child_by_id(world, view, zox_id(HorizontalScrollbar));
    } else if (step == 1) {
        zox_tst_waiting = "the horizontal bar to show";
        handle = zox_get_child_by_id(world, bar, zox_id(ScrollbarHandle));
        if (!zox_valid(handle) || zox_getv(handle, RenderDisabled) || !zox_tst_ui_pointer_position(world, canvas, bar, &point)) {
            return 0;
        }
        point.x += zox_getv(bar, LayoutSize).x / 2 - 3;     // the track's right end, away from the handle
        row_x = zox_getv(row, Position2).x;
        zox_tst_mouse(point, 0);
    } else if (step == 2) {
        zox_tst_waiting = "the track to receive the pointer";
        zox_tst_mouse(point, 0);
        if (zox_tst_mouse_target(world, dbg_player) != bar) {
            return 0;
        }
        zox_tst_mouse(point, 1);
    } else {
        zox_tst_waiting = "the handle to jump right and the row to shift left";
        zox_tst_mouse(point, 1);
        if (zox_get_link(world, handle, DraggerLink) != dbg_player || zox_getv(row, Position2).x >= row_x) {
            return 0;
        }
        zox_tst_check(zox_getv(handle, LayoutPosition).x == zox_getv(handle, LayoutConstraints).y, "the handle isn't at the track's end");
        return 1;
    }
    step++;
    return 0;
}
