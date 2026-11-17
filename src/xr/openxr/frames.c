// --------------------------------------------------
// XR Frame
// --------------------------------------------------
//  - begun at the top of the engine's frame (xr_frame_start, zoxp_begin), so everything read after it (the controllers'
//    poses, zoxp_inputs_extract) is predicted for this frame's display time, not the last one's (the hands ran a
//    frame behind)
//  - every begun frame is ended (xr_end_frame), with no layers when the runtime says not to render: a begun frame
//    left open breaks the runtime's frame pacing (a stutter)
static byte xr_frame_begun = 0;

static int xr_begin_frame() {
    xr_frame_state = (XrFrameState) {
        .type = XR_TYPE_FRAME_STATE
    };
    XrResult result = xrWaitFrame(
        xr_session,
        &(XrFrameWaitInfo){
            .type = XR_TYPE_FRAME_WAIT_INFO
        },
        &xr_frame_state
    );
    if (!xr_check_result(result, "xrWaitFrame")) {
        return 0;
    }
    result = xrBeginFrame(
        xr_session,
        &(XrFrameBeginInfo){
            .type = XR_TYPE_FRAME_BEGIN_INFO
        }
    );
    if (!xr_check_result(result, "xrBeginFrame")) {
        return 0;
    }
    xr_frame_begun = 1;
    return 1;
}

static inline XrCompositionLayerProjectionView xr_end_eye(
    XrPosef pose,
    float4 fov,
    int2 image_size,
    XrSwapchain swapchain
) {
    return (XrCompositionLayerProjectionView) {
        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW,
        .pose = pose,   // the eye's pose as located (xrLocateViews), as OpenXR asks
        .fov = float4_to_xr_fov(fov),
        .subImage = {
            .swapchain = swapchain,
            .imageRect = {
                .offset = { 0, 0 },
                .extent = int2_to_xr2(image_size)
            },
            .imageArrayIndex = 0
        }
    };
}


// NOTE: Submit frame to the headset
static void xr_end_frame() {
    if (!xr_frame_begun) {
        return;
    }
    xr_frame_begun = 0;
    if (!xr_frame_state.shouldRender) {
        xrEndFrame(
            xr_session,
            &(XrFrameEndInfo) {
                .type = XR_TYPE_FRAME_END_INFO,
                .displayTime = xr_frame_state.predictedDisplayTime,
                .environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
                .layerCount = 0,
                .layers = NULL
            }
        );
        return;
    }
    XrCompositionLayerProjectionView projection_views[2];
    for (int i = 0; i < 2; i++) {
        projection_views[i] = xr_end_eye(
            xr_eyes[i].pose,
            xr_eyes[i].fov,
            xr_eyes[i].image_size,
            xr_eyes[i].swapchain
        );
    }
    XrCompositionLayerProjection layer = {
        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION,
        .space = space,
        .viewCount = 2,
        .views = projection_views
    };
    const XrCompositionLayerBaseHeader* layers[] = {
        (const XrCompositionLayerBaseHeader*)&layer
    };
    XrResult result = xrEndFrame(
        xr_session,
        &(XrFrameEndInfo) {
            .type = XR_TYPE_FRAME_END_INFO,
            .displayTime = xr_frame_state.predictedDisplayTime,
            .environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
            .layerCount = 1,
            .layers = layers
        }
    );
    xr_check_result(result, "xrEndFrame");
}


extern byte xr_recentred;

// OpenXR's events, all of them (one a frame let them back up), the session started and stopped as they say
//  - the player recentring (the system's button): the tracking space moves, the body takes it up (fun/body.c)
static void xr_poll_events(byte* running) {
    for (;;) {
        XrEventDataBuffer event_buffer = { .type = XR_TYPE_EVENT_DATA_BUFFER };
        const XrResult result = xrPollEvent(xr_instance, &event_buffer);
        if (result == XR_EVENT_UNAVAILABLE) {
            return;
        }
        if (result != XR_SUCCESS) {
            zox_loge("[XR] xrPollEvent failed: %d", result);
            return;
        }
        if (event_buffer.type == XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING) {
            zox_log("[XR] the tracking space was recentred");
            xr_recentred = 1;
            continue;
        }
        if (event_buffer.type != XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED) {
            continue;
        }
        const XrEventDataSessionStateChanged* state = (XrEventDataSessionStateChanged*) &event_buffer;
        zox_log("[XR] Session state: %d", state->state);
        switch (state->state) {
            case XR_SESSION_STATE_READY: {
                const XrResult begun = xrBeginSession(
                    xr_session,
                    &(XrSessionBeginInfo) {
                        .type = XR_TYPE_SESSION_BEGIN_INFO,
                        .primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO
                    });
                if (XR_SUCCEEDED(begun)) {
                    xr_session_running = 1;
                    zox_log("[XR] Session started");
                } else {
                    zox_loge("[XR] xrBeginSession failed: %d", begun);
                }
                break;
            }
            case XR_SESSION_STATE_STOPPING:
                xrEndSession(xr_session);
                xr_session_running = 0;
                zox_log("[XR] Session stopped");
                break;
            case XR_SESSION_STATE_EXITING:
            case XR_SESSION_STATE_LOSS_PENDING:
                *running = 0;
                break;
            default:
                break;
        }
    }
}

// the frame's start: events, then wait for and begin the headset's frame, returns whether one began
byte xr_frame_start(byte* running) {
    xr_poll_events(running);
    if (xr_frame_begun) {
        // one left open (the render didn't run): ended first, OpenXR wants them in pairs
        xr_end_frame();
    }
    return xr_session_running && xr_begin_frame();
}

// a frame begun to draw (tst/cube.c's loop): one the runtime doesn't want drawn is ended here
byte xr_can_render(byte* running) {
    if (!xr_frame_start(running)) {
        return 0;
    }
    if (!xr_frame_state.shouldRender) {
        xr_end_frame();
        return 0;
    }
    return 1;
}
