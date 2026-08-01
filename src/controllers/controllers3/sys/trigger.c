// TODO: Move this to Triggers Module
// TODO: Make use hotkeys instead, link hotkeys to actions
// TODO: Refactor the DisableMovement checks for a and b

// NOTE: Player Input -> Character Triggers
zox_sys2(Player3DTriggerSystem) {
    if (zox_dbg_touch_with_mouse) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(CharacterLink);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(CharacterLink, character_link);
        zox_sys_i(CameraLink, camera);
        if (state->value != zox_player_state_playing) {
            continue;
        }
        entity character = character_link->value;
        if (!zox_valid(character) || !zox_has(character, Character3) || !zox_valid(camera->value)) {
            continue;
        }
        if (zox_has(character, DisableMovement)) {
            continue;
        }
        byte camera_state = zox_getv(camera->value, CameraState);
        if (camera_state != zox_camera_state_first_person) {
            continue;
        }
        byte is_triggered_a = 0;
        byte is_triggered_b = 0;
        byte is_triggered_e = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!zox_valid(e2) || !zox_has(e2, DeviceDisabled) || zox_gett_value(e2, DeviceDisabled)) {
                continue;
            }
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(world, e2, children, children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (!zox_valid(e3)) {
                    continue;
                }
                zox_geter_value(e3, ZeviceDisabled, byte, disabled);
                if (disabled) {
                    continue;
                }
                if (zox_has(e3, ZevicePointer)) {
                    zox_geter_value(e3, ZevicePointer, byte, click);
                    // if (devices_get_pressed_this_frame(click)) {
                    if (devices_get_pressed(click)) {
                        is_triggered_a = 1;
                    }
                    // Mouse Pointer has this also
                    if (zox_has(e3, ZevicePointerRight)) {
                        zox_geter_value(e3, ZevicePointerRight, byte, click);
                        // if (devices_get_pressed_this_frame(click)) {
                        if (devices_get_pressed_this_frame(click)) {
                            is_triggered_b = 1;
                        }
                    }
                } else if (zox_has(e3, ZeviceButton)) {
                    zox_geter_value(e3, DeviceButtonType, byte, type);
                    if (type == zox_btn_rt) {
                        zox_geter_value(e3, ZeviceButton, byte, value);
                        if (devices_get_pressed_this_frame(value)) {
                            is_triggered_a = 1;
                        }
                    } else if (type == zox_btn_lt) {
                        zox_geter_value(e3, ZeviceButton, byte, value);
                        if (devices_get_pressed_this_frame(value)) {
                            is_triggered_b = 1;
                        }
                    }
                }
            }
        }
        if (is_triggered_e && !zox_getv(character, TriggerActionE)) {
            zox_set(character, TriggerActionE, { zox_dirty_trigger });
        }
        // used to be here, whats trigger e? idk
        if (is_triggered_a && !zox_gett_value(character, TriggerActionA)) {
            zox_set(character, TriggerActionA, { zox_dirty_trigger });
        }
        if (is_triggered_b && !zox_gett_value(character, TriggerActionB)) {
            zox_set(character, TriggerActionB, { zox_dirty_trigger });
        }
    }
} zox_sys_end(Player3DTriggerSystem);
