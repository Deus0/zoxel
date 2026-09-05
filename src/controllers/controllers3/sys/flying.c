// NOTE: Allows player to fly up or down when flymode is active
zox_sys2(PlayerFlySystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(DeviceMode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(DeviceMode, mode);
        if (state->value != zox_player_state_playing) {
            continue;
        }
        entity character = zox_get_link(world, e, Character);
        if (!zox_valid(character) || !zox_has(character, Character3)) {
            continue;
        }
        if (zox_has(character, DisableMovement)) {
            continue;
        }
        zox_geter_value(character, FlyMode, byte, flying);
        if (!flying) {
            continue;
        }
        entity camera = zox_get_link(world, character, Camera);
        byte camera_mode = zox_valid(camera) ?
            zox_getv(camera, CameraState) :
            zox_camera_state_first_person;
        if (camera_mode == zox_camera_state_free) {
            continue;
        }
        float input = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
        //for (int j = 0; j < devices->length; j++) {
        //    entity device = devices->value[j];
            if (!zox_valid(e2) || zox_getv(e2, DeviceDisabled)) {
                continue;
            }
            if (mode->value == zox_device_mode_keyboardmouse && zox_has(e2, Keyboard)) {
                zox_geter(e2, Keyboard, keyboard);
                if (keyboard->q.is_pressed) input -= fly_run_acc_y;
                if (keyboard->e.is_pressed) input += fly_run_acc_y;
            }
        }
        if (input) {
            zox_muter(character, Acceleration3D, acc);
            acc->value.y += input;
        }
    }
} zox_sys_end(PlayerFlySystem);
