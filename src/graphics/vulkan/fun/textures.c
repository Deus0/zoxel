// Textures: rgba images, uploaded through the frame's upload commands; nearest and repeating, as gl's are made
//  - rgb is widened to rgba (few gpus sample R8G8B8)
//  - a texture a submission still reads gets a new image when it's written, the old is freed after

static inline guint spawn_gpu_texture_buffer() {
    const uint id = vk_table_add(&vk_textures);
    vk_texture* texture = vk_table_get(&vk_textures, id);
    texture->live = 1;
    zox_gpu_textures_live++;
    return id;
}

void vulkan_texture_release(vk_texture* texture) {
    if (texture->image) {
        vulkan_garbage_add((vk_garbage_item) { .image = texture->image, .view = texture->view,
            .memory = texture->memory, .serial = texture->used });
    }
    texture->image = VK_NULL_HANDLE;
    texture->view = VK_NULL_HANDLE;
    texture->memory = VK_NULL_HANDLE;
}

static inline void zox_gpu_dispose_texture(guint id) {
    vk_texture* texture = vk_texture_of(id);
    if (!texture) {
        return;
    }
    vulkan_texture_release(texture);
    texture->live = 0;
    vk_table_remove(&vk_textures, id);
    zox_gpu_textures_live--;
}

void vulkan_image_barrier(VkCommandBuffer commands, VkImage image, VkImageLayout from, VkImageLayout to,
    VkAccessFlags src_access, VkAccessFlags dst_access, VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage)
{
    const VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = src_access,
        .dstAccessMask = dst_access,
        .oldLayout = from,
        .newLayout = to,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
    };
    vkCmdPipelineBarrier(commands, src_stage, dst_stage, 0, 0, NULL, 0, NULL, 1, &barrier);
}

// the texture is these rgba pixels (NULL: zeros) at size
void vulkan_texture_data(guint id, int2 size, const byte* rgba) {
    vk_texture* texture = vk_texture_of(id);
    if (!texture || size.x <= 0 || size.y <= 0) {
        return;
    }
    if (!texture->image || vk_busy(texture->used) || !int2_equals(texture->size, size)) {
        vulkan_texture_release(texture);
        const VkExtent2D extent = { size.x, size.y };
        if (!vulkan_create_image(extent, VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, &texture->image, &texture->memory))
        {
            return;
        }
        texture->view = vulkan_create_view(texture->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
        texture->size = size;
        texture->used = 0;
    }
    const VkDeviceSize bytes = (VkDeviceSize) size.x * size.y * 4;
    const vk_allocation staging = vulkan_slab_allocate(bytes);
    if (!staging.handle) {
        return;
    }
    if (rgba) {
        memcpy(staging.mapped, rgba, bytes);
    } else {
        memset(staging.mapped, 0, bytes);
    }
    const VkCommandBuffer commands = vulkan_upload_commands();
    vulkan_image_barrier(commands, texture->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT);
    const VkBufferImageCopy copy = {
        .bufferOffset = staging.offset,
        .imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
        .imageExtent = { size.x, size.y, 1 },
    };
    vkCmdCopyBufferToImage(commands, staging.buffer, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    vulkan_image_barrier(commands, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    // the image and staging buffer are read by the submission that copies them
    texture->used = vk_serial_recording();
    vulkan_garbage_add((vk_garbage_item) { .allocation = staging.handle, .serial = vk_serial_recording() });
}

static inline void zox_gpu_set_texture_color_rgba(guint id, int2 size, const void* ptr) {
    vulkan_texture_data(id, size, ptr);
}

static inline void zox_gpu_set_texture_color_rgb(guint id, int2 size, const void* ptr) {
    if (!ptr || size.x <= 0 || size.y <= 0) {
        vulkan_texture_data(id, size, NULL);
        return;
    }
    const byte* rgb = ptr;
    const int pixels = size.x * size.y;
    byte* rgba = malloc((size_t) pixels * 4);
    for (int i = 0; i < pixels; i++) {
        rgba[i * 4] = rgb[i * 3];
        rgba[i * 4 + 1] = rgb[i * 3 + 1];
        rgba[i * 4 + 2] = rgb[i * 3 + 2];
        rgba[i * 4 + 3] = 255;
    }
    vulkan_texture_data(id, size, rgba);
    free(rgba);
}

static inline void zox_gpu_clear_texture_rgba(guint id) {
    vulkan_texture_data(id, int2_one, NULL);
}

static inline void zox_gpu_clear_texture_rgb(guint id) {
    vulkan_texture_data(id, int2_one, NULL);
}

static inline void zox_gpu_bind_texture(guint id) {
    vk_ctx->texture_bound = id;
}

static inline void zox_gpu_reset_texture() {
    vk_ctx->texture_bound = 0;
}

byte vulkan_create_sampler() {
    const VkSamplerCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_NEAREST,
        .minFilter = VK_FILTER_NEAREST,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .maxLod = 0,
    };
    if (!vulkan_check(vkCreateSampler(vk_device, &info, NULL, &vk_sampler), "vkCreateSampler")) {
        return 0;
    }
    // what a sampler reads with no texture bound (gl reads black)
    const byte black[4] = { 0, 0, 0, 255 };
    vk_texture_black = spawn_gpu_texture_buffer();
    zox_gpu_textures_live--;    // not the game's
    vulkan_texture_data(vk_texture_black, int2_one, black);
    return 1;
}
