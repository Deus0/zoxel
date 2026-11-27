// fits a window's 8 resize handles (ins/resize.c) to its size, hidden and not dragable while docked
static void window_resize_handles_fit(ecs* world, entity window, int2 size) {
    const byte docked = zox_has(window, Docked);
    iter it2 = zox_children(world, window);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity handle = it2.entities[j];
            if (!zox_has(handle, WindowResizeHandle)) {
                continue;
            }
            const int2 handle_size = window_resize_handle_size(size, zox_getv(handle, Anchor));
            if (!int2_equals(zox_getv(handle, LayoutSize), handle_size)) {
                zox_setv(handle, LayoutSize, handle_size);
                zox_add(handle, LayoutSizeDirty);
            }
            if (docked == zox_has(handle, Dragable)) {
                zox_setv(handle, RenderDisabled, docked);
                if (docked) {
                    zox_remove(handle, Dragable);
                } else {
                    zox_add(handle, Dragable);
                }
            }
        }
    }
}

static void window_layout(ecs* world, entity window, int2 size) {
    window_resize_handles_fit(world, window, size);
    const entity header = zox_get_child_by_id(world, window, zox_id(Header));
    const entity body = zox_get_child_by_id(world, window, zox_id(WindowBody));
    const entity scrollview = zox_get_link(world, window, ScrollviewLink);
    if (!header || !body) {
        return;
    }
    window_undock_button(world, window, header);
    const int2 header_size = { size.x, zox_getv(header, LayoutSize).y };
    const entity footer = zox_get_child_by_id(world, window, zox_id(WindowFooter));
    const int footer_height = footer ? zox_getv(footer, LayoutSize).y : 0;
    const entity search = window_search_bar_shown(world, window);
    const int search_height = search ? zox_getv(search, LayoutSize).y : 0;
    const int2 body_size = { size.x, int_max(1, size.y - header_size.y - footer_height - search_height) };
    if (int2_equals(zox_getv(body, LayoutSize), body_size) &&
        (!zox_valid(scrollview) || !zox_has(scrollview, LayoutSize) ||
            int2_equals(zox_getv(scrollview, LayoutSize), body_size))) {
        return;     // already fits, its scroll view too
    }
    zox_setv(header, LayoutSize, header_size);
    if (search) {
        zox_setv(search, LayoutSize, ((int2) { size.x, search_height }));
        zox_setv(search, LayoutPosition, ((int2) { 0, -header_size.y - search_height / 2 }));
        zox_add(search, LayoutSizeDirty);
    }
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
        scrollview_fit_list_size(world, list, body_size, scrollview_bar_size(world, scrollview, 0),
            scrollview_bar_size(world, scrollview, 1));
        const byte2 padding = zox_getv(list, TextPadding);
        const byte2 margins = zox_getv(list, ListMargins);
        const int row = int_max(1, zox_getv(list, ElementFontSize) + 2 * padding.y + zox_getv(list, ListPadding).y);
        const int visible = int_clamp((body_size.y - 2 * margins.y) / row, 1, 255);
        zox_setv(list, ListVisible, visible);
        if (zox_has(list, VirtualList)) {
            zox_add(list, Dirty);
        } else {
            const int max_start = int_max(0, (int) zox_get_children_count(world, list) - visible);
            zox_setv(list, ListStart, int_min(zox_getv(list, ListStart), max_start));
            zox_add(list, ListDirty);
        }
        zox_add(list, ListPositionDirty);
        zox_add(list, ListPositionUpdate);
    }
    if (scrollbar) {
        zox_add(scrollbar, LayoutSizeDirty);
    }
}

static void window_layout_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        window_layout(world, e, zox_getv(e, LayoutSize));
    }
}

// a docked window's list got the core Dirty (set up last in a window's spawn, so everything spawned with it is there): its
//  insides fit the window again (in a spawn frame the scroll view's own spawn size can land after the dock layout)
static void window_list_layout_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity window = zox_get_parent_by_id(world, it->entities[i], zox_id(Window));
        if (window && zox_has(window, Docked) && zox_has(window, LayoutSize)) {
            window_layout(world, window, zox_getv(window, LayoutSize));
        }
    }
}
