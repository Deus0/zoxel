void anchor_size_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(AnchorSize);
    zox_sys_out(LayoutSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(AnchorSize, anchor);
        zox_sys_o(LayoutSize, size);
        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent) ||
            !zox_has(parent, LayoutSize))
        {
            continue;
        }
        int2 parent_size = zox_getv(parent, LayoutSize);
        size->value.x = ceil(parent_size.x * anchor->value.x);
        size->value.y = ceil(parent_size.y * anchor->value.y);
    }
    zox_sys_on_end();
} zoxd_system(anchor_size_system);
