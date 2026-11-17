// A headset's frame: begun first thing (xr_frame_begin_system), free roam from its sticks and head, each eye into its
//  swapchain, then ended
//  - the begin waits for the headset (xrWaitFrame) at the top of the frame, so the frame's poses are this frame's
//  - the render runs in zoxp_render, after the transforms; it runs the render systems inside it
//  - the eyes are located last thing before drawing, the freshest head for the display time
//  - the ui is 3D panels (huds while zox_huds3D), drawn with the other 3D render systems
// frame timing, logged every 2s (logcat, tag zoxel): where a headset's frame goes, to find what's slow
//  - wait: in xrWaitFrame (the runtime pacing us: high is fine, we're early), sim: the engine's frame between the
//    wait and the eyes, eyes: drawing both, late: frames whose display time skipped past one (dropped frames)
static double xr_timing_wait = 0, xr_timing_sim = 0, xr_timing_eyes = 0, xr_timing_since = 0, xr_timing_mark = 0;
static int xr_timing_frames = 0, xr_timing_late = 0;
static XrTime xr_timing_last_display = 0;

static inline double xr_timing_now() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

static void xr_timing_frame_end(double eyes_start) {
    const double now = xr_timing_now();
    xr_timing_eyes += now - eyes_start;
    xr_timing_frames++;
    const XrTime display = xr_frame_state.predictedDisplayTime;
    const XrTime period = xr_frame_state.predictedDisplayPeriod;
    if (xr_timing_last_display && period > 0 && display - xr_timing_last_display > period + period / 2) {
        xr_timing_late++;
    }
    xr_timing_last_display = display;
    if (!xr_timing_since) {
        xr_timing_since = now;
    }
    if (now - xr_timing_since >= 2) {
        const float n = (float) xr_timing_frames;
        zox_log("[XR] fps [%.1f] (display %.1f hz) ms: wait [%.2f] sim [%.2f] eyes [%.2f], late frames [%i]",
            n / (now - xr_timing_since), period > 0 ? 1e9 / (double) period : 0,
            1000 * xr_timing_wait / n, 1000 * xr_timing_sim / n, 1000 * xr_timing_eyes / n, xr_timing_late);
        xr_timing_wait = xr_timing_sim = xr_timing_eyes = 0;
        xr_timing_frames = xr_timing_late = 0;
        xr_timing_since = now;
    }
}

void xr_frame_begin_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    byte running = 1;
    const double wait_start = xr_timing_now();
    const byte begun = xr_frame_start(&running);
    xr_timing_mark = xr_timing_now();
    xr_timing_wait += xr_timing_mark - wait_start;
    if (begun && xr_get_eyes()) {
        // the head for this frame's body (the head bone), located again just before the eyes draw
        xr_head_now_rotation = xr_head_rotation;
        xr_head_now_position = xr_head_position;
        xr_cull_set(world, xr_eyes[0].fov, xr_eyes[1].fov);
    }
    if (!running) {
        zox_log("[XR] session ended, ending the game");
        engine_end();
    }
    zox_sys_on_end();
} zoxd_system(xr_frame_begin_system);

void xr_camera_render_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    if (!xr_frame_begun) {
        return;
    }
    const double eyes_start = xr_timing_now();
    xr_timing_sim += eyes_start - xr_timing_mark;
    xr_update_time();
    if (!xr_get_eyes()) {
        xr_end_frame();
        return;
    }
    // the sticks drive the menus while one's open (fun/controller.c), the actions synced as the inputs were read
    const byte roaming = xr_roam_input_enabled(world);
    const float2 move = roaming ? xr_controller_stick(world, xr_hand_left) : float2_zero;
    const float2 turn = roaming ? xr_controller_stick(world, xr_hand_right) : float2_zero;
    xr_roam_update(world, xr_camera, move, turn, xr_head_rotation, xr_head_position, delta_time);
    if (xr_frame_state.shouldRender) {
        const float4x4 body = xr_view_body;
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
    }
    xr_end_frame();
    xr_timing_frame_end(eyes_start);
    zox_sys_on_end();
} zoxd_system(xr_camera_render_system);

void zox_systems_openxr(ecs* world) {
    // first, before the inputs (zoxp_inputs_extract) read the controllers at its display time
    zox_system_1(xr_frame_begin_system, zoxp_begin, 0);
    zox_system_1(
        xr_openxr_controllers_system,
        zoxp_inputs_extract,
        [in] xr.XrHand,
        [out] inputs.DeviceHasInput,
        [none] xr.XrController
    );
    zox_system_1(xr_camera_render_system, zoxp_render, 0);
}
