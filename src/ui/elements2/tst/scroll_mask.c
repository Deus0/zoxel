static entity zox_tst_scroll_mask_list(ecs* world, entity canvas, int y, const char* text, entity* rows) {
    const int2 size = { 128, 64 };
    const entity view = spawn_scrollview(world, canvas, (int2) { 0, y }, size, float2_half, 10, 2, 2, 0);
    const LayoutParentData root = { .e = canvas }, parent = { .e = view };
    SpawnListElement elements[] = { { .text = text }, { .text = "Short" } };
    const entity list = spawn_list(world, root, parent,
        (ElementSpawnData) { .prefab = prefab_list, .anchor = float2_half, .size = size },
        (SpawnList) { .elements = elements, .count = 2, .visible_count = 2, .font_size = 12,
            .margins = { 4, 4 }, .button_padding = { 4, 4 } },
        zox_alignment_left, rows);
    zox_set_link(world, list, ScrollviewLink, view);
    zox_set_link(world, view, ListUILink, list);
    scrollview_fit_list_size(world, list, size, 0, 0);     // the list is the viewport, its Mask
    return view;
}

static byte zox_tst_scroll_mask_ready(ecs* world) {
    static entity canvas, view, fit, row, handle;
    static int step;
    static int2 handle_point;
    static float row_x;
    zox_tst_progress(step);
    switch (step) {
        case 0: {
            zox_tst_waiting = "the scroll fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            entity rows[2];
            view = zox_tst_scroll_mask_list(world, canvas, 42,
                "This row is much wider than the scroll area", rows);
            row = rows[0];
            fit = zox_tst_scroll_mask_list(world, canvas, -42, "Fits", rows);
            break;
        }
        case 1: {
            zox_tst_waiting = "the scrollviews to lay out their rows and handles";
            const entity bar = zox_get_child_by_id(world, view, zox_id(HorizontalScrollbar));
            handle = zox_get_child_by_id(world, bar, zox_id(ScrollbarHandle));
            if (!zox_valid(handle) || zox_getv(handle, RenderDisabled) ||
                !zox_tst_ui_pointer_position(world, canvas, handle, &handle_point)) {
                return 0;
            }
            const entity fit_bar = zox_get_child_by_id(world, fit, zox_id(HorizontalScrollbar));
            zox_tst_check(!zox_getv(bar, RenderDisabled) && zox_getv(handle, LayoutSize).x < zox_getv(bar, LayoutSize).x,
                "wide list did not show a proportional scrollbar");
            zox_tst_check(zox_valid(fit_bar) && zox_getv(fit_bar, RenderDisabled), "fitting list showed a horizontal scrollbar");
            // the viewport: the list masks, not the scroll view, and it stops above the shown horizontal bar
            const entity list = zox_get_link(world, view, ListUILink);
            zox_tst_check(zox_has(list, Mask) && !zox_has(view, Mask), "the list isn't the scroll view's mask");
            zox_tst_check(zox_getv(list, LayoutSize).y == zox_getv(view, LayoutSize).y - zox_getv(bar, LayoutSize).y,
                "the list is [%i] high, it overlaps the horizontal bar", zox_getv(list, LayoutSize).y);
            const entity fit_list = zox_get_link(world, fit, ListUILink);
            zox_tst_check(zox_getv(fit_list, LayoutSize).y == zox_getv(fit, LayoutSize).y,
                "a list with no bars showing isn't the scroll view's size");
            zox_setv(handle, Layer, 32);
            row_x = zox_getv(row, Position2).x;
            zox_tst_mouse(handle_point, 0);
            break;
        }
        case 2:
            zox_tst_waiting = "the horizontal handle to receive the pointer";
            if (!zox_tst_ui_pointer_position(world, canvas, handle, &handle_point)) {
                return 0;
            }
            zox_tst_mouse(handle_point, 0);
            if (zox_tst_mouse_target(world, dbg_player) != handle) {
                return 0;
            }
            zox_tst_mouse(handle_point, 1);
            break;
        case 3:
            zox_tst_waiting = "the horizontal handle to be grabbed";
            if (zox_get_link(world, handle, DraggerLink) != dbg_player) {
                return 0;
            }
            zox_tst_mouse(int2_add(handle_point, (int2) { 100, 0 }), 1);
            break;
        default:
            zox_tst_waiting = "the wide row to shift left";
            if (zox_getv(row, Position2).x >= row_x) {
                return 0;
            }
            return 1;
    }
    step++;
    return 0;
}
