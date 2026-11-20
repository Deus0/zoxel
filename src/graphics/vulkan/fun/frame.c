// A frame: its command buffer opens at the first call that draws (vulkan_frame_begin), and
// app_gpu_end_system submits and presents it (vulkan_frame_end)

byte vulkan_create_frames() {
    const VkCommandPoolCreateInfo pool = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = vk_queue_family,
    };
    if (!vulkan_check(vkCreateCommandPool(vk_device, &pool, NULL, &vk_pool), "vkCreateCommandPool")) {
        return 0;
    }
    const VkCommandBufferAllocateInfo allocate = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vk_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = vk_frames_max,
    };
    if (!vulkan_check(vkAllocateCommandBuffers(vk_device, &allocate, vk_commands), "vkAllocateCommandBuffers")
        || !vulkan_check(vkAllocateCommandBuffers(vk_device, &allocate, vk_uploads), "vkAllocateCommandBuffers"))
    {
        return 0;
    }
    const VkSemaphoreCreateInfo semaphore = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    const VkFenceCreateInfo fence = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT };
    for (int i = 0; i < vk_frames_max; i++) {
        vkCreateSemaphore(vk_device, &semaphore, NULL, &vk_acquired[i]);
        vkCreateFence(vk_device, &fence, NULL, &vk_fences[i]);
    }
    return 1;
}

// the image to draw to: the swapchain's next, remade if it's out of date
byte vulkan_acquire() {
    if (vk_resized || !vk_framebuffers[0]) {
        if (!vulkan_recreate_target()) {
            return 0;
        }
    }
    if (!vk_surface) {
        vk_image = 0;
        return 1;
    }
    VkResult result = vkAcquireNextImageKHR(vk_device, vk_swapchain, UINT64_MAX, vk_acquired[vk_frame],
        VK_NULL_HANDLE, &vk_image);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vk_resized = 1;
        return 0;
    }
    return result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR;
}

// the frame's image as a pass's target: clearing it (vk_pass_clear) or carrying it on (vk_pass_load); contents
// inline for draws, or secondary command buffers (fun/workers.c)
void vulkan_pass_begin(VkCommandBuffer commands, VkRenderPass render_pass, VkSubpassContents contents) {
    const VkClearValue clears[2] = {
        { .color = { .float32 = { vk_clear_color.x, vk_clear_color.y, vk_clear_color.z, vk_clear_color.w } } },
        { .depthStencil = { 1, 0 } },
    };
    const VkRenderPassBeginInfo pass = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = render_pass,
        .framebuffer = vk_target_framebuffer,
        .renderArea = { { 0, 0 }, vk_target_extent },
        .clearValueCount = 2,
        .pClearValues = clears,
    };
    vkCmdBeginRenderPass(commands, &pass, contents);
}

void vulkan_target_frame(VkRenderPass pass);

// a render texture left bound: its pass ended, the frame's image the target again
void vulkan_target_frame_if_texture() {
    if (vk_fbo_bound && vk_recording) {
        vkCmdEndRenderPass(vk_contexts[0].commands);
        vulkan_target_frame(vk_pass_load);
    }
}

// opens the frame if it isn't, 0 if there's nothing to draw to (minimised, not made)
byte vulkan_frame_begin() {
    if (vk_recording) {
        return 1;
    }
    if (!vk_device) {
        return 0;
    }
    if (!vk_submitted) {
        vulkan_frame_wait();
        if (!vulkan_acquire()) {
            return 0;
        }
    }
    const VkCommandBuffer commands = vk_commands[vk_frame];
    vkResetCommandBuffer(commands, 0);
    const VkCommandBufferBeginInfo begin = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    vkBeginCommandBuffer(commands, &begin);
    vk_fbo_bound = 0;
    vk_target_texture = 0;
    vk_target_framebuffer = vk_framebuffers[vk_image];
    vk_target_extent = vk_extent;
    vulkan_pass_begin(commands, vk_submitted ? vk_pass_load : vk_pass_clear, VK_SUBPASS_CONTENTS_INLINE);
    vk_recording = 1;
    vk_contexts[0].commands = commands;
    vk_contexts[0].pipeline_bound = VK_NULL_HANDLE;
    vk_contexts[0].dynamic_dirty = 1;
    return 1;
}

