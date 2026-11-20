// --------------------------------------------------
// XR Graphics: Vulkan (XR_KHR_vulkan_enable2)
// --------------------------------------------------
//  - OpenXR makes the backend's instance and device (the hooks in graphics/vulkan/set/_.c), adding the extensions
//    the headset needs, and picks its gpu; the backend draws offscreen (no window, no surface) between the eyes
//  - each swapchain image an external target (graphics/vulkan/fun/external.c): drawn as the frame is, its rows top
//    first as OpenXR reads them, so no y flip and no winding change (flip_y 0)
//  - an eye's drawing is submitted before its image is released (vulkan_external_submit), as OpenXR asks
//  - the images must be the offscreen frame's format (R8G8B8A8 unorm, vulkan_create_offscreen): the frame's pipelines
//    draw into them
#define xr_graphics_extension XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME
#define xr_graphics_flip_y 0

static PFN_xrCreateVulkanInstanceKHR xr_create_vulkan_instance = NULL;
static PFN_xrGetVulkanGraphicsDevice2KHR xr_get_vulkan_device = NULL;
static PFN_xrCreateVulkanDeviceKHR xr_create_vulkan_device = NULL;

static int xr_graphics_begin() {
    return 1;
}

// the backend disposes its own device and instance (graphics/vulkan/_.c)
static void xr_graphics_end() { }

static VkResult xr_vulkan_create_instance(const VkInstanceCreateInfo* info, VkInstance* instance) {
    const XrVulkanInstanceCreateInfoKHR xr_info = {
        .type = XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR,
        .systemId = system_id,
        .pfnGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vulkanCreateInfo = info,
    };
    VkResult vulkan_result = VK_SUCCESS;
    if (!xr_check_result(xr_create_vulkan_instance(xr_instance, &xr_info, instance, &vulkan_result),
        "xrCreateVulkanInstanceKHR"))
    {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return vulkan_result;
}

static VkResult xr_vulkan_pick_physical(VkPhysicalDevice* physical) {
    const XrVulkanGraphicsDeviceGetInfoKHR xr_info = {
        .type = XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR,
        .systemId = system_id,
        .vulkanInstance = vk_instance,
    };
    if (!xr_check_result(xr_get_vulkan_device(xr_instance, &xr_info, physical), "xrGetVulkanGraphicsDevice2KHR")) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
}

static VkResult xr_vulkan_create_device(VkPhysicalDevice physical, const VkDeviceCreateInfo* info, VkDevice* device) {
    const XrVulkanDeviceCreateInfoKHR xr_info = {
        .type = XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR,
        .systemId = system_id,
        .pfnGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vulkanPhysicalDevice = physical,
        .vulkanCreateInfo = info,
    };
    VkResult vulkan_result = VK_SUCCESS;
    if (!xr_check_result(xr_create_vulkan_device(xr_instance, &xr_info, device, &vulkan_result),
        "xrCreateVulkanDeviceKHR"))
    {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return vulkan_result;
}

static int xr_vulkan_proc(const char* name, PFN_xrVoidFunction* function) {
    return xr_check_result(xrGetInstanceProcAddr(xr_instance, name, function), name) && *function;
}

// OpenXR's requirements (asked before the session), then the backend made through it: offscreen, OpenXR's gpu
static int xr_graphics_system() {
    PFN_xrGetVulkanGraphicsRequirements2KHR get_requirements = NULL;
    if (!xr_vulkan_proc("xrGetVulkanGraphicsRequirements2KHR", (PFN_xrVoidFunction*) &get_requirements) ||
        !xr_vulkan_proc("xrCreateVulkanInstanceKHR", (PFN_xrVoidFunction*) &xr_create_vulkan_instance) ||
        !xr_vulkan_proc("xrGetVulkanGraphicsDevice2KHR", (PFN_xrVoidFunction*) &xr_get_vulkan_device) ||
        !xr_vulkan_proc("xrCreateVulkanDeviceKHR", (PFN_xrVoidFunction*) &xr_create_vulkan_device))
    {
        return 0;
    }
    XrGraphicsRequirementsVulkan2KHR requirements = {
        .type = XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR
    };
    if (!xr_check_result(get_requirements(xr_instance, system_id, &requirements),
        "xrGetVulkanGraphicsRequirements2KHR"))
    {
        return 0;
    }
    zox_log("[XR] Vulkan versions: %u.%u to %u.%u",
        XR_VERSION_MAJOR(requirements.minApiVersionSupported), XR_VERSION_MINOR(requirements.minApiVersionSupported),
        XR_VERSION_MAJOR(requirements.maxApiVersionSupported), XR_VERSION_MINOR(requirements.maxApiVersionSupported));
    vk_create_instance_with = xr_vulkan_create_instance;
    vk_pick_physical_with = xr_vulkan_pick_physical;
    vk_create_device_with = xr_vulkan_create_device;
    if (!vulkan_create_instance(NULL, 0) || !vulkan_initialize((int2) { 64, 64 })) {
        zox_loge("[XR] the vulkan backend failed to start");
        return 0;
    }
    using_gpu = vk_on_gpu;
    return 1;
}

static const void* xr_graphics_binding() {
    static XrGraphicsBindingVulkan2KHR binding;
    binding = (XrGraphicsBindingVulkan2KHR) {
        .type = XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR,
        .instance = vk_instance,
        .physicalDevice = vk_physical,
        .device = vk_device,
        .queueFamilyIndex = vk_queue_family,
        .queueIndex = 0,
    };
    return &binding;
}

// the offscreen frame's format (made at its first frame, after this), which the pipelines draw: -1 if not offered
static int64_t xr_graphics_format(const int64_t* formats, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        if (formats[i] == VK_FORMAT_R8G8B8A8_UNORM) {
            return formats[i];
        }
    }
    zox_loge("[XR] the swapchains don't offer the frame's format (R8G8B8A8 unorm)");
    return -1;
}

// an external target per swapchain image
static int xr_graphics_eyes_begin() {
    for (int i = 0; i < 2; i++) {
        xr_eye* eye = &xr_eyes[i];
        eye->targets = calloc(eye->image_count, sizeof(uint));
        if (!eye->targets) {
            return 0;
        }
        for (uint32_t j = 0; j < eye->image_count; j++) {
            eye->targets[j] = vulkan_external_target(eye->images[j].image, VK_FORMAT_R8G8B8A8_UNORM, eye->image_size);
            if (!eye->targets[j]) {
                zox_loge("[XR] eye [%i] image [%u]'s target failed", i, j);
                return 0;
            }
        }
    }
    return 1;
}

// the targets themselves go with the backend (vulkan_dispose_external_targets)
static void xr_graphics_eyes_end() {
    for (int i = 0; i < 2; i++) {
        free(xr_eyes[i].targets);
        xr_eyes[i].targets = NULL;
    }
}

static int xr_graphics_eye_begin(byte i) {
    xr_eye* eye = &xr_eyes[i];
    if (!xr_swapchain_acquire(eye)) {
        return 0;
    }
    vulkan_bind_external_target(eye->targets[eye->image_index]);
    return 1;
}

static void xr_graphics_eye_end(byte i) {
    vulkan_external_submit();
    xr_swapchain_release(&xr_eyes[i]);
}
