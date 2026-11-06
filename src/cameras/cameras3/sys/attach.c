zox_sys2(CameraAttachSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(EntityTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(EntityTarget, target);
        zox_setv(e, CameraState, zox_camera_state_first_person);
        zox_set_parent(world, e, target->value);
    }
} zox_sys_end(CameraAttachSystem);
