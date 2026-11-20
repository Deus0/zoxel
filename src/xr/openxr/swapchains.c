// --------------------------------------------------
// XR Swapchains
// --------------------------------------------------

// the eye's next image acquired and waited on (eye.image_index), drawn into until xr_swapchain_release
static inline int xr_swapchain_acquire(xr_eye* eye) {
    XrResult result = xrAcquireSwapchainImage(
        eye->swapchain,
        &(XrSwapchainImageAcquireInfo) {
            XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO
        },
        &eye->image_index
    );
    if (!xr_check_result(result, "xrAcquireSwapchainImage")) {
        return 0;
    }
    result = xrWaitSwapchainImage(
        eye->swapchain,
        &(XrSwapchainImageWaitInfo) {
            XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
            NULL,
            XR_INFINITE_DURATION
        }
    );
    return xr_check_result(result, "xrWaitSwapchainImage");
}

static inline void xr_swapchain_release(const xr_eye* eye) {
    xrReleaseSwapchainImage(
        eye->swapchain,
        &(XrSwapchainImageReleaseInfo) {
            XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO
        }
    );
}
