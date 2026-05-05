extern void set_line2_canvas_position(ecs*, entity);


float2 get_element_position(int2 position, int2 size) {

    float2 sizef = int2_to_float2(size);
    float aspect_ratio = sizef.x / sizef.y;
    float2 positionf = int2_to_float2(position);
    float2_divide_float2(&positionf, sizef);
    // we get our 0 to 1, make -0.5 to 0.5, then stretch x along canvas
    positionf.x -= 0.5f;
    positionf.y -= 0.5f;
    positionf.x *= aspect_ratio;

    return positionf;
}

int2 get_element_pixel_positionv(int2 parent_position, int2 parent_size, int2 position, float2 anchor) {

    int2 output = parent_position;
    // position is actually the centre point, so get the bottom left corner here

    // Compute parent origin in pixels (top-left)
    output.x -= parent_size.x / 2;
    output.y -= parent_size.y / 2;
    // zox_log("   - Top Left of Parent [%ix%i] - centre [%ix%i] size [%ix%i]", output.x, output.y, parent_position.x, parent_position.y, parent_size.x, parent_size.y);

    // now centre it within the parent element / canvas
    output.x += (int) (parent_size.x * anchor.x);
    output.y += (int) (parent_size.y * anchor.y);

    // add local position offset
    output.x += position.x;
    output.y += position.y;

    return output;
}

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

            canvas_position->value = get_element_pixel_positionv(
                parent_position,
                parent_size,
                position,
                anchor
            );

            // NOTE: we pass canvas position down recursively
            cposition = canvas_position->value;
        }
        set_line2_canvas_position(world, e);
    }

    // also set children ones
    if (zox_has(e, LayoutSize)) {
        zox_geter_value(e, LayoutSize, int2, size);
        entity children[layouts2_children_capacity];
        uint count = zox_get_children(world, e, children, layouts2_children_capacity);
        for (uint i = 0; i < count; i++) {
            entity e2 = children[i];
            set_layout_canvas_position_recursively_new(world, e2, cposition, size, 0);
        }
    }
}




void set_layout_child_position_recursively_new(ecs* world, entity e, int2 canvas_size) {

    if (!zox_valid(e)) {
        return;
    }

    if (zox_has(e, CanvasPosition) && zox_has(e, Position2)) {
        zox_geter_value(e, CanvasPosition, int2, canvas_position);
        zox_muter(e, Position2, position2);

        position2->value = get_element_position(canvas_position, canvas_size);
    }

    // also set children ones
    entity children[layouts2_children_capacity];
    uint count = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint i = 0; i < count; i++) {
        entity child = children[i];
        set_layout_child_position_recursively_new(world, child, canvas_size);
    }
}

