static void window_dock_layout(ecs* world, entity window, int2 position, int2 size) {
    const entity header = zox_get_child_by_id(world, window, zox_id(Header));
    const entity body = zox_get_child_by_id(world, window, zox_id(WindowBody));
    const entity handle = zox_get_child_by_id(world, window, zox_id(DragResizes));
    const entity scrollview = zox_get_link(world, window, ScrollviewLink);
    if (!header || !body) {
        return;
    }
    const int2 header_size = { size.x, zox_getv(header, LayoutSize).y };
    const entity footer = zox_get_child_by_id(world, window, zox_id(WindowFooter));
    const int footer_height = footer ? zox_getv(footer, LayoutSize).y : 0;
    const int2 body_size = { size.x, int_max(1, size.y - header_size.y - footer_height) };
    if (!int2_equals(zox_getv(window, LayoutPosition), position)) {
        zox_setv(window, LayoutPosition, position);
    }
    if (!int2_equals(zox_getv(window, LayoutSize), size)) {
        zox_setv(window, LayoutSize, size);
        zox_add(window, LayoutSizeDirty);
    }
    if (handle) {
        const int2 handle_size = zox_getv(window, Anchor).y == 0 ? (int2) { size.x, 6 } : (int2) { 6, size.y };
        if (!int2_equals(zox_getv(handle, LayoutSize), handle_size)) {
            zox_setv(handle, LayoutSize, handle_size);
            zox_add(handle, LayoutSizeDirty);
        }
    }
    if (int2_equals(zox_getv(body, LayoutSize), body_size)) {
        return;
    }
    zox_setv(header, LayoutSize, header_size);
    zox_setv(body, LayoutSize, body_size);
    zox_setv(body, LayoutPosition, ((int2) { 0, footer_height + body_size.y / 2 }));
    zox_add(window, LayoutSizeDirty);
    zox_add(header, LayoutSizeDirty);
    zox_add(body, LayoutSizeDirty);
    if (footer) {
        zox_setv(footer, LayoutSize, ((int2) { size.x, footer_height }));
        zox_add(footer, LayoutSizeDirty);
    }
    if (!zox_valid(scrollview)) {
        return;
    }
    zox_setv(scrollview, LayoutSize, body_size);
    const entity list = zox_get_link(world, scrollview, ListUILink);
    const entity scrollbar = zox_get_child_by_id(world, scrollview, zox_id(Scrollbar));
    if (scrollbar) {
        zox_setv(scrollbar, LayoutSize, ((int2) { zox_getv(scrollbar, LayoutSize).x, body_size.y }));
    }
    if (zox_valid(list)) {
        const byte2 padding = zox_getv(list, TextPadding);
        const byte2 margins = zox_getv(list, ListMargins);
        const int row = int_max(1, zox_getv(list, ElementFontSize) + 2 * padding.y + zox_getv(list, ListPadding).y);
        const int visible = int_clamp((body_size.y - 2 * margins.y) / row, 1, 255);
        const int max_start = int_max(0, (int) zox_get_children_count(world, list) - visible);
        zox_setv(list, LayoutSize, body_size);
        zox_setv(list, ListVisible, visible);
        zox_setv(list, ListStart, int_min(zox_getv(list, ListStart), max_start));
        zox_add(list, ListDirty);
        zox_add(list, ListPositionDirty);
        zox_add(list, ListPositionUpdate);
    }
    if (scrollbar) {
        zox_add(scrollbar, LayoutSizeDirty);
    }
}

