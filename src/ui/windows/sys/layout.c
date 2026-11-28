// a docked window resizes from its inner edge only, the side facing the game (none docked on top)
static byte window_resize_handle_docked(float2 handle, float2 dock) {
    if (dock.y == 1) {
        return 0;
    }
    if (dock.y == 0) {
        return float2_equals(handle, ((float2) { 0.5f, 1 }));
    }
    return float2_equals(handle, ((float2) { 1 - dock.x, 0.5f }));
}

// a resize handle shows and drags unless its window is docked and it isn't on its inner edge
static void window_resize_handle_state(ecs* world, entity window, entity handle) {
    const byte shown = !zox_has(window, Docked) ||
        window_resize_handle_docked(zox_getv(handle, Anchor), zox_getv(window, Anchor));
    if (shown != zox_has(handle, Dragable)) {
        zox_setv(handle, RenderDisabled, !shown);
        if (shown) {
            zox_add(handle, Dragable);
        } else {
            zox_remove(handle, Dragable);
        }
    }
}

// fits a window's 8 resize handles (ins/resize.c) to its size; docked, only its inner edge's shows and drags
static void window_resize_handles_fit(ecs* world, entity window, int2 size) {
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
            window_resize_handle_state(world, window, handle);
        }
    }
}

// a handle spawned after its window's layout (in its spawn frame, its children are deferred) fits its window's dock
static void window_resize_handle_added(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity window = zox_get_parent(world, it->entities[i]);
        if (zox_valid(window) && zox_has(window, Anchor)) {
            window_resize_handle_state(world, window, it->entities[i]);
        }
    }
}

// the vertical bar of a scroll view (not its horizontal one)
static inline entity scrollview_vertical_bar(ecs* world, entity scrollview) {
    iter it = zox_children(world, scrollview);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            if (zox_has(it.entities[i], Scrollbar) && !zox_has(it.entities[i], HorizontalScrollbar)) {
                zox_children_end(it);
                return it.entities[i];
            }
        }
    }
    return 0;
}

// a grid's window (ins/grid.c, an inventory's slots): its columns fit its width, its rows follow, those that don't
//  fit scroll (ListStart its first shown row, ListVisible how many fit, grid_system places them from it, the list's
//  Mask clips the rest)
//  - its vertical bar shows while rows don't fit, hides when they all do; the grid narrower by it while it shows
//  - the window is at least a cell, never grown to fit its rows (it moved about as they reflowed)
//  - chrome: the window's height outside its body (header, footer, search bar)
static void window_grid_fit(ecs* world, entity window, entity scrollview, int2* size, int chrome) {
    const entity grid = zox_valid(scrollview) ? zox_get_link(world, scrollview, ListUILink) : 0;
    if (!zox_valid(grid) || !zox_has(grid, GridSize)) {
        return;
    }
    const entity cell = zox_get_child_by_id(world, grid, zox_id(LayoutSize));
    if (!cell) {
        return;
    }
    const int2 cell_size = zox_getv(cell, LayoutSize);
    const byte2 margins = zox_getv(grid, GridMargins);
    const int gap = zox_getv(grid, GridPadding).x;     // grid_system's between columns
    const int count = zox_get_children_count(world, grid);
    const entity bar = scrollview_vertical_bar(world, scrollview);
    const int bar_width = bar ? zox_getv(bar, LayoutSize).x : 0;
    // at least one cell, and its bar
    const int2 least = { cell_size.x + 2 * margins.x + bar_width, chrome + cell_size.y + 2 * margins.y };
    if (size->x < least.x || size->y < least.y) {
        *size = (int2) { int_max(size->x, least.x), int_max(size->y, least.y) };
        zox_setv(window, LayoutSize, *size);
    }
    const int fits = int_max(1, (size->y - chrome - 2 * margins.y) / int_max(1, cell_size.y));
    // its columns in a width, without its bar first, with it if the rows don't fit then
    byte shown = 0;
    int columns = 1, rows = 1;
    for (byte with_bar = 0; with_bar < 2; with_bar++) {
        const int width = size->x - (with_bar ? bar_width : 0);
        columns = int_clamp((width - 2 * margins.x + gap) / int_max(1, cell_size.x + gap), 1, int_clamp(count, 1, 255));
        rows = int_min(255, (count + columns - 1) / columns);
        shown = with_bar;
        if (rows <= fits || !bar) {
            break;
        }
    }
    const byte2 grid_size = { columns, rows };
    if (!byte2_equals(zox_getv(grid, GridSize), grid_size)) {
        zox_setv(grid, GridSize, grid_size);
    }
    // its bar: in place, the layout reads it next (scrollview_bar_size)
    if (bar && zox_getv(bar, RenderDisabled) == shown) {
        zox_muter(bar, RenderDisabled, hidden);
        hidden->value = !shown;
        set_children_render_disabled(world, bar, !shown);
    }
    // the rows that fit, its scroll kept in them
    const int visible = int_min(fits, rows);
    zox_setv(grid, ListVisible, visible);
    const int start = int_min(zox_getv(grid, ListStart), rows - visible);
    if (start != zox_getv(grid, ListStart)) {
        zox_setv(grid, ListStart, start);
    }
    zox_add(grid, ListDirty);   // its bar's handle (scrollbar_handle_event)
    zox_add(grid, Refresh);     // grid_system places its cells for its new size, next frame
}

