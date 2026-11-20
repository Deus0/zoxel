// The target: what a frame draws to, the swapchain's images (or one offscreen image) and a depth buffer

VkFormat vulkan_pick_depth_format() {
    const VkFormat formats[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM };
    for (int i = 0; i < 3; i++) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(vk_physical, formats[i], &properties);
        if (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return formats[i];
        }
    }
    return VK_FORMAT_D16_UNORM;
}

// the window's format: 8 bits a channel, not srgb (colors look as they do on gl)
VkSurfaceFormatKHR vulkan_pick_surface_format() {
    uint count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical, vk_surface, &count, NULL);
    VkSurfaceFormatKHR* formats = malloc(sizeof(VkSurfaceFormatKHR) * count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical, vk_surface, &count, formats);
    VkSurfaceFormatKHR chosen = formats[0];
    for (uint i = 0; i < count; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM || formats[i].format == VK_FORMAT_R8G8B8A8_UNORM) {
            chosen = formats[i];
            break;
        }
    }
    free(formats);
    return chosen;
}

// clear: opens a frame, clearing it; load: carries on one after a read back (same layouts, so they're compatible)
VkRenderPass vulkan_create_pass(byte clear) {
    const VkAttachmentDescription attachments[2] = {
        {
            .format = vk_color_format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = clear ? VK_IMAGE_LAYOUT_UNDEFINED : vk_final_layout,
            .finalLayout = vk_final_layout,
        },
        {
            .format = vk_depth_format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = clear ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        },
    };
    const VkAttachmentReference color = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
    const VkAttachmentReference depth = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
    const VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color,
        .pDepthStencilAttachment = &depth,
    };
    // waits for the image to be acquired, and for the last frame's depth writes (one depth buffer)
    const VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
            | VK_PIPELINE_STAGE_TRANSFER_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
            | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    };
    const VkRenderPassCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 2,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };
    VkRenderPass pass = VK_NULL_HANDLE;
    vulkan_check(vkCreateRenderPass(vk_device, &info, NULL, &pass), "vkCreateRenderPass");
    return pass;
}

// the window's size in pixels, what the surface says unless it leaves it to us
//  - android gives it in the device's own orientation, turned (currentTransform) when the screen's rotated: the
//    swapchain is made upright (preTransform identity, the compositor turns it), so a quarter turn swaps its sides
VkExtent2D vulkan_surface_extent(const VkSurfaceCapabilitiesKHR* capabilities) {
    if (capabilities->currentExtent.width != UINT32_MAX) {
        VkExtent2D extent = capabilities->currentExtent;
        if (capabilities->currentTransform & (VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR | VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR)) {
            extent = (VkExtent2D) { extent.height, extent.width };
        }
        return extent;
    }
    VkExtent2D extent = { vk_window_size.x, vk_window_size.y };
    extent.width = int_max(capabilities->minImageExtent.width, int_min(extent.width, capabilities->maxImageExtent.width));
    extent.height = int_max(capabilities->minImageExtent.height, int_min(extent.height, capabilities->maxImageExtent.height));
    return extent;
}

// fifo waits for the screen (vsync, always there); without vsync mailbox, else immediate
VkPresentModeKHR vulkan_pick_present_mode() {
    if (vk_vsync) {
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    uint count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical, vk_surface, &count, NULL);
    VkPresentModeKHR modes[16];
    count = count < 16 ? count : 16;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical, vk_surface, &count, modes);
    VkPresentModeKHR chosen = VK_PRESENT_MODE_FIFO_KHR;
    for (uint i = 0; i < count; i++) {
        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return modes[i];
        }
        if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            chosen = modes[i];
        }
    }
    return chosen;
}

