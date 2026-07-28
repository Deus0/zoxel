static inline void set_position_rotation_scale_recursive(
    ecs* world,
    entity e,
    float3 parent_position,
    float4 parent_rotation,
    float3 parent_scale
) {
#ifdef zox_safety_checks
    if (!zox_valid(e)) {
        return;
    }
#endif
    if (zox_has(e, DisableTransform)) {
        return;
    }
    if (!zox_has(e, Position3D) || !zox_has(e, Rotation3D)) {
        return;
    }
    byte updated = 0;
    float3 local_position = zox_has(e, LocalPosition3D) ? zox_getv(e, LocalPosition3D) : float3_zero;
    float4 local_rotation = zox_has(e, LocalRotation3D) ? zox_getv(e, LocalRotation3D) : quaternion_identity;
    float3 local_scale = zox_has(e, LocalScale3) ? zox_getv(e, LocalScale3) : (zox_has(e, LocalScale1) ? float3_single(zox_getv(e, LocalScale1)) : float3_one);
    // world_position = parent_position + parent_rotation * (parent_scale * local_position)
    float3 world_position = float3_add(
        parent_position,
            float4_rotate_float3(parent_rotation,
                float3_multiply(local_position, parent_scale)));
    float4 world_rotation = quaternion_rotate(parent_rotation, local_rotation);
    float3 world_scale = zox_has(e, DisableParentScale) ?
        local_scale :
        float3_multiply(parent_scale, local_scale);
    zox_mut_begin(e, Position3D, old_position);
    if (!float3_equals(world_position, old_position->value))
    {
        old_position->value = world_position;
        updated = 1;
    }
    zox_mut_begin(e, Rotation3D, old_rotation);
    if (!float4_equals(world_rotation, old_rotation->value))
    {
        old_rotation->value = world_rotation;
        updated = 1;
    }
    if (zox_has(e, Scale3)) {
        zox_mut_begin(e, Scale3, scale);
        if (!float3_equals(scale->value, world_scale)) {
            scale->value = world_scale;
            updated = 1;
        }
    } else if (zox_has(e, Scale1)) {
        zox_mut_begin(e, Scale1, scale);
        if (scale->value != world_scale.y) {
            scale->value = world_scale.y;
            updated = 1;
            // zox_log("%s's Scale1 Set [%f]", zox_getn(e), world_scale.y);
        }
    }
    if (!updated) {
        // return;
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            set_position_rotation_scale_recursive(world, e2, world_position, world_rotation, world_scale);
        }
    }
}

// Uses flecs children parenting for recursively transformingthings
zox_sys2(TransformChildrenSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        float3 world_scale = zox_has(e, Scale3) ? zox_getv(e, Scale3) : (zox_has(e, Scale1) ? float3_single(zox_getv(e, Scale1)) : float3_one);
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                set_position_rotation_scale_recursive(world, e2, position->value, rotation->value, world_scale);
            }
        }
        zox_sys_increment();
    }
} zox_sys_end(TransformChildrenSystem);
