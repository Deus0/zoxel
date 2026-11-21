// an observer on AttachDirty (a one-off event, removed here)
void camera_attach_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Target);
    for (int i = 0; i < it->count; i++) {
        zox_remove(it->entities[i], AttachDirty);
        zox_sys_e();
        zox_sys_i(Target, target);
        zox_setv(e, CameraState, zox_camera_state_first_person);
        zox_set_parent(world, e, target->value);
    }
}
