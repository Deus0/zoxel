void set_position_rotation_recursive(ecs* world, entity e, float3 pposition, float4 protation) {
    if (!zox_valid(e) || !zox_has(e, Position3D) || !zox_has(e, Rotation3D)) {
        return;
    }
    float3 localp = zox_has(e, LocalPosition3D) ? zox_gett_value(e, LocalPosition3D) : float3_zero;
    float4 localr = zox_has(e, LocalRotation3D) ? zox_gett_value(e, LocalRotation3D) : quaternion_identity;
    zox_muter(e, Position3D, nposition);
    zox_muter(e, Rotation3D, nrotation);
    nrotation->value = protation;
    nposition->value = localp;
    float4_rotate_float3_p(nrotation->value, &nposition->value);
    float3_add_float3_p(&nposition->value, pposition);
    quaternion_rotate_quaternion_p(&nrotation->value, localr);
    /*entity children[transforms3_children_capacity];
    uint count = zox_get_children(world, e, children, transforms3_children_capacity);
    for (uint j = 0; j < count; j++) {
        entity e2 = children[j];*/
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            set_position_rotation_recursive(world, e2, nposition->value, nrotation->value);
        }
    }
}

// Uses flecs children parenting for recursively transformingthings
zox_sys2(PositionRotation3System) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        /*entity children[transforms3_children_capacity];
        uint count = zox_get_children(world, e, children, transforms3_children_capacity);
        for (uint j = 0; j < count; j++) {
            entity e2 = children[j];*/
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                set_position_rotation_recursive(world, e2, position->value, rotation->value);
            }
        }
    }
} zox_sys_end(PositionRotation3System);
