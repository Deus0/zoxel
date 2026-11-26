// a queue family that draws (and presents to the surface, if there is one), -1 if none
int vulkan_queue_family(VkPhysicalDevice physical) {
    uint count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &count, NULL);
    VkQueueFamilyProperties* families = malloc(sizeof(VkQueueFamilyProperties) * count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &count, families);
    int found = -1;
    for (uint i = 0; i < count && found < 0; i++) {
        VkBool32 presents = VK_TRUE;
        if (vk_surface) {
            vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, vk_surface, &presents);
        }
        if ((families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && presents) {
            found = i;
        }
    }
    free(families);
    return found;
}

// the best gpu that can draw: discrete, then integrated, then any (lavapipe, the software one tests use)
byte vulkan_pick_physical() {
    // the one another api says (OpenXR: the headset's gpu)
    if (vk_pick_physical_with) {
        if (!vulkan_check(vk_pick_physical_with(&vk_physical), "pick physical device") || !vk_physical ||
            vulkan_queue_family(vk_physical) < 0)
        {
            zox_loge("[vulkan] the device given can't draw");
            return 0;
        }
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(vk_physical, &properties);
        vk_on_gpu = properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_CPU;
        zox_log("+ vulkan device [%s] (given)", properties.deviceName);
        return 1;
    }
    uint count = 0;
    vkEnumeratePhysicalDevices(vk_instance, &count, NULL);
    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * count);
    vkEnumeratePhysicalDevices(vk_instance, &count, devices);
    int best_score = -1;
    for (uint i = 0; i < count; i++) {
        if (vulkan_queue_family(devices[i]) < 0) {
            continue;
        }
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[i], &properties);
        const int score = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 3 :
            properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? 2 : 1;
        if (score > best_score) {
            best_score = score;
            vk_physical = devices[i];
        }
    }
    free(devices);
    if (!vk_physical) {
        zox_loge("[vulkan] no device can draw%s", vk_surface ? " to the window" : "");
        return 0;
    }
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(vk_physical, &properties);
    vk_on_gpu = properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_CPU;
    zox_log("+ vulkan device [%s] api [%i.%i]", properties.deviceName,
        VK_API_VERSION_MAJOR(properties.apiVersion), VK_API_VERSION_MINOR(properties.apiVersion));
    return 1;
}

byte vulkan_create_device() {
    if (!vulkan_pick_physical()) {
        return 0;
    }
    vk_queue_family = vulkan_queue_family(vk_physical);
    const float priority = 1;
    const VkDeviceQueueCreateInfo queue = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = vk_queue_family,
        .queueCount = 1,
        .pQueuePriorities = &priority,
    };
    const char* swapchain = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    // gl's line widths and point sizes, where the gpu has them
    VkPhysicalDeviceFeatures supported;
    vkGetPhysicalDeviceFeatures(vk_physical, &supported);
    const VkPhysicalDeviceFeatures features = {
        .wideLines = supported.wideLines,
        .largePoints = supported.largePoints,
        .independentBlend = supported.independentBlend,
    };
    vk_wide_lines = supported.wideLines;
    vk_outlines = supported.independentBlend;
    // without largePoints every point is 1 pixel (particles tiny): some phone gpus lack it, logged to tell
    VkPhysicalDeviceProperties limits;
    vkGetPhysicalDeviceProperties(vk_physical, &limits);
    zox_log("+ vulkan points: large [%i] size [%.1f to %.1f], wide lines [%i]", supported.largePoints,
        limits.limits.pointSizeRange[0], limits.limits.pointSizeRange[1], supported.wideLines);
    const VkDeviceCreateInfo info = {
        .pEnabledFeatures = &features,
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue,
        .enabledExtensionCount = vk_surface ? 1 : 0,
        .ppEnabledExtensionNames = &swapchain,
    };
    const VkResult created = vk_create_device_with ? vk_create_device_with(vk_physical, &info, &vk_device) :
        vkCreateDevice(vk_physical, &info, NULL, &vk_device);
    if (!vulkan_check(created, "vkCreateDevice")) {
        return 0;
    }
    vkGetDeviceQueue(vk_device, vk_queue_family, 0, &vk_queue);
    return 1;
}

