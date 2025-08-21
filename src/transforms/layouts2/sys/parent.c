int2 get_element_pixel_positionv(
    const int2 parent_pixel_positionv,
    const int2 parent_size,
    const int2 pixel_position,
    const float2 anchor
) {
    int2 pixel_positionv = parent_pixel_positionv;
    // position is actually the centre point, so get the bottom left corner here
    pixel_positionv.x -= parent_size.x / 2;
    pixel_positionv.y -= parent_size.y / 2;
    // now centre it within the parent element / canvas
    pixel_positionv.x += (int) (parent_size.x * anchor.x);
    pixel_positionv.y += (int) (parent_size.y * anchor.y);
    // add local position offset
    pixel_positionv.x += pixel_position.x;
    pixel_positionv.y += pixel_position.y;
    return pixel_positionv;
}

void set_child_canvas_position(
    ecs* world,
    entity e,
    int2 parent_position,
    int2 parent_size
) {
    zox_geter_value(e, PixelPosition, int2, position);
    zox_geter_value(e, Anchor, float2, anchor);
    zox_muter(e, CanvasPosition, canvas_position);
    canvas_position->value = get_element_pixel_positionv(
        parent_position,
        parent_size,
        position,
        anchor);
    // also set children ones
    if (zox_has(e, Children)) {
        zox_geter_value(e, PixelSize, int2, size);
        zox_geter(e, Children, children);
        for (int i = 0; i < children->length; i++) {
            entity e2 = children->value[i];
            set_child_canvas_position(
                world,
                e2,
                canvas_position->value,
                size
            );
        }
    }
}


extern void anchor_element_position2(int2*, const float2, const int2);

void LayoutParentPositionSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutPositionDirty);
    zox_sys_in(PixelPosition);
    zox_sys_in(PixelSize);
    zox_sys_in(Anchor);
    zox_sys_in(ParentLink);
    zox_sys_out(CanvasPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutPositionDirty, dirty);
        zox_sys_i(PixelPosition, layout_position);
        zox_sys_i(PixelSize, layout_size);
        zox_sys_i(Anchor, anchor);
        zox_sys_i(ParentLink, parent);
        zox_sys_o(CanvasPosition, canvas_position);
        if (dirty->value != zox_dirty_active ||
            !zox_valid(parent->value) ||
            !zox_has(parent->value, PixelPosition) ||
            !zox_has(parent->value, PixelSize)) {
            continue;
        }
        zox_geter_value(parent->value, CanvasPosition, int2, parent_position);
        zox_geter_value(parent->value, PixelSize, int2, parent_size);

        int2 position = layout_position->value;
        anchor_element_position2(
            &position,
            anchor->value,
            layout_size->value);
        canvas_position->value = get_element_pixel_positionv(
            parent_position,
            parent_size,
            position,
            anchor->value);
        // zox_geter_value(e, PixelSize, int2, size);
        // set_child_canvas_position(world, e, canvas_position->value, size);
    }
} zoxd_system2(LayoutParentPositionSystem);