void anchor_element_size2D(int2 *size, float2 anchor, int2 parent_size) {
    size->x = ceil(parent_size.x * anchor.x);
    size->y = ceil(parent_size.y * anchor.y);
    // zox_log("+ new size [%ix%i] from parent [%ix%i] anchor [%fx%f]", size->x, size->y, parent_size.x, parent_size.y, anchor.x, anchor.y)
}

zox_sys2(AnchorSizeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(AnchorSize);
    zox_sys_in(ParentLink);
    zox_sys_out(LayoutSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(AnchorSize, anchor);
        zox_sys_i(ParentLink, parent);
        zox_sys_o(LayoutSize, size);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!zox_valid(parent->value)) {
            continue;
        }

        zox_geter_value(parent->value, LayoutSize, int2, parent_size);
        anchor_element_size2D(&size->value, anchor->value, parent_size);

        /*zox_log("+ [%s] Anchor Sized: %ix%i - parent [%ix%i]",
            zox_get_name(it->entities[i]),
            size->value.x, size->value.y,
            parent_size.x, parent_size.y);*/

    }
} zox_sys_end(AnchorSizeSystem);
