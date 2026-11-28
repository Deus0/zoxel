// Compute (gpu meshing, doc/plans/gpu_meshing.md), as gl does it: in the order it's called
//  - a dispatch mid frame goes in the frame's commands, its pass paused round it (a draw after sees what it wrote);
//    outside a frame it goes in the upload commands, before the frame's draws; barriers either side
//  - storage buffers are host visible slabs (written by the cpu, the face count read back)
//  - zox_gpu_storage_read waits for the work that writes the buffer, as gl's read does (a stall)
//  - a storage mesh "vao" is which buffers feed a draw program's vertices, lights and indices; drawn indirect

typedef struct {
    uint vertices;
    uint indices;
    uint location;
    uint extra;
    uint extra_location;
    uint uvs;
    uint uvs_location;
    byte live;
    uint extra2_location;   // a second uint a vertex in extra after the first (terrain's blocks' light), 0 none
} vk_vao;

vk_table vk_vaos = { .size = sizeof(vk_vao) };
uint vk_storage_bound[vk_slots_max];
uint vk_compute_bound;

#define vk_binding_compute_block 16     // a compute stage's uniforms, past its storage buffers' bindings

static inline byte zox_gpu_compute_supported() {
    return 1;
}

// a compute program from its source (gl's: no #version), 0 if it failed (logged)
guint zox_gpu_compute_program(const char* source) {
    const uint stage_id = vulkan_compile_stage(source, 2);
    const vk_shader* stage = vk_shader_of(stage_id);
    if (!stage) {
        return 0;
    }
    const uint id = vk_table_add(&vk_programs);
    vk_program* program = vk_table_get(&vk_programs, id);
    program->live = 1;
    program->compute = 1;
    zox_gpu_programs_live++;
    uint* words = malloc(stage->count * 4);
    memcpy(words, stage->words, stage->count * 4);
    VkDescriptorSetLayoutBinding bindings[vk_slots_max + 1];
    int bindings_count = 0;
    for (int i = 0; i < stage->storages_count; i++) {
        bindings[bindings_count++] = (VkDescriptorSetLayoutBinding) { stage->storages[i].value,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, NULL };
        program->storages[program->storages_count++] = stage->storages[i].value;
    }
    if (stage->block_word >= 0) {
        words[stage->block_word] = vk_binding_compute_block;
        bindings[bindings_count++] = (VkDescriptorSetLayoutBinding) { vk_binding_compute_block,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_COMPUTE_BIT, NULL };
        program->data_size[0] = stage->block_size;
        for (int m = 0; m < stage->members_count; m++) {
            const int index = vulkan_uniform_add(program, stage->members[m].name);
            program->uniforms[index].offset[0] = stage->members[m].offset;
            program->uniforms[index].stride[0] = stage->members[m].stride;
        }
    }
    program->modules[0] = vulkan_create_module(words, stage->count);
    free(words);
    zox_gpu_dispose_shader(stage_id);
    const VkDescriptorSetLayoutCreateInfo set_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = bindings_count,
        .pBindings = bindings,
    };
    vkCreateDescriptorSetLayout(vk_device, &set_info, NULL, &program->set_layout);
    const VkPipelineLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &program->set_layout,
    };
    vkCreatePipelineLayout(vk_device, &layout_info, NULL, &program->layout);
    const VkComputePipelineCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = program->modules[0], .pName = "main" },
        .layout = program->layout,
    };
    if (!program->modules[0] || !vulkan_check(vkCreateComputePipelines(vk_device, VK_NULL_HANDLE, 1, &info, NULL,
        &program->compute_pipeline), "vkCreateComputePipelines"))
    {
        zox_gpu_dispose_program(id);
        return 0;
    }
    return id;
}

// a draw program from gl sources (no #version), as zox_gpu_compile_shader and a material make one
guint zox_gpu_draw_program(const char* vertex_source, const char* fragment_source) {
    const guint2 shader = zox_gpu_compile_shader(vertex_source, fragment_source);
    if (!shader.x) {
        return 0;
    }
    const guint program = spawn_gpu_material_program(shader);
    zox_gpu_dispose_shader(shader.x);
    zox_gpu_dispose_shader(shader.y);
    return program;
}

