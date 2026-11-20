// A draw: gl's state turned into a pipeline (found, or made and kept by its program), vertex buffers, uniforms
// (copied into the frame's ring) and textures (a descriptor set, reused while they match), then the draw

#define vk_topology_triangles 0
#define vk_topology_lines 1
#define vk_topology_points 2

VkPipeline vulkan_create_pipeline(const vk_program* program, const vk_pipeline_key* key) {
    const VkPipelineShaderStageCreateInfo stages[2] = {
        { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = program->modules[0], .pName = "main" },
        { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = program->modules[1], .pName = "main" },
    };
    // one binding an input, as gl's attributes each read their own buffer
    VkVertexInputBindingDescription bindings[vk_slots_max];
    VkVertexInputAttributeDescription attributes[vk_slots_max];
    for (int i = 0; i < key->inputs_count; i++) {
        bindings[i] = (VkVertexInputBindingDescription) { i, key->inputs[i].stride,
            key->inputs[i].instanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX };
        attributes[i] = (VkVertexInputAttributeDescription) { key->inputs[i].location, i, key->inputs[i].format, 0 };
    }
    const VkPipelineVertexInputStateCreateInfo input = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = key->inputs_count,
        .pVertexBindingDescriptions = bindings,
        .vertexAttributeDescriptionCount = key->inputs_count,
        .pVertexAttributeDescriptions = attributes,
    };
    const VkPrimitiveTopology topologies[3] = { VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
        VK_PRIMITIVE_TOPOLOGY_POINT_LIST };
    const VkPipelineInputAssemblyStateCreateInfo assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = topologies[key->topology],
    };
    const VkPipelineViewportStateCreateInfo viewport = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };
    // gl's front faces are counter clockwise; the flipped viewport keeps them so on screen, a render texture's
    // unflipped one (gl's rows, bottom first) turns them clockwise
    const VkPipelineRasterizationStateCreateInfo raster = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = key->cull ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE,
        .frontFace = key->target ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = key->depth_bias,
        .lineWidth = 1,
    };
    const VkPipelineMultisampleStateCreateInfo multisample = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };
    const VkPipelineDepthStencilStateCreateInfo depth = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = key->depth_test,
        .depthWriteEnable = key->depth_write,
        .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
    };
    const VkPipelineColorBlendAttachmentState blend_attachment = {
        .blendEnable = key->blend,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = key->color_write ? 0xF : 0,
    };
    const VkPipelineColorBlendStateCreateInfo blend = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &blend_attachment,
    };
    const VkDynamicState dynamics[4] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH, VK_DYNAMIC_STATE_DEPTH_BIAS };
    const VkPipelineDynamicStateCreateInfo dynamic = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 4,
        .pDynamicStates = dynamics,
    };
    const VkGraphicsPipelineCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = stages,
        .pVertexInputState = &input,
        .pInputAssemblyState = &assembly,
        .pViewportState = &viewport,
        .pRasterizationState = &raster,
        .pMultisampleState = &multisample,
        .pDepthStencilState = &depth,
        .pColorBlendState = &blend,
        .pDynamicState = &dynamic,
        .layout = program->layout,
        .renderPass = key->target ? vk_pass_texture_begin : vk_pass_clear,
    };
    VkPipeline pipeline = VK_NULL_HANDLE;
    vulkan_check(vkCreateGraphicsPipelines(vk_device, VK_NULL_HANDLE, 1, &info, NULL, &pipeline),
        "vkCreateGraphicsPipelines");
    return pipeline;
}

// workers share programs' pipelines: found and made under vk_lock
VkPipeline vulkan_pipeline_of(vk_program* program, const vk_pipeline_key* key) {
    pthread_mutex_lock(&vk_lock);
    VkPipeline pipeline = VK_NULL_HANDLE;
    for (int i = 0; i < program->pipelines_count && !pipeline; i++) {
        if (memcmp(&program->pipelines[i].key, key, sizeof(vk_pipeline_key)) == 0) {
            pipeline = program->pipelines[i].pipeline;
        }
    }
    if (!pipeline) {
        pipeline = vulkan_create_pipeline(program, key);
        if (pipeline) {
            program->pipelines = realloc(program->pipelines, sizeof(vk_pipeline) * (program->pipelines_count + 1));
            program->pipelines[program->pipelines_count++] = (vk_pipeline) { *key, pipeline };
        }
    }
    pthread_mutex_unlock(&vk_lock);
    return pipeline;
}

