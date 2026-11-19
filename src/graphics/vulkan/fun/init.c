// after the instance (and the window's surface, vulkan_create_instance): the device and its frames
//  - the target's made at the first frame (a minimised window has none to make)
byte vulkan_initialize(int2 size) {
    vk_window_size = size;
    return vulkan_create_device() && vulkan_create_frames();
}

byte test_graphics() {
    return vk_device ? EXIT_SUCCESS : EXIT_FAILURE;
}

void vulkan_dispose() {
    if (vk_device) {
        vkDeviceWaitIdle(vk_device);
        vulkan_dispose_framebuffers();
        vkDestroyRenderPass(vk_device, vk_pass_clear, NULL);
        vkDestroyRenderPass(vk_device, vk_pass_load, NULL);
        vkDestroySwapchainKHR(vk_device, vk_swapchain, NULL);
        vulkan_dispose_frames();
        vkDestroyDevice(vk_device, NULL);
        vk_device = VK_NULL_HANDLE;
    }
    vulkan_dispose_instance();
}
