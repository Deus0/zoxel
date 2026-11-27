// tst: scroll_corner, a list wider and taller than its view shows both bars, meeting at the corner, not overlapping
static byte zox_tst_scroll_corner_ready(ecs* world) {
    static entity view;
    static int step;
    if (!step) {
        const entity canvas = zox_tst_ui_fixture_canvas(world);
        if (!canvas) {
            return 0;
        }
        SpawnListElement rows[] = {
            { .text = "A row much wider than its view", .type = list_element_type_button },
            { .text = "Second", .type = list_element_type_button },
            { .text = "Third", .type = list_element_type_button },
            { .text = "Fourth", .type = list_element_type_button },
            { .text = "Fifth", .type = list_element_type_button },
            { .text = "Sixth", .type = list_element_type_button },
            { .text = "Seventh", .type = list_element_type_button },
            { .text = "Eighth", .type = list_element_type_button }
        };
        const SpawnList data = window_list_data(rows, 8, 2, 16, byte2_single(4));
        const int2 size = { 160, 112 };
        const entity3 window = spawn_window(world, prefab_window, prefab_body, "Corner", canvas,
            int2_zero, size, float2_half, 12, byte2_single(4), NULL);
        view = spawn_window_list_view(world, canvas, window.z, int2_zero, data,
            (int2) { 140, 112 }, size, 1, 10, zox_alignment_left, NULL).x;
        step++;
        return 0;
    }
    zox_tst_waiting = "both scrollbars to show";
    const entity vertical = zox_get_child_by_id(world, view, zox_id(Scrollbar));
    const entity horizontal = zox_get_child_by_id(world, view, zox_id(HorizontalScrollbar));
    if (!vertical || !horizontal || zox_getv(vertical, RenderDisabled) || zox_getv(horizontal, RenderDisabled)) {
        return 0;
    }
    const int2 size = zox_getv(view, LayoutSize);
    const int2 v = zox_getv(vertical, LayoutSize), h = zox_getv(horizontal, LayoutSize);
    if (h.x != size.x - v.x || v.y != size.y - h.y) {
        return 0;   // fitted the frame after the bar shows
    }
    zox_tst_check(zox_getv(horizontal, LayoutPosition).x == -v.x / 2, "horizontal bar not left of the vertical one");
    zox_tst_check(zox_getv(vertical, LayoutPosition).y == h.y / 2, "vertical bar not above the horizontal one");
    return 1;
}
