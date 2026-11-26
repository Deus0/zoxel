static void window_dock_float(ecs* world, entity window, float2 anchor, int2 position) {
    const entity header = zox_get_child_by_id(world, window, zox_id(Header));
    const entity handle = zox_get_child_by_id(world, window, zox_id(DragResizes));
    const entity button = zox_get_child_by_id(world, header, zox_id(WindowDockButton));
    zox_remove(window, Docked);
    zox_setv(window, Anchor, anchor);
    zox_setv(window, LayoutPosition, position);
    zox_add(window, BoundToParent);
    zox_add(header, Dragable);
    if (button) {
        zox_setv(button, RenderDisabled, 1);
    }
    if (handle) {
        zox_remove(handle, Dragable);
        zox_setv(handle, RenderDisabled, 1);
    }
}

static void window_dock_undock(ecs* world, entity window) {
    const float2 anchor = zox_getv(window, Anchor);
    int2 position = zox_getv(window, LayoutPosition);
    if (anchor.y == 0) {
        position.y += 40;
    } else {
        position.x += anchor.x == 0 ? 40 : -40;
    }
    window_dock_float(world, window, anchor, position);
    zox_add(zox_get_parent(world, window), Dirty);
}

static void window_dock_undock_click(ecs* world, ClickEventData event) {
    const entity header = zox_get_parent(world, event.clicked);
    const entity window = zox_get_parent(world, header);
    if (zox_has(window, Docked)) {
        window_dock_undock(world, window);
    }
}

static void window_dock_controls(ecs* world, entity window, entity header, float2 anchor, int2 size) {
    zox_remove(window, BoundToParent);
    zox_remove(window, LayoutConstraints);
    zox_remove(header, Dragable);
    if (!zox_get_child_by_id(world, header, zox_id(CloseButton))) {
        spawn_close_button(world, header, 0, 18, 4, 0, (ClickEvent) { button_event_close_window });
    }
    entity button = zox_get_child_by_id(world, header, zox_id(WindowDockButton));
    if (!button) {
        button = spawn_button(world, prefab_clickable, header, "<",
            (int2) { -36, 0 }, (int2) { 20, 20 }, window_dock_right, zox_alignment_centre,
            12, byte2_single(0), header_fill, header_outline, header_font_fill, header_font_outline).x;
        zox_setv(button, ClickEvent, window_dock_undock_click);
        zox_add(button, WindowDockButton);
    }
    zox_setv(button, RenderDisabled, 0);

    const byte bottom = anchor.y == 0;
    const float2 edge = bottom ? (float2) { 0.5f, 1 } : (float2) { 1 - anchor.x, 0.5f };
    const int2 handle_size = bottom ? (int2) { size.x, 6 } : (int2) { 6, size.y };
    const int2 handle_position = bottom ? (int2) { 0, -3 } : (int2) { anchor.x == 0 ? -3 : 3, 0 };
    entity handle = zox_get_child_by_id(world, window, zox_id(DragResizes));
    if (!handle) {
        handle = spawn_handle(world, prefab_handle, window, size, handle_position, handle_size, edge,
            header_outline, header_outline, bottom);
        zox_add(handle, DragResizes);
        zox_link(world, handle, DraggedLink, window);
        zox_setv(handle, LayoutConstraints, int4_zero);
        zox_setv(handle, BonusLayer, 6);
    } else {
        zox_setv(handle, Anchor, edge);
        zox_setv(handle, LayoutPosition, handle_position);
        zox_setv(handle, LayoutSize, handle_size);
        zox_add(handle, LayoutSizeDirty);
        zox_add(handle, Dragable);
        zox_setv(handle, RenderDisabled, 0);
    }
}
