static void dock_attach(ecs* world, entity element, float2 anchor, int2 size) {
    zox_setv(element, Anchor, anchor);
    zox_setv(element, LayoutSize, size);
    zox_add(element, Docked);
}

static void dock_float(ecs* world, entity element, float2 anchor, int2 position) {
    const entity drag = zox_get_link(world, element, DockDragLink);
    const entity handle = zox_get_child_by_id(world, element, zox_id(DockResizeHandle));
    zox_remove(element, Docked);
    zox_setv(element, Anchor, anchor);
    zox_setv(element, LayoutPosition, position);
    zox_add(element, BoundToParent);
    if (zox_valid(drag)) {
        zox_add(drag, Dragable);
    }
    zox_add(element, Dirty);
    if (handle) {
        zox_remove(handle, Dragable);
        zox_setv(handle, RenderDisabled, 1);
    }
}

// a regular floating window: a quarter of the canvas wide, half high, toward the side it was docked on
static void dock_undock(ecs* world, entity element) {
    const float2 anchor = zox_getv(element, Anchor);
    const entity canvas = zox_get_parent(world, element);
    const int2 canvas_size = zox_valid(canvas) ? zox_getv(canvas, LayoutSize) : zox_getv(element, LayoutSize);
    const int2 size = { int_max(120, canvas_size.x / 4), int_max(120, canvas_size.y / 2) };
    int2 position = int2_zero;
    if (anchor.y == 0) {
        position.y = -canvas_size.y / 4;
    } else if (anchor.y == 1) {
        position.y = canvas_size.y / 4;
    } else {
        position.x = anchor.x == 0 ? -canvas_size.x / 4 : canvas_size.x / 4;
    }
    zox_setv(element, LayoutSize, size);
    dock_float(world, element, float2_half, position);
}

static byte dock_has_own_resize(ecs* world, entity element) {
    byte own = 0;
    iter it = zox_children(world, element);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && !own; i++) {
            own = zox_has(it.entities[i], DragResizes) && !zox_has(it.entities[i], DockResizeHandle);
        }
    }
    return own;
}

// a docked element's edge handle, unless it brings its own (a window's resize handles, windows/sys/layout.c)
static void dock_resize_handle(ecs* world, entity element, float2 anchor, int2 size) {
    entity handle = zox_get_child_by_id(world, element, zox_id(DockResizeHandle));
    if (dock_has_own_resize(world, element)) {
        if (handle) {
            zox_delete(handle);     // made in its spawn frame, before its own handles were there
        }
        return;
    }
    if (anchor.y == 1) {
        if (handle && zox_has(handle, Dragable)) {
            zox_remove(handle, Dragable);
            zox_setv(handle, RenderDisabled, 1);
        }
        return;
    }
    const byte bottom = anchor.y == 0;
    const float2 edge = bottom ? (float2) { 0.5f, 1 } : (float2) { 1 - anchor.x, 0.5f };
    const int2 handle_size = bottom ? (int2) { size.x, 6 } : (int2) { 6, size.y };
    const int2 handle_position = bottom ? (int2) { 0, -3 } : (int2) { anchor.x == 0 ? -3 : 3, 0 };
    if (!handle) {
        const color fill = { 44, 44, 44, 222 };
        handle = spawn_handle(world, prefab_handle, element, size, handle_position, handle_size, edge,
            fill, fill, bottom);
        zox_add(handle, DragResizes);
        zox_add(handle, DockResizeHandle);
        zox_link(world, handle, DraggedLink, element);
        zox_setv(handle, LayoutConstraints, int4_zero);
        zox_setv(handle, BonusLayer, 6);
    } else if (!int2_equals(zox_getv(handle, LayoutSize), handle_size) ||
        !float2_equals(zox_getv(handle, Anchor), edge) || !zox_has(handle, Dragable)) {
        zox_setv(handle, Anchor, edge);
        zox_setv(handle, LayoutPosition, handle_position);
        zox_setv(handle, LayoutSize, handle_size);
        zox_add(handle, LayoutSizeDirty);
        zox_add(handle, Dragable);
        zox_setv(handle, RenderDisabled, 0);
    }
}
