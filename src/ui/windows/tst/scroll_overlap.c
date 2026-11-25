// tst: scrollview_overlap, wheel input moves the top list and its handle in a shared screen region.
static byte zox_tst_scroll_overlap_ready(ecs* world) {
    static entity canvas, windows[2], views[2], handles[2], lists[2], rear_header;
    static int step, click_step, wheel_step, initial_y[2], front_y;
    static int2 overlap, header;
    zox_tst_progress(step);
    switch (step) {
        case 0: {
            zox_tst_waiting = "the scrollview fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            SpawnListElement rows[] = {
                { .text = "First", .type = list_element_type_button },
                { .text = "Second", .type = list_element_type_button },
                { .text = "Third", .type = list_element_type_button },
                { .text = "Fourth", .type = list_element_type_button }
            };
            const SpawnList data = window_list_data(rows, 4, 2, 16, byte2_single(4));
            const int2 size = { 192, 112 };
            for (int i = 0; i < 2; i++) {
                const entity3 window = spawn_window(world, prefab_window, prefab_body, i ? "Front" : "Rear", canvas,
                    (int2) { i ? 48 : -48, 0 }, size, float2_half, 12, byte2_single(4), NULL);
                const entity2 view = spawn_window_list_view(world, canvas, window.z, int2_zero, data,
                    (int2) { 172, 112 }, size, 1, 10, zox_alignment_centre, NULL);
                windows[i] = window.x;
                views[i] = view.x;
                lists[i] = view.y;
                handles[i] = get_scrollview_handle(world, view.x);
                if (!i) {
                    rear_header = window.y;
                }
            }
            break;
        }
        case 1:
            zox_tst_waiting = "the scrollviews to finish opening";
            if (zox_getv(windows[0], LocalScale1) != 1 || zox_getv(windows[1], LocalScale1) != 1 ||
                !zox_tst_ui_pointer_position(world, canvas, views[1], &overlap) ||
                !zox_tst_ui_pointer_position(world, canvas, rear_header, &header)) {
                return 0;
            }
            if (!zox_valid(handles[0]) || !zox_valid(handles[1])) {
                zox_tst_check(0, "fixture scrollviews need both scrollbar handles");
                return 1;
            }
            initial_y[0] = zox_getv(handles[0], LayoutPosition).y;
            initial_y[1] = zox_getv(handles[1], LayoutPosition).y;
            overlap.x -= 48;
            header.x -= 48;
            zox_tst_mouse(overlap, 0);
            break;
        case 2:
        case 4: {
            const int active = step == 2 ? 1 : 0;
            const int other = 1 - active;
            zox_tst_waiting = "a wheel notch over both scrollviews";
            if (wheel_step < 4) {
                zox_tst_mouse(overlap, 0);
                zox_tst_mouse_wheel(wheel_step == 1 ? -1 : 0);
                wheel_step++;
                return 0;
            }
            wheel_step = 0;
            const int active_y = zox_getv(handles[active], LayoutPosition).y;
            zox_tst_check(zox_getv(lists[active], ListStart) == 1 && active_y < initial_y[active],
                "wheel did not scroll the top list [%i] and its handle", active);
            zox_tst_check(zox_getv(lists[other], ListStart) == (step == 2 ? 0 : 1) &&
                zox_getv(handles[other], LayoutPosition).y == (step == 2 ? initial_y[other] : front_y),
                "wheel moved the covered list [%i] or its handle", other);
            front_y = active_y;
            if (step == 4) {
                return 1;
            }
            break;
        }
        default:
            zox_tst_waiting = "raising the rear scrollview's window";
            if (!zox_tst_ui_click_at(header, &click_step)) {
                return 0;
            }
            zox_tst_check(zox_getv(lists[0], Layer) > zox_getv(lists[1], Layer), "the rear list did not move above the front list");
            break;
    }
    step++;
    return 0;
}
