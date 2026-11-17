// A headset's frame: OpenXR's events and frame, free roam from its sticks and head, each eye into its swapchain
//  - runs in zoxp_render, after the transforms; no need to time it, it runs the render systems inside it
//  - the ui is 3D panels (huds while zox_huds3D), drawn with the other 3D render systems
void xr_camera_render_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    byte running = 1;
    if (!xr_can_render(&running)) {
        if (!running) {
            zox_log("[XR] session ended, ending the game");
            engine_end();
        }
        return;
    }
    xr_get_eyes();
    xr_update_time();
    // the sticks drive the menus while one's open (fun/controller.c), the actions synced as the inputs were read
    const byte roaming = xr_roam_input_enabled(world);
    const float2 move = roaming ? xr_controller_stick(world, xr_hand_left) : float2_zero;
    const float2 turn = roaming ? xr_controller_stick(world, xr_hand_right) : float2_zero;
    xr_roam_update(world, xr_camera, move, turn, xr_head_rotation, delta_time);
    const float4x4 body = xr_roam_body_matrix();
    glFrontFace(GL_CW);     // y flipped into the swapchain images (fun/eye_render.c), so the winding too
    for (int i = 0; i < 2; i++) {
        const xr_eye eye = xr_eyes[i];
        if (!xr_eye_render_begin(eye.swapchain, eye.images, eye.fbo)) {
            zox_loge("[XR] xr_eye_render_begin failed");
            continue;
        }
        xr_render_eye(world, eye.matrix, eye.fov, body, float2_zero, int2_to_float2(eye.image_size), 1);
        xr_eye_render_end(eye.swapchain);
    }
    glFrontFace(GL_CCW);
    xr_end_frame();
    zox_sys_on_end();
} zoxd_system(xr_camera_render_system);

void zox_systems_openxr(ecs* world) {
    zox_system_1(
        xr_openxr_controllers_system,
        zoxp_inputs_extract,
        [in] xr.XrHand,
        [out] inputs.DeviceHasInput,
        [none] xr.XrController
    );
    zox_system_1(xr_camera_render_system, zoxp_render, 0);
}
