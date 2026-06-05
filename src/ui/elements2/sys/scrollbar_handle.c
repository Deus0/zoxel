// Resizes the scrollbar handle upon list updates
// TODO: shouldn't this be on scrollarea resizes??
zox_sys2(ScrollbarHandleSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ListDirty);
    zox_sys_in(ListVisible);
    zox_sys_in(ScrollviewLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ListDirty, state);
        zox_sys_i(ListVisible, visible);
        zox_sys_i(ScrollviewLink, scrollview);
        if (state->value != zox_dirty_active) {
            continue;
        }
        // set handle size here
        zox_geter_value(scrollview->value, ListUILink, entity, list);
        uint list_children_length = zox_get_children_count(world, list);
        /*entity list_children[layouts2_children_capacity];
        uint list_children_length = zox_get_children(world, list, list_children, layouts2_children_capacity);*/
        if (!list_children_length) {
            continue;
        }
        /*entity scrollarea_children[layouts2_children_capacity];
        uint scrollarea_children_length = zox_get_children(world, scrollview->value, scrollarea_children, layouts2_children_capacity);
        if (!scrollarea_children_length) {
            continue;
        }*/
        entity scrollbar = zox_get_child_by_id(world, scrollview->value, zox_id(Scrollbar)); // scrollarea_children[0];
        if (!zox_valid(scrollbar)) {
            continue;
        }
        entity scrollbar_children[layouts2_children_capacity];
        uint scrollbar_children_length = zox_get_children(world, scrollbar, scrollbar_children, layouts2_children_capacity);
        if (!scrollbar_children_length) {
            continue;
        }
        entity handle = scrollbar_children[0];
        zox_geter_value(scrollbar, LayoutSize, int2, scrollbar_size);
        int handle_height = list_children_length <= visible->value ? scrollbar_size.y : (int) scrollbar_size.y * (float_min(1, (float) visible->value / (float) list_children_length));
        int ybounds = (scrollbar_size.y / 2) - handle_height / 2;
        int2 handle_size = (int2) { scrollbar_size.x, handle_height };
        int2 handle_position = (int2) { 0, (scrollbar_size.y - handle_height) / 2 };
        zox_set(handle, LayoutSize, { handle_size });
        zox_set(handle, LayoutSizeDirty, { zox_dirty_trigger });
        zox_set(handle, LayoutPosition, { handle_position });
        zox_set(handle, LayoutPositionDirty, { zox_dirty_trigger });
        zox_set(handle, LayoutConstraints, { (int4) { 0, 0, -ybounds, ybounds } });
        //zox_log("Resizing Scrollbar Handle [%s]:", zox_get_name(handle));
        //zox_log("   + new height [%i] scrollbar height [%i]", handle_height, scrollbar_size.y );
        //zox_log("   visible [%i] existing [%i]", visible->value, list_children_length);

    }
} zox_sys_end(ScrollbarHandleSystem);
