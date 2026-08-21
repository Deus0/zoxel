#include "helpers.c"
#include "egl.c"
#include "init.c"
#include "frames.c"
#include "swapchains.c"
#include "eyes.c"
#include "input.c"
#include "time.c"


/*static XrInstance xr_instance = XR_NULL_HANDLE;

static int xr_check_result(XrResult result, const char* name) {
    if (XR_FAILED(result)) {
        zox_loge(
            "[XR] %s failed: %d (0x%08x)",
            name,
            result,
            (unsigned int) result
        );
        return 0;
    }

    zox_log("[XR] %s succeeded", name);
    return 1;
}

int zox_xr_init(void) {

    zox_log("[XR] ========================================");
    zox_log("[XR] Initializing OpenXR");
    zox_log("[XR] ========================================");

    // --------------------------------------------------------
    // Android JNI
    // --------------------------------------------------------

    JNIEnv* env = (JNIEnv*) SDL_GetAndroidJNIEnv();

    if (!env) {
        zox_loge("[XR] SDL_GetAndroidJNIEnv() returned NULL");
        return 0;
    }

    zox_log("[XR] JNIEnv acquired");

    JavaVM* java_vm = NULL;

    jint jni_result = (*env)->GetJavaVM(env, &java_vm);

    if (jni_result != JNI_OK || !java_vm) {
        zox_loge(
            "[XR] GetJavaVM failed: %d JavaVM=%p",
            jni_result,
            (void*) java_vm
        );
        return 0;
    }

    zox_log(
        "[XR] JavaVM acquired: %p",
        (void*) java_vm
    );

    jobject activity = (jobject) SDL_GetAndroidActivity();

    if (!activity) {
        zox_loge(
            "[XR] SDL_GetAndroidActivity() returned NULL"
        );
        return 0;
    }

    zox_log(
        "[XR] Android Activity acquired: %p",
        (void*) activity
    );

    PFN_xrInitializeLoaderKHR initialize_loader = NULL;

    XrResult result = xrGetInstanceProcAddr(
        XR_NULL_HANDLE,
        "xrInitializeLoaderKHR",
        (PFN_xrVoidFunction*) &initialize_loader
    );

    if (!xr_check_result(result, "xrGetInstanceProcAddr(xrInitializeLoaderKHR)")) {
        return 0;
    }

    if (!initialize_loader) {
        zox_loge("[XR] xrInitializeLoaderKHR function pointer is NULL");
        return 0;
    }

    XrLoaderInitInfoAndroidKHR loader_info = {
        .type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR,
        .next = NULL,
        .applicationVM = java_vm,
        .applicationContext = activity
    };

    zox_log("[XR] Initializing OpenXR Android loader");

    result = initialize_loader(
        (const XrLoaderInitInfoBaseHeaderKHR*) &loader_info
    );

    if (!xr_check_result(result, "xrInitializeLoaderKHR")) {
        return 0;
    }

    zox_log("[XR] OpenXR Android loader initialized");

    // --------------------------------------------------------
    // Enumerate OpenXR instance extensions
    // --------------------------------------------------------

    uint32_t extension_count = 0;

    result = xrEnumerateInstanceExtensionProperties(
        NULL,
        0,
        &extension_count,
        NULL
    );

    if (!xr_check_result(
        result,
        "xrEnumerateInstanceExtensionProperties(count)"
    )) {
        return 0;
    }

    zox_log(
        "[XR] Runtime exposes %u instance extensions",
        extension_count
    );

    XrExtensionProperties* extensions = NULL;

    if (extension_count > 0) {

        extensions = malloc(
            sizeof(XrExtensionProperties) * extension_count
        );

        if (!extensions) {
            zox_loge(
                "[XR] Failed to allocate extension list"
            );
            return 0;
        }

        for (uint32_t i = 0; i < extension_count; i++) {
            extensions[i].type =
                XR_TYPE_EXTENSION_PROPERTIES;
            extensions[i].next = NULL;
        }

        uint32_t returned_extension_count = 0;

        result = xrEnumerateInstanceExtensionProperties(
            NULL,
            extension_count,
            &returned_extension_count,
            extensions
        );

        if (!xr_check_result(
            result,
            "xrEnumerateInstanceExtensionProperties"
        )) {
            free(extensions);
            return 0;
        }

        int found_android_extension = 0;

        for (uint32_t i = 0; i < returned_extension_count; i++) {

            zox_log(
                "[XR] Extension: %s",
                extensions[i].extensionName
            );

            if (strcmp(
                extensions[i].extensionName,
                XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME
            ) == 0) {
                found_android_extension = 1;
            }
        }

        free(extensions);

        if (!found_android_extension) {
            zox_loge(
                "[XR] Runtime does not expose %s",
                XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME
            );
            return 0;
        }

    } else {

        zox_loge(
            "[XR] Runtime returned zero instance extensions"
        );
        return 0;
    }

    zox_log(
        "[XR] Android create-instance extension available"
    );

    // --------------------------------------------------------
    // Android instance information
    // --------------------------------------------------------

    XrInstanceCreateInfoAndroidKHR android_info = {
        .type = XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR,
        .next = NULL,
        .applicationVM = java_vm,
        .applicationActivity = activity
    };

    zox_log(
        "[XR] Android instance information prepared"
    );

    // --------------------------------------------------------
    // Create instance
    // --------------------------------------------------------

    const char* enabled_extensions[] = {
        XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME
    };

    XrInstanceCreateInfo create_info = {
        .type = XR_TYPE_INSTANCE_CREATE_INFO,
        .next = &android_info,
        .createFlags = 0,

        .applicationInfo = {
            .applicationName = "Zoxel",
            .applicationVersion = 1,
            .engineName = "Zoxel",
            .engineVersion = 1,
            .apiVersion = XR_MAKE_VERSION(1, 0, 0)
        },

        .enabledApiLayerCount = 0,
        .enabledApiLayerNames = NULL,

        .enabledExtensionCount = 1,
        .enabledExtensionNames = enabled_extensions
    };

    zox_log(
        "[XR] Calling xrCreateInstance()"
    );

    result = xrCreateInstance(
        &create_info,
        &xr_instance
    );

    if (!xr_check_result(
        result,
        "xrCreateInstance"
    )) {
        xr_instance = XR_NULL_HANDLE;
        return 0;
    }

    zox_log(
        "[XR] ========================================"
    );

    zox_log(
        "[XR] OpenXR instance created!"
    );

    zox_log(
        "[XR] ========================================"
    );

    return 1;
}

void zox_xr_shutdown(void) {

    zox_log("[XR] Shutting down");

    if (xr_instance != XR_NULL_HANDLE) {

        XrResult result =
            xrDestroyInstance(xr_instance);

        xr_check_result(
            result,
            "xrDestroyInstance"
        );

        xr_instance = XR_NULL_HANDLE;
    }
}*/