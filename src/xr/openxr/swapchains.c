// --------------------------------------------------
// XR Swapchains
// --------------------------------------------------

// Begins the swap chain and binds the cameras fbo/dbos
// NOTE: Each frame, we bind the cameras buffers to a new texture given by the XR SwapChain

// NOTE: Binds XR Swapchain Texture to FBO
// NOTE: Needs a xrReleaseSwapchainImage after called
static inline int xr_eye_render_begin(
    XrSwapchain xr_swapchain,
    XrSwapchainImageOpenGLESKHR* eye_images,
    GLuint fbo)
{
    uint32_t image_index = 0;
    XrResult result = xrAcquireSwapchainImage(
        xr_swapchain,
        &(XrSwapchainImageAcquireInfo) {
            XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO
        },
        &image_index
    );
    if (!xr_check_result(result, "xrAcquireSwapchainImage")) {
        return 0;
    }
    result = xrWaitSwapchainImage(
        xr_swapchain,
        &(XrSwapchainImageWaitInfo) {
            XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
            NULL,
            XR_INFINITE_DURATION
        }
    );
    if (!xr_check_result(result, "xrWaitSwapchainImage")) {
        return 0;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        eye_images[image_index].image,
        0
    );
    return 1;
}

static inline void xr_eye_render_end(XrSwapchain xr_swapchain) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    xrReleaseSwapchainImage(
        xr_swapchain,
        &(XrSwapchainImageReleaseInfo) {
            XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO
        }
    );
}