// gl's glUseProgram for compute and draw programs both (0 lets go of either)
static inline void zox_gpu_compute_use(guint id) {
    const vk_program* program = vk_program_of(id);
    if (program && program->compute) {
        vk_compute_bound = id;
    } else {
        vk_compute_bound = 0;
        vk_ctx->program_bound = id;
    }
}

static inline void zox_gpu_compute_uniform_int(guint program, const char* name, int value) {
    vulkan_program_set(program, name, &value, 4, 1);
}

static inline void zox_gpu_compute_uniform_uint(guint program, const char* name, uint value) {
    vulkan_program_set(program, name, &value, 4, 1);
}

static inline void zox_gpu_compute_uniform_uint_array(guint program, const char* name, const uint* values, int count) {
    vulkan_program_set(program, name, values, 4, count);
}

// Storage buffers

guint zox_gpu_storage_buffer(size_t size, const void* data) {
    const guint id = zox_gpu_create_buffer();
    vk_buffer_of(id)->host = 1;
    vulkan_buffer_data(id, data, size);
    return id;
}

// a storage buffer the cpu reads back: host visible as every storage buffer here
guint zox_gpu_storage_buffer_read(size_t size, const void* data) {
    return zox_gpu_storage_buffer(size, data);
}

static inline void zox_gpu_storage_upload(guint buffer, size_t size, const void* data) {
    vulkan_buffer_data(buffer, data, size);
}

// gl's glBufferSubData at offset, the rest kept
static inline void zox_gpu_storage_write(guint id, size_t offset, size_t size, const void* data) {
    vk_buffer* buffer = vk_buffer_of(id);
    if (!buffer || !data || !size) {
        return;
    }
    const VkDeviceSize end = offset + size;
    if (!vulkan_buffer_reserve(buffer, end > buffer->size ? end : buffer->size, 1)) {
        return;
    }
    vulkan_buffer_write(buffer, offset, data, size);
    if (end > buffer->size) {
        buffer->size = end;
    }
}

static inline void zox_gpu_storage_bind(guint binding, guint buffer) {
    if (binding < vk_slots_max) {
        vk_storage_bound[binding] = buffer;
    }
}

// a fence after what's recorded so far: its submission's serial (gl's glFenceSync)
static inline ulong zox_gpu_fence() {
    return vk_serial_recording();
}

// has the gpu passed the fence (it never waits), reads of what came before it won't stall then
//  - asks the frames' fences (vkGetFenceStatus), not only vk_serial_done: that moves when a frame's slot is waited on
//    for reuse, ~3 frames after it ran (gpu meshes' counts came back 3 frames late, streaming ~3x slower than gl)
//  - one queue finishes in order: a done submission means every serial up to its own is done
static inline byte zox_gpu_fence_done(ulong fence) {
    if (!vk_busy(fence)) {
        return 1;
    }
    if (fence > vk_serial_submitted) {
        return 0;
    }
    for (int f = 0; f < vk_frames_max; f++) {
        if (vk_fence_serial[f] >= fence && vk_fences[f] && vkGetFenceStatus(vk_device, vk_fences[f]) == VK_SUCCESS) {
            if (vk_fence_serial[f] > vk_serial_done) {
                vk_serial_done = vk_fence_serial[f];
            }
            return 1;
        }
    }
    return 0;
}

static inline void zox_gpu_fence_dispose(ulong fence) {
    (void) fence;
}

