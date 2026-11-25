// tst: slider_drag, the real pointer clamps the handle and its emitted value, then releases it off the slider.
static float zox_tst_slider_value;
static uint zox_tst_slider_events;
static entity zox_tst_slider_dragger;

static void zox_tst_slider_slid(ecs* world, const SlideEventData* event) {
    (void) world;
    zox_tst_slider_value = event->value;
    zox_tst_slider_dragger = event->player;
    zox_tst_slider_events++;
}

static byte zox_tst_slider_drag_ready(ecs* world) {
    static entity canvas, handle;
    static int step;
    static uint released_events;
    static int2 centre;
    const float2 bounds = { -2, 6 };
    const int limit = 80;
    zox_tst_progress(step);
    switch (step) {
        case 0:
            zox_tst_waiting = "the slider fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            handle = spawn_slider(world, (LayoutParentData) { .e = canvas }, (LayoutParentData) { .e = canvas },
                (ElementSpawnData) { .prefab = prefab_slider, .anchor = float2_half, .size = { 184, 32 } },
                (SpawnSliderData) { .name = "Fixture slider", .prefab_handle = prefab_handle,
                    .value = 2, .bounds = bounds, .handle_width = 24 },
                button_fill, button_outline, 12, color_white, color_black).y;
            zox_setv(handle, SlideEvent, zox_tst_slider_slid);
            zox_setv(handle, Layer, 32);
            break;
        case 1:
            zox_tst_waiting = "the pointer to hover the slider handle";
            if (!zox_tst_ui_pointer_position(world, canvas, handle, &centre)) {
                return 0;
            }
            zox_tst_mouse(centre, 0);
            if (zox_tst_mouse_target(world, dbg_player) != handle) {
                return 0;
            }
            zox_tst_mouse(centre, 1);
            break;
        case 2:
            zox_tst_waiting = "grabbing the slider handle";
            if (zox_get_link(world, handle, DraggerLink) != dbg_player) {
                return 0;
            }
            zox_tst_check(int2_equals(zox_getv(handle, LayoutPosition), int2_zero), "the slider did not start at its midpoint");
            zox_tst_mouse(int2_add(centre, (int2) { 160, 44 }), 1);
            break;
        case 3:
            zox_tst_waiting = "the slider to reach the right end";
            if (zox_tst_slider_value < bounds.y) {
                return 0;
            }
            zox_tst_check(zox_tst_slider_value == bounds.y && int2_equals(zox_getv(handle, LayoutPosition), ((int2) { limit, 0 })),
                "the slider overshot its right limit or moved vertically");
            zox_tst_check(zox_tst_slider_dragger == dbg_player, "moving the slider did not report its dragger");
            zox_tst_mouse(int2_add(centre, (int2) { -160, -44 }), 1);
            break;
        case 4:
            zox_tst_waiting = "the slider to reach the left end";
            if (zox_tst_slider_value > bounds.x) {
                return 0;
            }
            zox_tst_check(zox_tst_slider_value == bounds.x && int2_equals(zox_getv(handle, LayoutPosition), ((int2) { -limit, 0 })),
                "the slider overshot its left limit or moved vertically");
            zox_tst_mouse(int2_add(centre, (int2) { -160, -44 }), 0);
            break;
        case 5:
            zox_tst_waiting = "the off-slider release to clear the drag";
            if (zox_getv(handle, DraggableState) || zox_get_link(world, handle, DraggerLink)) {
                return 0;
            }
            released_events = zox_tst_slider_events;
            zox_tst_mouse(centre, 0);
            break;
        default:
            zox_tst_check(zox_tst_slider_events == released_events && zox_tst_slider_value == bounds.x &&
                int2_equals(zox_getv(handle, LayoutPosition), ((int2) { -limit, 0 })), "unpressed motion still dragged the slider");
            return 1;
    }
    step++;
    return 0;
}
