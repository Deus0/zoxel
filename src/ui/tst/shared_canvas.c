// tst: shared_canvas_input, a shared overlay receives input above a player's canvas and returns it when deleted.
static int zox_tst_shared_clicks[2];
static entity zox_tst_shared_buttons[2];

static void zox_tst_shared_clicked(ecs* world, ClickEventData event) {
    (void) world;
    for (int i = 0; i < 2; i++) {
        zox_tst_shared_clicks[i] += event.clicked == zox_tst_shared_buttons[i];
    }
}

static byte zox_tst_shared_canvas_ready(ecs* world) {
    static entity canvas, overlay;
    static int step, click_step;
    static int2 point;
    zox_tst_progress(step);
    switch (step) {
        case 0:
            zox_tst_waiting = "the player's fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            zox_tst_shared_buttons[0] = zox_tst_ui_color_button(world, canvas, int2_zero, float2_half,
                zox_tst_ui_red, zox_tst_shared_clicked);
            zox_setv(zox_tst_shared_buttons[0], Layer, 64);
            break;
        case 1: {
            zox_tst_waiting = "clicking the player's canvas before opening the overlay";
            if (!zox_tst_ui_pointer_position(world, canvas, zox_tst_shared_buttons[0], &point) ||
                !zox_tst_ui_click_at(point, &click_step)) {
                return 0;
            }
            zox_tst_check(zox_tst_shared_clicks[0] == 1, "the player's canvas missed its first click");
            overlay = spawn_canvas(world, prefab_canvas, zox_get_link(world, canvas, CameraLink),
                zox_getv(canvas, LayoutSize), zox_getv(canvas, ScreenToCanvas), zox_get_link(world, canvas, AppLink));
            zox_link(world, dbg_player, CanvasLink, overlay);
            zox_tst_shared_buttons[1] = zox_tst_ui_color_button(world, overlay, int2_zero, float2_half,
                zox_tst_ui_green, zox_tst_shared_clicked);
            zox_setv(zox_tst_shared_buttons[1], Layer, 1);
            break;
        }
        case 2:
            zox_tst_waiting = "clicking the shared canvas over the same spot";
            if (!zox_tst_ui_click_at(point, &click_step)) {
                return 0;
            }
            zox_tst_check(zox_tst_shared_clicks[1] == 1 && zox_tst_shared_clicks[0] == 1,
                "the shared canvas click went to the wrong canvas or hit both");
            zox_delete(overlay);
            break;
        default:
            zox_tst_waiting = "clicking the player's canvas again after closing the overlay";
            if (!zox_tst_ui_click_at(point, &click_step)) {
                return 0;
            }
            zox_tst_check(zox_tst_shared_clicks[0] == 2 && zox_tst_shared_clicks[1] == 1,
                "deleting the shared canvas did not restore input to the player's button");
            return 1;
    }
    step++;
    return 0;
}
