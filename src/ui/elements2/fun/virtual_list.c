// A page holds the slot entities in ChildIndex order. The owner supplies their targets.
typedef struct {
    entity slots[UINT8_MAX + 1];
    int capacity;
    int filled;
} VirtualListPage;

static void virtual_list_row_unlinked(ecs* world, entity row, entity target) {
    if (!zox_valid(row) || !zox_has(row, ListSlot) ||
        zox_has(row, ListHidden) || zox_get_link(world, row, RowTargetLink)) {
        return;
    }
    const entity list = zox_get_parent(world, row);
    zox_add(row, ListHidden);
    zox_add(row, Dirty);
    zox_add(list, ListPositionDirty);
    if (!target) {
        zox_add(list, Dirty);
    }
}

static void scroll_virtual_list_rows(ecs* world, entity list, int rows) {
    zox_setv(list, ListScrollRows, rows);
    zox_add(list, Dirty);
}

static void scroll_virtual_list_fraction(ecs* world, entity list, float fraction) {
    zox_setv(list, ListScrollFraction, float_min(1, float_max(0, fraction)));
    zox_add(list, Dirty);
}

static int virtual_list_start(ecs* world, entity list, int count, int anchor) {
    const int max_start = int_max(0, count - zox_getv(list, ListVisible));
    int start = int_max(0, anchor);
    if (zox_has(list, ListScrollFraction)) {
        start = (int) roundf(max_start * zox_getv(list, ListScrollFraction));
        zox_remove(list, ListScrollFraction);
    }
    if (zox_has(list, ListScrollRows)) {
        start += zox_getv(list, ListScrollRows);
        zox_remove(list, ListScrollRows);
    }
    return int_clamp(start, 0, max_start);
}

static byte virtual_list_prepare_slots(ecs* world, entity list, VirtualListPage* page) {
    page->capacity = zox_getv(list, ListVisible) + 1;
    const int count = get_list_children_ordered(world, list, page->slots, UINT8_MAX + 1);
    if (count == page->capacity) {
        return 0;
    }
    for (int i = 0; i < count; i++) {
        zox_delete(page->slots[i]);
    }
    return 1;
}

static void virtual_list_bind(ecs* world, VirtualListPage* page, entity target) {
    const entity slot = page->slots[page->filled++];
    zox_set_link(world, slot, RowTargetLink, target);
    if (target) {
        zox_remove(slot, ListHidden);
    } else {
        zox_add(slot, ListHidden);
    }
    zox_add(slot, Dirty);
}

static void virtual_list_finish(ecs* world, entity list, VirtualListPage* page, entity first, int count, int start) {
    while (page->filled < page->capacity) {
        virtual_list_bind(world, page, 0);
    }
    zox_set_link(world, list, ListStartLink, first);
    zox_add(list, ListPositionDirty);
    const entity view = zox_get_link(world, list, ScrollviewLink);
    const entity handle = zox_valid(view) ? get_scrollview_handle(world, view) : 0;
    if (!handle) {
        return;
    }
    const entity bar = zox_get_parent(world, handle);
    const int2 size = zox_getv(bar, LayoutSize);
    const int visible = zox_getv(list, ListVisible);
    const int max_start = count - visible;
    const int length = max_start > 0 ? int_max(1, size.y * visible / count) : size.y;
    layout_scrollbar_handle(world, handle, size, length, max_start > 0 ? start / (float) max_start : 0, 0);
}

static void virtual_list_slot_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity slot = it->entities[i];
        const byte hidden = !zox_get_link(world, slot, RowTargetLink);
        if (hidden != zox_getv(slot, RenderDisabled)) {
            set_children_render_disabled(world, slot, hidden);
        }
    }
}
