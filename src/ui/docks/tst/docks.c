static int2 zox_tst_dock_origin(ecs* world, entity window, int2 canvas_size) {
    const int2 size = zox_getv(window, LayoutSize);
    const int2 position = zox_getv(window, LayoutPosition);
    const float2 anchor = zox_getv(window, Anchor);
    return (int2) { (int) (anchor.x * canvas_size.x) + position.x - size.x / 2,
                    (int) (anchor.y * canvas_size.y) + position.y - size.y / 2 };
}

static void zox_tst_dock_view(ecs* world, entity camera, int2 size, int2 origin, int2 area) {
    const float4 cell = zox_getv(camera, ScreenToCanvas);
    zox_tst_check(int2_equals(screen_to_canvas_position(size, cell), origin), "game viewport origin is wrong");
    zox_tst_check(int2_equals(screen_to_canvas_size(size, cell), area), "game viewport size is wrong");
    zox_tst_check(int2_equals(zox_getv(camera, ScreenPosition), origin) &&
        int2_equals(zox_getv(camera, PixelSize), camera_viewport_size(world, camera, area)),
        "game camera did not apply the viewport");
}

static void zox_tst_dock_geometry(ecs* world, entity canvas, entity camera, entity4 docks) {
    const int2 size = zox_getv(canvas, LayoutSize);
    const int2 left = zox_getv(docks.x, LayoutSize);
    const int2 right = zox_getv(docks.y, LayoutSize);
    const int2 bottom = zox_getv(docks.z, LayoutSize);
    const int below = size.y - (docks.w ? 24 : 0);
    zox_tst_check(int2_equals(zox_tst_dock_origin(world, docks.x, size), int2_zero) && left.y == below,
        "left dock does not fill the left edge");
    zox_tst_check(int2_equals(zox_tst_dock_origin(world, docks.y, size), ((int2) { size.x - right.x, 0 })) &&
        right.y == below, "right dock does not fill the right edge");
    zox_tst_check(int2_equals(zox_tst_dock_origin(world, docks.z, size), ((int2) { left.x, 0 })),
        "bottom dock does not start at the inner edge of the left dock");
    zox_tst_check(bottom.x > 0 && left.x + bottom.x + right.x == size.x &&
        bottom.y > 0 && bottom.y < size.y, "docks overlap or leave a gap");
    zox_tst_dock_view(world, camera, size, (int2) { left.x, bottom.y }, (int2) { bottom.x, below - bottom.y });
    const entity ui_camera = zox_get_link(world, canvas, CameraLink);
    zox_tst_check(float4_equals(zox_getv(ui_camera, ScreenToCanvas), single_screen_to_canvas) &&
        float4_equals(zox_getv(canvas, ScreenToCanvas), single_screen_to_canvas), "UI no longer fills the screen");
}

static entity zox_tst_spawn_dock_window(ecs* world, entity canvas, const char* title, float2 anchor, int2 size) {
    return spawn_window_list_at(world, prefab_window, canvas, int2_zero, size, zox_alignment_left,
        anchor, title, 18, 12, byte2_single(2), 6, 1, (ClickEvent) { button_event_close_window }).x;
}

