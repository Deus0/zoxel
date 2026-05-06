void set_camera_locked(ecs *world, entity e, entity target) {

    zox_set(e, CameraState, { zox_camera_state_first_person });
    zox_set(e, Roaming, { 0 });
    zox_set(e, ParentLink, { target });

    // zox_remove(e, EulerOverride);

    /*if (camera_follow_mode == zox_camera_follow_mode_attach) {

    } else if (camera_follow_mode == zox_camera_follow_mode_follow_xz) {
        zox_set(e, CameraFollowLink, { target });
    }*/

    // zox_set(camera, EternalRotation, { quaternion_identity })
    // set_camera_transform(world, e, target, zox_camera_state_first_person);
}

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

        set_camera_locked(world, e, target->value);
    }
} zox_sys_end(CameraAttachSystem);
