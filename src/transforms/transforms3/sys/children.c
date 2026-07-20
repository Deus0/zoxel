static inline void set_position_rotation_recursive(ecs* world, entity e, float3 parent_position, float4 parent_rotation) {
#ifdef zox_safety_checks
    if (!zox_valid(e)) {
        return;
    }
#endif
    if (!zox_has(e, Position3D) || !zox_has(e, Rotation3D) || zox_has(e, DisableTransform)) {
        return;
    }
    byte updated = 0;
    float3 local_position = zox_has(e, LocalPosition3D) ? zox_getv(e, LocalPosition3D) : float3_zero;
    float4 local_rotation = zox_has(e, LocalRotation3D) ? zox_getv(e, LocalRotation3D) : quaternion_identity;
    float3 new_position = local_position;
    float4 new_rotation = parent_rotation;
    float4_rotate_float3_p(new_rotation, &new_position);
    float3_add_float3_p(&new_position, parent_position);
    quaternion_rotate_quaternion_p(&new_rotation, local_rotation);
    zox_mut_begin(e, Position3D, old_position);
    // if (!float3_equals(new_position, old_position->value))
    {
        old_position->value = new_position;
        // zox_mut_end(e, Position3D);
        updated = 1;
    }
    zox_mut_begin(e, Rotation3D, old_rotation);
    // if (!float4_equals(new_rotation, old_rotation->value))
    {
        old_rotation->value = new_rotation;
        // zox_mut_end(e, Rotation3D);
        updated = 1;
    }
    if (!updated) {
        return;
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            set_position_rotation_recursive(world, e2, new_position, new_rotation);
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
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                set_position_rotation_recursive(world, e2, position->value, rotation->value);
            }
        }
        zox_sys_increment();
    }
} zox_sys_end(TransformChildrenSystem);
