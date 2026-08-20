// --------------------------------------------------
// XR Swapchains
// --------------------------------------------------

// Begins the swap chain and binds the cameras fbo/dbos
static int xr_eye_render_begin(
    XrSwapchain xr_swapchain,
    XrSwapchainImageOpenGLESKHR* eye_images,
    float3 position,
    float4 rotation,
    float4 fov,
    int2 image_size,
    GLuint fbo,
    GLuint rbo)
{
    uint32_t image_index = 0;
    XrResult result = xrAcquireSwapchainImage(
        xr_swapchain,
        &(XrSwapchainImageAcquireInfo){ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO },
        &image_index
    );
    if (XR_FAILED(result)) {
        return 0;
    }
    result = xrWaitSwapchainImage(
        xr_swapchain,
        &(XrSwapchainImageWaitInfo){
            XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
            NULL,
            XR_INFINITE_DURATION
        }
    );
    if (XR_FAILED(result)) {
        return 0;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // This doesnt need to be done every frame, just when image is resized
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, image_size.x, image_size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        eye_images[image_index].image,
        0
    );
    // xr camera end
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