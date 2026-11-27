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
            }
            zox_add(handle, LayoutSizeDirty);   // placed on the window's new edges
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

// a grid body (an inventory's slots): its columns fit its width, its rows follow, the window grows to fit them
//  - chrome: the window's height outside its body (header, footer, search bar)
static void window_grid_fit(ecs* world, entity window, entity body, int2* size, int chrome) {
    const entity cell = zox_has(body, GridSize) ? zox_get_child_by_id(world, body, zox_id(LayoutSize)) : 0;
    if (!cell) {
        return;
    }
    const int2 cell_size = zox_getv(cell, LayoutSize);
    const byte2 margins = zox_getv(body, GridMargins);
    const int gap = zox_getv(body, GridPadding).x;     // grid_system's between columns
    const int count = zox_get_children_count(world, body);
    const int columns = int_clamp((size->x - 2 * margins.x + gap) / int_max(1, cell_size.x + gap), 1,
        int_clamp(count, 1, 255));
    const int rows = int_min(255, (count + columns - 1) / columns);
    const byte2 grid = { columns, rows };
    if (!byte2_equals(zox_getv(body, GridSize), grid)) {
        zox_setv(body, GridSize, grid);
    }
    zox_add(body, Refresh);     // grid_system places its cells for its new size, next frame
    const int2 least = { columns * (cell_size.x + gap) - gap + 2 * margins.x, chrome + rows * cell_size.y + 2 * margins.y };
    if (size->x < least.x || size->y < least.y) {
        *size = (int2) { int_max(size->x, least.x), int_max(size->y, least.y) };
        zox_setv(window, LayoutSize, *size);
    }
}

// a list's window (a game menu's) no narrower than its widest row, its margins and its scrollbar; a virtual list's rows
//  (the editor's names) are cut by its mask instead
static void window_rows_fit(ecs* world, entity window, entity scrollview, int2* size) {
    const entity list = zox_valid(scrollview) ? zox_get_link(world, scrollview, ListUILink) : 0;
    if (!zox_valid(list) || zox_has(list, VirtualList)) {
        return;
    }
    int widest = 0;
    iter it = zox_children(world, list);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            widest = int_max(widest, zox_getv(it.entities[i], LayoutSize).x);
        }
    }
    const int least = widest + 2 * zox_getv(list, ListMargins).x + scrollview_bar_size(world, scrollview, 0);
    if (widest && size->x < least) {
        size->x = least;
        zox_setv(window, LayoutSize, *size);
    }
}

// its title fits its header beside its buttons (close, undock): a centred title keeps clear of them on both sides
static void window_header_fit(ecs* world, entity header, int width) {
    const entity text = zox_get_child_by_id(world, header, zox_id(TextFit));
    if (!text) {
        return;
    }
    int buttons = 0;
    iter it = zox_children(world, header);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity button = it.entities[i];
            const byte hidden = zox_has(button, RenderDisabled) && zox_getv(button, RenderDisabled);
            if (button != text && zox_has(button, LayoutSize) && !hidden) {
                buttons = int_max(buttons, -zox_getv(button, LayoutPosition).x + zox_getv(button, LayoutSize).x);
            }
        }
    }
    const byte centred = zox_getv(text, Anchor).x == 0.5f;
    const int fit = int_max(1, width - buttons * (centred ? 2 : 1));
    if (zox_getv(text, TextFitWidth) != fit) {
        zox_setv(text, TextFitWidth, fit);
    }
}

// a floating window stays on its canvas: no larger than it, its edges on it
static void window_canvas_fit(ecs* world, entity window, int2* size) {
    const entity canvas = zox_get_parent_by_id(world, window, zox_id(Canvas));
    if (zox_has(window, Docked) || !zox_has(window, BoundToParent) || !zox_valid(canvas)) {
        return;
    }
    const int2 canvas_size = zox_getv(canvas, LayoutSize);
    const int2 fit = { int_min(size->x, canvas_size.x), int_min(size->y, canvas_size.y) };
    if (!int2_equals(fit, *size)) {
        *size = fit;
        zox_setv(window, LayoutSize, fit);
    }
    // as canvas_bounds_system's constraints: its centre's range for its anchor
    const float2 anchor = zox_getv(window, Anchor);
    const int2 position = zox_getv(window, LayoutPosition);
    const int2 kept = {
        int_clamp(position.x, -canvas_size.x * anchor.x + fit.x / 2, canvas_size.x * (1 - anchor.x) - fit.x / 2),
        int_clamp(position.y, -canvas_size.y * anchor.y + fit.y / 2, canvas_size.y * (1 - anchor.y) - fit.y / 2) };
    if (!int2_equals(kept, position)) {
        zox_setv(window, LayoutPosition, kept);
    }
}

static void window_layout(ecs* world, entity window, int2 size) {
    const entity header = zox_get_child_by_id(world, window, zox_id(Header));
    const entity body = zox_get_child_by_id(world, window, zox_id(WindowBody));
    const entity scrollview = zox_get_link(world, window, ScrollviewLink);
    if (!header || !body) {
        window_canvas_fit(world, window, &size);
        window_resize_handles_fit(world, window, size);
        return;
    }
    window_undock_button(world, window, header);
    const int header_height = zox_getv(header, LayoutSize).y;
    const entity footer = zox_get_child_by_id(world, window, zox_id(WindowFooter));
    const int footer_height = footer ? zox_getv(footer, LayoutSize).y : 0;
    const entity search = window_search_bar_shown(world, window);
    const int search_height = search ? zox_getv(search, LayoutSize).y : 0;
    window_grid_fit(world, window, body, &size, header_height + footer_height + search_height);
    window_rows_fit(world, window, scrollview, &size);
    window_canvas_fit(world, window, &size);     // after: a grid's rows can grow it
    window_resize_handles_fit(world, window, size);
    const int2 header_size = { size.x, header_height };
    const int2 body_size = { size.x, int_max(1, size.y - header_size.y - footer_height - search_height) };
    if (int2_equals(zox_getv(body, LayoutSize), body_size) &&
        (!zox_valid(scrollview) || !zox_has(scrollview, LayoutSize) ||
            int2_equals(zox_getv(scrollview, LayoutSize), body_size))) {
        return;     // already fits, its scroll view too
    }
    zox_setv(header, LayoutSize, header_size);
    window_header_fit(world, header, header_size.x);
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