static void window_dock_resize_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity canvas = zox_get_parent(world, e);
        if (!zox_valid(canvas) || !zox_has(canvas, Canvas)) {
            continue;
        }
        const int2 canvas_size = zox_getv(canvas, LayoutSize);
        const entity3 docks = docked_windows_find(world, canvas);
        const int2 before = zox_getv(e, LayoutSize);
        int2 size = before;
        if (e == docks.z) {
            const int limit = window_dock_limit(canvas_size.y);
            size.y = int_clamp(size.y, int_min(120, limit), limit);
        } else if (e == docks.x || e == docks.y) {
            const entity other = e == docks.x ? docks.y : docks.x;
            const int other_width = other ? zox_getv(other, LayoutSize).x : 0;
            const int limit = int_max(0, window_dock_limit(canvas_size.x) - other_width);
            size.x = int_clamp(size.x, int_min(120, limit), limit);
        } else {
            continue;
        }
        if (!int2_equals(size, before)) {
            zox_setv(e, LayoutSize, size);
        }
        zox_add(canvas, Dirty);
    }
}

static void docked_windows_layout_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity canvas = it->entities[i];
        const int2 size = zox_getv(canvas, LayoutSize);
        if (size.x <= 0 || size.y <= 0) {
            continue;
        }
        const entity3 docks = docked_windows_find(world, canvas);
        int left = docks.x ? zox_getv(docks.x, LayoutSize).x : 0;
        int right = docks.y ? zox_getv(docks.y, LayoutSize).x : 0;
        int bottom = docks.z ? zox_getv(docks.z, LayoutSize).y : 0;
        const int width_limit = window_dock_limit(size.x);
        if (left + right > width_limit) {
            left = (int) ((double) left * width_limit / (left + right));
            right = width_limit - left;
        }
        bottom = int_min(bottom, window_dock_limit(size.y));
        const int middle = size.x - left - right;
        if (docks.x) {
            window_dock_layout(world, docks.x, (int2) { left / 2, 0 }, (int2) { left, size.y });
        }
        if (docks.y) {
            window_dock_layout(world, docks.y, (int2) { right / 2 - right, 0 }, (int2) { right, size.y });
        }
        if (docks.z) {
            const int2 position = { left + middle / 2 - size.x / 2, bottom / 2 };
            window_dock_layout(world, docks.z, position, (int2) { middle, bottom });
        }
        const byte has_docks = docks.x || docks.y || docks.z;
        if (!has_docks && !zox_has(canvas, DockedWindowsViewport)) {
            continue;
        }
        const entity player = zox_get_link(world, canvas, PlayerLink);
        const entity camera = zox_valid(player) ? zox_get_link(world, player, CameraLink) : 0;
        if (!zox_valid(camera)) {
            continue;
        }
        if (players_playing > 1 || main_cameras_count > 1) {
            zox_remove(canvas, DockedWindowsViewport);
            continue;
        }
        if (has_docks) {
            zox_add(canvas, DockedWindowsViewport);
        } else {
            zox_remove(canvas, DockedWindowsViewport);
        }
        const float4 cell = { middle / (float) size.x, (size.y - bottom) / (float) size.y,
                              left / (float) size.x, bottom / (float) size.y };
        if (!float4_equals(zox_getv(camera, ScreenToCanvas), cell)) {
            zox_setv(camera, ScreenToCanvas, cell);
            const entity app = zox_get_link(world, canvas, AppLink);
            if (zox_valid(app)) {
                zox_add(app, WindowSizeDirty);
            }
        }
    }
}

static void window_dock_initialize(ecs* world, entity window) {
    if (!zox_valid(window) || !zox_has(window, Docked)) {
        return;
    }
    const entity canvas = zox_get_parent(world, window);
    const entity header = zox_get_child_by_id(world, window, zox_id(Header));
    const float2 anchor = zox_getv(window, Anchor);
    if (!header || !zox_has(canvas, Canvas) ||
        !(float2_equals(anchor, window_dock_left) || float2_equals(anchor, window_dock_right) ||
          float2_equals(anchor, window_dock_bottom)))
    {
        return;
    }
    const entity occupied = window_dock_at(docked_windows_find_except(world, canvas, window), anchor);
    if (occupied) {
        window_dock_undock(world, occupied);
    }
    window_dock_controls(world, window, header, anchor, zox_getv(window, LayoutSize));
    zox_add(canvas, Dirty);
}

