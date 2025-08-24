int2 get_element_pixel_positionv(
    const int2 parent_position,
    const int2 parent_size,
    const int2 position,
    const float2 anchor
) {
    int2 output = parent_position;
    // position is actually the centre point, so get the bottom left corner here
    output.x -= parent_size.x / 2;
    output.y -= parent_size.y / 2;
    // now centre it within the parent element / canvas
    output.x += (int) (parent_size.x * anchor.x);
    output.y += (int) (parent_size.y * anchor.y);
    // add local position offset
    output.x += position.x;
    output.y += position.y;
    return output;
}

void set_child_canvas_position(
    ecs* world,
    entity e,
    int2 parent_position,
    int2 parent_size,
    byte skip
) {
    int2 cposition = parent_position;
    if (!skip) {
        if (zox_valid(e) && zox_has(e, LayoutPosition) && zox_has(e, CanvasPosition) && zox_has(e, Anchor)) {
            zox_geter_value(e, LayoutPosition, int2, position);
            zox_geter_value(e, Anchor, float2, anchor);
            zox_muter(e, CanvasPosition, canvas_position);
            canvas_position->value = get_element_pixel_positionv(
                parent_position,
                parent_size,
                position,
                anchor);
            cposition = canvas_position->value;
        }
    }
    // also set children ones
    if (zox_has(e, Children)) {
        zox_geter_value(e, LayoutSize, int2, size);
        zox_geter(e, Children, children);
        for (int i = 0; i < children->length; i++) {
            entity e2 = children->value[i];
            set_child_canvas_position(
                world,
                e2,
                cposition,
                size,
                0
            );
        }
    }
}

void anchor_element_position2(
    int2* position,
    const float2 anchor,
    const int2 parent_size
) {
    position->x += (parent_size.x / 2.0f) - ceil(parent_size.x * anchor.x);
    position->y += (parent_size.y / 2.0f) - ceil(parent_size.y * anchor.y);
}

void LayoutParentPositionSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutPositionDirty);
    zox_sys_in(LayoutPosition);
    zox_sys_in(LayoutSize);
    zox_sys_in(Anchor);
    zox_sys_in(ParentLink);
    zox_sys_out(CanvasPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutPositionDirty, dirty);
        zox_sys_i(LayoutPosition, layout_position);
        zox_sys_i(LayoutSize, layout_size);
        zox_sys_i(Anchor, anchor);
        zox_sys_i(ParentLink, parent);
        zox_sys_o(CanvasPosition, canvas_position);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!zox_valid(parent->value) || !zox_has(parent->value, LayoutSize)) {
            zox_log("! invalid parent [%s::%lu]",
                zox_get_name(it->entities[i]),
                it->entities[i]);
            continue;
        }

        zox_geter_value(parent->value, LayoutSize, int2, parent_size);
        int2 parent_position;
        if (zox_has(parent->value, CanvasPosition)) {
            parent_position = zox_get_value(parent->value, CanvasPosition);
        } else if (zox_has(parent->value, LayoutPosition)) {
            parent_position = zox_get_value(parent->value, LayoutPosition);
        } else {
            parent_position = int2_zero;
        }

        int2 position = layout_position->value;
        /*anchor_element_position2(
            &position,
            anchor->value,
            parent_size
        );*/ // layout_size->value);
        canvas_position->value = get_element_pixel_positionv(
            parent_position,
            parent_size,
            position,
            anchor->value
        );
        zox_sys_e();
        set_child_canvas_position(
            world,
            e,
            canvas_position->value,
            layout_size->value,
            1
        );

        /*zox_log("[%s] anc [%.1fx%.1f] loc [%ix%i] -> [%ix%i] => pos [%ix%i] from parent p[%ix%i] s[%ix%i]",
            zox_get_name(it->entities[i]),
            anchor->value.x,
            anchor->value.y,
            layout_position->value.x,
            layout_position->value.y,
            position.x,
            position.y,
            canvas_position->value.x,
            canvas_position->value.y,
            parent_position.x,
            parent_position.y,
            parent_size.x,
            parent_size.y
        );*/
    }
} zoxd_system2(LayoutParentPositionSystem);