// A headset's runtime (fun/runtime.c): OpenXR's frame, inputs and eyes
//  - the frame begins first thing (xrWaitFrame, zoxp_begin), so the frame's poses (the controllers', read in
//    zoxp_inputs_extract) are predicted for this frame's display time, the same time the eyes draw at
//  - the eyes are located last thing before drawing, the freshest head for the display time
//  - every begun frame is ended, with no layers when the runtime says not to render (openxr/frames.c)
//  - the inputs: OpenXR's actions synced once a frame, then each hand's stick, buttons, aim and grip poses
//  - a swapchain image is read top down, y flipped (fun/eye_render.c), its triangles clockwise
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

static double xr_timing_eyes_start = 0;

static byte xr_openxr_frame_begin(ecs* world, byte* running) {
    (void) world;
    const double wait_start = xr_timing_now();
    const byte begun = xr_frame_start(running);
    xr_timing_mark = xr_timing_now();
    xr_timing_wait += xr_timing_mark - wait_start;
    return begun;
}

static byte xr_openxr_locate(ecs* world, float4* head_rotation, float3* head_position, xr_eye_view eyes[2]) {
    (void) world;
    if (!xr_frame_begun || !xr_get_eyes()) {
        return 0;
    }
    *head_rotation = xr_head_rotation;
    *head_position = xr_head_position;
    for (byte i = 0; i < 2; i++) {
        eyes[i] = (xr_eye_view) {
            .matrix = xr_eyes[i].matrix,
            .fov = xr_eyes[i].fov,
            .viewport_position = float2_zero,
            .viewport_size = int2_to_float2(xr_eyes[i].image_size),
        };
    }
    return 1;
}

static byte xr_openxr_inputs(ecs* world) {
    (void) world;
    return xr_session_running && xr_update_input();
}

static float2 xr_openxr_stick(ecs* world, byte hand) {
    (void) world;
    return hand == xr_hand_right ? xr_get_right_stick() : xr_get_left_stick();
}

static byte xr_openxr_button(ecs* world, byte hand, byte type) {
    (void) world;
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

static byte xr_openxr_pose(byte hand, byte grip, float3* position, float4* rotation) {
    XrPosef pose;
    if (!xr_frame_begun) {
        return 0;
    }
    const XrTime time = xr_frame_state.predictedDisplayTime;
    if (!(grip ? xr_get_grip(hand, time, &pose) : xr_get_aim(hand, time, &pose))) {
        return 0;
    }
    *position = xr_vector_to_float3(pose.position);
    *rotation = xr_quaternion_to_float4(pose.orientation);
    return 1;
}

static byte xr_openxr_aim(ecs* world, byte hand, float3* position, float4* rotation) {
    (void) world;
    return xr_openxr_pose(hand, 0, position, rotation);
}

static byte xr_openxr_grip(ecs* world, byte hand, float3* position, float4* rotation) {
    (void) world;
    return xr_openxr_pose(hand, 1, position, rotation);
}

// the render's start: the frame's time step from its display times
static float xr_openxr_delta(ecs* world) {
    (void) world;
    xr_timing_eyes_start = xr_timing_now();
    xr_timing_sim += xr_timing_eyes_start - xr_timing_mark;
    xr_update_time();
    return delta_time;
}

static byte xr_openxr_should_render(ecs* world) {
    (void) world;
    return xr_frame_state.shouldRender;
}

static byte xr_openxr_eye_begin(ecs* world, byte i) {
    (void) world;
    const xr_eye eye = xr_eyes[i];
    if (!xr_eye_render_begin(eye.swapchain, eye.images, eye.fbo)) {
        zox_loge("[XR] xr_eye_render_begin failed");
        return 0;
    }
    glFrontFace(GL_CW);     // y flipped into the swapchain images (fun/eye_render.c), so the winding too
    return 1;
}

static void xr_openxr_eye_end(ecs* world, byte i) {
    (void) world;
    glFrontFace(GL_CCW);
    xr_eye_render_end(xr_eyes[i].swapchain);
}

static void xr_openxr_frame_end(ecs* world) {
    (void) world;
    const byte drew = xr_frame_begun;
    xr_end_frame();
    if (drew) {
        xr_timing_frame_end(xr_timing_eyes_start);
    }
}

const xr_runtime xr_openxr_runtime = {
    .name = "openxr",
    .frame_begin = xr_openxr_frame_begin,
    .locate = xr_openxr_locate,
    .inputs = xr_openxr_inputs,
    .stick = xr_openxr_stick,
    .button = xr_openxr_button,
    .aim = xr_openxr_aim,
    .grip = xr_openxr_grip,
    .delta = xr_openxr_delta,
    .should_render = xr_openxr_should_render,
    .eye_begin = xr_openxr_eye_begin,
    .eye_end = xr_openxr_eye_end,
    .frame_end = xr_openxr_frame_end,
    .flip_y = 1,
};