// what the gpu wrote (waits for it, as gl's read does), 1 if it could
static inline byte zox_gpu_storage_read(guint id, size_t offset, size_t size, void* out) {
    vk_buffer* buffer = vk_buffer_of(id);
    if (!buffer || !buffer->mapped || offset + size > buffer->capacity) {
        return 0;
    }
    if (vk_busy(buffer->used)) {
        if (buffer->used > vk_serial_submitted) {
            // recorded, not submitted: the frame so far submitted (it carries on after, vk_pass_load)
            if (!vulkan_frame_begin()) {
                return 0;
            }
            vulkan_target_frame_if_texture();
            vkCmdEndRenderPass(vk_contexts[0].commands);
            vulkan_submit(0);
        }
        vkQueueWaitIdle(vk_queue);
        vk_serial_done = vk_serial_submitted;
    }
    memcpy(out, buffer->mapped + offset, size);
    return 1;
}

// everything before it (draws reading, the cpu's writes) done before a dispatch, or its results before what's after
void vulkan_compute_barrier(VkCommandBuffer commands, byte after) {
    const VkAccessFlags reads = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT
        | VK_ACCESS_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT;
    const VkPipelineStageFlags others = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
        | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    const VkMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
        .srcAccessMask = after ? VK_ACCESS_SHADER_WRITE_BIT : reads | VK_ACCESS_SHADER_WRITE_BIT
            | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_HOST_WRITE_BIT,
        .dstAccessMask = after ? reads | VK_ACCESS_HOST_READ_BIT : VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
    };
    vkCmdPipelineBarrier(commands, after ? VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT : others | VK_PIPELINE_STAGE_HOST_BIT,
        after ? others | VK_PIPELINE_STAGE_HOST_BIT : VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 1, &barrier, 0, NULL, 0, NULL);
}

static inline void zox_gpu_compute_dispatch(uint groups_x, uint groups_y, uint groups_z) {
    vk_program* program = vk_program_of(vk_compute_bound);
    if (!program || !program->compute_pipeline) {
        return;
    }
    vk_program_state* state = vulkan_program_state(vk_compute_bound, program);
    VkBuffer ring = VK_NULL_HANDLE;
    uint ring_offset = 0;
    if (program->data_size[0] && !vulkan_ring_push(state->data[0], program->data_size[0], &ring, &ring_offset)) {
        return;
    }
    const VkDescriptorSet set = vulkan_allocate_set(program->set_layout);
    if (!set) {
        return;
    }
    const ulong serial = vk_serial_recording();
    VkDescriptorBufferInfo infos[vk_slots_max + 1];
    VkWriteDescriptorSet writes[vk_slots_max + 1];
    int count = 0;
    for (int i = 0; i < program->storages_count; i++) {
        const int binding = program->storages[i];
        vk_buffer* buffer = binding < vk_slots_max ? vk_buffer_of(vk_storage_bound[binding]) : NULL;
        if (!buffer || !buffer->buffer) {
            zox_loge("[vulkan] compute storage binding [%i] has no buffer", binding);
            return;
        }
        buffer->used = serial;
        infos[count] = (VkDescriptorBufferInfo) { buffer->buffer, buffer->offset, buffer->capacity };
        writes[count] = (VkWriteDescriptorSet) { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, .dstSet = set,
            .dstBinding = binding, .descriptorCount = 1, .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pBufferInfo = &infos[count] };
        count++;
    }
    if (program->data_size[0]) {
        infos[count] = (VkDescriptorBufferInfo) { ring, 0, program->data_size[0] };
        writes[count] = (VkWriteDescriptorSet) { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, .dstSet = set,
            .dstBinding = vk_binding_compute_block, .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .pBufferInfo = &infos[count] };
        count++;
    }
    vkUpdateDescriptorSets(vk_device, count, writes, 0, NULL);
    // mid frame: its pass paused round the dispatch (gl's order); else before the frame, in the uploads
    const byte in_frame = vk_recording;
    const VkCommandBuffer commands = in_frame ? vk_contexts[0].commands : vulkan_upload_commands();
    if (in_frame) {
        vkCmdEndRenderPass(commands);
    }
    vulkan_compute_barrier(commands, 0);
    vkCmdBindPipeline(commands, VK_PIPELINE_BIND_POINT_COMPUTE, program->compute_pipeline);
    vkCmdBindDescriptorSets(commands, VK_PIPELINE_BIND_POINT_COMPUTE, program->layout, 0, 1, &set,
        program->data_size[0] ? 1 : 0, &ring_offset);
    vkCmdDispatch(commands, groups_x, groups_y, groups_z);
    vulkan_compute_barrier(commands, 1);
    if (in_frame) {
        vulkan_pass_begin(commands, vulkan_target_load_pass(), VK_SUBPASS_CONTENTS_INLINE);
        vk_contexts[0].pipeline_bound = VK_NULL_HANDLE;
        vk_contexts[0].dynamic_dirty = 1;
    }
}

