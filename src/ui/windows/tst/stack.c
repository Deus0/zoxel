// tst: window_stack_input, the top window both draws and receives the click in a shared region.
static entity zox_tst_stack_windows[2];
static entity zox_tst_stack_buttons[2];
static int zox_tst_stack_clicks[2];

static void zox_tst_stack_clicked(ecs* world, ClickEventData event) {
    (void) world;
    for (int i = 0; i < 2; i++) {
        zox_tst_stack_clicks[i] += event.clicked == zox_tst_stack_buttons[i];
    }
}

static byte zox_tst_window_stack_ready(ecs* world) {
    static entity canvas, rear_header;
    static int step, click_step;
    static int2 overlap, header;
    static const char* const shots[] = { "front.ppm", "raised.ppm" };
    zox_tst_progress(step);
    switch (step) {
        case 0:
            zox_tst_waiting = "the UI fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            for (int i = 0; i < 2; i++) {
                const int x = i ? 48 : -48;
                const entity3 window = spawn_window(world, prefab_window, prefab_body, i ? "Front" : "Rear",
                    canvas, (int2) { x, 0 }, (int2) { 192, 96 }, float2_half, 12, byte2_single(4), NULL);
                zox_tst_stack_windows[i] = window.x;
                zox_tst_stack_buttons[i] = zox_tst_ui_color_button(world, window.z, (int2) { -x, 0 },
                    float2_half, i ? zox_tst_ui_green : zox_tst_ui_red, zox_tst_stack_clicked);
                if (!i) {
                    rear_header = window.y;
                }
            }
            break;
        case 1: {
            zox_tst_waiting = "both windows to finish opening and lay out their controls";
            if (zox_getv(zox_tst_stack_windows[0], LocalScale1) != 1 ||
                zox_getv(zox_tst_stack_windows[1], LocalScale1) != 1 ||
                !zox_tst_ui_pointer_position(world, canvas, zox_tst_stack_buttons[1], &overlap) ||
                !zox_tst_ui_pointer_position(world, canvas, rear_header, &header)) {
                return 0;
            }
            header.x -= 48;
            int2 rear_button;
            const byte aligned = zox_tst_ui_pointer_position(world, canvas, zox_tst_stack_buttons[0], &rear_button) &&
                abs(overlap.x - rear_button.x) <= 1 && abs(overlap.y - rear_button.y) <= 1;
            zox_tst_check(aligned, "fixture buttons do not overlap");
            if (!aligned) {
                return 1;
            }
            zox_tst_ui_capture(shots[0]);
            break;
        }
        case 2:
        case 5: {
            zox_tst_waiting = "the window stack screenshot";
            if (zox_screenshot_request[0]) {
                return 0;
            }
            const int image = step == 2 ? 0 : 1;
            int2 size = int2_zero;
            byte* pixels = zox_tst_load_ppm(zox_tst_path(shots[image]), &size);
            zox_tst_check(zox_tst_ui_pixel_is(pixels, size, overlap, image ? zox_tst_ui_red : zox_tst_ui_green),
                "%s window did not draw its button at [%i,%i]", image ? "raised" : "front", overlap.x, overlap.y);
            free(pixels);
            break;
        }
        case 3:
            zox_tst_waiting = "a click on the front button";
            if (!zox_tst_ui_click_at(overlap, &click_step)) {
                return 0;
            }
            zox_tst_check(zox_tst_stack_clicks[0] == 0 && zox_tst_stack_clicks[1] == 1,
                "front click reached rear [%i], front [%i]", zox_tst_stack_clicks[0], zox_tst_stack_clicks[1]);
            break;
        case 4:
            zox_tst_waiting = "a click on the rear window's exposed header";
            if (!zox_tst_ui_click_at(header, &click_step)) {
                return 0;
            }
            zox_tst_check(zox_getv(zox_tst_stack_buttons[0], Layer) > zox_getv(zox_tst_stack_buttons[1], Layer),
                "clicking the exposed header did not raise the rear window");
            zox_tst_ui_capture(shots[1]);
            break;
        default:
            zox_tst_waiting = "a click on the raised button";
            if (!zox_tst_ui_click_at(overlap, &click_step)) {
                return 0;
            }
            zox_tst_check(zox_tst_stack_clicks[0] == 1 && zox_tst_stack_clicks[1] == 1,
                "after raising and clicking, rear [%i], front [%i]", zox_tst_stack_clicks[0], zox_tst_stack_clicks[1]);
            return 1;
    }
    step++;
    return 0;
}
