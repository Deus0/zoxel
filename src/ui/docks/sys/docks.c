static void dock_added_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity canvas = zox_get_parent(world, e);
        if (zox_valid(canvas) && zox_has(canvas, Canvas)) {
            const entity occupied = dock_at(docks_find_except(world, canvas, e), zox_getv(e, Anchor));
            if (occupied) {
                dock_undock(world, occupied);
            }
            zox_remove(e, BoundToParent);
            zox_remove(e, LayoutConstraints);
            zox_add(e, Dirty);
            zox_add(canvas, Dirty);
        }
    }
}

static void dock_removed_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity canvas = zox_get_parent(world, it->entities[i]);
        if (zox_valid(canvas) && zox_has(canvas, Canvas)) {
            zox_add(canvas, Dirty);
        }
    }
}

static void dock_layout(ecs* world, entity element, int2 position, int2 size) {
    if (!int2_equals(zox_getv(element, LayoutPosition), position) ||
        !int2_equals(zox_getv(element, LayoutSize), size)) {
        zox_setv(element, LayoutPosition, position);
        zox_setv(element, LayoutSize, size);
        zox_add(element, Dirty);
    }
}

static void dock_dirty_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity canvas = zox_get_parent(world, e);
        if (!zox_valid(canvas) || !zox_has(canvas, Canvas)) {
            continue;
        }
        zox_add(e, LayoutSizeDirty);
        zox_add(canvas, Dirty);
    }
}

static byte dock_is_resizing(ecs* world, entity element) {
    const entity handle = element ? zox_get_child_by_id(world, element, zox_id(DockResizeHandle)) : 0;
    return handle && zox_getv(handle, DraggableState);
}

static void dock_viewport(ecs* world, entity canvas, byte has_docks, float4 cell) {
    if (!has_docks && !zox_has(canvas, DockViewport)) {
        return;
    }
    const entity player = zox_get_link(world, canvas, PlayerLink);
    const entity camera = zox_valid(player) ? zox_get_link(world, player, CameraLink) : 0;
    if (!zox_valid(camera)) {
        return;
    }
    if (players_playing > 1 || main_cameras_count > 1) {
        zox_remove(canvas, DockViewport);
        return;
    }
    if (has_docks) {
        zox_add(canvas, DockViewport);
    } else {
        zox_remove(canvas, DockViewport);
    }
    if (!float4_equals(zox_getv(camera, ScreenToCanvas), cell)) {
        zox_setv(camera, ScreenToCanvas, cell);
        const entity app = zox_get_link(world, canvas, AppLink);
        if (zox_valid(app)) {
            zox_add(app, WindowSizeDirty);
        }
    }
}

static void docks_layout_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity canvas = it->entities[i];
        const int2 size = zox_getv(canvas, LayoutSize);
        if (size.x <= 0 || size.y <= 0) {
            continue;
        }
        const entity4 docks = docks_find(world, canvas);
        int left = docks.x ? zox_getv(docks.x, LayoutSize).x : 0;
        int right = docks.y ? zox_getv(docks.y, LayoutSize).x : 0;
        int bottom = docks.z ? zox_getv(docks.z, LayoutSize).y : 0;
        const int width_limit = dock_limit(size.x);
        if (dock_is_resizing(world, docks.x)) {
            const int limit = int_max(0, width_limit - right);
            left = int_clamp(left, int_min(120, limit), limit);
        } else if (dock_is_resizing(world, docks.y)) {
            const int limit = int_max(0, width_limit - left);
            right = int_clamp(right, int_min(120, limit), limit);
        }
        left = int_max(0, left);
        right = int_max(0, right);
        if (left + right > width_limit) {
            left = (int) ((double) left * width_limit / (left + right));
            right = width_limit - left;
        }
        const int top = docks.w ? int_clamp(zox_getv(docks.w, LayoutSize).y, 0, size.y) : 0;
        const int below = size.y - top;
        const int side_y = below / 2 - size.y / 2;
        const int height_limit = dock_limit(below);
        const int bottom_min = dock_is_resizing(world, docks.z) ? int_min(120, height_limit) : 0;
        bottom = int_clamp(bottom, bottom_min, height_limit);
        const int middle = size.x - left - right;
        const entity sides[] = { docks.x, docks.y, docks.z, docks.w };
        const float2 anchors[] = { dock_left, dock_right, dock_bottom, dock_top };
        const int2 sizes[] = { { left, below }, { right, below }, { middle, bottom }, { size.x, top } };
        const int2 positions[] = {
            { left / 2, side_y }, { right / 2 - right, side_y },
            { left + middle / 2 - size.x / 2, bottom / 2 }, { 0, top / 2 - top }
        };
        for (int j = 0; j < 4; j++) {
            const entity element = sides[j];
            if (!element) {
                continue;
            }
            dock_layout(world, element, positions[j], sizes[j]);
            const entity drag = zox_get_link(world, element, DockDragLink);
            if (zox_valid(drag) && zox_has(drag, Dragable)) {
                zox_remove(drag, Dragable);
            }
            dock_resize_handle(world, element, anchors[j], sizes[j]);
        }
        const float4 cell = { middle / (float) size.x, (below - bottom) / (float) size.y,
                              left / (float) size.x, bottom / (float) size.y };
        dock_viewport(world, canvas, docks.x || docks.y || docks.z || docks.w, cell);
    }
}

