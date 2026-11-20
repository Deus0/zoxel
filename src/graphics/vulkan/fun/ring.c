// Per frame in flight: rings of host memory (uniforms, client vertex arrays), descriptor pools, an upload command
// buffer; all free again once the frame's fence is waited (vulkan_frame_wait)

void vulkan_frame_wait() {
    if (vk_frame_waited) {
        return;
    }
    vkWaitForFences(vk_device, 1, &vk_fences[vk_frame], VK_TRUE, UINT64_MAX);
    if (vk_fence_serial[vk_frame] > vk_serial_done) {
        vk_serial_done = vk_fence_serial[vk_frame];
    }
    vulkan_garbage_collect();
    // every context's: only the main thread waits, between frames, no worker records then
    for (int c = 0; c < vk_contexts_max; c++) {
        vk_context* context = &vk_contexts[c];
        for (int i = 0; i < context->pools_count[vk_frame]; i++) {
            vkResetDescriptorPool(vk_device, context->pools[vk_frame][i], 0);
        }
        if (context->command_pools[vk_frame]) {
            vkResetCommandPool(vk_device, context->command_pools[vk_frame], 0);
        }
        context->pool_index = 0;
        context->ring_index = 0;
        context->ring_used = 0;
        context->secondary_index = 0;
    }
    vk_ring_epoch++;
    vk_frame_waited = 1;
}

// copies bytes into this frame's ring: the buffer and offset it's at, 0 if it couldn't
byte vulkan_ring_push(const void* data, VkDeviceSize size, VkBuffer* buffer, uint* offset) {
    if (size > vk_ring_size) {
        zox_loge("[vulkan] [%lu] bytes is over the ring's size", (ulong) size);
        return 0;
    }
    vulkan_frame_wait();
    VkDeviceSize start = (vk_ctx->ring_used + vk_uniform_align - 1) / vk_uniform_align * vk_uniform_align;
    if (start + size > vk_ring_size) {
        vk_ctx->ring_index++;
        start = 0;
    }
    if (vk_ctx->ring_index >= vk_rings_max) {
        zox_loge("[vulkan] the frame's rings are full");
        vk_ctx->ring_index = vk_rings_max - 1;
        return 0;
    }
    if (vk_ctx->ring_index >= vk_ctx->rings_count[vk_frame]) {
        const int i = vk_ctx->rings_count[vk_frame]++;
        if (!vulkan_create_host_buffer(vk_ring_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
            | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            &vk_ctx->rings[vk_frame][i], &vk_ctx->rings_memory[vk_frame][i]))
        {
            return 0;
        }
        vkMapMemory(vk_device, vk_ctx->rings_memory[vk_frame][i], 0, VK_WHOLE_SIZE, 0, (void**) &vk_ctx->rings_mapped[vk_frame][i]);
    }
    memcpy(vk_ctx->rings_mapped[vk_frame][vk_ctx->ring_index] + start, data, size);
    vk_ctx->ring_used = start + size;
    *buffer = vk_ctx->rings[vk_frame][vk_ctx->ring_index];
    *offset = start;
    return 1;
}

VkDescriptorPool vulkan_create_pool() {
    const VkDescriptorPoolSize sizes[2] = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 4096 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2048 },
    };
    const VkDescriptorPoolCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 2048,
        .poolSizeCount = 2,
        .pPoolSizes = sizes,
    };
    VkDescriptorPool pool = VK_NULL_HANDLE;
    vulkan_check(vkCreateDescriptorPool(vk_device, &info, NULL, &pool), "vkCreateDescriptorPool");
    return pool;
}

// a set from this frame's pools, another pool when they're full
VkDescriptorSet vulkan_allocate_set(VkDescriptorSetLayout layout) {
    while (vk_ctx->pool_index < vk_pools_max) {
        if (vk_ctx->pool_index >= vk_ctx->pools_count[vk_frame]) {
            vk_ctx->pools[vk_frame][vk_ctx->pools_count[vk_frame]++] = vulkan_create_pool();
        }
        const VkDescriptorSetAllocateInfo info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = vk_ctx->pools[vk_frame][vk_ctx->pool_index],
            .descriptorSetCount = 1,
            .pSetLayouts = &layout,
        };
        VkDescriptorSet set;
        if (vkAllocateDescriptorSets(vk_device, &info, &set) == VK_SUCCESS) {
            return set;
        }
        vk_ctx->pool_index++;
    }
    zox_loge("[vulkan] the frame's descriptor pools are full");
    return VK_NULL_HANDLE;
}

// the frame's upload commands (copies into textures), begun on first use, submitted before the frame's
VkCommandBuffer vulkan_upload_commands() {
    vulkan_frame_wait();
    const VkCommandBuffer commands = vk_uploads[vk_frame];
    if (!vk_uploading) {
        vkResetCommandBuffer(commands, 0);
        const VkCommandBufferBeginInfo begin = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        vkBeginCommandBuffer(commands, &begin);
        vk_uploading = 1;
    }
    return commands;
}

void vulkan_dispose_rings() {
    for (int c = 0; c < vk_contexts_max; c++) {
        vk_context* context = &vk_contexts[c];
        for (int f = 0; f < vk_frames_max; f++) {
            for (int i = 0; i < context->rings_count[f]; i++) {
                vkDestroyBuffer(vk_device, context->rings[f][i], NULL);
                vkFreeMemory(vk_device, context->rings_memory[f][i], NULL);
            }
            for (int i = 0; i < context->pools_count[f]; i++) {
                vkDestroyDescriptorPool(vk_device, context->pools[f][i], NULL);
            }
            if (context->command_pools[f]) {
                vkDestroyCommandPool(vk_device, context->command_pools[f], NULL);
            }
            free(context->secondaries[f]);
        }
        free(context->programs);
        memset(context, 0, sizeof(vk_context));
    }
}
