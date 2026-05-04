/*
zox_sys2(ParentRotationSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ParentLink);
    zox_sys_in(LocalRotation3D);
    zox_sys_out(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ParentLink, parent);
        zox_sys_i(LocalRotation3D, localRotation3D);
        zox_sys_o(Rotation3D, rotation3D);

        if (!zox_valid(parent->value)) {
            continue;
        }

        set_rotation_from_parents(world, parent->value, &rotation3D->value, localRotation3D->value);
    }
} zox_sys_end(ParentRotationSystem);*/
