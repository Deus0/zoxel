

// --------------------------------------------------
// Grab eye data from our XR device
// --------------------------------------------------

// TODO: Just use our math data here
static int xr_get_views(XrView views[2]) {
    views[0].type = XR_TYPE_VIEW;
    views[1].type = XR_TYPE_VIEW;
    XrViewState view_state = {
        .type = XR_TYPE_VIEW_STATE
    };
    XrViewLocateInfo locate_info = {
        .type = XR_TYPE_VIEW_LOCATE_INFO,
        .viewConfigurationType =
            XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        .displayTime = xr_frame_state.predictedDisplayTime,
        .space = space
    };
    uint32_t view_count = 0;
    XrResult result = xrLocateViews(
        xr_session,
        &locate_info,
        &view_state,
        2,
        &view_count,
        views
    );
    if (XR_FAILED(result) || view_count != 2) {
        zox_loge(
            "[XR] xrLocatViews failed: %d count=%u",
            result,
            view_count
        );
        return 0;
    }
    return 1;
}

static float4 xr_head_rotation = { 0, 0, 0, 1 };   // the headset's, both eyes share it
static float3 xr_head_position = { 0, 0, 0 };      // the headset's in the tracking space, between the eyes: walking in the room

// NOTE: Gets Eye data and applies our camera transform to it
static int xr_get_eyes() {
    XrView views[2];
    if (!xr_get_views(views)) {
        return 0;
    }
    for (int i = 0; i < 2; i++) {
        xr_view_to_eye_data(
            views[i],
            &xr_eyes[i].matrix,
            &xr_eyes[i].fov
        );
        xr_eyes[i].pose = views[i].pose;
    }
    xr_head_rotation = xr_quaternion_to_float4(views[0].pose.orientation);
    xr_head_position = float3_scale(float3_add(
        xr_vector_to_float3(views[0].pose.position),
        xr_vector_to_float3(views[1].pose.position)), 0.5f);
    return 1;
}