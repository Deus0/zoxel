// Drawing into images another api owns (OpenXR's eye swapchains, xr/openxr/vulkan.c), as the frame's image is drawn
//  - an external target: the image (in VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL before and after, as OpenXR hands
//    its images over), a depth image of its own, a framebuffer; its pass is the frame pass's formats (R8G8B8A8
//    unorm, the offscreen frame's), so the frame's pipelines draw into it
//  - drawn as the frame is (vk_target_texture 0: y flipped by the viewport into gl's y up), so its rows are vulkan's,
//    top first, as OpenXR's vulkan images are read
//  - a split (workers.c) carries on in vk_pass_external_load: the frame's carry on pass would leave the image in
//    the frame's final layout, not the one OpenXR takes back
//  - vulkan_external_submit after drawing one: its commands queued before the other api lets go of the image
//    (xrReleaseSwapchainImage), the frame carries on after (vk_pass_load)
#define vk_external_targets_max 32

typedef struct {
    VkImageView view;
    VkImage depth;
    VkDeviceMemory depth_memory;
    VkImageView depth_view;
    VkFramebuffer framebuffer;
    VkExtent2D extent;
} vk_external_target;

vk_external_target vk_external_targets[vk_external_targets_max];
uint vk_external_targets_count = 0;

static inline VkRenderPass vulkan_create_external_pass(VkFormat format, byte begin) {
    const VkAttachmentDescription attachments[2] = {
        {
            .format = format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,       // the eyes clear their own rect
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        },
        {
            .format = vk_depth_format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = begin ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = begin ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
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
    const VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
            | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
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
    vulkan_check(vkCreateRenderPass(vk_device, &info, NULL, &pass), "vkCreateRenderPass (external)");
    return pass;
}

// a target drawing into image (format, size), its id (1 up), 0 if it couldn't be made
uint vulkan_external_target(VkImage image, VkFormat format, int2 size) {
    if (vk_external_targets_count >= vk_external_targets_max || !vk_device) {
        return 0;
    }
    if (!vk_depth_format) {
        vk_depth_format = vulkan_pick_depth_format();
    }
    if (!vk_pass_external) {
        vk_pass_external = vulkan_create_external_pass(format, 1);
        vk_pass_external_load = vulkan_create_external_pass(format, 0);
        if (!vk_pass_external || !vk_pass_external_load) {
            return 0;
        }
    }
    vk_external_target* target = &vk_external_targets[vk_external_targets_count];
    *target = (vk_external_target) { .extent = { size.x, size.y } };
    if (!vulkan_create_image(target->extent, vk_depth_format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        &target->depth, &target->depth_memory))
    {
        return 0;
    }
    target->view = vulkan_create_view(image, format, VK_IMAGE_ASPECT_COLOR_BIT);
    target->depth_view = vulkan_create_view(target->depth, vk_depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);
    const VkImageView attachments[2] = { target->view, target->depth_view };
    const VkFramebufferCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = vk_pass_external,
        .attachmentCount = 2,
        .pAttachments = attachments,
        .width = target->extent.width,
        .height = target->extent.height,
        .layers = 1,
    };
    if (!vulkan_check(vkCreateFramebuffer(vk_device, &info, NULL, &target->framebuffer), "vkCreateFramebuffer (external)")) {
        return 0;
    }
    return ++vk_external_targets_count;
}

// draws go into the external target until vulkan_external_submit
void vulkan_bind_external_target(uint id) {
    if (!id || id > vk_external_targets_count || !vulkan_frame_begin()) {
        return;
    }
    const vk_external_target* target = &vk_external_targets[id - 1];
    vkCmdEndRenderPass(vk_contexts[0].commands);
    vk_fbo_bound = 0;
    vk_target_texture = 0;
    vk_target_external = 1;
    vk_target_framebuffer = target->framebuffer;
    vk_target_extent = target->extent;
    vulkan_pass_begin(vk_contexts[0].commands, vk_pass_external, VK_SUBPASS_CONTENTS_INLINE);
    vk_contexts[0].pipeline_bound = VK_NULL_HANDLE;
    vk_contexts[0].dynamic_dirty = 1;
}

// what's drawn so far submitted (the external target's pass ended): it may be let go of now; draws after begin the
//  frame again on its own image (vk_pass_load)
void vulkan_external_submit() {
    if (!vk_recording) {
        return;
    }
    vkCmdEndRenderPass(vk_contexts[0].commands);
    vulkan_submit(0);
    vk_target_external = 0;
}

void vulkan_dispose_external_targets() {
    for (uint i = 0; i < vk_external_targets_count; i++) {
        vk_external_target* target = &vk_external_targets[i];
        vkDestroyFramebuffer(vk_device, target->framebuffer, NULL);
        vkDestroyImageView(vk_device, target->view, NULL);
        vkDestroyImageView(vk_device, target->depth_view, NULL);
        vkDestroyImage(vk_device, target->depth, NULL);
        vkFreeMemory(vk_device, target->depth_memory, NULL);
    }
    vk_external_targets_count = 0;
    if (vk_pass_external) {
        vkDestroyRenderPass(vk_device, vk_pass_external, NULL);
        vkDestroyRenderPass(vk_device, vk_pass_external_load, NULL);
        vk_pass_external = VK_NULL_HANDLE;
        vk_pass_external_load = VK_NULL_HANDLE;
    }
}
