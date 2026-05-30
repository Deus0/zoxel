zox_sys2(CameraAttachSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(AttachDirty);
    zox_sys_in(EntityTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(AttachDirty, state);
        zox_sys_i(EntityTarget, target);
        if (state->value != zox_dirty_active) {
            continue;
        }
        zox_set(e, CameraState, { zox_camera_state_first_person });
        zox_set(e, Roaming, { 0 });
        zox_set_parent(world, e, target->value);
    }
} zox_sys_end(CameraAttachSystem);
