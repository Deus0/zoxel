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
        zox_geter(list, Children, list_children);
        byte existing = list_children->length;
        zox_geter(scrollview->value, Children, scrollarea_children);
        entity scrollbar = scrollarea_children->value[0];
        zox_geter(scrollbar, Children, scrollbar_children);
        entity handle = scrollbar_children->value[0];
        zox_geter_value(scrollbar, LayoutSize, int2, scrollbar_size);

        int handle_height = existing <= visible->value ?
            scrollbar_size.y :
            (int) scrollbar_size.y * (float_min(1, (float) visible->value / (float) existing));
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
        //zox_log("   visible [%i] existing [%i]", visible->value, existing);

    }
} zox_sys_end(ScrollbarHandleSystem);
