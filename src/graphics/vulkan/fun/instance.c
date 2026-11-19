// 1 if the call worked, else logs what failed
byte vulkan_check(VkResult result, const char* what) {
    if (result == VK_SUCCESS) {
        return 1;
    }
    zox_loge("[vulkan] [%s] failed [%i]", what, result);
    return 0;
}

// validation messages are errors, so a test fails on them (and the harness stops at 100)
static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_on_message(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* user)
{
    (void) type;
    (void) user;
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        vk_validation_errors++;
        zox_loge("[vulkan] %s", data->pMessage);
    } else {
        zox_logw("[vulkan] %s", data->pMessage);
    }
    return VK_FALSE;
}

byte vulkan_has_layer(const char* name) {
    uint count = 0;
    vkEnumerateInstanceLayerProperties(&count, NULL);
    VkLayerProperties* layers = malloc(sizeof(VkLayerProperties) * count);
    vkEnumerateInstanceLayerProperties(&count, layers);
    byte found = 0;
    for (uint i = 0; i < count && !found; i++) {
        found = strcmp(layers[i].layerName, name) == 0;
    }
    free(layers);
    return found;
}

// extensions: the window's (from sdl), none for an offscreen target
//  - debug builds turn on the validation layer when it's installed (vulkan-validation-layers)
byte vulkan_create_instance(const char* const* extensions, uint extensions_count) {
    const char* validation = "VK_LAYER_KHRONOS_validation";
    byte validate = 0;
#ifdef zox_debug
    validate = vulkan_has_layer(validation);
#endif
    const char* all[16];
    uint count = 0;
    for (uint i = 0; i < extensions_count && count < 15; i++) {
        all[count++] = extensions[i];
    }
    if (validate) {
        all[count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }
    const VkApplicationInfo app = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "zoxel",
        .pEngineName = "zox",
        .apiVersion = VK_API_VERSION_1_1,
    };
    const VkInstanceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app,
        .enabledExtensionCount = count,
        .ppEnabledExtensionNames = all,
        .enabledLayerCount = validate,
        .ppEnabledLayerNames = &validation,
    };
    if (!vulkan_check(vkCreateInstance(&info, NULL, &vk_instance), "vkCreateInstance")) {
        return 0;
    }
    if (validate) {
        const VkDebugUtilsMessengerCreateInfoEXT messenger = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
            .pfnUserCallback = vulkan_on_message,
        };
        PFN_vkCreateDebugUtilsMessengerEXT create = (PFN_vkCreateDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(vk_instance, "vkCreateDebugUtilsMessengerEXT");
        if (create) {
            create(vk_instance, &messenger, NULL, &vk_messenger);
        }
        zox_log("+ vulkan validation on");
    }
    return 1;
}

void vulkan_dispose_instance() {
    if (vk_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT destroy = (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(vk_instance, "vkDestroyDebugUtilsMessengerEXT");
        if (destroy) {
            destroy(vk_instance, vk_messenger, NULL);
        }
        vk_messenger = VK_NULL_HANDLE;
    }
    if (vk_surface) {
        vkDestroySurfaceKHR(vk_instance, vk_surface, NULL);
        vk_surface = VK_NULL_HANDLE;
    }
    if (vk_instance) {
        vkDestroyInstance(vk_instance, NULL);
        vk_instance = VK_NULL_HANDLE;
    }
}
