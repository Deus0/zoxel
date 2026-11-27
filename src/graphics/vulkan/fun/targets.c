// Render textures (gl's fbos and rbos): a camera draws into a texture (the colour) with a depth image (the rbo),
// the post processing then draws that texture to the screen (cameras, ui render_textures)
//  - zox_gpu_bind_fbo ends the pass on the frame's image and begins one on the texture, zox_gpu_reset_fbo goes back
//    (vk_pass_load); split recording inherits whichever target is bound (vk_target_*)
//  - two passes: begin (its depth cleared) and load (carried on, after a split's restart); both keep the colour
//    (cameras clear their own rect) and leave it shader readable, for the post processing's draw this frame
//  - a texture written again (a resize) gets a new image: the fbo's framebuffer is made again for it
//  - outlines (doc/plans/outlines.md): the rbo has an outline mask image beside its depth (rg32ui, colour attachment
//    1, cleared as the pass begins), both left shader readable after the pass for the post processing's outline

typedef struct {
    uint texture;
    uint rbo;
    VkFramebuffer framebuffer;
    VkImageView view;           // what its framebuffer was made with
    VkImageView depth_view;
    VkExtent2D extent;
    byte live;
} vk_fbo;

typedef struct {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkImage mask_image;
    VkDeviceMemory mask_memory;
    VkImageView mask_view;
    int2 size;
    ulong used;
    byte live;
} vk_rbo;

vk_table vk_fbos = { .size = sizeof(vk_fbo) };
vk_table vk_rbos = { .size = sizeof(vk_rbo) };
byte zox_fbos_supported = 1;

static inline vk_fbo* vk_fbo_of(uint id) {
    vk_fbo* fbo = vk_table_get(&vk_fbos, id);
    return fbo && fbo->live ? fbo : NULL;
}

static inline vk_rbo* vk_rbo_of(uint id) {
    vk_rbo* rbo = vk_table_get(&vk_rbos, id);
    return rbo && rbo->live ? rbo : NULL;
}

#define vk_mask_format VK_FORMAT_R32G32_UINT

// its colour bytes, or half floats for an hdr camera (doc/plans/hdr.md)
VkRenderPass vulkan_create_texture_pass(byte begin, byte hdr) {
    const VkAttachmentDescription attachments[3] = {
        {
            .format = hdr ? vk_hdr_format : VK_FORMAT_R8G8B8A8_UNORM,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        },
        {
            .format = vk_depth_format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = begin ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            // read only after: the outline pass samples it
            .initialLayout = begin ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
        },
        {
            .format = vk_mask_format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = begin ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = begin ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        },
    };
    const VkAttachmentReference colors[2] = {
        { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
        { 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
    const VkAttachmentReference depth = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
    const VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = vk_outlines ? 2 : 1,
        .pColorAttachments = colors,
        .pDepthStencilAttachment = &depth,
    };
    // after its last sampling (and depth use), and before the next sampling of what it drew
    const VkSubpassDependency dependencies[2] = {
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        },
        {
            .srcSubpass = 0,
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
        },
    };
    const VkRenderPassCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = vk_outlines ? 3 : 2,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 2,
        .pDependencies = dependencies,
    };
    VkRenderPass pass = VK_NULL_HANDLE;
    vulkan_check(vkCreateRenderPass(vk_device, &info, NULL, &pass), "vkCreateRenderPass");
    return pass;
}

static inline guint zox_gpu_create_fbo() {
    const uint id = vk_table_add(&vk_fbos);
    ((vk_fbo*) vk_table_get(&vk_fbos, id))->live = 1;
    zox_gpu_fbos_live++;
    return id;
}

static inline guint zox_gpu_create_rbo() {
    const uint id = vk_table_add(&vk_rbos);
    ((vk_rbo*) vk_table_get(&vk_rbos, id))->live = 1;
    zox_gpu_rbos_live++;
    return id;
}

static inline void zox_gpu_link_fbo_rbo(guint fbo, guint rbo) {
    vk_fbo* target = vk_fbo_of(fbo);
    if (target) {
        target->rbo = rbo;
    }
}

static inline void zox_gpu_fbo_to_texture(guint fbo, guint texture) {
    vk_fbo* target = vk_fbo_of(fbo);
    if (target) {
        target->texture = texture;
    }
}