byte vulkan_create_swapchain() {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical, vk_surface, &capabilities);
    vk_extent = vulkan_surface_extent(&capabilities);
    if (!vk_extent.width || !vk_extent.height) {
        return 0;   // minimised, tried again next frame
    }
    vk_extent_made = 1;
    zox_log("+ vulkan swapchain [%ix%i] window [%ix%i] surface [%ix%i] turn [%i]", vk_extent.width, vk_extent.height,
        vk_window_size.x, vk_window_size.y, capabilities.currentExtent.width, capabilities.currentExtent.height,
        capabilities.currentTransform);
    const VkSurfaceFormatKHR format = vulkan_pick_surface_format();
    vk_color_format = format.format;
    uint images = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount && images > capabilities.maxImageCount) {
        images = capabilities.maxImageCount;
    }
    // screenshots and tests read the image back
    const VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        | (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    vk_readable = (usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0;
    const VkSwapchainKHR old = vk_swapchain;
    const VkSwapchainCreateInfoKHR info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = vk_surface,
        .minImageCount = images,
        .imageFormat = format.format,
        .imageColorSpace = format.colorSpace,
        .imageExtent = vk_extent,
        .imageArrayLayers = 1,
        .imageUsage = usage,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        // upright: the compositor turns it for a rotated screen (android); the game draws no turn of its own
        .preTransform = (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ?
            VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = vulkan_pick_present_mode(),
        .clipped = VK_TRUE,
        .oldSwapchain = old,
    };
    const byte made = vulkan_check(vkCreateSwapchainKHR(vk_device, &info, NULL, &vk_swapchain), "vkCreateSwapchainKHR");
    if (old) {
        vkDestroySwapchainKHR(vk_device, old, NULL);
    }
    if (!made) {
        vk_swapchain = VK_NULL_HANDLE;
        return 0;
    }
    vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &vk_images_count, NULL);
    if (vk_images_count > vk_images_max) {
        vk_images_count = vk_images_max;
    }
    vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &vk_images_count, vk_images);
    return 1;
}

// no window (--background, tests): one image the size asked for
byte vulkan_create_offscreen() {
    vk_extent = (VkExtent2D) { int_max(1, vk_window_size.x), int_max(1, vk_window_size.y) };
    vk_color_format = VK_FORMAT_R8G8B8A8_UNORM;
    vk_images_count = 1;
    vk_readable = 1;
    return vulkan_create_image(vk_extent, vk_color_format,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        &vk_images[0], &vk_offscreen_memory);
}

// the images' views, framebuffers and present semaphores, and the depth buffer
byte vulkan_create_framebuffers() {
    if (!vulkan_create_image(vk_extent, vk_depth_format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        &vk_depth, &vk_depth_memory))
    {
        return 0;
    }
    vk_depth_view = vulkan_create_view(vk_depth, vk_depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);
    const VkSemaphoreCreateInfo semaphore = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    for (uint i = 0; i < vk_images_count; i++) {
        vk_views[i] = vulkan_create_view(vk_images[i], vk_color_format, VK_IMAGE_ASPECT_COLOR_BIT);
        const VkImageView attachments[2] = { vk_views[i], vk_depth_view };
        const VkFramebufferCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = vk_pass_clear,
            .attachmentCount = 2,
            .pAttachments = attachments,
            .width = vk_extent.width,
            .height = vk_extent.height,
            .layers = 1,
        };
        if (!vulkan_check(vkCreateFramebuffer(vk_device, &info, NULL, &vk_framebuffers[i]), "vkCreateFramebuffer")) {
            return 0;
        }
        vkCreateSemaphore(vk_device, &semaphore, NULL, &vk_rendered[i]);
    }
    return 1;
}