// a memory type with the bits a resource allows and the properties asked for
int vulkan_memory_type(uint allowed, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memory;
    vkGetPhysicalDeviceMemoryProperties(vk_physical, &memory);
    for (uint i = 0; i < memory.memoryTypeCount; i++) {
        if ((allowed & (1 << i)) && (memory.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return -1;
}

// host visible memory, on the gpu too where it can be (integrated gpus, resizable bar)
int vulkan_host_memory_type(uint allowed) {
    const VkMemoryPropertyFlags host = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    const int local = vulkan_memory_type(allowed, host | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    return local >= 0 ? local : vulkan_memory_type(allowed, host);
}

// an image of levels mips with its own memory, 0 if it couldn't
byte vulkan_create_image_levels(
    VkExtent2D extent,
    VkFormat format,
    VkImageUsageFlags usage,
    uint levels,
    VkImage* image,
    VkDeviceMemory* memory)
{
    const VkImageCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = { extent.width, extent.height, 1 },
        .mipLevels = levels,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    if (!vulkan_check(vkCreateImage(vk_device, &info, NULL, image), "vkCreateImage")) {
        return 0;
    }
    VkMemoryRequirements needs;
    vkGetImageMemoryRequirements(vk_device, *image, &needs);
    const VkMemoryAllocateInfo allocate = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = needs.size,
        .memoryTypeIndex = vulkan_memory_type(needs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    };
    if (!vulkan_check(vkAllocateMemory(vk_device, &allocate, NULL, memory), "vkAllocateMemory")) {
        return 0;
    }
    vkBindImageMemory(vk_device, *image, *memory, 0);
    return 1;
}

// an image with its own memory, 0 if it couldn't
byte vulkan_create_image(VkExtent2D extent, VkFormat format, VkImageUsageFlags usage, VkImage* image,
    VkDeviceMemory* memory)
{
    return vulkan_create_image_levels(extent, format, usage, 1, image, memory);
}

// a view of its first levels mips
VkImageView vulkan_create_view_levels(VkImage image, VkFormat format, VkImageAspectFlags aspect, uint levels) {
    const VkImageViewCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange = { aspect, 0, levels, 0, 1 },
    };
    VkImageView view = VK_NULL_HANDLE;
    vulkan_check(vkCreateImageView(vk_device, &info, NULL, &view), "vkCreateImageView");
    return view;
}

VkImageView vulkan_create_view(VkImage image, VkFormat format, VkImageAspectFlags aspect) {
    return vulkan_create_view_levels(image, format, aspect, 1);
}

// a host visible buffer (read backs, uploads), 0 if it couldn't
byte vulkan_create_host_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* buffer, VkDeviceMemory* memory) {
    const VkBufferCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
    };
    if (!vulkan_check(vkCreateBuffer(vk_device, &info, NULL, buffer), "vkCreateBuffer")) {
        return 0;
    }
    VkMemoryRequirements needs;
    vkGetBufferMemoryRequirements(vk_device, *buffer, &needs);
    const VkMemoryAllocateInfo allocate = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = needs.size,
        .memoryTypeIndex = vulkan_host_memory_type(needs.memoryTypeBits),
    };
    if (!vulkan_check(vkAllocateMemory(vk_device, &allocate, NULL, memory), "vkAllocateMemory")) {
        vkDestroyBuffer(vk_device, *buffer, NULL);
        return 0;
    }
    vkBindBufferMemory(vk_device, *buffer, *memory, 0);
    return 1;
}

// a gpu only buffer (meshes on gpus whose memory isn't host visible), 0 if it couldn't
byte vulkan_create_device_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* buffer, VkDeviceMemory* memory) {
    const VkBufferCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
    };
    if (!vulkan_check(vkCreateBuffer(vk_device, &info, NULL, buffer), "vkCreateBuffer")) {
        return 0;
    }
    VkMemoryRequirements needs;
    vkGetBufferMemoryRequirements(vk_device, *buffer, &needs);
    const VkMemoryAllocateInfo allocate = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = needs.size,
        .memoryTypeIndex = vulkan_memory_type(needs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    };
    if (!vulkan_check(vkAllocateMemory(vk_device, &allocate, NULL, memory), "vkAllocateMemory")) {
        vkDestroyBuffer(vk_device, *buffer, NULL);
        return 0;
    }
    vkBindBufferMemory(vk_device, *buffer, *memory, 0);
    return 1;
}

// meshes staged (gpu only memory, written by copies) unless the gpu's memory is host visible and big: one memory
// (integrated gpus, lavapipe) or resizable bar; a 256MB host visible window (most discrete gpus) isn't enough
//  - ZOX_VK_STAGING=1 stages anyway (testing the path on any gpu)
byte vulkan_pick_meshes_staged() {
    const char* forced = getenv("ZOX_VK_STAGING");
    if (forced && atoi(forced)) {
        return 1;
    }
    VkPhysicalDeviceMemoryProperties memory;
    vkGetPhysicalDeviceMemoryProperties(vk_physical, &memory);
    const VkMemoryPropertyFlags direct = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    for (uint i = 0; i < memory.memoryTypeCount; i++) {
        const VkMemoryType type = memory.memoryTypes[i];
        if ((type.propertyFlags & direct) == direct && memory.memoryHeaps[type.heapIndex].size >= ((VkDeviceSize) 1 << 30)) {
            return 0;
        }
    }
    return 1;
}
