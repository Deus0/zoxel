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
    if (texture->target_view) {
        vulkan_garbage_add((vk_garbage_item) { .view = texture->target_view, .serial = texture->used });
    }
    texture->target_view = VK_NULL_HANDLE;
    texture->mips = 0;
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
    if (!texture->image || texture->hdr || vk_busy(texture->used) || !int2_equals(texture->size, size)) {
        vulkan_texture_release(texture);
        texture->hdr = 0;
        const VkExtent2D extent = { size.x, size.y };
        if (!vulkan_create_image(extent, VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            &texture->image, &texture->memory))
        {
            return;
        }
        texture->view = vulkan_create_view(texture->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
        texture->size = size;
        texture->used = 0;
    }
    const VkDeviceSize bytes = (VkDeviceSize) size.x * size.y * 4;
    const vk_allocation staging = vulkan_slab_allocate(bytes, 0);
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

// a camera's colour target can hold colours past 1 (doc/plans/hdr.md), while a tonemap is on: half floats are a
//  colour attachment, blended and sampled on every vulkan gpu
#define vk_hdr_format VK_FORMAT_R16G16B16A16_SFLOAT

static inline byte zox_gpu_hdr_supported() {
    return 1;
}

// bloom's mips: half floats blitted down, filtered (doc/plans/hdr.md); vulkan promises these on RGBA16F, checked
static inline byte zox_gpu_bloom_supported() {
    static byte supported = 2;
    if (supported == 2) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(vk_physical, vk_hdr_format, &properties);
        const VkFormatFeatureFlags needs = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
        supported = (properties.optimalTilingFeatures & needs) == needs;
    }
    return supported;
}

// a camera's colour target in half floats, cleared to zero, left shader readable as render textures' passes expect
//  - mips (bloom): every level, its pass draws to the first (target_view), they're made as it ends
static inline void zox_gpu_set_texture_color_hdr(guint id, int2 size, byte mips) {
    vk_texture* texture = vk_texture_of(id);
    if (!texture || size.x <= 0 || size.y <= 0) {
        return;
    }
    vulkan_texture_release(texture);
    uint levels = 1;
    if (mips) {
        for (int longest = int_max(size.x, size.y); longest > 1; longest /= 2) {
            levels++;
        }
    }
    const VkExtent2D extent = { size.x, size.y };
    if (!vulkan_create_image_levels(extent, vk_hdr_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, levels, &texture->image, &texture->memory))
    {
        return;
    }
    texture->view = vulkan_create_view_levels(texture->image, vk_hdr_format, VK_IMAGE_ASPECT_COLOR_BIT, levels);
    texture->target_view = levels > 1 ? vulkan_create_view(texture->image, vk_hdr_format, VK_IMAGE_ASPECT_COLOR_BIT) :
        VK_NULL_HANDLE;
    texture->size = size;
    texture->hdr = 1;
    texture->mips = levels;
    const VkCommandBuffer commands = vulkan_upload_commands();
    const VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, levels, 0, 1 };
    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = texture->image,
        .subresourceRange = range,
    };
    vkCmdPipelineBarrier(commands, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0,
        NULL, 1, &barrier);
    const VkClearColorValue zero = { .float32 = { 0, 0, 0, 0 } };
    vkCmdClearColorImage(commands, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &zero, 1, &range);
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    vkCmdPipelineBarrier(commands, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0,
        NULL, 1, &barrier);
    texture->used = vk_serial_recording();
}

// a texture's mips from its first level, each blitted from the one before (outside a render pass: as its camera's
//  pass ends, zox_gpu_reset_fbo); all left shader readable for the post pass' bloom
void vulkan_texture_mips(VkCommandBuffer commands, vk_texture* texture) {
    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = texture->image,
        .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
    };
    // the first level as drawn, read by the blit
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    vkCmdPipelineBarrier(commands, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
        NULL, 0, NULL, 1, &barrier);
    int width = texture->size.x, height = texture->size.y;
    for (uint level = 1; level < texture->mips; level++) {
        const int next_width = int_max(1, width / 2), next_height = int_max(1, height / 2);
        // last frame's, discarded once its bloom read it
        barrier.subresourceRange.baseMipLevel = level;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        vkCmdPipelineBarrier(commands, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
            NULL, 0, NULL, 1, &barrier);
        const VkImageBlit blit = {
            .srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, level - 1, 0, 1 },
            .srcOffsets = { { 0, 0, 0 }, { width, height, 1 } },
            .dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, level, 0, 1 },
            .dstOffsets = { { 0, 0, 0 }, { next_width, next_height, 1 } },
        };
        vkCmdBlitImage(commands, texture->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
        // read by the next level's blit
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        vkCmdPipelineBarrier(commands, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0,
            NULL, 1, &barrier);
        width = next_width;
        height = next_height;
    }
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = texture->mips;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    vkCmdPipelineBarrier(commands, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0,
        NULL, 1, &barrier);
}

// made as its pass ends (zox_gpu_reset_fbo), gl makes them here
static inline void zox_gpu_fbo_mips(guint fbo) {
    (void) fbo;
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
    // bloom's mips (doc/plans/hdr.md): linear within and between them, nearest up close (a downscaled view stays
    //  pixelated); clamped, its blur doesn't wrap round the view's edges
    const VkSamplerCreateInfo mips_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_NEAREST,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .maxLod = 16,
    };
    if (!vulkan_check(vkCreateSampler(vk_device, &mips_info, NULL, &vk_sampler_mips), "vkCreateSampler")) {
        return 0;
    }
    // what a sampler reads with no texture bound (gl reads black)
    const byte black[4] = { 0, 0, 0, 255 };
    vk_texture_black = spawn_gpu_texture_buffer();
    zox_gpu_textures_live--;    // not the game's
    vulkan_texture_data(vk_texture_black, int2_one, black);
    return 1;
}
