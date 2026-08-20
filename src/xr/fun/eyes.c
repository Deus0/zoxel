// --------------------------------------------------
// Grab eye data from our XR device
// --------------------------------------------------

// TODO: Just use our math data here
static int xr_get_views(XrView views[2]) {
    XrViewState view_state = {
        .type = XR_TYPE_VIEW_STATE
    };
    XrViewLocateInfo locate_info = {
        .type = XR_TYPE_VIEW_LOCATE_INFO,
        .viewConfigurationType =
            XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        .displayTime = frame_state.predictedDisplayTime,
        .space = space
    };
    uint32_t view_count = 0;
    XrResult result = xrLocateViews(
        session,
        &locate_info,
        &view_state,
        2,
        &view_count,
        views
    );
    if (XR_FAILED(result) || view_count != 2) {
        zox_loge(
            "[XR] xrLocateViews failed: %d count=%u",
            result,
            view_count
        );
        return 0;
    }
    return 1;
}