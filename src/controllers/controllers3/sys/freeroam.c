void set_camera_free(
    ecs *world,
    entity e)
{
    // zox_set(e, CharacterLink, { 0 });
    float4 rotation = zox_getv(e, Rotation3D);
    float3 euler = quaternion_to_euler(rotation);
    zox_add(e, EulerOverride);
    zox_set(e, Euler, { euler });
    // zox_set(e, ParentLink, { 0 });
    zox_set_parent(world, e, 0);
    if (camera_follow_mode == zox_camera_follow_mode_follow_xz) {
        zox_setv(e, CameraFollowLink, 0)
    }
    if (local_mouse) {
        zox_setv(local_mouse, MouseLock, 0);
    }
    zox_add(e, CanRoam);
}

void attach_camera_to_character(
    ecs *world,
    entity e,
    entity character)
{
    if (zox_has(e, CanRoam)) {
        zox_remove(e, CanRoam);
    }
    if (!zox_valid(character)) {
        zox_log_error("Invalid Character [attach_camera_to_character]");
        return;
    }
    float3 euler = (float3) { 0, 180, 0 };
    // Initial Linking
    zox_setv(e, CharacterLink, character);
    zox_setv(character, CameraLink, e);
    // reset using head bone
    zox_remove(e, EulerOverride);
    zox_setv(e, CameraState, zox_camera_state_first_person);
    zox_setv(e, Euler, euler);
    zox_setv(e, LocalRotation3D,
        quaternion_from_euler(float3_scale(euler, degreesToRadians)));
    // set_camera_locked(world, e, character);
    // TODO: Add CameraDirty to Character for headbone adjustment system
    zox_setv(character, SkeletonDirty, zox_dirty_trigger);
    if (local_mouse) {
        zox_setv(local_mouse, MouseLock, 1);
    }
    zox_set_parent(world, e, character);
}

zox_sys2(FreeRoamToggleSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(CharacterLink);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(CharacterLink, character);
        zox_sys_i(CameraLink, camera);
        if (state->value != zox_player_state_playing) {
            // continue;
        }
        if (!zox_valid(camera->value)) {
            continue;
        }
        byte is_toggle = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!zox_valid(e2) || !zox_has(e2, DeviceDisabled)) {
                continue;
            }
            if (zox_getv(e2, DeviceDisabled)) {
                continue;
            }
            /*uint children_capacity = zox_children_capacity;
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
                if (zox_has(e3, ZeviceButton)) {
                    zox_geter(e3, DeviceButtonType, type);
                    if (type->value == zox_btn_right_stick_push) {
                        zox_geter_value(e3, ZeviceButton, byte, button);
                        if (devices_get_pressed_this_frame(button)) {
                            is_toggle_camera = 1;
                            break;
                        }
                    }
                }
            }*/
            if (zox_has(e2, Keyboard)) {
                zox_geter(e2, Keyboard, keyboard);
                if (keyboard->f1.pressed_this_frame) {
                    is_toggle = 1;
                    break;
                }
            }
        }
        if (is_toggle) {
            byte old = zox_getv(camera->value, CameraState);
            byte new = old;
            if (old == zox_camera_state_free) {
                new = zox_camera_state_first_person;
                zox_remove(camera->value, Roaming);
                attach_camera_to_character(
                    world,
                    camera->value,
                    character->value);
            } else {
                new = zox_camera_state_free;
                set_camera_free(
                    world,
                    camera->value);
            }
            zox_setv(camera->value, CameraState, new);
            // zox_log("= toggling free roam - old: [%i] -> new: [%i]", old, state->value);
        }
    }
} zox_sys_end(FreeRoamToggleSystem);