// submits what's recorded (the pass already ended), after the uploads: the frame's first submit waits for its
// image, the last signals the present
void vulkan_submit(byte last) {
    const VkCommandBuffer buffers[2] = { vk_uploads[vk_frame], vk_commands[vk_frame] };
    vkEndCommandBuffer(buffers[1]);
    if (vk_uploading) {
        vkEndCommandBuffer(buffers[0]);
    }
    const VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const byte waits = vk_surface && !vk_submitted;
    const byte signals = vk_surface && last;
    const VkSubmitInfo submit = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = waits,
        .pWaitSemaphores = &vk_acquired[vk_frame],
        .pWaitDstStageMask = &stage,
        .commandBufferCount = vk_uploading ? 2 : 1,
        .pCommandBuffers = vk_uploading ? buffers : &buffers[1],
        .signalSemaphoreCount = signals,
        .pSignalSemaphores = &vk_rendered[vk_image],
    };
    vkResetFences(vk_device, 1, &vk_fences[vk_frame]);
    vulkan_check(vkQueueSubmit(vk_queue, 1, &submit, vk_fences[vk_frame]), "vkQueueSubmit");
    vk_fence_serial[vk_frame] = ++vk_serial_submitted;
    vk_uploading = 0;
    vk_recording = 0;
    vk_submitted = 1;
}

// submits and presents the frame (an empty one clears to vk_clear_color)
void vulkan_frame_end() {
    if (!vulkan_frame_begin()) {
        return;
    }
    vulkan_target_frame_if_texture();
    vkCmdEndRenderPass(vk_commands[vk_frame]);
    vulkan_submit(1);
    vk_submitted = 0;
    if (vk_surface) {
        const VkPresentInfoKHR present = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &vk_rendered[vk_image],
            .swapchainCount = 1,
            .pSwapchains = &vk_swapchain,
            .pImageIndices = &vk_image,
        };
        const VkResult result = vkQueuePresentKHR(vk_queue, &present);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            vk_resized = 1;
        }
    }
    vk_frame = (vk_frame + 1) % vk_frames_max;
    vk_frame_waited = 0;
    vk_frames_presented++;
}

// clears a rect of the target, in gl's coordinates (from the bottom left)
void vulkan_clear_rect(float2 position, float2 size) {
    if (!vulkan_frame_begin()) {
        return;
    }
    const VkClearAttachment clears[2] = {
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .colorAttachment = 0,
            .clearValue.color = { .float32 = { vk_clear_color.x, vk_clear_color.y, vk_clear_color.z, vk_clear_color.w } },
        },
        { .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .clearValue.depthStencil = { 1, 0 } },
    };
    const int x = int_max(0, (int) position.x);
    // a render texture's rows are gl's (bottom first), the frame's image's top first
    const int y = int_max(0, vk_target_texture ? (int) position.y : (int) (vk_target_extent.height - position.y - size.y));
    const VkClearRect rect = {
        .rect = {
            { x, y },
            { int_min((int) size.x, vk_target_extent.width - x), int_min((int) size.y, vk_target_extent.height - y) },
        },
        .layerCount = 1,
    };
    if (rect.rect.extent.width > 0 && rect.rect.extent.height > 0) {
        vkCmdClearAttachments(vk_contexts[0].commands, 2, clears, 1, &rect);
    }
}

static inline void zox_gpu_set_clear_color(float4 clear) {
    vk_clear_color = clear;
}

static inline void zox_gpu_clear_viewport() {
    vulkan_clear_rect(float2_zero, (float2) { vk_target_extent.width, vk_target_extent.height });
}

static inline void zox_gpu_clear_rect(float2 position, float2 size) {
    vulkan_clear_rect(position, size);
}

// waits for the gpu to finish all it was given (gl's glFinish)
static inline void zox_gpu_finish() {
    if (vk_device) {
        vkDeviceWaitIdle(vk_device);
    }
}

void vulkan_dispose_frames() {
    for (int i = 0; i < vk_frames_max; i++) {
        vkDestroySemaphore(vk_device, vk_acquired[i], NULL);
        vkDestroyFence(vk_device, vk_fences[i], NULL);
    }
    vkDestroyCommandPool(vk_device, vk_pool, NULL);
}
