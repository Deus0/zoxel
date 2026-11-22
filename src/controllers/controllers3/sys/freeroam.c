void set_camera_free(
    ecs *world,
    entity e)
{
    // its character's look held: the body keeps its pose, to be seen from outside (bones LookHeld)
    const entity head = zox_get_parent(world, e);
    const entity skeleton = zox_valid(head) && zox_has(head, Bone) ? zox_get_link(world, head, SkeletonLink) : 0;
    if (zox_valid(skeleton)) {
        zox_set(skeleton, LookHeld, { zox_getv(e, LocalRotation3D) });
        zox_set(skeleton, LookHeldEye, { zox_getv(e, LocalPosition3D) });
    }
    float4 rotation = zox_getv(e, Rotation3D);
    float3 euler = quaternion_to_euler(rotation);
    zox_add(e, EulerOverride);
    zox_set(e, Euler, { euler });
    zox_set_parent(world, e, 0);
    if (camera_follow_mode == zox_camera_follow_mode_follow_xz) {
        zox_set_link(world, e, CameraFollowLink, 0);
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
        zox_loge("Invalid Character [attach_camera_to_character]");
        return;
    }
    float3 euler = (float3) { 0, 180, 0 };
    // Initial Linking
    zox_link(world, e, CharacterLink, character);
    zox_link(world, character, CameraLink, e);
    // reset using head bone
    zox_remove(e, EulerOverride);
    zox_setv(e, CameraState, zox_camera_state_first_person);
    zox_setv(e, Euler, euler);
    zox_setv(e, LocalRotation3D,
        quaternion_from_euler(float3_scale(euler, degreesToRadians)));
    // set_camera_locked(world, e, character);
    // TODO: Add CameraDirty to Character for headbone adjustment system
    entity skeleton = get_skeleton(world, character);
    if (zox_valid(skeleton)) {
        zox_add(skeleton, SkeletonDirty);
        if (zox_has(skeleton, LookHeld)) {
            zox_remove(skeleton, LookHeld);     // its look the camera's again
            zox_remove(skeleton, LookHeldEye);
        }
    }
    if (local_mouse) {
        zox_setv(local_mouse, MouseLock, 1);
    }
    zox_set_parent(world, e, character);
}

void free_roam_toggle_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (state->value != zox_player_state_playing) {
            // continue;
        }
        entity camera = zox_get_link(world, e, CameraLink);
        if (!zox_valid(camera)) {
            continue;
        }
        byte is_toggle = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (zox_has(e2, Disabled)) {
                continue;
            }
            /*uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(world, e2, children, children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (zox_has(e3, Disabled)) {
                    continue;
                }
                if (zox_has(e3, ZeviceButton)) {
                    const DeviceButtonType* type = zox_get(e3, DeviceButtonType);
                    if (type->value == zox_btn_right_stick_push) {
                        byte button = zox_getv(e3, ZeviceButton);
                        if (devices_get_pressed_this_frame(button)) {
                            is_toggle_camera = 1;
                            break;
                        }
                    }
                }
            }*/
            if (zox_has(e2, Keyboard)) {
                const Keyboard* keyboard = zox_get(e2, Keyboard);
                if (keyboard->f1.pressed_this_frame) {
                    is_toggle = 1;
                    break;
                }
            }
        }
        if (is_toggle) {
            entity character = zox_get_link(world, e, CharacterLink);
            byte old = zox_getv(camera, CameraState);
            byte new = old;
            if (old == zox_camera_state_free) {
                new = zox_camera_state_first_person;
                zox_remove(camera, Roaming);
                attach_camera_to_character(
                    world,
                    camera,
                    character);
            } else {
                new = zox_camera_state_free;
                set_camera_free(
                    world,
                    camera);
            }
            zox_setv(camera, CameraState, new);
            // zox_log("= toggling free roam - old: [%i] -> new: [%i]", old, state->value);
        }
    }
    zox_sys_on_end();
} zoxd_system(free_roam_toggle_system);
