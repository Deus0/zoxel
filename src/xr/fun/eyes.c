

void initialize_camera_buffers(GLuint* fbo, GLuint* rbo) {
    glGenFramebuffers(1, fbo);
    glGenRenderbuffers(1, rbo);
}

void dispose_camera_buffers(GLuint* fbo, GLuint* rbo) {
    glDeleteRenderbuffers(1, rbo);
    glDeleteFramebuffers(1, fbo);
}

void set_camera_buffers_size(GLuint fbo, GLuint rbo, int2 size) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT24,
        size.x,
        size.y);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}



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
    }
    return 1;
}