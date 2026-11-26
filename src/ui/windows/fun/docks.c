static const float2 window_dock_left = { 0, 0.5f };
static const float2 window_dock_right = { 1, 0.5f };
static const float2 window_dock_bottom = { 0.5f, 0 };

static entity3 docked_windows_find_except(ecs* world, entity canvas, entity excluded) {
    entity3 docks = { 0 };
    iter children = zox_children(world, canvas);
    while (zox_children_next(children)) {
        for (int i = 0; i < children.count; i++) {
            const entity window = children.entities[i];
            if (window == excluded || !zox_has(window, Docked) || !zox_has(window, Window)) {
                continue;
            }
            const float2 anchor = zox_getv(window, Anchor);
            if (float2_equals(anchor, window_dock_left)) {
                docks.x = window;
            } else if (float2_equals(anchor, window_dock_right)) {
                docks.y = window;
            } else if (float2_equals(anchor, window_dock_bottom)) {
                docks.z = window;
            }
        }
    }
    return docks;
}

static entity3 docked_windows_find(ecs* world, entity canvas) {
    return docked_windows_find_except(world, canvas, 0);
}

static int window_dock_limit(int extent) {
    return extent - (extent + 2) / 3;
}

static entity window_dock_at(entity3 docks, float2 anchor) {
    if (anchor.y == 0) {
        return docks.z;
    }
    return anchor.x == 0 ? docks.x : docks.y;
}

static int2 window_dock_default_size(int2 canvas_size, float2 anchor) {
    return anchor.y == 0 ? (int2) { canvas_size.x, canvas_size.y / 4 }
        : (int2) { canvas_size.x / 5, canvas_size.y };
}

static void window_dock_hide_preview(ecs* world, entity canvas, entity header) {
    const entity preview = zox_get_child_by_id(world, canvas, zox_id(WindowDockPreview));
    if (preview && zox_get_link(world, preview, DraggedLink) == header) {
        zox_setv(preview, RenderDisabled, 1);
    }
}

static void window_dock_close(ecs* world, entity canvas, entity window) {
    if (zox_valid(canvas) && zox_has(canvas, Canvas)) {
        window_dock_hide_preview(world, canvas, zox_get_child_by_id(world, window, zox_id(Header)));
    }
    const byte docked = zox_has(window, Docked);
    zox_delete(window);
    if (docked) {
        zox_add(canvas, Dirty);
    }
}

static void button_event_close_window(ecs* world, ClickEventData event) {
    entity parent = zox_get_parent(world, event.clicked);
    entity window = zox_get_parent(world, parent);
    window_dock_close(world, zox_get_parent(world, window), window);
}