static void dock_show_preview(ecs* world, entity canvas, entity drag, entity4 docks, float2 anchor) {
    const int2 size = zox_getv(canvas, LayoutSize);
    const entity occupied = dock_at(docks, anchor);
    int2 rect = occupied ? zox_getv(occupied, LayoutSize) : dock_default_size(size, anchor);
    const int left = docks.x ? zox_getv(docks.x, LayoutSize).x : 0;
    const int right = docks.y ? zox_getv(docks.y, LayoutSize).x : 0;
    const int top = docks.w ? zox_getv(docks.w, LayoutSize).y : 0;
    const int below = size.y - top;
    int2 position = { 0, 0 };
    if (anchor.y == 0) {
        rect.x = size.x - left - right;
        rect.y = int_min(rect.y, dock_limit(below));
        position = (int2) { left + rect.x / 2 - size.x / 2, rect.y / 2 };
    } else if (anchor.y == 1) {
        rect.x = size.x;
        rect.y = int_clamp(rect.y, 0, size.y);
        position.y = rect.y / 2 - rect.y;
    } else {
        const int other = anchor.x == 0 ? right : left;
        const int limit = dock_limit(size.x);
        if (rect.x + other > limit) {
            rect.x = (int) ((double) rect.x * limit / (rect.x + other));
        }
        rect.y = below;
        position.y = below / 2 - size.y / 2;
        position.x = anchor.x == 0 ? rect.x / 2 : rect.x / 2 - rect.x;
    }
    entity preview = zox_get_child_by_id(world, canvas, zox_id(DockPreview));
    if (!preview) {
        const color fill = { 84, 155, 255, 96 };
        preview = spawn_uic(world, prefab_element_frame, canvas, anchor, position, rect, rect, fill, fill);
        zox_add(preview, DockPreview);
        zox_setv(preview, Layer, 250);
    } else {
        zox_setv(preview, Anchor, anchor);
        zox_setv(preview, LayoutPosition, position);
        if (!int2_equals(zox_getv(preview, LayoutSize), rect)) {
            zox_setv(preview, LayoutSize, rect);
            zox_add(preview, LayoutSizeDirty);
        }
    }
    zox_set_link(world, preview, DraggedLink, drag);
    zox_setv(preview, RenderDisabled, 0);
}

static void dock_drag_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity element = zox_get_link(world, e, DraggedLink);
        if (!zox_valid(element) || zox_get_link(world, element, DockDragLink) != e || zox_has(element, Docked)) {
            continue;
        }
        const entity canvas = zox_get_parent(world, element);
        const entity pointer = zox_get_link(world, e, ZeviceLink);
        if (!zox_has(canvas, Canvas) || !zox_has(pointer, ZevicePointer) || !zox_has(pointer, ZevicePointerPosition)) {
            continue;
        }
        const byte dragging = zox_getv(e, DraggableState);
        const int2 point = zox_getv(pointer, ZevicePointerPosition);
        CanvasViewport viewport;
        const float2* side = NULL;
        if (get_canvas_viewport(world, canvas, &viewport) && is_in_canvas_viewport(&viewport, point)) {
            if (point.x - viewport.position.x <= 32) {
                side = &dock_left;
            } else if (viewport.position.x + viewport.size.x - point.x <= 32) {
                side = &dock_right;
            } else if (point.y - viewport.position.y <= 32) {
                side = &dock_bottom;
            } else if (viewport.position.y + viewport.size.y - point.y <= 32) {
                side = &dock_top;
            }
        }
        if (!dragging || !side) {
            dock_hide_preview(world, canvas, e);
        }
        if (!side || (!dragging && !devices_get_released_this_frame(zox_getv(pointer, ZevicePointer)))) {
            continue;
        }
        const entity4 docks = docks_find(world, canvas);
        if (dragging) {
            dock_show_preview(world, canvas, e, docks, *side);
        } else {
            const entity occupied = dock_at(docks, *side);
            if (occupied) {
                dock_float(world, occupied, zox_getv(element, Anchor), zox_getv(element, LayoutPosition));
                zox_setv(canvas, WindowToTop, occupied);
            }
            dock_attach(world, element, *side, dock_default_size(zox_getv(canvas, LayoutSize), *side));
        }
    }
}