// viewport (gl's flipped: y from the bottom, a negative height), scissor, line width, depth bias
void vulkan_set_dynamic(VkCommandBuffer commands) {
    const float height = vk_target_extent.height;
    const float4 gl = vk_ctx->viewport_set ? vk_ctx->viewport : (float4) { 0, 0, vk_target_extent.width, height };
    // the frame's image flipped to gl's y up; a render texture not, its rows bottom first as gl's are sampled
    const VkViewport viewport = vk_target_texture ? (VkViewport) { gl.x, gl.y, gl.z, gl.w, 0, 1 } :
        (VkViewport) { gl.x, height - gl.y, gl.z, -gl.w, 0, 1 };
    const VkRect2D scissor = { { 0, 0 }, vk_target_extent };
    vkCmdSetViewport(commands, 0, 1, &viewport);
    vkCmdSetScissor(commands, 0, 1, &scissor);
    vkCmdSetLineWidth(commands, vk_wide_lines ? vk_ctx->line_width : 1);
    vkCmdSetDepthBias(commands, vk_ctx->bias_units, 0, vk_ctx->bias_factor);
    vk_ctx->dynamic_dirty = 0;
}

// the program's uniforms (each stage's default block, its named blocks) and texture
byte vulkan_bind_descriptors(uint id, vk_program* program, VkCommandBuffer commands) {
    vk_program_state* state = vulkan_program_state(id, program);
    VkBuffer buffers[vk_slots_max + 2];
    uint offsets[vk_slots_max + 2];
    int sizes[vk_slots_max + 2];
    int bindings[vk_slots_max + 2];
    int count = 0;
    for (byte s = 0; s < 2; s++) {
        if (!state->data[s]) {
            continue;
        }
        if (state->data_dirty[s] || state->data_epoch[s] != vk_ring_epoch) {
            if (!vulkan_ring_push(state->data[s], program->data_size[s], &state->data_ring[s], &state->data_offset[s])) {
                return 0;
            }
            state->data_epoch[s] = vk_ring_epoch;
            state->data_dirty[s] = 0;
        }
        buffers[count] = state->data_ring[s];
        offsets[count] = state->data_offset[s];
        sizes[count] = program->data_size[s];
        bindings[count++] = s ? vk_binding_fragment_block : vk_binding_vertex_block;
    }
    for (int b = 0; b < program->blocks_count; b++) {
        const vk_block* block = &program->blocks[b];
        vk_buffer* buffer = block->point >= 0 && block->point < vk_slots_max ? vk_buffer_of(vk_ubo_points[block->point]) : NULL;
        if (!buffer || !vulkan_ubo_ring(buffer, &buffers[count], &offsets[count])) {
            return 0;
        }
        sizes[count] = block->size;
        bindings[count++] = block->binding;
    }
    vk_texture* texture = vk_texture_of(vk_ctx->texture_bound);
    if (!texture || !texture->image) {
        texture = vk_texture_of(vk_texture_black);
    }
    if (program->samplers_count) {
        texture->used = vk_serial_recording();
    }
    byte reuse = state->set && state->set_epoch == vk_ring_epoch && state->set_view == texture->view;
    for (int i = 0; reuse && i < count; i++) {
        reuse = state->set_buffers[i] == buffers[i];
    }
    if (!reuse) {
        state->set = vulkan_allocate_set(program->set_layout);
        if (!state->set) {
            return 0;
        }
        VkDescriptorBufferInfo buffer_infos[vk_slots_max + 2];
        VkWriteDescriptorSet writes[vk_slots_max * 2 + 2];
        int writes_count = 0;
        for (int i = 0; i < count; i++) {
            buffer_infos[i] = (VkDescriptorBufferInfo) { buffers[i], 0, sizes[i] };
            writes[writes_count++] = (VkWriteDescriptorSet) {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, .dstSet = state->set, .dstBinding = bindings[i],
                .descriptorCount = 1, .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                .pBufferInfo = &buffer_infos[i] };
            state->set_buffers[i] = buffers[i];
        }
        const VkDescriptorImageInfo image = { vk_sampler, texture->view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
        for (int i = 0; i < program->samplers_count; i++) {
            writes[writes_count++] = (VkWriteDescriptorSet) {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, .dstSet = state->set,
                .dstBinding = program->samplers[i], .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .pImageInfo = &image };
        }
        vkUpdateDescriptorSets(vk_device, writes_count, writes, 0, NULL);
        state->set_epoch = vk_ring_epoch;
        state->set_view = texture->view;
    }
    vkCmdBindDescriptorSets(commands, VK_PIPELINE_BIND_POINT_GRAPHICS, program->layout, 0, 1, &state->set,
        count, offsets);
    return 1;
}

// everything a draw needs bound; vertices and instances size client arrays (copied to the ring), 0 to skip it
byte vulkan_draw_begin(byte topology, uint vertices, uint instances) {
    vk_program* program = vk_program_of(vk_ctx->program_bound);
    if (!program || !vulkan_frame_begin()) {
        return 0;
    }
    const VkCommandBuffer commands = vk_ctx->commands;
    const ulong serial = vk_serial_recording();
    vk_pipeline_key key;
    memset(&key, 0, sizeof(key));
    key.topology = topology;
    key.target = vk_target_texture;
    key.blend = vk_ctx->blend;
    key.depth_test = vk_ctx->depth_test;
    key.depth_write = vk_ctx->depth_test && vk_ctx->depth_write;     // gl writes no depth with its test off
    key.cull = vk_ctx->cull && topology == vk_topology_triangles;
    key.color_write = vk_ctx->color_write;
    key.depth_bias = topology == vk_topology_triangles ? vk_ctx->bias_fill : topology == vk_topology_lines ? vk_ctx->bias_lines : 0;
    key.inputs_count = program->inputs_count;
    VkBuffer buffers[vk_slots_max];
    VkDeviceSize offsets[vk_slots_max];
    for (int i = 0; i < program->inputs_count; i++) {
        const int location = program->inputs[i].value;
        const vk_attribute* attribute = location < vk_slots_max ? &vk_ctx->attributes[location] : NULL;
        key.inputs[i].location = location;
        if (!attribute || !attribute->enabled) {
            // gl reads a constant for an input with no attribute: zeros here
            key.inputs[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            buffers[i] = vk_zeros;
            offsets[i] = 0;
            continue;
        }
        const uint stride = attribute->stride ? attribute->stride : vulkan_format_size(attribute->format);
        key.inputs[i].format = attribute->format;
        key.inputs[i].stride = stride;
        key.inputs[i].instanced = attribute->divisor > 0;
        if (attribute->buffer) {
            vk_buffer* buffer = vk_buffer_of(attribute->buffer);
            if (!buffer || !buffer->buffer) {
                return 0;
            }
            buffer->used = serial;
            buffers[i] = buffer->buffer;
            offsets[i] = buffer->offset + (VkDeviceSize) (size_t) attribute->pointer;
        } else {
            // client memory: what this draw reads of it, copied now
            const uint count = attribute->divisor ? instances : vertices;
            const VkDeviceSize bytes = count ? (VkDeviceSize) (count - 1) * stride + vulkan_format_size(attribute->format) : 0;
            uint offset = 0;
            if (!attribute->pointer || !bytes || !vulkan_ring_push(attribute->pointer, bytes, &buffers[i], &offset)) {
                return 0;
            }
            offsets[i] = offset;
        }
    }
    const VkPipeline pipeline = vulkan_pipeline_of(program, &key);
    if (!pipeline) {
        return 0;
    }
    if (!vulkan_bind_descriptors(vk_ctx->program_bound, program, commands)) {
        return 0;
    }
    if (pipeline != vk_ctx->pipeline_bound) {
        vkCmdBindPipeline(commands, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vk_ctx->pipeline_bound = pipeline;
    }
    if (vk_ctx->dynamic_dirty) {
        vulkan_set_dynamic(commands);
    }
    if (program->inputs_count) {
        vkCmdBindVertexBuffers(commands, 0, program->inputs_count, buffers, offsets);
    }
    return 1;
}

// an indexed draw from the bound element buffer
void vulkan_draw_indexed(byte topology, uint indices, uint instances) {
    vk_buffer* elements = vk_buffer_of(vk_ctx->element_bound);
    if (!indices || !instances || !elements || !elements->buffer || !vulkan_draw_begin(topology, 0, instances)) {
        return;
    }
    const VkCommandBuffer commands = vk_ctx->commands;
    elements->used = vk_serial_recording();
    vkCmdBindIndexBuffer(commands, elements->buffer, elements->offset, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commands, indices, instances, 0, 0, 0);
}

void vulkan_draw(byte topology, uint vertices, uint instances) {
    if (!vertices || !instances || !vulkan_draw_begin(topology, vertices, instances)) {
        return;
    }
    vkCmdDraw(vk_ctx->commands, vertices, instances, 0, 0);
}

static inline void zox_gpu_render(gsizei length) {
    vulkan_draw_indexed(vk_topology_triangles, length, 1);
}

static inline void zox_gpu_render3(gsizei length) {
    vulkan_draw_indexed(zox_render_lines ? vk_topology_lines : vk_topology_triangles, length, 1);
}

static inline void zox_gpu_render_as_lines(gsizei length) {
    vulkan_draw_indexed(vk_topology_lines, length, 1);
}

static inline void zox_gpu_render_triangles_instanced(gsizei indicies, gsizei length) {
    vulkan_draw_indexed(vk_topology_triangles, indicies, length);
}

static inline void zox_gpu_render_points(gsizei length) {
    vulkan_draw(vk_topology_points, length, 1);
}

static inline void zox_gpu_render_points_instanced(gsizei length) {
    vulkan_draw(vk_topology_points, 1, length);
}

static inline void zox_gpu_render_lines(gsizei length) {
    vulkan_draw(vk_topology_lines, length, 1);
}

static inline void zox_gpu_render_lines_instanced(gsizei count) {
    vulkan_draw(vk_topology_lines, 2, count);
}
