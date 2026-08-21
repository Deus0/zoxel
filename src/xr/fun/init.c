// --------------------------------------------------
// XR Globals
// --------------------------------------------------

static XrFrameState frame_state;
static XrTime previous_display_time = 0;
static XrInstance xr_instance = XR_NULL_HANDLE;
static XrSystemId system_id;
static XrSession session;
static XrSpace space;
static XrSwapchain xr_swapchains[2];
static XrViewConfigurationView views_cfg[2];
static XrSwapchainImageOpenGLESKHR *images[2];
static uint32_t image_count[2];

static inline int xr_check_result(XrResult result, const char *name) {
    if (XR_FAILED(result)) {
        zox_loge("[XR] %s failed: %d", name, result);
        return 0;
    }
    zox_log("[XR] %s", name);
    return 1;
}

// --------------------------------------------------
// XR Setup
// --------------------------------------------------

static int xr_setup_system(void) {
    zox_log("[XR] Setting up system");
    XrSystemGetInfo system_info = {
        .type = XR_TYPE_SYSTEM_GET_INFO,
        .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
    };
    XrResult result = xrGetSystem(
        xr_instance,
        &system_info,
        &system_id
    );
    if (!xr_check_result(result, "xrGetSystem")) {
        return 0;
    }
    XrGraphicsRequirementsOpenGLESKHR requirements = {
        .type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR
    };
    PFN_xrGetOpenGLESGraphicsRequirementsKHR get_requirements = NULL;
    result = xrGetInstanceProcAddr(
        xr_instance,
        "xrGetOpenGLESGraphicsRequirementsKHR",
        (PFN_xrVoidFunction *)&get_requirements
    );
    if (!xr_check_result(
        result,
        "xrGetInstanceProcAddr(xrGetOpenGLESGraphicsRequirementsKHR)"
    )) {
        return 0;
    }
    if (!get_requirements) {
        zox_loge(

            "[XR] OpenGL ES requirements function missing"
        );
        return 0;
    }
    result = get_requirements(
        xr_instance,
        system_id,
        &requirements
    );
    if (!xr_check_result(
        result,
        "xrGetOpenGLESGraphicsRequirementsKHR"
    )) {
        return 0;
    }
    zox_log(
        "[XR] GL min version: %u.%u",
        XR_VERSION_MAJOR(requirements.minApiVersionSupported),
        XR_VERSION_MINOR(requirements.minApiVersionSupported)
    );
    zox_log(
        "[XR] GL max version: %u.%u",
        XR_VERSION_MAJOR(requirements.maxApiVersionSupported),
        XR_VERSION_MINOR(requirements.maxApiVersionSupported)
    );
    zox_log(
        "[XR] GL actual: %s",
        (const char *)glGetString(GL_VERSION)
    );
    return 1;
}

static int xr_setup_views(void) {
    uint32_t view_count = 0;
    XrResult result = xrEnumerateViewConfigurationViews(
        xr_instance,
        system_id,
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        0,
        &view_count,
        NULL
    );
    if (!xr_check_result(result,
        "xrEnumerateViewConfigurationViews(count)"
    )) {
        return 0;
    }
    zox_log("[XR] View count: %u", view_count);
    if (view_count != 2) {
        zox_loge("[XR] Expected 2 views, got %u", view_count);
        return 0;
    }
    for (uint32_t i = 0; i < view_count; i++) {
        views_cfg[i].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
        views_cfg[i].next = NULL;
    }
    result = xrEnumerateViewConfigurationViews(
        xr_instance,
        system_id,
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        view_count,
        &view_count,
        views_cfg
    );
    return xr_check_result(
        result,
        "xrEnumerateViewConfigurationViews"
    );
}

