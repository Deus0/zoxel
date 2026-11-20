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
    return xr_graphics_system();
}

static int xr_setup_views() {
    uint32_t view_count = 0;
    XrResult result = xrEnumerateViewConfigurationViews(
        xr_instance,
        system_id,
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        0,
        &view_count,
        NULL
    );
    if (!xr_check_result(result, "xrEnumerateViewConfigurationViews(count)")) {
        return 0;
    }
    zox_log("[XR] View count: %u", view_count);
    if (view_count != 2) {
        zox_loge("[XR] Expected 2 views, got %u", view_count);
        return 0;
    }
    XrViewConfigurationView views_cfg[2];
    for (uint32_t i = 0; i < view_count; i++) {
        xr_eyes[i].config.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
        xr_eyes[i].config.next = NULL;
        views_cfg[i] = xr_eyes[i].config;
    }
    result = xrEnumerateViewConfigurationViews(
        xr_instance,
        system_id,
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        view_count,
        &view_count,
        views_cfg
    );
    if (!xr_check_result(
        result,
        "xrEnumerateViewConfigurationViews"
    )) {
        return 0;
    }
    // NOTE: Used for swapchains
    xr_eyes[0].config = views_cfg[0];
    xr_eyes[1].config = views_cfg[1];
    xr_eyes[0].image_size = xr_config_to_image_size(views_cfg[0]);
    xr_eyes[1].image_size = xr_config_to_image_size(views_cfg[1]);
    return 1;
}

static int xr_setup_session(void) {
    XrSessionCreateInfo session_info = {
        .type = XR_TYPE_SESSION_CREATE_INFO,
        .next = xr_graphics_binding(),
        .createFlags = 0,
        .systemId = system_id
    };
    XrResult result = xrCreateSession(
        xr_instance,
        &session_info,
        &xr_session
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
        xr_session,
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

static int xr_setup_swapchains() {
    uint32_t format_count = 0;
    XrResult result = xrEnumerateSwapchainFormats(
        xr_session,
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
        xr_session,
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
    const int64_t format = xr_graphics_format(formats, format_count);
    if (format < 0) {
        free(formats);
        return 0;
    }
    zox_log("[XR] Swapchain format: %lld", (long long) format);
    free(formats);
    for (int i = 0; i < 2; i++) {
        XrSwapchainCreateInfo info = {
            .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
            .next = NULL,
            .createFlags = 0,
            .usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
            .format = format,
            .sampleCount =
                xr_eyes[i].config.recommendedSwapchainSampleCount,
            .width =
                xr_eyes[i].config.recommendedImageRectWidth,
            .height =
                xr_eyes[i].config.recommendedImageRectHeight,
            .faceCount = 1,
            .arraySize = 1,
            .mipCount = 1
        };
        result = xrCreateSwapchain(
            xr_session,
            &info,
            &xr_eyes[i].swapchain
        );
        if (!xr_check_result(result,
            "xrCreateSwapchain"
        )) {
            return 0;
        }
        result = xrEnumerateSwapchainImages(
            xr_eyes[i].swapchain,
            0,
            &xr_eyes[i].image_count,
            NULL
        );
        if (!xr_check_result(result,
            "xrEnumerateSwapchainImages(count)"
        )) {
            return 0;
        }
        xr_eyes[i].images = calloc(
            xr_eyes[i].image_count,
            sizeof(*xr_eyes[i].images)
        );
        if (!xr_eyes[i].images) {
            zox_loge("[XR] Swapchain image allocation failed");
            return 0;
        }
        for (uint32_t j = 0; j < xr_eyes[i].image_count; j++) {
            xr_eyes[i].images[j].type = xr_image_type;
        }
        result = xrEnumerateSwapchainImages(
            xr_eyes[i].swapchain,
            xr_eyes[i].image_count,
            &xr_eyes[i].image_count,
            (XrSwapchainImageBaseHeader *)xr_eyes[i].images
        );
        if (!xr_check_result(result, "xrEnumerateSwapchainImages")) {
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
    int found_graphics = 0;
    for (uint32_t i = 0; i < returned_extension_count; i++) {
        if (strcmp(
            extensions[i].extensionName,
            XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME
        ) == 0) {
            found_android = 1;
        }
        if (strcmp(
            extensions[i].extensionName,
            xr_graphics_extension
        ) == 0) {
            found_graphics = 1;
        }
    }
    free(extensions);
    if (!found_android) {
        zox_loge( "[XR] Android extension unavailable");
        return 0;
    }
    if (!found_graphics) {
        zox_loge("[XR] %s unavailable", xr_graphics_extension);
        return 0;
    }
    const char *enabled_extensions[] = {
        XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME,
        xr_graphics_extension
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

static void xr_shutdown() {
    if (xr_instance != XR_NULL_HANDLE) {
        xrDestroyInstance(xr_instance);
        xr_instance = XR_NULL_HANDLE;
        zox_log("[XR] Instance destroyed");
    }
}
