void anchor_element_size2D(
    int2 *size,
    const float2 anchor,
    const int2 parent_size
) {
    size->x = ceil(parent_size.x * anchor.x);
    size->y = ceil(parent_size.y * anchor.y);
    // zox_log("+ new size [%ix%i] from parent [%ix%i] anchor [%fx%f]", size->x, size->y, parent_size.x, parent_size.y, anchor.x, anchor.y)
}

void AnchorSizeSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(AnchorSize);
    zox_sys_in(ParentLink);
    zox_sys_out(PixelSize);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(AnchorSize, anchor);
        zox_sys_i(ParentLink, parent);
        zox_sys_o(PixelSize, size);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        zox_geter_value(parent->value, PixelSize, int2, parent_size);
        anchor_element_size2D(&size->value, anchor->value, parent_size);

        /*zox_log("+ [%s] Anchor Sized: %ix%i - parent [%ix%i]",
            zox_get_name(it->entities[i]),
            size->value.x, size->value.y,
            parent_size.x, parent_size.y);*/

    }
} zoxd_system2(AnchorSizeSystem);