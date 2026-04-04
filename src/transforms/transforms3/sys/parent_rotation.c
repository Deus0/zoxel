void set_rotation_from_parents(ecs *world, entity parent, float4 *rotation3D, float4 local_rotation3D) {
    if (!parent || !zox_has(parent, Rotation3D)) {
        return;
    }
    float4 parent_rotation = zox_get_value(parent, Rotation3D)
    *rotation3D = parent_rotation;
    quaternion_rotate_quaternion_p(rotation3D, local_rotation3D);
}

zox_sys2(ParentRotationSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ParentLink);
    zox_sys_in(LocalRotation3D);
    zox_sys_out(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ParentLink, parentLink);
        zox_sys_i(LocalRotation3D, localRotation3D);
        zox_sys_o(Rotation3D, rotation3D);

        if (!zox_valid(parentLink->value)) {
            continue;
        }

        set_rotation_from_parents(world, parentLink->value, &rotation3D->value, localRotation3D->value);
    }
} zox_sys_end(ParentRotationSystem);