void vulkan_rbo_release(vk_rbo* rbo) {
    if (rbo->image) {
        vulkan_garbage_add((vk_garbage_item) { .image = rbo->image, .view = rbo->view, .memory = rbo->memory,
            .serial = rbo->used });
    }
    if (rbo->mask_image) {
        vulkan_garbage_add((vk_garbage_item) { .image = rbo->mask_image, .view = rbo->mask_view,
            .memory = rbo->mask_memory, .serial = rbo->used });
    }
    rbo->image = VK_NULL_HANDLE;
    rbo->view = VK_NULL_HANDLE;
    rbo->memory = VK_NULL_HANDLE;
    rbo->mask_image = VK_NULL_HANDLE;
    rbo->mask_view = VK_NULL_HANDLE;
    rbo->mask_memory = VK_NULL_HANDLE;
}

// the rbo's depth image at size (a new one if its size changed)
static inline void zox_gpu_set_rbo_size(guint id, int2 size) {
    vk_rbo* rbo = vk_rbo_of(id);
    if (!rbo || size.x <= 0 || size.y <= 0 || (rbo->image && int2_equals(rbo->size, size))) {
        return;
    }
    vulkan_rbo_release(rbo);
    const VkExtent2D extent = { size.x, size.y };
    if (!vulkan_create_image(extent, vk_depth_format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT, &rbo->image, &rbo->memory) || (vk_outlines && !vulkan_create_image(extent,
        vk_mask_format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, &rbo->mask_image,
        &rbo->mask_memory)))
    {
        return;
    }
    rbo->view = vulkan_create_view(rbo->image, vk_depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);
    if (vk_outlines) {
        rbo->mask_view = vulkan_create_view(rbo->mask_image, vk_mask_format, VK_IMAGE_ASPECT_COLOR_BIT);
    }
    rbo->size = size;
    rbo->used = 0;
}

static inline void zox_gpu_dispose_rbo(guint id) {
    vk_rbo* rbo = vk_rbo_of(id);
    if (!rbo) {
        return;
    }
    vulkan_rbo_release(rbo);
    rbo->live = 0;
    vk_table_remove(&vk_rbos, id);
    zox_gpu_rbos_live--;
}

static inline void zox_gpu_dispose_fbo(guint id) {
    vk_fbo* fbo = vk_fbo_of(id);
    if (!fbo) {
        return;
    }
    if (fbo->framebuffer) {
        vulkan_garbage_add((vk_garbage_item) { .framebuffer = fbo->framebuffer, .serial = vk_serial_recording() });
    }
    fbo->live = 0;
    vk_table_remove(&vk_fbos, id);
    zox_gpu_fbos_live--;
}

// the frame's image as the target again (after a render texture), a pass begun on it
void vulkan_target_frame(VkRenderPass pass) {
    vk_fbo_bound = 0;
    vk_target_texture = 0;
    vk_target_external = 0;
    vk_target_framebuffer = vk_framebuffers[vk_image];
    vk_target_extent = vk_extent;
    vulkan_pass_begin(vk_contexts[0].commands, pass, VK_SUBPASS_CONTENTS_INLINE);
    vk_contexts[0].pipeline_bound = VK_NULL_HANDLE;
    vk_contexts[0].dynamic_dirty = 1;
}

// draws go into the fbo's texture (and its rbo's depth) until zox_gpu_reset_fbo
static inline void zox_gpu_bind_fbo(guint id) {
    vk_fbo* fbo = vk_fbo_of(id);
    vk_texture* texture = fbo ? vk_texture_of(fbo->texture) : NULL;
    vk_rbo* rbo = fbo ? vk_rbo_of(fbo->rbo) : NULL;
    if (!texture || !texture->image || !rbo || !rbo->image || (vk_outlines && !rbo->mask_image) ||
        !vulkan_frame_begin())
    {
        return;
    }
    const byte hdr = texture->hdr;
    if (!vk_pass_texture_begin[hdr]) {
        vk_pass_texture_begin[hdr] = vulkan_create_texture_pass(1, hdr);
        vk_pass_texture_load[hdr] = vulkan_create_texture_pass(0, hdr);
    }
    const ulong serial = vk_serial_recording();
    const VkExtent2D extent = { int_min(texture->size.x, rbo->size.x), int_min(texture->size.y, rbo->size.y) };
    // with mips (bloom) it draws to the first level
    const VkImageView color_view = texture->target_view ? texture->target_view : texture->view;
    if (!fbo->framebuffer || fbo->view != color_view || fbo->depth_view != rbo->view) {
        if (fbo->framebuffer) {
            vulkan_garbage_add((vk_garbage_item) { .framebuffer = fbo->framebuffer, .serial = serial });
        }
        const VkImageView attachments[3] = { color_view, rbo->view, rbo->mask_view };
        const VkFramebufferCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = vk_pass_texture_begin[hdr],
            .attachmentCount = vk_outlines ? 3 : 2,
            .pAttachments = attachments,
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        };
        fbo->framebuffer = VK_NULL_HANDLE;
        if (!vulkan_check(vkCreateFramebuffer(vk_device, &info, NULL, &fbo->framebuffer), "vkCreateFramebuffer")) {
            return;
        }
        fbo->view = color_view;
        fbo->depth_view = rbo->view;
        fbo->extent = extent;
    }
    texture->used = serial;
    rbo->used = serial;
    vkCmdEndRenderPass(vk_contexts[0].commands);
    vk_fbo_bound = id;
    vk_target_texture = 1 + hdr;
    vk_target_external = 0;
    vk_target_framebuffer = fbo->framebuffer;
    vk_target_extent = fbo->extent;
    vulkan_pass_begin(vk_contexts[0].commands, vk_pass_texture_begin[hdr], VK_SUBPASS_CONTENTS_INLINE);
    vk_contexts[0].pipeline_bound = VK_NULL_HANDLE;
    vk_contexts[0].dynamic_dirty = 1;
}

