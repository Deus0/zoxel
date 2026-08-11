static inline void set_position_rotation_scale2_recursive(
    ecs* world,
    entity e,
    float2 parent_position,
    float parent_rotation,
    float2 parent_scale,
    byte dbg_log
) {
    #ifdef zox_safety_checks
    if (!zox_valid(e)) {
        return;
    }
    #endif
    if (zox_has(e, DisableTransform) || zox_has(e, DisableParentTransform)) {
        return;
    }
    if (zox_has(e, LinePoints2) && zox_has(e, LocalLinePoints2)) {
        const LocalLinePoints2* local_line = zox_get(e, LocalLinePoints2);
        zox_mut_begin(e, LinePoints2, line);
        line->start = float2_add(
            parent_position,
            float2_rotate(
                float2_multiply(local_line->start, parent_scale),
                    parent_rotation));
        line->end = float2_add(
            parent_position,
            float2_rotate(
                float2_multiply(local_line->end, parent_scale),
                    parent_rotation));
        if (dbg_log) {
            zox_log("[%s] line [%fx%f] -> [%fx%f]", zox_getn(e), line->start.x, line->start.y, line->end.x, line->end.y);
        }
    }
    if (!zox_has(e, Position2)) {
        return;
    }
    byte updated = 0;
    float2 local_position = zox_has(e, LocalPosition2) ? zox_getv(e, LocalPosition2) : float2_zero;
    float local_rotation = zox_has(e, LocalRotation2) ? zox_getv(e, LocalRotation2) : 0;
    float2 local_scale = zox_has(e, LocalScale2) ? zox_getv(e, LocalScale2) : (zox_has(e, LocalScale1) ? float2_single(zox_getv(e, LocalScale1)) : float2_one);
    // world_position = parent_position + parent_rotation * (parent_scale * local_position)
    float2 world_position = float2_add(
        parent_position,
            float2_rotate(
                float2_multiply(local_position, parent_scale), parent_rotation));
    float world_rotation = parent_rotation + local_rotation;
    float2 world_scale = float2_multiply(parent_scale, local_scale);
    zox_mut_begin(e, Position2, old_position);
    if (!float2_equals(world_position, old_position->value))
    {
        old_position->value = world_position;
        updated = 1;
        if (dbg_log) {
            zox_log("New Position2 [%fx%f]", world_position.x, world_position.y);
        }
    }
    if (zox_has(e, Rotation2)) {
        zox_mut_begin(e, Rotation2, old_rotation);
        if (world_rotation != old_rotation->value) {
            old_rotation->value = world_rotation;
            updated = 1;
        }
    }
    if (zox_has(e, Scale2)) {
        zox_mut_begin(e, Scale2, scale);
        if (!float2_equals(scale->value, world_scale)) {
            scale->value = world_scale;
            updated = 1;
        }
    } else if (zox_has(e, Scale1)) {
        zox_mut_begin(e, Scale1, scale);
        if (scale->value != world_scale.y) {
            scale->value = world_scale.y;
            updated = 1;
        }
    }
    if (!updated) {
        // return;
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            set_position_rotation_scale2_recursive(world, e2, world_position, world_rotation, world_scale, dbg_log);
        }
    }
}

// Uses flecs children parenting for recursively transformingthings
zox_sys2(Transform2ChildrenSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position2);
    // zox_sys_in(Rotation2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position2, position);
        // zox_sys_i(Rotation2, rotation);
        float rotation = zox_has(e, Rotation2) ? zox_getv(e, Rotation2) : 0;
        float2 world_scale = zox_has(e, Scale2) ? zox_getv(e, Scale2) : (zox_has(e, Scale1) ? float2_single(zox_getv(e, Scale1)) : float2_one);
        if (dbg_log) {
            zox_log("Root Position2 [%fx%f]", position->value.x, position->value.y);
        }
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                set_position_rotation_scale2_recursive(world, e2, position->value, rotation, world_scale, dbg_log);
            }
        }
        zox_sys_increment();
    }
} zox_sys_end(Transform2ChildrenSystem);
