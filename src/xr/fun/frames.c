// --------------------------------------------------
// XR Frame
// --------------------------------------------------

static int xr_begin_frame() {
    frame_state = (XrFrameState) {
        .type = XR_TYPE_FRAME_STATE
    };
    XrResult result = xrWaitFrame(
        session,
        &(XrFrameWaitInfo){
            .type = XR_TYPE_FRAME_WAIT_INFO
        },
        &frame_state
    );
    if (XR_FAILED(result)) {
        zox_loge("[XR] xrWaitFrame failed: %d", result);
        return 0;
    }
    result = xrBeginFrame(
        session,
        &(XrFrameBeginInfo){
            .type = XR_TYPE_FRAME_BEGIN_INFO
        }
    );
    if (XR_FAILED(result)) {
        zox_loge("[XR] xrBeginFrame failed: %d", result);
        return 0;
    }
    return 1;
}

// NOTE: Submit eye data to the headset
// TODO: We can do this per eye
static void xr_end_frame(
    float3 eye_position_left,
    float3 eye_position_right,
    float4 eye_rotation_left,
    float4 eye_rotation_right,
    float4 eye_fov_left,
    float4 eye_fov_right
) {
    if (!frame_state.shouldRender) {
        xrEndFrame(
            session,
            &(XrFrameEndInfo){
                .type = XR_TYPE_FRAME_END_INFO,
                .displayTime = frame_state.predictedDisplayTime,
                .environmentBlendMode =
                    XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
                .layerCount = 0,
                .layers = NULL
            }
        );
        return;
    }
    XrCompositionLayerProjectionView projection_views[2] = {
        {
            .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW
        },
        {
            .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW
        }
    };
    projection_views[0] = zox_eye_to_xr_projection_view(
        eye_position_left,
        eye_rotation_left,
        eye_fov_left,
        (int2) {
            views_cfg[0].recommendedImageRectWidth,
            views_cfg[0].recommendedImageRectHeight
        },
        xr_swapchains[0]
    );
    projection_views[1] = zox_eye_to_xr_projection_view(
        eye_position_right,
        eye_rotation_right,
        eye_fov_right,
        (int2) {
            views_cfg[1].recommendedImageRectWidth,
            views_cfg[1].recommendedImageRectHeight
        },
        xr_swapchains[1]
    );
    XrCompositionLayerProjection layer = {
        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION,
        .next = NULL,
        .layerFlags = 0,
        .space = space,
        .viewCount = 2,
        .views = projection_views
    };
    const XrCompositionLayerBaseHeader *layers[] = {
        (const XrCompositionLayerBaseHeader *)&layer
    };
    XrResult result = xrEndFrame(
        session,
        &(XrFrameEndInfo){
            .type = XR_TYPE_FRAME_END_INFO,
            .next = NULL,
            .displayTime = frame_state.predictedDisplayTime,
            .environmentBlendMode =
                XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
            .layerCount = 1,
            .layers = layers
        }
    );
    if (XR_FAILED(result)) {
        zox_loge("[XR] xrEndFrame failed: %d", result);
    }
}