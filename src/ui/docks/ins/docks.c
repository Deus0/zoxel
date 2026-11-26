static void dock_attach(ecs* world, entity element, float2 anchor, int2 size) {
    zox_setv(element, Anchor, anchor);
    zox_setv(element, LayoutSize, size);
    zox_add(element, Docked);
}

static void dock_float(ecs* world, entity element, float2 anchor, int2 position) {
    const entity drag = zox_get_link(world, element, DockDragLink);
    const entity handle = zox_get_child_by_id(world, element, zox_id(DragResizes));
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

static void dock_undock(ecs* world, entity element) {
    const float2 anchor = zox_getv(element, Anchor);
    int2 position = zox_getv(element, LayoutPosition);
    if (anchor.y == 0) {
        position.y += 40;
    } else if (anchor.y == 1) {
        position.y -= 40;
    } else {
        position.x += anchor.x == 0 ? 40 : -40;
    }
    dock_float(world, element, anchor, position);
}

static void dock_resize_handle(ecs* world, entity element, float2 anchor, int2 size) {
    entity handle = zox_get_child_by_id(world, element, zox_id(DragResizes));
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
