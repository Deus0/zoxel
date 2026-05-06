/*
void set_layout_canvas_position_recursively(ecs* world, entity e, int2 parent_position, int2 parent_size, byte skip) {

    if (!zox_valid(e)) {
        return;
    }

    int2 cposition = parent_position;
    if (!skip) {
        if (zox_has(e, LayoutPosition) && zox_has(e, CanvasPosition) && zox_has(e, Anchor) && zox_has(e, LayoutSize)) {
            zox_geter_value(e, LayoutPosition, int2, position);
            zox_geter_value(e, Anchor, float2, anchor);
            zox_muter(e, CanvasPosition, canvas_position);

            canvas_position->value = get_element_pixel_positionv(
                parent_position,
                parent_size,
                position,
                anchor
            );

            // zox_geter_value(e, LayoutSize, int2, size);
            // zox_log("+++ (set) Canvas Position [%s] [%ix%i] - Position [%ix%i] Anchor [%fx%f] Size [%ix%i]  +++", zox_get_name(e), canvas_position->value.x, canvas_position->value.y, position.x, position.y, anchor.x, anchor.y, size.x, size.y);

            // NOTE: we pass canvas position down recursively
            cposition = canvas_position->value;
        }
        set_line2_canvas_position(world, e);
    }

    // also set children ones
    if (zox_has(e, Children) && zox_has(e, LayoutSize)) {
        zox_geter_value(e, LayoutSize, int2, size);
        zox_geter(e, Children, children);
        for (int i = 0; i < children->length; i++) {
            entity e2 = children->value[i];

            set_layout_canvas_position_recursively(world, e2, cposition, size, 0);

            // For now we make the old hierarchy also use the new one
            set_layout_canvas_position_recursively_new(world, e2, cposition, size, 0);
        }
    }
}

void anchor_element_position2(int2* position, float2 anchor, int2 parent_size) {
    position->x += (parent_size.x / 2.0f) - ceil(parent_size.x * anchor.x);
    position->y += (parent_size.y / 2.0f) - ceil(parent_size.y * anchor.y);
}

zox_sys2(LayoutParentPositionSystem) {
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

        // Can set immediately when dirty
        if (dirty->value != zox_dirty_trigger && dirty->value != zox_dirty_active) {
            continue;
        }

        if (!zox_valid(parent->value) || !zox_has(parent->value, LayoutSize)) {
            zox_sys_e();
            zox_logw("Layout [%s:lu] has invalid parent [%s]", zox_get_name(e), e, parent->value);
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
        canvas_position->value = get_element_pixel_positionv(parent_position, parent_size, position, anchor->value);

        zox_sys_e();
        // zox_log("+++ (root) Canvas Position [%s] [%ix%i] - Position [%ix%i] Anchor [%fx%f] Size [%ix%i] +++", zox_get_name(e), canvas_position->value.x, canvas_position->value.y, position.x, position.y, anchor->value.x, anchor->value.y, parent_size.x, parent_size.y);

        set_layout_canvas_position_recursively(world, e, canvas_position->value, layout_size->value, 1);
    }
} zox_sys_end(LayoutParentPositionSystem);*/

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