// what's remade on a resize (the swapchain itself is replaced, not destroyed, so the window keeps showing)
void vulkan_dispose_framebuffers() {
    for (uint i = 0; i < vk_images_count; i++) {
        vkDestroyFramebuffer(vk_device, vk_framebuffers[i], NULL);
        vkDestroyImageView(vk_device, vk_views[i], NULL);
        vkDestroySemaphore(vk_device, vk_rendered[i], NULL);
        vk_framebuffers[i] = VK_NULL_HANDLE;
        vk_views[i] = VK_NULL_HANDLE;
        vk_rendered[i] = VK_NULL_HANDLE;
    }
    vkDestroyImageView(vk_device, vk_depth_view, NULL);
    vkDestroyImage(vk_device, vk_depth, NULL);
    vkFreeMemory(vk_device, vk_depth_memory, NULL);
    vk_depth_view = VK_NULL_HANDLE;
    vk_depth = VK_NULL_HANDLE;
    vk_depth_memory = VK_NULL_HANDLE;
    if (vk_offscreen_memory) {
        vkDestroyImage(vk_device, vk_images[0], NULL);
        vkFreeMemory(vk_device, vk_offscreen_memory, NULL);
        vk_offscreen_memory = VK_NULL_HANDLE;
    }
}

// the swapchain or offscreen image, their framebuffers; the passes on first use (their formats don't change)
byte vulkan_create_target() {
    if (!(vk_surface ? vulkan_create_swapchain() : vulkan_create_offscreen())) {
        return 0;
    }
    if (!vk_pass_clear) {
        vk_depth_format = vulkan_pick_depth_format();
        vk_final_layout = vk_surface ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        vk_pass_clear = vulkan_create_pass(1);
        vk_pass_load = vulkan_create_pass(0);
    }
    return vulkan_create_framebuffers();
}

// after a resize, or when the swapchain says it's out of date
byte vulkan_recreate_target() {
    vkDeviceWaitIdle(vk_device);
    vk_serial_done = vk_serial_submitted;
    vulkan_dispose_framebuffers();
    vk_resized = 0;
    return vulkan_create_target();
}

// android destroys the window's surface while the app's in the background: the swapchain and surface let go of,
// frames skipped until vulkan_surface_found gives it a new one (a black screen on coming back, 2026-11-20)
void vulkan_surface_lost() {
    if (!vk_surface || !vk_device) {
        return;
    }
    vkDeviceWaitIdle(vk_device);
    vk_serial_done = vk_serial_submitted;
    vulkan_dispose_framebuffers();
    vkDestroySwapchainKHR(vk_device, vk_swapchain, NULL);
    vk_swapchain = VK_NULL_HANDLE;
    vkDestroySurfaceKHR(vk_instance, vk_surface, NULL);
    vk_surface = VK_NULL_HANDLE;
    vk_surface_lost = 1;
}

// the window's new surface (made by the app, sdl): the swapchain's made again at the next frame
void vulkan_surface_found(VkSurfaceKHR surface) {
    vk_surface = surface;
    vk_surface_lost = 0;
    vk_resized = 1;
}

void vulkan_set_vsync(byte value) {
    if (value != vk_vsync) {
        vk_vsync = value;
        vk_resized = 1;
    }
}

// suboptimal (android says it every frame for an upright swapchain on a rotated screen): made again only if the
// surface's size changed, a rotation or a resize, not every frame
void vulkan_check_extent() {
    if (!vk_surface || !vk_swapchain) {
        return;
    }
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical, vk_surface, &capabilities);
    const VkExtent2D extent = vulkan_surface_extent(&capabilities);
    if (extent.width != vk_extent.width || extent.height != vk_extent.height) {
        vk_resized = 1;
    }
}

// a swapchain made since last asked, its size (what's drawn): the game's size follows it (apps/sdl events.c), once
//  - android gave sdl's size unturned at start while the swapchain was upright: the viewport was off the frame till a
//    rotation's resize
byte vulkan_take_extent(int2* size) {
    if (!vk_extent_made) {
        return 0;
    }
    vk_extent_made = 0;
    *size = (int2) { vk_extent.width, vk_extent.height };
    return 1;
}

void vulkan_resize(int2 size) {
    if (!int2_equals(size, vk_window_size)) {
        vk_window_size = size;
        vk_resized = 1;
    }
}
