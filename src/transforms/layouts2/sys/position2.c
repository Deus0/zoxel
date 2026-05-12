// This converts our layout position to our canvas real position
float2 get_element_position(int2 position, float2 canvas_sizef, float aspect_ratio) {
    float2 positionf = int2_to_float2(position);
    // Gets [0,1] Values
    float2_divide_float2(&positionf, canvas_sizef);
    // Shift [0,1] to [-0.5,0.5]
    positionf.x -= 0.5f;
    positionf.y -= 0.5f;
    // Multiply by Canvas Aspect Ratio
    positionf.x *= aspect_ratio;
    return positionf;
}

void set_layout_child_position_recursively_new(ecs* world, entity e, float2 canvas_sizef, float aspect_ratio) {
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            if (!zox_valid(e2) || !zox_has(e2, CanvasPosition) || !zox_has(e2, Position2)) {
                continue;
            }
            zox_geter_value(e2, CanvasPosition, int2, position);
            zox_muter(e2, Position2, positionf);
            positionf->value = get_element_position(position, canvas_sizef, aspect_ratio);
            set_layout_child_position_recursively_new(world, e2, canvas_sizef, aspect_ratio);
        }
    }
}

// NOTE: Dirty flag only triggers on one entity, needs to propogate changes down the tree!
zox_sys2(LayoutToCanvasSystem) {
    byte is_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutPositionDirty);
    zox_sys_in(CanvasPosition);
    zox_sys_out(Position2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutPositionDirty, dirty);
        zox_sys_i(CanvasPosition, position);
        zox_sys_o(Position2, positionf);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        zox_geter_value(canvas, LayoutSize, int2, canvas_size);
        float2 canvas_sizef = int2_to_float2(canvas_size);
        float aspect_ratio = canvas_sizef.x / canvas_sizef.y;
        positionf->value = get_element_position(position->value, canvas_sizef, aspect_ratio);
        set_layout_child_position_recursively_new(world, e, canvas_sizef, aspect_ratio);
        if (is_log) {
            zox_log("-[%s] RPos [%fx%f] CPos [%ix%i]", zox_get_name(e), positionf->value.x, positionf->value.y, position->value.x, position->value.y);
        }
    }
} zox_sys_end(LayoutToCanvasSystem);
