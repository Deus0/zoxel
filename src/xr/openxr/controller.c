// A headset's controllers into each XR controller's zevices and pose (fun/controller.c), as the inputs are read
//  - syncs OpenXR's actions once a frame, then each hand's stick, buttons and aim pose
//  - the pose: the aim, where the hand points (rays at 3D ui), at this frame's predicted display time (the frame
//    begins before the inputs, openxr/render.c), the same time the eyes draw at
static byte xr_openxr_button(byte hand, byte type) {
    switch (type) {
        case zox_btn_a:
        case zox_btn_x:
            return xr_get_bool(primary_action, hand) ||
                (type == zox_btn_a && xr_get_float(trigger_action, hand) >= xr_trigger_cutoff);
        case zox_btn_b:
        case zox_btn_y:
            return xr_get_bool(secondary_action, hand);
        // pause: the menu button, or Y (a headset's menu button is often the system's, unseen by the app)
        case zox_btn_start: return xr_get_bool(menu_action, hand) || (!hand && xr_get_bool(secondary_action, hand));
        case zox_btn_lb:
        case zox_btn_rb:
            return xr_get_float(grip_action, hand) >= xr_trigger_cutoff;
        case zox_btn_lt:
        case zox_btn_rt:
            return xr_get_float(trigger_action, hand) >= xr_trigger_cutoff;
        default: return 0;
    }
}

void xr_openxr_controllers_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    if (!xr_session_running || !xr_update_input()) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(XrHand);
    zox_sys_out(DeviceHasInput);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(XrHand, hand);
        zox_sys_o(DeviceHasInput, has_input);
        const byte right = hand->value == xr_hand_right;
        const float2 stick = right ? xr_get_right_stick() : xr_get_left_stick();
        byte any = stick.x != 0 || stick.y != 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                const entity e2 = it2.entities[j];
                if (zox_has(e2, ZeviceStick)) {
                    xr_controller_stick_set(world, e2, stick);
                } else if (zox_has(e2, ZeviceButton)) {
                    const byte held = xr_openxr_button(right, zox_getv(e2, DeviceButtonType));
                    xr_controller_button(world, e2, held);
                    any |= held;
                }
            }
        }
        has_input->value = any;
        XrPosef pose;
        if (xr_frame_begun && xr_get_aim(right, xr_frame_state.predictedDisplayTime, &pose)) {
            xr_controller_pose(world, e, xr_vector_to_float3(pose.position), xr_quaternion_to_float4(pose.orientation));
        }
    }
    zox_sys_on_end();
} zoxd_system(xr_openxr_controllers_system);
