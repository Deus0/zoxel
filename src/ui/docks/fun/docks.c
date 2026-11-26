static const float2 dock_left = { 0, 0.5f };
static const float2 dock_right = { 1, 0.5f };
static const float2 dock_bottom = { 0.5f, 0 };
static const float2 dock_top = { 0.5f, 1 };

static entity4 docks_find_except(ecs* world, entity canvas, entity excluded) {
    entity4 docks = { 0 };
    iter children = zox_children(world, canvas);
    while (zox_children_next(children)) {
        for (int i = 0; i < children.count; i++) {
            const entity element = children.entities[i];
            if (element == excluded || !zox_has(element, Docked) || !zox_has(element, LayoutSize) || !zox_has(element, Anchor)) {
                continue;
            }
            const float2 anchor = zox_getv(element, Anchor);
            if (float2_equals(anchor, dock_left)) {
                docks.x = element;
            } else if (float2_equals(anchor, dock_right)) {
                docks.y = element;
            } else if (float2_equals(anchor, dock_bottom)) {
                docks.z = element;
            } else if (float2_equals(anchor, dock_top)) {
                docks.w = element;
            }
        }
    }
    return docks;
}

static entity4 docks_find(ecs* world, entity canvas) {
    return docks_find_except(world, canvas, 0);
}

static int dock_limit(int extent) {
    return extent - (extent + 2) / 3;
}

static entity dock_at(entity4 docks, float2 anchor) {
    if (float2_equals(anchor, dock_left)) {
        return docks.x;
    }
    if (float2_equals(anchor, dock_right)) {
        return docks.y;
    }
    if (float2_equals(anchor, dock_bottom)) {
        return docks.z;
    }
    return float2_equals(anchor, dock_top) ? docks.w : 0;
}

static int2 dock_default_size(int2 canvas_size, float2 anchor) {
    return anchor.x == 0.5f ? (int2) { canvas_size.x, canvas_size.y / 4 }
        : (int2) { canvas_size.x / 5, canvas_size.y };
}

static void dock_hide_preview(ecs* world, entity canvas, entity drag) {
    const entity preview = zox_get_child_by_id(world, canvas, zox_id(DockPreview));
    if (preview && zox_get_link(world, preview, DraggedLink) == drag) {
        zox_setv(preview, RenderDisabled, 1);
    }
}

static void dock_close(ecs* world, entity canvas, entity element) {
    if (zox_valid(canvas) && zox_has(canvas, Canvas)) {
        dock_hide_preview(world, canvas, zox_get_link(world, element, DockDragLink));
    }
    const byte docked = zox_has(element, Docked);
    zox_delete(element);
    if (docked) {
        zox_add(canvas, Dirty);
    }
}

