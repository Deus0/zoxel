// NOTE: Repositions list elementswhen ListPositionDirty is active
zox_sys2(ListSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_in(ListPositionDirty);
    zox_sys_in(ListPadding);
    zox_sys_in(ListMargins);
    zox_sys_in(ListStart);
    zox_sys_in(ListAlignment);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSize, size);
        zox_sys_i(ListPositionDirty, state);
        zox_sys_i(ListPadding, padding);
        zox_sys_i(ListMargins, margins);
        zox_sys_i(ListStart, start);
        zox_sys_i(ListAlignment, alignment);
        if (state->value != zox_dirty_active) {
            continue;
        }
        byte indent_size = size->value.x / 12;
        // calculate total size first: ListUIMax - center it?
        int list_position_y = (int) (size->value.y / 2);
        list_position_y -= margins->value.y;
        // start buffer
        if (start->value) {
            entity first_child = zox_get_child_by_id(world, e, zox_id(LayoutPositionDirty));
            int2 first_size = zox_valid(first_child) ? zox_getv(first_child, LayoutSize) : int2_zero;
            list_position_y += start->value * (first_size.y + padding->value.y);
        }
        uint k = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++, k++) {
                entity e2 = it2.entities[j];
                if (!zox_valid(e2) || !zox_has(e2, LayoutPositionDirty)) {
                    continue;
                }
                zox_geter_value(e2, LayoutSize, int2, child_size);
                zox_muter(e2, LayoutPosition, position);
                zox_muter(e2, LayoutPositionDirty, dirty);
                if (k == 0) {
                    list_position_y -= child_size.y / 2;
                } else {
                    list_position_y -= child_size.y;
                }
                position->value.y = list_position_y;
                if (alignment->value == zox_alignment_left) {
                    position->value.x = margins->value.x - size->value.x / 2 + child_size.x / 2;
                } else if (alignment->value == zox_alignment_right) {
                    position->value.x = - margins->value.x + size->value.x / 2 - child_size.x / 2;
                }
                if (zox_has(e2, ListIndent)) {
                    byte indent = zox_getv(e2, ListIndent);
                    position->value.x += indent * indent_size;
                }
                dirty->value = zox_dirty_trigger;
                list_position_y -= padding->value.y;
            }
        }
    }
} zox_sys_end(ListSystem);
