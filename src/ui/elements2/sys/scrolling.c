// when scroll_front_drags, get window, move elements by amount
//      > e->parent->parent->children[2+] for list elements - if vertical scrollbar

// TODO: ListLink -> Link scrollbar directly to list
zox_sys2(ScrollbarSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DraggableState);
    zox_sys_in(LayoutPosition);
    zox_sys_in(LayoutSize);
    zox_sys_in(ScrollviewLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DraggableState, state);
        zox_sys_i(LayoutPosition, position);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(ScrollviewLink, scrollview);

        if (!state->value) {
            continue;
        }

        entity scrollbar = zox_get_parent(world, e);
        // entity scrollbar = parent->value;
        if (!zox_valid(scrollbar) || !zox_has(scrollbar, LayoutSize)) {
            zox_log_error("Scrollbar parent is invalid");
            continue;
        }

        zox_geter_value(scrollbar, LayoutSize, int2, scrollbarsize);
        if (scrollbarsize.y == size->value.y) {
            continue;
        }

        // entity scrollview = zox_get_value(scrollbar, ParentLink);
        if (!zox_valid(scrollview->value)) {
            zox_log_error("ScrollView is invalid (scrollbar [%s]'s parent)", zox_get_name(scrollbar));
            continue;
        }

        entity list_ui = zox_has(scrollview->value, ListUILink) ? zox_gett_value(scrollview->value, ListUILink) : 0;

        if (!zox_valid(list_ui)) {
            zox_log_error("List UI: Invalid [%s]", zox_get_name(scrollview->value));
            continue;
        }

        if (!zox_has(list_ui, ListVisible) || !zox_has(list_ui, ListStart)) {
            zox_log_error("List UI: Invalid Components. [%s]", zox_get_name(list_ui));
            continue;
        }

        entity list_elements[layouts2_children_capacity];
        uint list_elements_length = zox_get_children(world, list_ui, list_elements, layouts2_children_capacity);
        zox_geter_value(list_ui, ListVisible, byte, visible);
        zox_geter_value(list_ui, ListStart, byte, start);

        int scrollbar_offset = scrollbarsize.y - size->value.y;
        int anchored_position_y = -position->value.y + scrollbar_offset / 2;
        float scroll_percentage = anchored_position_y / (float) scrollbar_offset;
        // scrollview_spawn - scrollview_shown gives us total_starts

        byte total_elements = list_elements_length;
        byte total_starts = total_elements - visible;

        byte new_start = (byte) round(total_starts * scroll_percentage);
        if (start != new_start) {
            // zox_log("new scroll position: %i", new_start);
            zox_set(list_ui, ListStart, { new_start });
            zox_set(list_ui, ListPositionDirty, { zox_dirty_trigger });
        }
    }
} zox_sys_end(ScrollbarSystem);
