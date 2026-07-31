/*extern void set_line2_canvas_position(ecs*, entity);

int2 get_element_pixel_positionv(int2 parent_position, int2 parent_size, int2 position, float2 anchor) {
    int2 output = parent_position;
    // position is actually the centre point, so get the bottom left corner here
    // Compute parent origin in pixels (top-left)
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

// Sets canvas position (global for canvas) using parent position and size to calculate it, from parent to child
void set_layout_canvas_position_recursively_new(ecs* world, entity e, int2 parent_position, int2 parent_size, byte skip) {
    if (!zox_valid(e)) {
        return;
    }
    int2 cposition = parent_position;
    if (!skip) {
        if (zox_has(e, LayoutPosition) && zox_has(e, CanvasPosition) && zox_has(e, Anchor) && zox_has(e, LayoutSize)) {
            zox_geter_value(e, LayoutPosition, int2, position);
            zox_geter_value(e, Anchor, float2, anchor);
            zox_muter(e, CanvasPosition, canvas_position);
            canvas_position->value = get_element_pixel_positionv(parent_position, parent_size, position, anchor);
            // NOTE: we pass canvas position down recursively
            cposition = canvas_position->value;
        }
        set_line2_canvas_position(world, e);
    }
    // also set children ones
    if (!zox_has(e, LayoutSize)) {
        return;
    }
    zox_geter_value(e, LayoutSize, int2, size);
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            set_layout_canvas_position_recursively_new(world, e2, cposition, size, 0);
        }
    }
}

// Converts local layout positions to canvas ones
zox_sys2(LocalLayoutSystem) {
    byte is_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutPositionDirty);
    zox_sys_in(LayoutPosition);
    zox_sys_in(LayoutSize);
    zox_sys_in(Anchor);
    zox_sys_out(CanvasPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutPositionDirty, dirty);
        zox_sys_i(LayoutPosition, layout_position);
        zox_sys_i(LayoutSize, layout_size);
        zox_sys_i(Anchor, anchor);
        zox_sys_o(CanvasPosition, cposition);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        zox_sys_e();
        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent)) {
            continue;
        }
        if (!zox_has(parent, LayoutSize)) {
            zox_logw("Layout Parent Invalid; Child [%s:%lu] Parent [%lu]", zox_get_name(e), e, parent);
            continue;
        }
        zox_geter_value(parent, LayoutSize, int2, parent_size);
        int2 parent_position;
        if (zox_has(parent, Canvas) && zox_has(parent, LayoutPosition)) {
            parent_position = zox_get_value(parent, LayoutPosition);
        } else if (zox_has(parent, CanvasPosition)) {
            parent_position = zox_get_value(parent, CanvasPosition);
        } else {
            parent_position = int2_zero;
        }
        cposition->value = get_element_pixel_positionv(parent_position, parent_size, layout_position->value, anchor->value);
        if (is_log) {
            zox_log("-[%s] LPos[%ix%i] CPos [%ix%i]", zox_get_name(e), layout_position->value.x, layout_position->value.y, cposition->value.x, cposition->value.y);
        }
        set_layout_canvas_position_recursively_new(world, e, cposition->value, layout_size->value, 1);
    }
} zox_sys_end(LocalLayoutSystem);
*/
            /*zox_log("  -Anchor (%fx%f)", anchor->value.x, anchor->value.y);
            zox_log("  -P(%s) PSize [%ix%i]", zox_get_name(parent), parent_size.x, parent_size.y);*/