// a list's window (a game menu's) no narrower than its widest row, its margins and its scrollbar; a virtual list's rows
//  (the editor's names) and a docked window's (the inspector's) are cut by its mask instead, scrolled sideways
static void window_rows_fit(ecs* world, entity window, entity scrollview, int2* size) {
    const entity list = zox_valid(scrollview) ? zox_get_link(world, scrollview, ListUILink) : 0;
    if (!zox_valid(list) || zox_has(list, VirtualList) || zox_has(window, Docked)) {
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

// places a shown header button left of edge, returns the new edge
static int window_header_button_fit(ecs* world, entity button, byte close, int edge, int gap) {
    if (!button || (zox_has(button, RenderDisabled) && zox_getv(button, RenderDisabled))) {
        return edge;
    }
    const int w = zox_getv(button, LayoutSize).x;
    // the close button aligns its right edge on its position, the others centre on it
    const int x = close ? -edge : -(edge + w / 2);
    if (zox_getv(button, LayoutPosition).x != x) {
        zox_setv(button, LayoutPosition, ((int2) { x, 0 }));
        zox_add(button, LayoutSizeDirty);   // placed there
    }
    return edge + w + gap;
}

// its buttons sit right to left from its edge (close, undock, search, its own), a gap between, its icon on the left, and
//  its title fits between: returns the width they need (its least)
static int window_header_fit(ecs* world, entity header, int width) {
    const int gap = 4;
    const entity buttons[] = {
        zox_get_child_by_id(world, header, zox_id(CloseButton)),
        zox_get_child_by_id(world, header, zox_id(UndockButton)),
        zox_get_child_by_id(world, header, zox_id(WindowSearchButton)) };
    int edge = 0;   // the buttons' width from the right edge
    for (int i = 0; i < 3; i++) {
        edge = window_header_button_fit(world, buttons[i], i == 0, edge, gap);
    }
    iter it = zox_children(world, header);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            if (zox_has(it.entities[i], HeaderButton)) {
                edge = window_header_button_fit(world, it.entities[i], 0, edge, gap);
            }
        }
    }
    const entity text = zox_get_child_by_id(world, header, zox_id(TextFit));
    if (!text) {
        return edge;
    }
    const byte centred = zox_getv(text, Anchor).x == 0.5f;
    // an icon: the title starts after it, its left edge kept there as its font fits (text_fit_system)
    const entity icon = zox_get_child_by_id(world, header, zox_id(HeaderIcon));
    int left = 0;
    if (icon) {
        // a square of its header's height, inset
        const int side = int_max(1, zox_getv(header, LayoutSize).y - 2 * window_header_icon_inset);
        if (zox_getv(icon, LayoutSize).x != side) {
            zox_setv(icon, LayoutSize, int2_single(side));
            zox_setv(icon, LayoutPosition, ((int2) { side / 2 + window_header_icon_inset, 0 }));
            zox_add(icon, LayoutSizeDirty);
        }
        left = side + window_header_icon_inset + gap;
    }
    // after the icon: a side title keeps its left edge there as its font fits (text_fit_system), a centred one centres
    //  between the icon and the buttons
    int x = 0;
    if (centred) {
        x = (left - edge) / 2;
    } else if (icon) {
        const TextData* data = zox_get(text, TextData);
        x = left + calculate_zext_size(data->value, data->length, zox_getv(text, TextFontSize),
            zox_getv(text, TextPadding), default_line_padding).x / 2;
    }
    if ((icon || centred) && zox_getv(text, LayoutPosition).x != x) {
        zox_setv(text, LayoutPosition, ((int2) { x, 0 }));
        zox_add(text, LayoutSizeDirty);
    }
    const int fit = int_max(1, width - left - edge);
    if (zox_getv(text, TextFitWidth) != fit) {
        zox_setv(text, TextFitWidth, fit);
    }
    return left + edge;
}

