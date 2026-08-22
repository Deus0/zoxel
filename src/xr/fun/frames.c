// --------------------------------------------------
// XR Frame
// --------------------------------------------------

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
    return 1;
}

static inline XrCompositionLayerProjectionView zox_eye_to_xr_projection_view(
    float4x4 eye_matrix,
    float4 fov,
    int2 image_size,
    XrSwapchain swapchain
) {
    XrCompositionLayerProjectionView view = {
        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW,
        .pose = float4x4_to_xr_pose(eye_matrix),
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
    return view;
}

static inline XrCompositionLayerProjectionView xr_end_eye(
    float4x4 eye_matrix,
    float4 fov,
    int2 image_size,
    XrSwapchain swapchain
) {
    return (XrCompositionLayerProjectionView) {
        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW,
        .pose = float4x4_to_xr_pose(eye_matrix),
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
            xr_eyes[i].matrix,
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


byte xr_can_render(byte* running) {
    // XR Events
    XrEventDataBuffer event_buffer = {
        .type = XR_TYPE_EVENT_DATA_BUFFER
    };
    XrResult result = xrPollEvent(
        xr_instance,
        &event_buffer
    );
    if (result == XR_SUCCESS) {
        if (event_buffer.type ==
            XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED) {
            XrEventDataSessionStateChanged *state =
                (XrEventDataSessionStateChanged *)&event_buffer;
            zox_log("[XR] Session state: %d", state->state);

            switch (state->state) {
                case XR_SESSION_STATE_READY: {
                    result = xrBeginSession(
                        xr_session,
                        &(XrSessionBeginInfo){
                            .type = XR_TYPE_SESSION_BEGIN_INFO,
                            .primaryViewConfigurationType =
                                XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO
                        }
                    );
                    if (XR_SUCCEEDED(result)) {
                        xr_session_running = 1;
                        zox_log("[XR] Session started");
                    } else {
                        zox_loge(
                            "[XR] xrBeginSession failed: %d",
                            result
                        );
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
    } else if (result != XR_EVENT_UNAVAILABLE) {
        zox_loge("[XR] xrPollEvent failed: %d", result);
    }

    return xr_session_running &&
        xr_begin_frame() &&
        xr_frame_state.shouldRender;
}
