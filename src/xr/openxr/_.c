// OpenXR: the session, eyes, swapchains and inputs (Android; desktop Linux later). Nothing of the engine's
//  ecs here but the render (render.c, included by xr/_.c), so tst/cube.c builds from it alone
//  - its graphics: opengl es (gl.c, EGL's context) or vulkan (vulkan.c, XR_KHR_vulkan_enable2: OpenXR makes the
//    backend's instance and device), each the same xr_graphics_ functions
#include "_.h"

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

#ifdef zox_vulkan
    typedef XrSwapchainImageVulkan2KHR xr_image;
    #define xr_image_type XR_TYPE_SWAPCHAIN_IMAGE_VULKAN2_KHR
#else
    typedef XrSwapchainImageOpenGLESKHR xr_image;
    #define xr_image_type XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR
#endif

typedef struct xr_eye {
    XrSwapchain swapchain;
    XrViewConfigurationView config;
    xr_image* images;
    uint32_t image_count;
    uint32_t image_index;   // acquired this frame
    float4 fov;
    float4x4 matrix;
    XrPosef pose;           // as located this frame, handed back to OpenXR on submit
    int2 image_size;
#ifdef zox_vulkan
    uint* targets;          // an external target (graphics/vulkan/fun/external.c) per image
#else
    GLuint fbo;
    GLuint rbo;
#endif
} xr_eye;

static xr_eye xr_eyes[2];

#include "helpers.c"
#include "swapchains.c"
#ifdef zox_vulkan
    #include "vulkan.c"
#else
    #include "gl.c"
#endif
#include "eyes.c"
#include "init.c"
#include "frames.c"
#include "input.c"
#include "time.c"

static int xr_begin() {
    if (!xr_graphics_begin()) {
        zox_loge("[XR] graphics setup failed");
        return 0;
    }
    if (!xr_init()) {
        zox_loge("[XR] OpenXR initialization failed");
        xr_graphics_end();
        return 0;
    }
    if (!xr_setup()) {
        zox_loge("[XR] OpenXR setup failed");
        xr_shutdown();
        xr_graphics_end();
        return 0;
    }
    if (!xr_input_init()) {
        zox_loge("[XR] OpenXR Input initialization failed");
        xr_shutdown();
        xr_graphics_end();
        return 0;
    }
    if (!xr_graphics_eyes_begin()) {
        zox_loge("[XR] eye targets failed");
        xr_shutdown();
        xr_graphics_end();
        return 0;
    }
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
    xr_graphics_eyes_end();
    for (int i = 0; i < 2; i++) {
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
    xr_graphics_end();
}