static inline void zox_gpu_reset_fbo() {
    if (!vk_fbo_bound || !vk_recording) {
        return;
    }
    vkCmdEndRenderPass(vk_contexts[0].commands);
    // bloom's mips, between the passes
    const vk_fbo* fbo = vk_fbo_of(vk_fbo_bound);
    vk_texture* texture = fbo ? vk_texture_of(fbo->texture) : NULL;
    if (texture && texture->image && texture->mips > 1) {
        vulkan_texture_mips(vk_contexts[0].commands, texture);
    }
    const guint was = vk_fbo_bound;
    vulkan_target_frame(vk_pass_load);
    // hdr output: back to its canvas, not the screen (graphics/dat/hdr_output.c)
    if (zox_gpu_default_fbo && was != zox_gpu_default_fbo) {
        zox_gpu_bind_fbo(zox_gpu_default_fbo);
    }
}

void vulkan_dispose_targets() {
    for (uint i = 1; i <= vk_fbos.count; i++) {
        zox_gpu_dispose_fbo(i);
    }
    for (uint i = 1; i <= vk_rbos.count; i++) {
        zox_gpu_dispose_rbo(i);
    }
    vk_table_dispose(&vk_fbos);
    vk_table_dispose(&vk_rbos);
    for (byte i = 0; i < 2; i++) {
        vkDestroyRenderPass(vk_device, vk_pass_texture_begin[i], NULL);
        vkDestroyRenderPass(vk_device, vk_pass_texture_load[i], NULL);
        vk_pass_texture_begin[i] = vk_pass_texture_load[i] = VK_NULL_HANDLE;
    }
}

// outlines (doc/plans/outlines.md): the mask cleared as a render texture's pass begins, drawn to by outlined models
static inline byte zox_gpu_outlines_supported() {
    return vk_outlines;
}

// an rbo's outline mask, as a unit's id (zox_gpu_bind_texture_unit)
static inline guint zox_gpu_rbo_mask(guint rbo) {
    return rbo ? rbo | vk_rbo_mask_bit : 0;
}

static inline void zox_gpu_outline_mask(byte on) {
    vk_ctx->outline_mask = on;
}

// cleared by the pass' begin
static inline void zox_gpu_clear_outline_mask() { }

// units past 0 take a render camera's depth (its rbo) or outline mask (zox_gpu_rbo_mask)
static inline void zox_gpu_bind_texture_unit(byte unit, guint id) {
    if (unit < vk_units_max) {
        vk_ctx->units_bound[unit] = id;
    }
}

// a unit's image for a sampler: 0 the bound texture (black if none), past it an rbo's depth or mask (black if none)
VkDescriptorImageInfo vulkan_unit_image(byte unit) {
    const ulong serial = vk_serial_recording();
    if (unit > 0 && unit < vk_units_max && vk_ctx->units_bound[unit]) {
        const uint id = vk_ctx->units_bound[unit];
        vk_rbo* rbo = vk_rbo_of(id & ~vk_rbo_mask_bit);
        if (rbo && rbo->image) {
            rbo->used = serial;
            return id & vk_rbo_mask_bit ?
                (VkDescriptorImageInfo) { vk_sampler, rbo->mask_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } :
                (VkDescriptorImageInfo) { vk_sampler, rbo->view, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL };
        }
    }
    vk_texture* texture = unit == 0 ? vk_texture_of(vk_ctx->texture_bound) : NULL;
    if (!texture || !texture->image) {
        texture = vk_texture_of(vk_texture_black);
    }
    texture->used = serial;
    return (VkDescriptorImageInfo) { texture->mips > 1 ? vk_sampler_mips : vk_sampler, texture->view,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
}
