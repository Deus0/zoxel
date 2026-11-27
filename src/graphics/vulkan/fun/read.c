// a half float's value (scrgb's frame read back)
static inline float vulkan_half_to_float(ushort h) {
    const int exponent = (h >> 10) & 31, mantissa = h & 1023;
    const float value = exponent == 0 ? mantissa / 16777216.0f :
        exponent == 31 ? 65504.0f : (1.0f + mantissa / 1024.0f) * ldexpf(1.0f, exponent - 15);
    return h & 0x8000 ? -value : value;
}

// The frame being drawn as rgb pixels, top row first, free it after; NULL if it can't (see the opengl module's)
//  - hdr output's (graphics/dat/hdr_output.c): its encoded values, 10 bit's top 8, scrgb's half floats clamped at 1
//  - submits what's drawn so far and waits for it, a later draw this frame carries on with vk_pass_load
static inline byte* zox_gpu_read_screen(int2 size) {
    if (size.x <= 0 || size.y <= 0 || !vulkan_frame_begin() || !vk_readable) {
        return NULL;
    }
    vulkan_target_frame_if_texture();
    const VkCommandBuffer commands = vk_commands[vk_frame];
    const VkImage image = vk_images[vk_image];
    const uint width = vk_extent.width;
    const uint height = vk_extent.height;
    VkBuffer buffer;
    VkDeviceMemory memory;
    const byte half = vk_color_format == VK_FORMAT_R16G16B16A16_SFLOAT;
    const uint pixel_bytes = half ? 8 : 4;
    if (!vulkan_create_host_buffer((VkDeviceSize) width * height * pixel_bytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT, &buffer, &memory)) {
        return NULL;
    }
    vkCmdEndRenderPass(commands);
    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
        .oldLayout = vk_final_layout,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
    };
    vkCmdPipelineBarrier(commands, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0, NULL, 0, NULL, 1, &barrier);
    const VkBufferImageCopy copy = {
        .imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
        .imageExtent = { width, height, 1 },
    };
    vkCmdCopyImageToBuffer(commands, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1, &copy);
    // back as the load pass expects it
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = vk_final_layout;
    vkCmdPipelineBarrier(commands, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0, 0, NULL, 0, NULL, 1, &barrier);
    vulkan_submit(0);
    vkWaitForFences(vk_device, 1, &vk_fences[vk_frame], VK_TRUE, UINT64_MAX);
    vk_serial_done = vk_serial_submitted;
    byte* rgb = malloc((size_t) size.x * size.y * 3);
    const byte* rgba = NULL;
    vkMapMemory(vk_device, memory, 0, VK_WHOLE_SIZE, 0, (void**) &rgba);
    // bgra swapchains (most) swap red and blue; past the image's edge is black
    const byte bgra = vk_color_format == VK_FORMAT_B8G8R8A8_UNORM || vk_color_format == VK_FORMAT_B8G8R8A8_SRGB;
    const byte bits10 = vk_color_format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 ? 1 :
        vk_color_format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 ? 2 : 0;
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            byte* to = &rgb[((size_t) y * size.x + x) * 3];
            if (x >= (int) width || y >= (int) height) {
                to[0] = to[1] = to[2] = 0;
                continue;
            }
            if (half) {
                const ushort* from = (const ushort*) &rgba[((size_t) y * width + x) * 8];
                for (byte c = 0; c < 3; c++) {
                    const float v = vulkan_half_to_float(from[c]);
                    to[c] = v <= 0 ? 0 : v >= 1 ? 255 : (byte) (v * 255 + 0.5f);
                }
                continue;
            }
            if (bits10) {
                const uint packed = *(const uint*) &rgba[((size_t) y * width + x) * 4];
                const byte low = (packed >> 2) & 255, middle = (packed >> 12) & 255, high = (packed >> 22) & 255;
                to[0] = bits10 == 1 ? low : high;
                to[1] = middle;
                to[2] = bits10 == 1 ? high : low;
                continue;
            }
            const byte* from = &rgba[((size_t) y * width + x) * 4];
            to[0] = from[bgra ? 2 : 0];
            to[1] = from[1];
            to[2] = from[bgra ? 0 : 2];
        }
    }
    vkUnmapMemory(vk_device, memory);
    vkDestroyBuffer(vk_device, buffer, NULL);
    vkFreeMemory(vk_device, memory, NULL);
    return rgb;
}
