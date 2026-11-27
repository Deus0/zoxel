// Resizes the scrollbar handle upon list updates
// TODO: shouldn't this be on scrollarea resizes??
// an observer on ListDirty (a one-off event, removed here)
void scrollbar_handle_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ListVisible);
    zox_sys_in(ListStart);
    for (int i = 0; i < it->count; i++) {
        zox_remove(it->entities[i], ListDirty);
        if (zox_has(it->entities[i], VirtualList)) {
            continue;
        }
        zox_sys_i(ListVisible, visible);
        zox_sys_i(ListStart, start);
        entity scrollview = zox_get_link(world, it->entities[i], ScrollviewLink);
        if (!zox_valid(scrollview)) {
            continue;
        }
        // set handle size here
        entity list = zox_get_link(world, scrollview, ListUILink);
        // its rows: a grid's (its cells in rows), else its children; none: the handle fills its bar
        const int list_children_length = zox_has(list, GridSize) ? zox_getv(list, GridSize).y :
            (int) zox_get_children_count(world, list);
        entity scrollbar = zox_get_child_by_id(world, scrollview, zox_id(Scrollbar)); // scrollarea_children[0];
        if (!zox_valid(scrollbar)) {
            continue;
        }
        entity scrollbar_children[layouts2_children_capacity];
        uint scrollbar_children_length = zox_get_children(world, scrollbar, scrollbar_children, layouts2_children_capacity);
        if (!scrollbar_children_length) {
            continue;
        }
        entity handle = scrollbar_children[0];
        const int2 size = zox_getv(scrollbar, LayoutSize);
        const int max_start = list_children_length - visible->value;
        const int length = max_start > 0 ? size.y * visible->value / list_children_length : size.y;
        const float fraction = max_start > 0 ? start->value / (float) max_start : 0;
        layout_scrollbar_handle(world, handle, size, length, fraction, 0);
    }
}
