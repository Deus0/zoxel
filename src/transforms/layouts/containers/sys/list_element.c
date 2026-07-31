// Reposition the List Element when it resizes so it matches alignment
zox_sys2(ListElementPositionSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_in(LayoutSizeDirty);
    zox_sys_out(LayoutPosition);
    zox_sys_out(LayoutPositionDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSize, size);
        zox_sys_i(LayoutSizeDirty, state);
        zox_sys_o(LayoutPosition, position);
        zox_sys_o(LayoutPositionDirty, dirty);

        if (state->value != zox_dirty_active) {
            continue;
        }

        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent) || !zox_has(parent, ListAlignment) || !zox_has(parent, ListMargins) || !zox_has(parent, LayoutSize)) {
            continue;
        }

        zox_geter_value(parent, ListAlignment, byte, alignment);
        if (alignment == zox_alignment_centre) {
            continue;
        }

        zox_geter_value(parent, ListMargins, byte2, margins);
        zox_geter_value(parent, LayoutSize, int2, lsize);

        if (alignment == zox_alignment_left) {
            position->value.x = margins.x - lsize.x / 2 + size->value.x / 2;
        } else if (alignment == zox_alignment_right) {
            position->value.x = - margins.x + lsize.x / 2 - size->value.x / 2;
        } else {
            continue;
        }

        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(ListElementPositionSystem);
