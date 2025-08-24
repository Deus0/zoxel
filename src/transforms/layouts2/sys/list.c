void ListSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ListDirty);
    zox_sys_in(Children);
    zox_sys_in(LayoutSize);
    zox_sys_in(ListPadding);
    zox_sys_in(ListMargins);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(ListDirty, dirty);
        zox_sys_i(Children, children);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(ListPadding, padding);
        zox_sys_i(ListMargins, margins);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        // calculate total size first: ListUIMax - center it?

        int list_position_y = (int) (size->value.y / 2);
        list_position_y -= margins->value.y;

        for (int j = 0; j < children->length; j++) {
            const entity child = children->value[j];

            if (!zox_valid(child)) {
                continue;
            }

            zox_geter_value(child, LayoutSize, int2, child_size);
            zox_muter(child, LayoutPosition, position);
            zox_muter(child, LayoutPositionDirty, position_dirty);

            if (j == 0) list_position_y -= child_size.y / 2;
            else list_position_y -= child_size.y;

            position->value.y = list_position_y;
            position_dirty->value = zox_dirty_trigger;

            list_position_y -= padding->value.y;
        }
    }
} zoxd_system2(ListSystem);