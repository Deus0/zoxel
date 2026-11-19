void camera_attach_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Target);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Target, target);
        zox_setv(e, CameraState, zox_camera_state_first_person);
        zox_set_parent(world, e, target->value);
    }
    zox_sys_on_end();
} zoxd_system(camera_attach_system);
