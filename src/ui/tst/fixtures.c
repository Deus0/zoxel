// The ready callback runs once per frame. Allow input extraction and its observers to settle at each edge.
static byte zox_tst_ui_click_at(int2 point, int* step) {
    if (*step == 6) {
        *step = 0;
        return 1;
    }
    zox_tst_mouse(point, *step >= 2 && *step < 4);
    (*step)++;
    return 0;
}

// Reuse the boot canvas and devices. An empty click activates mouse mode before clicking any fixture widget.
static entity zox_tst_ui_fixture_canvas(ecs* world) {
    static byte cleared;
    static int click_step;
    const entity canvas = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CanvasLink) : 0;
    CanvasViewport viewport;
    if (!get_canvas_viewport(world, canvas, &viewport) || !zox_tst_mouse_pointer(world, dbg_player)) {
        return 0;
    }
    if (!cleared) {
        zox_defer_begin();
        iter children = zox_children(world, canvas);
        while (zox_children_next(children)) {
            for (int i = 0; i < children.count; i++) {
                const entity child = children.entities[i];
                if (!zox_has(child, Tooltip)) {
                    zox_delete(child);
                }
            }
        }
        zox_defer_end();
        cleared = 1;
    }
    return zox_tst_ui_click_at(int2_add(viewport.position, (int2) { 8, 8 }), &click_step) ? canvas : 0;
}

static void zox_tst_ui_fixture_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}

static const color zox_tst_ui_red = { 255, 0, 0, 255 };
static const color zox_tst_ui_green = { 0, 255, 0, 255 };

static void zox_tst_ui_capture(const char* name) {
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(name));
}

static byte zox_tst_ui_pixel_is(const byte* pixels, int2 size, int2 point, color expected) {
    if (!pixels || point.x < 0 || point.y < 0 || point.x >= size.x || point.y >= size.y) {
        return 0;
    }
    const byte* pixel = zox_tst_ppm_pixel(pixels, size, point.x, size.y - 1 - point.y);
    return abs(pixel[0] - expected.r) < 16 && abs(pixel[1] - expected.g) < 16 && abs(pixel[2] - expected.b) < 16;
}
