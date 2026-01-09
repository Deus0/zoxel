zox_sys2(PlayerFlySystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLinks);
    zox_sys_in(DeviceMode);
    zox_sys_in(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CharacterLink, character_link);
        zox_sys_i(DeviceLinks, devices);
        zox_sys_i(DeviceMode, mode);

        const entity character = character_link->value;
        if (!zox_valid(character) || !zox_has(character, Character3)) {
            continue;
        }

        zox_geter_value(character, DisableMovement, byte, disabled);
        if (disabled) {
            continue;
        }

        zox_geter_value(character, FlyMode, byte, flying);
        if (!flying) {
            continue;
        }

        zox_geter_value(character, CameraLink, entity, camera);
        byte camera_mode = zox_valid(camera) ? zox_gett_value(camera, CameraState) : zox_camera_state_first_person;
        if (camera_mode == zox_camera_state_free) {
            continue;
        }

        float input = 0;

        for (int j = 0; j < devices->length; j++) {
            const entity device = devices->value[j];
            if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }
            if (mode->value == zox_device_mode_keyboardmouse && zox_has(device, Keyboard)) {
                zox_geter(device, Keyboard, keyboard)
                if (keyboard->q.is_pressed) input -= fly_run_acc;
                if (keyboard->e.is_pressed) input += fly_run_acc;
            }
        }

        if (input) {
            zox_muter(character, Acceleration3D, acc);
            acc->value.y += input;
        }
    }
} zox_sys_end(PlayerFlySystem);