static byte zox_tst_docks_ready(ecs* world) {
    static int step, original_left;
    static int2 resized, floating_position, floating_size, start, drop, swap_position;
    static float2 swap_anchor;
    static entity canvas, floating, handle, header, top;
    static entity3 windows;
    zox_tst_progress(step);
    if (!canvas) {
        canvas = zox_tst_ui_fixture_canvas(world);
    }
    const entity camera = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CameraLink) : 0;
    if (!canvas || !zox_valid(camera) || zox_has(canvas, Dirty)) {
        return 0;
    }
    const int2 size = zox_getv(canvas, LayoutSize);
    const entity4 docks = docks_find(world, canvas);
    switch (step) {
        case 0:
            floating_size = (int2) { 180, 100 };
            floating = zox_tst_spawn_dock_window(world, canvas, "Floating", dock_left, floating_size);
            floating_position = zox_getv(floating, LayoutPosition);
            zox_add(canvas, Dirty);
            break;
        case 1:
            zox_tst_check(!docks.x && int2_equals(zox_getv(floating, LayoutPosition), floating_position) &&
                int2_equals(zox_getv(floating, LayoutSize), floating_size), "layout moved an untagged window");
            zox_tst_dock_view(world, camera, size, int2_zero, size);
            zox_delete(floating);
            zox_defer_begin();
            windows.x = zox_tst_spawn_dock_window(world, canvas, "Left", dock_left,
                dock_default_size(size, dock_left));
            windows.y = zox_tst_spawn_dock_window(world, canvas, "Right", dock_right,
                dock_default_size(size, dock_right));
            windows.z = zox_tst_spawn_dock_window(world, canvas, "Bottom", dock_bottom,
                dock_default_size(size, dock_bottom));
            zox_add(windows.x, Docked);
            zox_add(windows.y, Docked);
            zox_add(windows.z, Docked);
            zox_defer_end();
            break;
        case 2: {
            zox_tst_waiting = "the deferred docks to lay out";
            if (!docks.x || !docks.y || !docks.z ||
                !zox_has(canvas, DockViewport)) {
                return 0;
            }
            zox_tst_dock_geometry(world, canvas, camera, docks);
            original_left = zox_getv(docks.x, LayoutSize).x;
            zox_tst_check(original_left == size.x / 5 && zox_getv(docks.y, LayoutSize).x == size.x / 5 &&
                zox_getv(docks.z, LayoutSize).y == size.y / 4, "default dock sizes are wrong");
            handle = zox_get_child_by_id(world, docks.x, zox_id(DockResizeHandle));
            zox_tst_check(handle, "left dock has no resize handle");
            if (!handle) {
                return 1;
            }
            zox_defer_begin();
            top = spawn_uic(world, prefab_element_frame, canvas, dock_top, int2_zero,
                (int2) { 48, 24 }, (int2) { 48, 24 }, color_white, color_white);
            zox_add(top, Docked);
            zox_defer_end();
            break;
        }
        case 3:
            zox_tst_waiting = "the plain top dock to lay out";
            if (docks.w != top) {
                return 0;
            }
            zox_tst_check(!zox_has(top, Window) && int2_equals(zox_getv(top, LayoutSize), ((int2) { size.x, 24 })) &&
                int2_equals(zox_tst_dock_origin(world, top, size), ((int2) { 0, size.y - 24 })) &&
                !zox_get_child_by_id(world, top, zox_id(DockResizeHandle)), "plain top dock does not fill the top edge");
            zox_tst_dock_geometry(world, canvas, camera, docks);
            zox_delete(top);
            zox_setv(handle, DraggingDelta, ((int2) { 40, 0 }));
            zox_setv(handle, DraggableState, 1);
            break;
        case 4:
            zox_tst_waiting = "the resize handle drag";
            if (zox_getv(docks.x, LayoutSize).x == original_left) {
                return 0;
            }
            zox_setv(handle, DraggableState, 0);
            zox_setv(handle, DraggingDelta, int2_zero);
            zox_tst_check(zox_getv(docks.x, LayoutSize).x == original_left + 40, "left dock did not grow by the drag delta");
            zox_tst_dock_geometry(world, canvas, camera, docks);
            resized = (int2) { size.x - 21, size.y - 19 };
            const entity app = zox_get_link(world, canvas, AppLink);
            zox_sdl_window_size(zox_getv(app, SDLWindow), resized);
            on_window_resized(world, app, resized, 0);
            break;
        case 5:
            zox_tst_waiting = "the canvas to resize and the left window to finish opening";
            if (!int2_equals(size, resized) || zox_getv(windows.x, LocalScale1) != 1) {
                return 0;
            }
            zox_tst_dock_geometry(world, canvas, camera, docks);
            header = zox_get_child_by_id(world, docks.x, zox_id(Header));
            zox_tst_click(world, dbg_player, zox_get_child_by_id(world, header, zox_id(UndockButton)));
            break;
        case 6: {
            zox_tst_waiting = "the undocked window and camera";
            CanvasViewport viewport;
            if (!zox_tst_ui_pointer_position(world, canvas, header, &start) ||
                !get_canvas_viewport(world, canvas, &viewport)) {
                return 0;
            }
            zox_tst_check(!docks.x && zox_has(header, Dragable) &&
                int2_equals(zox_getv(windows.x, LayoutSize), ((int2) { int_max(120, size.x / 4), int_max(120, size.y / 2) })) &&
                int2_equals(zox_getv(windows.x, LayoutPosition), ((int2) { -size.x / 4, 0 })),
                "undock did not make a regular floating window and restore dragging");
            const int bottom = zox_getv(docks.z, LayoutSize).y;
            zox_tst_dock_view(world, camera, size, (int2) { 0, bottom },
                (int2) { size.x - zox_getv(docks.y, LayoutSize).x, size.y - bottom });
            drop = (int2) { viewport.position.x + viewport.size.x - 8, start.y };
            zox_tst_mouse(start, 0);
            break;
        }
        case 7:
            zox_tst_waiting = "the pointer to hover the floating header";
            if (zox_tst_mouse_target(world, dbg_player) != header) {
                return 0;
            }
            zox_tst_mouse(start, 1);
            break;
        case 8:
            zox_tst_waiting = "the floating header to start dragging";
            if (!zox_getv(header, DraggableState)) {
                return 0;
            }
            zox_tst_mouse(drop, 1);
            break;
        case 9: {
            zox_tst_waiting = "the right dock preview";
            const entity preview = zox_get_child_by_id(world, canvas, zox_id(DockPreview));
            if (!preview || zox_getv(preview, RenderDisabled)) {
                return 0;
            }
            zox_tst_check(float2_equals(zox_getv(preview, Anchor), dock_right) &&
                int2_equals(zox_getv(preview, LayoutSize), zox_getv(windows.y, LayoutSize)), "wrong dock preview rectangle");
            swap_anchor = zox_getv(windows.x, Anchor);
            swap_position = zox_getv(windows.x, LayoutPosition);
            zox_tst_mouse(drop, 0);
            break;
        }
        case 10: {
            zox_tst_waiting = "the mouse release to swap the right dock";
            if (docks.y != windows.x || zox_has(header, Dragable)) {
                return 0;
            }
            zox_tst_check(!zox_has(windows.y, Docked) &&
                float2_equals(zox_getv(windows.y, Anchor), swap_anchor) &&
                int2_equals(zox_getv(windows.y, LayoutPosition), swap_position), "occupied dock did not float at the drop position");
            const entity preview = zox_get_child_by_id(world, canvas, zox_id(DockPreview));
            zox_tst_check(zox_getv(preview, RenderDisabled), "preview is still visible after release");
            const int bottom = zox_getv(docks.z, LayoutSize).y;
            zox_tst_dock_view(world, camera, size, (int2) { 0, bottom },
                (int2) { size.x - size.x / 5, size.y - bottom });
            const entity closing[] = { windows.x, windows.y, windows.z };
            for (int i = 0; i < 3; i++) {
                const entity close_header = zox_get_child_by_id(world, closing[i], zox_id(Header));
                zox_tst_click(world, dbg_player, zox_get_child_by_id(world, close_header, zox_id(CloseButton)));
            }
            break;
        }
        default:
            zox_tst_waiting = "the close buttons to delete the windows";
            if (zox_valid(windows.x) || zox_valid(windows.y) || zox_valid(windows.z)) {
                return 0;
            }
            zox_tst_dock_view(world, camera, size, int2_zero, size);
            return 1;
    }
    step++;
    return 0;
}

static void zox_tst_docks_check(ecs* world) {
    zox_tst_ui_fixture_check(world);
    zox_tst_check(zox_log_errors_count == 0, "[%u] errors logged", zox_log_errors_count);
}
