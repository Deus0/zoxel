extern byte zox_disable_post_processing;   // cameras module, later

// after the instance (and the window's surface, vulkan_create_instance): the device, its frames and fallbacks
//  - the target's made at the first frame (a minimised window has none to make)
byte vulkan_initialize(int2 size) {
    vk_window_size = size;
    vk_contexts[0].depth_write = 1;     // gl's defaults
    vk_contexts[0].color_write = 1;
    vk_contexts[0].line_width = 1;
    if (!vulkan_create_device() || !vulkan_create_frames()) {
        return 0;
    }
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(vk_physical, &properties);
    vk_uniform_align = properties.limits.minUniformBufferOffsetAlignment;
    VkFormatProperties rgb8;
    vkGetPhysicalDeviceFormatProperties(vk_physical, VK_FORMAT_R8G8B8_UNORM, &rgb8);
    vk_rgb8_vertex = (rgb8.bufferFeatures & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) != 0;
    if (!vulkan_create_host_buffer(64, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &vk_zeros, &vk_zeros_memory)) {
        return 0;
    }
    void* zeros;
    vkMapMemory(vk_device, vk_zeros_memory, 0, VK_WHOLE_SIZE, 0, &zeros);
    memset(zeros, 0, 64);
    vkUnmapMemory(vk_device, vk_zeros_memory);
    return vulkan_create_sampler();
}

byte test_graphics() {
    return vk_device ? EXIT_SUCCESS : EXIT_FAILURE;
}

void vulkan_dispose_tables() {
    for (uint i = 1; i <= vk_programs.count; i++) {
        zox_gpu_dispose_program(i);
    }
    for (uint i = 1; i <= vk_buffers.count; i++) {
        zox_gpu_dispose_buffer(i);
    }
    for (uint i = 1; i <= vk_textures.count; i++) {
        zox_gpu_dispose_texture(i);
    }
    for (uint i = 1; i <= vk_shaders.count; i++) {
        zox_gpu_dispose_shader(i);
    }
    vk_table_dispose(&vk_programs);
    vk_table_dispose(&vk_buffers);
    vk_table_dispose(&vk_textures);
    vk_table_dispose(&vk_shaders);
}

void vulkan_dispose() {
    vulkan_stop_workers();
    if (vk_device) {
        vkDeviceWaitIdle(vk_device);
        vk_serial_done = vk_serial_submitted;
        vulkan_dispose_tables();
        vulkan_garbage_dispose();
        vulkan_dispose_rings();
        vulkan_dispose_slabs();
        vkDestroySampler(vk_device, vk_sampler, NULL);
        vkDestroyBuffer(vk_device, vk_zeros, NULL);
        vkFreeMemory(vk_device, vk_zeros_memory, NULL);
        vulkan_dispose_framebuffers();
        vkDestroyRenderPass(vk_device, vk_pass_clear, NULL);
        vkDestroyRenderPass(vk_device, vk_pass_load, NULL);
        vkDestroySwapchainKHR(vk_device, vk_swapchain, NULL);
        vulkan_dispose_frames();
        vkDestroyDevice(vk_device, NULL);
        vk_device = VK_NULL_HANDLE;
    }
    if (vk_shaderc) {
        shaderc_compiler_release(vk_shaderc);
        vk_shaderc = NULL;
    }
    vulkan_dispose_instance();
}