// the side a window is being resized from (its dragged resize handle's anchor), its centre if none is dragged
static float2 window_resizing_side(ecs* world, entity window) {
    iter it = zox_children(world, window);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity handle = it.entities[i];
            if (zox_has(handle, WindowResizeHandle) && zox_has(handle, DraggableState) && zox_getv(handle, DraggableState)) {
                zox_children_end(it);
                return zox_getv(handle, Anchor);
            }
        }
    }
    return float2_half;
}

// a window grown past the size it was given (to fit its rows or grid) grows on the side it's resized from, the far
//  side stays (round its centre both sides moved)
static void window_grown_keeps_side(ecs* world, entity window, int2 asked, int2 size) {
    if (int2_equals(asked, size)) {
        return;
    }
    const float2 side = window_resizing_side(world, window);
    const int2 sign = { side.x == 0 ? -1 : side.x == 1 ? 1 : 0, side.y == 0 ? -1 : side.y == 1 ? 1 : 0 };
    if (!sign.x && !sign.y) {
        return;
    }
    // in place: window_canvas_fit reads it next (a deferred set was undone by its)
    zox_muter(window, LayoutPosition, position);
    position->value.x += sign.x * (size.x - asked.x) / 2;
    position->value.y += sign.y * (size.y - asked.y) / 2;
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
    const int2 asked = size;
    window_grid_fit(world, window, scrollview, &size, header_height + footer_height + search_height);
    const entity view_list = zox_valid(scrollview) ? zox_get_link(world, scrollview, ListUILink) : 0;
    const byte grid = zox_valid(view_list) && zox_has(view_list, GridSize);
    if (!grid) {
        window_rows_fit(world, window, scrollview, &size);
    }
    window_grown_keeps_side(world, window, asked, size);
    window_canvas_fit(world, window, &size);     // after: a grid's rows can grow it
    // its header's icon and buttons fit (they can spawn after its first layout): it's no narrower than them
    const int least = window_header_fit(world, header, size.x);
    if (size.x < least) {
        size.x = least;
        zox_setv(window, LayoutSize, size);
        window_header_fit(world, header, size.x);
    }
    window_resize_handles_fit(world, window, size);
    const int2 header_size = { size.x, header_height };
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
    scrollview_fit_vertical_bar(world, scrollview, scrollview_bar_size(world, scrollview, 1));
    if (zox_valid(list)) {
        scrollview_fit_list_size(world, list, body_size, scrollview_bar_size(world, scrollview, 0),
            scrollview_bar_size(world, scrollview, 1));
        if (grid) {
            zox_add(list, ListDirty);
            zox_add(list, Refresh);
            if (scrollbar) {
                zox_add(scrollbar, LayoutSizeDirty);
            }
            return;
        }
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
