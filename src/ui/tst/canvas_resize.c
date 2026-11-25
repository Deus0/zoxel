// tst: canvas_resize_input, a live edge-anchored control follows the canvas in drawing and hit testing.
static int zox_tst_resize_clicks;

static void zox_tst_resize_clicked(ecs* world, ClickEventData event) {
    (void) world;
    (void) event;
    zox_tst_resize_clicks++;
}

static byte zox_tst_canvas_resize_ready(ecs* world) {
    static entity canvas, button, app;
    static int step, click_step, shape, clicks_before;
    static int2 point, old_point;
    // within the test window's 460x460: a hidden window's gl surface never grows, past it reads garbage
    static const int2 sizes[] = { { 440, 300 }, { 300, 440 }, { 440, 300 } };
    static const char* const shots[] = { "wide.ppm", "tall.ppm", "wide_again.ppm" };
    const int2 inset = { 180, 190 };
    zox_tst_progress(shape * 10 + step);
    switch (step) {
        case 0:
            zox_tst_waiting = "the UI fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            app = zox_get_link(world, canvas, AppLink);
            button = zox_tst_ui_color_button(world, canvas, (int2) { -inset.x, -inset.y },
                (float2) { 1, 1 }, zox_tst_ui_red, zox_tst_resize_clicked);
            break;
        case 1:
            // Apply the same resize notification as SDL, including the offscreen Vulkan target.
            zox_sdl_window_size(zox_getv(app, SDLWindow), sizes[shape]);
            on_window_resized(world, app, sizes[shape], 0);
            break;
        case 2: {
            zox_tst_waiting = "the camera and live canvas to resize";
            CanvasViewport viewport;
            if (!get_canvas_viewport(world, canvas, &viewport) ||
                !int2_equals(viewport.size, sizes[shape]) || !int2_equals(zox_getv(canvas, LayoutSize), sizes[shape])) {
                return 0;
            }
            old_point = point;
            point = int2_add(viewport.position, int2_sub(sizes[shape], inset));
            zox_tst_ui_capture(shots[shape]);
            break;
        }
        case 3: {
            zox_tst_waiting = "the resized frame screenshot";
            if (zox_screenshot_request[0]) {
                return 0;
            }
            int2 size = int2_zero;
            byte* pixels = zox_tst_load_ppm(zox_tst_path(shots[shape]), &size);
            int2 hit;
            zox_tst_check(zox_tst_ui_pointer_position(world, canvas, button, &hit) &&
                abs(hit.x - point.x) <= 1 && abs(hit.y - point.y) <= 1,
                "at [%ix%i], the button's hit bounds did not follow its anchor", sizes[shape].x, sizes[shape].y);
            zox_tst_check(zox_tst_ui_pixel_is(pixels, size, point, zox_tst_ui_red),
                "at [%ix%i], the button did not draw at its expected anchor [%i,%i]",
                sizes[shape].x, sizes[shape].y, point.x, point.y);
            if (shape) {
                zox_tst_check(pixels && !zox_tst_ui_pixel_is(pixels, size, old_point, zox_tst_ui_red),
                    "after resizing, the button still drew at its old position [%i,%i]", old_point.x, old_point.y);
            }
            free(pixels);
            break;
        }
        case 4:
            zox_tst_waiting = "a click at the old anchor";
            if (!click_step) {
                clicks_before = zox_tst_resize_clicks;
            }
            if (shape && !zox_tst_ui_click_at(old_point, &click_step)) {
                return 0;
            }
            zox_tst_check(zox_tst_resize_clicks == clicks_before, "the old position still received a click");
            clicks_before = zox_tst_resize_clicks;
            break;
        default:
            zox_tst_waiting = "a click at the new anchor";
            if (!zox_tst_ui_click_at(point, &click_step)) {
                return 0;
            }
            zox_tst_check(zox_tst_resize_clicks - clicks_before == 1, "at [%ix%i] the anchored button received [%i] clicks, expected one",
                sizes[shape].x, sizes[shape].y, zox_tst_resize_clicks - clicks_before);
            shape++;
            step = 1;
            return shape == 3;
    }
    step++;
    return 0;
}
