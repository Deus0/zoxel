// when scroll_front_drags, get window, move elements by amount
//      > e->parent->parent->children[2+] for list elements - if vertical scrollbar

// TODO: ListLink -> Link scrollbar directly to list
zox_sys2(ScrollbarSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DraggableState);
    zox_sys_in(LayoutPosition);
    zox_sys_in(LayoutSize);
    zox_sys_in(ParentLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(DraggableState, state);
        zox_sys_i(LayoutPosition, position);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(ParentLink, parent);

        if (!state->value) {
            continue;
        }

        const entity scrollbar = parent->value;
        zox_geter_value(scrollbar, LayoutSize, int2, scrollbarsize);
        if (scrollbarsize.y == size->value.y) {
            continue;
        }
        entity scrollview = zox_get_value(scrollbar, ParentLink);
        zox_geter(scrollview, Children, scrollview_children);

        entity list_ui = scrollview_children->value[1];


        int scrollbar_offset = scrollbarsize.y - size->value.y;
        int anchored_position_y = -position->value.y + scrollbar_offset / 2;
        float scroll_percentage = anchored_position_y / (float) scrollbar_offset;
        // scrollview_spawn - scrollview_shown gives us total_starts

        // zox_geter_value(list_ui, LayoutSize, int2, window_size)
        zox_geter_value(list_ui, ListVisible, byte, visible);
        zox_geter_value(list_ui, ListStart, byte, start);
        zox_geter(list_ui, Children, list_elements);
        byte total_elements = list_elements->length;
        byte total_starts = total_elements - visible;

        byte new_start = (byte) round(total_starts * scroll_percentage);
        if (start != new_start) {
            // zox_log("new scroll position: %i", new_start);
            zox_set(list_ui, ListStart, { new_start });
            zox_set(list_ui, ListDirty, { zox_dirty_trigger });
        }
    }
} zox_sys_end(ScrollbarSystem);