static int xr_setup_session(void) {
    zox_log(
        "[XR] EGL display=%p config=%p context=%p",
        (void *)egl_display,
        (void *)egl_config,
        (void *)egl_context
    );
    XrGraphicsBindingOpenGLESAndroidKHR binding = {
        .type = XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR,
        .next = NULL,
        .display = egl_display,
        .config = egl_config,
        .context = egl_context
    };
    XrSessionCreateInfo session_info = {
        .type = XR_TYPE_SESSION_CREATE_INFO,
        .next = &binding,
        .createFlags = 0,
        .systemId = system_id
    };
    XrResult result = xrCreateSession(
        xr_instance,
        &session_info,
        &session
    );
    if (!xr_check_result(result, "xrCreateSession")) {
        return 0;
    }
    XrReferenceSpaceCreateInfo space_info = {
        .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
        .next = NULL,
        .referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL,
        .poseInReferenceSpace.orientation.w = 1.0f
    };
    result = xrCreateReferenceSpace(
        session,
        &space_info,
        &space
    );
    if (!xr_check_result(result,
        "xrCreateReferenceSpace"
    )) {
        return 0;
    }
    return 1;
}

static int xr_setup_swapchains(void) {
    uint32_t format_count = 0;
    XrResult result = xrEnumerateSwapchainFormats(
        session,
        0,
        &format_count,
        NULL
    );
    if (!xr_check_result(result,
        "xrEnumerateSwapchainFormats(count)"
    )) {
        return 0;
    }
    if (format_count == 0) {
        zox_loge(

            "[XR] No swapchain formats"
        );
        return 0;
    }
    int64_t *formats = malloc(
        sizeof(int64_t) * format_count
    );
    if (!formats) {
        zox_loge("[XR] Format allocation failed");
        return 0;
    }
    result = xrEnumerateSwapchainFormats(
        session,
        format_count,
        &format_count,
        formats
    );
    if (!xr_check_result(result,
        "xrEnumerateSwapchainFormats"
    )) {
        free(formats);
        return 0;
    }
    int64_t format = formats[0];
    for (uint32_t i = 0; i < format_count; i++) {
        if (formats[i] == GL_RGBA8) {
            format = GL_RGBA8;
            break;
        }
    }
    zox_log("[XR] Swapchain format: %lld", (long long) format);
    free(formats);
    for (int eye = 0; eye < 2; eye++) {
        XrSwapchainCreateInfo info = {
            .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
            .next = NULL,
            .createFlags = 0,
            .usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
            .format = format,
            .sampleCount =
                views_cfg[eye].recommendedSwapchainSampleCount,
            .width =
                views_cfg[eye].recommendedImageRectWidth,
            .height =
                views_cfg[eye].recommendedImageRectHeight,
            .faceCount = 1,
            .arraySize = 1,
            .mipCount = 1
        };
        result = xrCreateSwapchain(
            session,
            &info,
            &xr_swapchains[eye]
        );
        if (!xr_check_result(result,
            "xrCreateSwapchain"
        )) {
            return 0;
        }
        result = xrEnumerateSwapchainImages(
            xr_swapchains[eye],
            0,
            &image_count[eye],
            NULL
        );
        if (!xr_check_result(result,
            "xrEnumerateSwapchainImages(count)"
        )) {
            return 0;
        }
        images[eye] = calloc(
            image_count[eye],
            sizeof(*images[eye])
        );
        if (!images[eye]) {
            zox_loge("[XR] Swapchain image allocation failed");
            return 0;
        }
        for (uint32_t i = 0; i < image_count[eye]; i++) {
            images[eye][i].type =
                XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR;
        }
        result = xrEnumerateSwapchainImages(
            xr_swapchains[eye],
            image_count[eye],
            &image_count[eye],
            (XrSwapchainImageBaseHeader *)images[eye]
        );
        if (!xr_check_result(
            result,
            "xrEnumerateSwapchainImages"
        )) {
            return 0;
        }
    }
    zox_log("[XR] Swapchains ready");
    return 1;
}

static int xr_setup() {
    if (!xr_setup_system()) {
        return 0;
    }
    if (!xr_setup_views()) {
        return 0;
    }
    if (!xr_setup_session()) {
        return 0;
    }
    if (!xr_setup_swapchains()) {
        return 0;
    }
    zox_log("[XR] Setup complete");
    return 1;
}

// --------------------------------------------------
// XR Init
// --------------------------------------------------

