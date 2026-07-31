zox_sys2(AnchorSizeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(AnchorSize);
    zox_sys_out(LayoutSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(AnchorSize, anchor);
        zox_sys_o(LayoutSize, size);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent) || !zox_has(parent, LayoutSize)) {
            continue;
        }
        zox_geter_value(parent, LayoutSize, int2, parent_size);
        size->value.x = ceil(parent_size.x * anchor->value.x);
        size->value.y = ceil(parent_size.y * anchor->value.y);
    }
} zox_sys_end(AnchorSizeSystem);
