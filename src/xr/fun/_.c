// --------------------------------------------------
// XR Globals
// --------------------------------------------------

static XrInstance xr_instance = XR_NULL_HANDLE;
static XrSystemId system_id;
static XrSession xr_session;
static XrSpace space;
static int xr_session_running;
static XrTime previous_display_time = 0;
static XrFrameState xr_frame_state;

// --------------------------------------------------
// XR Eyes
// --------------------------------------------------

typedef struct xr_eye {
    XrSwapchain swapchain;
    XrViewConfigurationView config;
    XrSwapchainImageOpenGLESKHR* images;
    uint32_t image_count;
    float4 fov;
    float4x4 matrix;
    int2 image_size;
    GLuint fbo;
    GLuint rbo;
} xr_eye;

static xr_eye xr_eyes[2];

// for any math we didnt have in core yet
#include "math.c"
#include "egl.c"
#include "helpers.c"
#include "eyes.c"
#include "init.c"
#include "frames.c"
#include "swapchains.c"
#include "input.c"
#include "time.c"

static int xr_begin() {
    if (!egl_setup()) {
        zox_loge("[XR] EGL setup failed");
        return 0;
    }
    if (!xr_init()) {
        zox_loge("[XR] OpenXR initialization failed");
        egl_shutdown();
        return 0;
    }
    if (!xr_setup()) {
        zox_loge("[XR] OpenXR setup failed");
        xr_shutdown();
        egl_shutdown();
        return 0;
    }
    if (!xr_input_init()) {
        zox_loge("[XR] OpenXR Input initialization failed");
        xr_shutdown();
        egl_shutdown();
        return 0;
    }
    xr_init_cameras();
    return 1;
}

static void xr_end() {
    if (xr_session_running) {
        xrEndSession(xr_session);
        xr_session_running = 0;
    }
    if (space != XR_NULL_HANDLE) {
        xrDestroySpace(space);
        space = XR_NULL_HANDLE;
    }
    for (int i = 0; i < 2; i++) {
        dispose_camera_buffers(
            &xr_eyes[i].fbo,
            &xr_eyes[i].rbo);
        free(xr_eyes[i].images);
        xr_eyes[i].images = NULL;
        if (xr_eyes[i].swapchain != XR_NULL_HANDLE) {
            xrDestroySwapchain(xr_eyes[i].swapchain);
            xr_eyes[i].swapchain = XR_NULL_HANDLE;
        }
    }
    if (xr_session != XR_NULL_HANDLE) {
        xrDestroySession(xr_session);
        xr_session = XR_NULL_HANDLE;
    }
    xr_shutdown();
    egl_shutdown();
}