static int xr_init(void) {
    zox_log("[XR] Initializing");
    JNIEnv *env = (JNIEnv *)SDL_GetAndroidJNIEnv();
    if (!env) {
        zox_loge("[XR] SDL_GetAndroidJNIEnv failed");
        return 0;
    }
    JavaVM *java_vm = NULL;
    jint jni_result = (*env)->GetJavaVM(env, &java_vm);
    if (jni_result != JNI_OK || !java_vm) {
        zox_loge( "[XR] GetJavaVM failed");
        return 0;
    }
    jobject activity = (jobject)SDL_GetAndroidActivity();
    if (!activity) {
        zox_loge( "[XR] SDL_GetAndroidActivity failed");
        return 0;
    }
    PFN_xrInitializeLoaderKHR initialize_loader = NULL;
    XrResult result = xrGetInstanceProcAddr(
        XR_NULL_HANDLE,
        "xrInitializeLoaderKHR",
        (PFN_xrVoidFunction *)&initialize_loader
    );
    if (!xr_check_result(result, "xrGetInstanceProcAddr(xrInitializeLoaderKHR)")) {
        return 0;
    }
    if (!initialize_loader) {
        zox_loge( "[XR] xrInitializeLoaderKHR unavailable");
        return 0;
    }
    XrLoaderInitInfoAndroidKHR loader_info = {
        .type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR,
        .next = NULL,
        .applicationVM = java_vm,
        .applicationContext = activity
    };
    result = initialize_loader((const XrLoaderInitInfoBaseHeaderKHR *)&loader_info);
    if (!xr_check_result(result, "xrInitializeLoaderKHR")) {
        return 0;
    }
    uint32_t extension_count = 0;
    result = xrEnumerateInstanceExtensionProperties(NULL, 0, &extension_count, NULL);
    if (!xr_check_result(result, "xrEnumerateInstanceExtensionProperties(count)")) {
        return 0;
    }
    XrExtensionProperties *extensions = calloc(
        extension_count,
        sizeof(XrExtensionProperties)
    );
    if (!extensions) {
        zox_loge( "[XR] Extension allocation failed");
        return 0;
    }
    for (uint32_t i = 0; i < extension_count; i++) {
        extensions[i].type = XR_TYPE_EXTENSION_PROPERTIES;
    }
    uint32_t returned_extension_count = 0;
    result = xrEnumerateInstanceExtensionProperties(
        NULL,
        extension_count,
        &returned_extension_count,
        extensions
    );
    if (!xr_check_result(result, "xrEnumerateInstanceExtensionProperties")) {
        free(extensions);
        return 0;
    }
    int found_android = 0;
    int found_opengles = 0;
    for (uint32_t i = 0; i < returned_extension_count; i++) {
        if (strcmp(
            extensions[i].extensionName,
            XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME
        ) == 0) {
            found_android = 1;
        }
        if (strcmp(
            extensions[i].extensionName,
            XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME
        ) == 0) {
            found_opengles = 1;
        }
    }
    free(extensions);
    if (!found_android) {
        zox_loge( "[XR] Android extension unavailable");
        return 0;
    }
    if (!found_opengles) {
        zox_loge( "[XR] OpenGL ES extension unavailable");
        return 0;
    }
    const char *enabled_extensions[] = {
        XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME,
        XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME
    };
    XrInstanceCreateInfoAndroidKHR android_info = {
        .type = XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR,
        .next = NULL,
        .applicationVM = java_vm,
        .applicationActivity = activity
    };
    XrInstanceCreateInfo create_info = {
        .type = XR_TYPE_INSTANCE_CREATE_INFO,
        .next = &android_info,
        .applicationInfo = {
            .applicationName = "Zoxel",
            .applicationVersion = 1,
            .engineName = "Zoxel",
            .engineVersion = 1,
            .apiVersion = XR_MAKE_VERSION(1, 0, 0)
        },
        .enabledApiLayerCount = 0,
        .enabledApiLayerNames = NULL,
        .enabledExtensionCount = 2,
        .enabledExtensionNames = enabled_extensions
    };
    result = xrCreateInstance(&create_info, &xr_instance);
    if (!xr_check_result(result, "xrCreateInstance")) {
        xr_instance = XR_NULL_HANDLE;
        return 0;
    }
    return 1;
}

static void xr_shutdown(void) {
    if (xr_instance != XR_NULL_HANDLE) {
        xrDestroyInstance(xr_instance);
        xr_instance = XR_NULL_HANDLE;
        zox_log("[XR] Instance destroyed");
    }
}