// Storage meshes: a draw program's inputs straight from compute's buffers

guint zox_gpu_storage_mesh_vao(guint vertices, guint indices, guint location, guint extra, guint extra_location, guint uvs, guint uvs_location,
    guint extra2_location)
{
    const uint id = vk_table_add(&vk_vaos);
    vk_vao* vao = vk_table_get(&vk_vaos, id);
    *vao = (vk_vao) { vertices, indices, location, extra, extra_location, uvs, uvs_location, 1, extra2_location };
    return id;
}

static inline void zox_gpu_dispose_vao(guint id) {
    vk_vao* vao = vk_table_get(&vk_vaos, id);
    if (vao && vao->live) {
        vao->live = 0;
        vk_table_remove(&vk_vaos, id);
    }
}

// the vao's vertices (vec4) and lights (uint) as attributes, its indices, the count from command (written by compute)
static inline void zox_gpu_draw_indirect(guint vao_id, guint command_id) {
    const vk_vao* vao = vk_table_get(&vk_vaos, vao_id);
    vk_buffer* command = vk_buffer_of(command_id);
    vk_buffer* indices = vao && vao->live ? vk_buffer_of(vao->indices) : NULL;
    if (!indices || !indices->buffer || !command || !command->buffer
        || vao->location >= vk_slots_max || vao->extra_location >= vk_slots_max || vao->uvs_location >= vk_slots_max
        || vao->extra2_location >= vk_slots_max)
    {
        return;
    }
    vk_attribute* attributes = vk_ctx->attributes;
    const vk_attribute before[4] = { attributes[vao->location], attributes[vao->extra_location], attributes[vao->uvs_location],
        attributes[vao->extra2_location] };
    const uint extra_stride = vao->extra2_location ? 8 : 4;
    attributes[vao->location] = (vk_attribute) { 1, vao->vertices, NULL, VK_FORMAT_R32G32B32A32_SFLOAT, 16, 0 };
    attributes[vao->extra_location] = (vk_attribute) { 1, vao->extra, NULL, VK_FORMAT_R32_UINT, extra_stride, 0 };
    if (vao->extra2_location) {
        attributes[vao->extra2_location] = (vk_attribute) { 1, vao->extra, (const byte*) (size_t) 4, VK_FORMAT_R32_UINT, extra_stride, 0 };
    }
    if (vao->uvs) {
        attributes[vao->uvs_location] = (vk_attribute) { 1, vao->uvs, NULL, VK_FORMAT_R32G32_SFLOAT, 8, 0 };
    }
    const uint element_before = vk_ctx->element_bound;
    vk_ctx->element_bound = vao->indices;
    if (vulkan_draw_begin(vk_topology_triangles, 0, 1)) {
        const VkCommandBuffer commands = vk_ctx->commands;
        const ulong serial = vk_serial_recording();
        indices->used = serial;
        command->used = serial;
        vkCmdBindIndexBuffer(commands, indices->buffer, indices->offset, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexedIndirect(commands, command->buffer, command->offset, 1, 20);
    }
    // a vao is gl's own attribute state: the bound ones as they were
    attributes[vao->location] = before[0];
    attributes[vao->extra_location] = before[1];
    if (vao->uvs) {
        attributes[vao->uvs_location] = before[2];
    }
    if (vao->extra2_location) {
        attributes[vao->extra2_location] = before[3];
    }
    vk_ctx->element_bound = element_before;
}

void vulkan_dispose_compute() {
    vk_table_dispose(&vk_vaos);
}
