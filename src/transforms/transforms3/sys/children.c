/*void set_position_recursive(ecs* world, entity e, float3 pposition, float4 protation) {

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

    // If has children, keep going
    if (!zox_has(e, Children)) {
        return;
    }

    // float4 rotation = zox_has(e, Rotation3D) ? zox_gett_value(e, Rotation3D) : quaternion_identity;

    zox_geter(e, Children, children);
    for (int j = 0; j < children->length; j++) {
        entity e2 = children->value[j];

        set_position_recursive(world, e2, nposition->value, nrotation->value);
    }
}

// TODO: rotation as well recursively at same time
zox_sys2(ChildrenPositionSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(Children);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Children, children);

        for (int j = 0; j < children->length; j++) {
            entity e2 = children->value[j];

            set_position_recursive(world, e2, position->value, rotation->value);
        }
    }
} zox_sys_end(ChildrenPositionSystem);*/
