// First test just copy demo out and put our render system in



// NOTE:
void xr_frame_system(iter* it) {
    byte dbg_log = 0;
    /// This is the basic flow
    /*if (!rendering || !session_running) {
        return;
    }
    if (!xr_begin_frame()) {
        return;
    }
    if (!xr_get_eyes()) {
        xr_abort_frame();
        return;
    }/
    xr_prepare_cameras();
    ecs_run(world, xr_camera_render_system, 0, NULL);
    xr_finish_frame();*
    /*if (!rendering) {
        return;
    }
    if (!frame_state.shouldRender) {
        return;
    }*/
    for (int i = 0; i < 2; i++) {
        entity camera = xr_cameras[i];
        guint fbo = zox_getv(camera, FrameBufferLink);
        guint rbo = zox_getv(camera, FrameBufferLink);
        xr_eye_render(
            xr_swapchains[i],
            images[i],
            eye_left_position,
            eye_left_rotation,
            eye_fov_left,
            // TODO: Swap this to use PixelSize
            xr_config_to_image_size(views_cfg[i]),
            fbo,
            rbo
        );
    }
    // hmmm
    xr_end_frame(
        eye_left_position,
        eye_right_position,
        eye_left_rotation,
        eye_right_rotation,
        eye_fov_left,
        eye_fov_right);
} zoxd_system(xr_frame_system);