static void window_dock_added_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        // The window's Anchor and children can still be queued when Docked is added.
        delay_event(world, window_dock_initialize, it->entities[i], 0);
    }
}

static void window_dock_show_preview(ecs* world, entity canvas, entity header, entity3 docks, float2 anchor) {
    const int2 size = zox_getv(canvas, LayoutSize);
    const entity occupied = window_dock_at(docks, anchor);
    int2 rect = occupied ? zox_getv(occupied, LayoutSize) : window_dock_default_size(size, anchor);
    const int left = docks.x ? zox_getv(docks.x, LayoutSize).x : 0;
    const int right = docks.y ? zox_getv(docks.y, LayoutSize).x : 0;
    int2 position = { 0, 0 };
    if (anchor.y == 0) {
        rect.x = size.x - left - right;
        rect.y = int_min(rect.y, window_dock_limit(size.y));
        position = (int2) { left + rect.x / 2 - size.x / 2, rect.y / 2 };
    } else {
        const int other = anchor.x == 0 ? right : left;
        const int limit = window_dock_limit(size.x);
        if (rect.x + other > limit) {
            rect.x = (int) ((double) rect.x * limit / (rect.x + other));
        }
        rect.y = size.y;
        position.x = anchor.x == 0 ? rect.x / 2 : rect.x / 2 - rect.x;
    }
    entity preview = zox_get_child_by_id(world, canvas, zox_id(WindowDockPreview));
    if (!preview) {
        const color fill = { 84, 155, 255, 96 };
        preview = spawn_uic(world, prefab_element_frame, canvas, anchor, position, rect, rect, fill, fill);
        zox_add(preview, WindowDockPreview);
        zox_setv(preview, Layer, 250);
    } else {
        zox_setv(preview, Anchor, anchor);
        zox_setv(preview, LayoutPosition, position);
        if (!int2_equals(zox_getv(preview, LayoutSize), rect)) {
            zox_setv(preview, LayoutSize, rect);
            zox_add(preview, LayoutSizeDirty);
        }
    }
    zox_set_link(world, preview, DraggedLink, header);
    zox_setv(preview, RenderDisabled, 0);
}

static void window_dock_drag_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity window = zox_get_parent(world, e);
        const entity canvas = zox_get_parent(world, window);
        const entity pointer = zox_get_link(world, e, ZeviceLink);
        if (!zox_has(window, Window) || zox_has(window, Docked) || !zox_has(canvas, Canvas) ||
            !zox_has(pointer, ZevicePointer) || !zox_has(pointer, ZevicePointerPosition)) {
            continue;
        }
        const byte dragging = zox_getv(e, DraggableState);
        const int2 point = zox_getv(pointer, ZevicePointerPosition);
        CanvasViewport viewport;
        const float2* side = NULL;
        if (get_canvas_viewport(world, canvas, &viewport) && is_in_canvas_viewport(&viewport, point)) {
            if (point.x - viewport.position.x <= 32) {
                side = &window_dock_left;
            } else if (viewport.position.x + viewport.size.x - point.x <= 32) {
                side = &window_dock_right;
            } else if (point.y - viewport.position.y <= 32) {
                side = &window_dock_bottom;
            }
        }
        if (!dragging || !side) {
            window_dock_hide_preview(world, canvas, e);
        }
        if (!side || (!dragging && !devices_get_released_this_frame(zox_getv(pointer, ZevicePointer)))) {
            continue;
        }
        const entity3 docks = docked_windows_find(world, canvas);
        if (dragging) {
            window_dock_show_preview(world, canvas, e, docks, *side);
        } else {
            const entity occupied = window_dock_at(docks, *side);
            if (occupied) {
                window_dock_float(world, occupied, zox_getv(window, Anchor), zox_getv(window, LayoutPosition));
                zox_setv(canvas, WindowToTop, occupied);
            }
            zox_setv(window, Anchor, *side);
            zox_setv(window, LayoutSize, window_dock_default_size(zox_getv(canvas, LayoutSize), *side));
            zox_add(window, Docked);
        }
    }
}
