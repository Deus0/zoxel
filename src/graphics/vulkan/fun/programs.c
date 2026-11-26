// Programs: a vertex and fragment stage linked as gl links them, by name
//  - one binding plan: the vertex stage's default block 0, the fragment's 1, then named blocks and samplers
//    (shared by name across stages); the spir-v's binding literals are patched to it
//  - the fragment's inputs take the locations of the vertex outputs of the same name (each stage's locations
//    are mapped on its own)
//  - a uniform's gl location is its index in the program's uniforms, written into each stage's block it's in

#define vk_binding_vertex_block 0
#define vk_binding_fragment_block 1

// a binding for a named block or sampler, the same for the same name in either stage
int vulkan_binding_of(char names[][48], int* count, const char* name) {
    for (int i = 0; i < *count; i++) {
        if (strcmp(names[i], name) == 0) {
            return 2 + i;
        }
    }
    snprintf(names[*count], 48, "%s", name);
    return 2 + (*count)++;
}

int vulkan_uniform_add(vk_program* program, const char* name) {
    for (int i = 0; i < program->uniforms_count; i++) {
        if (strcmp(program->uniforms[i].name, name) == 0) {
            return i;
        }
    }
    const int i = program->uniforms_count++;
    program->uniforms = realloc(program->uniforms, sizeof(vk_uniform) * program->uniforms_count);
    vk_uniform* uniform = &program->uniforms[i];
    snprintf(uniform->name, 48, "%s", name);
    uniform->offset[0] = uniform->offset[1] = -1;
    uniform->stride[0] = uniform->stride[1] = 0;
    uniform->sampler = -1;
    return i;
}

VkShaderModule vulkan_create_module(const uint* words, uint count) {
    const VkShaderModuleCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = count * 4,
        .pCode = words,
    };
    VkShaderModule module = VK_NULL_HANDLE;
    vulkan_check(vkCreateShaderModule(vk_device, &info, NULL, &module), "vkCreateShaderModule");
    return module;
}

byte vulkan_link(vk_program* program, const vk_shader* stages[2]) {
    uint* words[2];
    char names[vk_slots_max * 2][48];
    int names_count = 0;
    VkDescriptorSetLayoutBinding bindings[vk_slots_max * 2 + 2];
    int bindings_count = 0;
    for (byte s = 0; s < 2; s++) {
        const vk_shader* stage = stages[s];
        words[s] = malloc(stage->count * 4);
        memcpy(words[s], stage->words, stage->count * 4);
        if (stage->block_word >= 0) {
            const int binding = s ? vk_binding_fragment_block : vk_binding_vertex_block;
            words[s][stage->block_word] = binding;
            bindings[bindings_count++] = (VkDescriptorSetLayoutBinding) { binding,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_ALL_GRAPHICS, NULL };
            program->data_size[s] = stage->block_size;
            for (int m = 0; m < stage->members_count; m++) {
                const int index = vulkan_uniform_add(program, stage->members[m].name);   // it reallocates
                vk_uniform* uniform = &program->uniforms[index];
                uniform->offset[s] = stage->members[m].offset;
                uniform->stride[s] = stage->members[m].stride;
            }
        }
        for (int b = 0; b < stage->blocks_count; b++) {
            const int before = names_count;
            const int binding = vulkan_binding_of(names, &names_count, stage->blocks[b].name);
            words[s][stage->blocks[b].word] = binding;
            if (names_count > before) {
                bindings[bindings_count++] = (VkDescriptorSetLayoutBinding) { binding,
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_ALL_GRAPHICS, NULL };
                vk_block* block = &program->blocks[program->blocks_count++];
                snprintf(block->name, 48, "%s", stage->blocks[b].name);
                block->binding = binding;
                block->size = stage->blocks[b].value;
            }
        }
        for (int t = 0; t < stage->samplers_count; t++) {
            const int before = names_count;
            const int binding = vulkan_binding_of(names, &names_count, stage->samplers[t].name);
            words[s][stage->samplers[t].word] = binding;
            if (names_count > before) {
                bindings[bindings_count++] = (VkDescriptorSetLayoutBinding) { binding,
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_ALL_GRAPHICS, NULL };
                const int index = vulkan_uniform_add(program, stage->samplers[t].name);
                program->uniforms[index].sampler = program->samplers_count;
                program->sampler_units[program->samplers_count] = 0;
                program->samplers[program->samplers_count++] = binding;
            }
        }
    }
    // the fragment's inputs where the vertex stage put them
    for (int i = 0; i < stages[1]->inputs_count; i++) {
        const vk_slot* input = &stages[1]->inputs[i];
        for (int o = 0; o < stages[0]->outputs_count; o++) {
            if (strcmp(stages[0]->outputs[o].name, input->name) == 0) {
                words[1][input->word] = stages[0]->outputs[o].value;
            }
        }
    }
    for (int i = 0; i < stages[0]->inputs_count; i++) {
        program->inputs[program->inputs_count++] = stages[0]->inputs[i];
    }
    program->modules[0] = vulkan_create_module(words[0], stages[0]->count);
    program->modules[1] = vulkan_create_module(words[1], stages[1]->count);
    free(words[0]);
    free(words[1]);
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
    return program->modules[0] && program->modules[1];
}

static inline void zox_gpu_dispose_program(guint id) {
    vk_program* program = vk_program_of(id);
    if (!program) {
        return;
    }
    // rare (gpu restores): waits rather than keeping its pipelines until the frames reading them are done
    vkDeviceWaitIdle(vk_device);
    vk_serial_done = vk_serial_submitted;
    for (int i = 0; i < program->pipelines_count; i++) {
        vkDestroyPipeline(vk_device, program->pipelines[i].pipeline, NULL);
    }
    free(program->pipelines);
    vkDestroyPipeline(vk_device, program->compute_pipeline, NULL);
    vkDestroyShaderModule(vk_device, program->modules[0], NULL);
    vkDestroyShaderModule(vk_device, program->modules[1], NULL);
    vkDestroyPipelineLayout(vk_device, program->layout, NULL);
    vkDestroyDescriptorSetLayout(vk_device, program->set_layout, NULL);
    free(program->uniforms);
    for (int c = 0; c < vk_contexts_max; c++) {
        if (id < vk_contexts[c].programs_count) {
            vk_program_state* state = &vk_contexts[c].programs[id];
            free(state->data[0]);
            free(state->data[1]);
            memset(state, 0, sizeof(vk_program_state));
        }
    }
    program->live = 0;
    vk_table_remove(&vk_programs, id);
    if (vk_ctx->program_bound == id) {
        vk_ctx->program_bound = 0;
    }
    zox_gpu_programs_live--;
}

static inline void zox_dispose_material(guint value) {
    zox_gpu_dispose_program(value);
}

guint spawn_gpu_material_program(guint2 shader) {
    const vk_shader* stages[2] = { vk_shader_of(shader.x), vk_shader_of(shader.y) };
    if (!stages[0] || !stages[1]) {
        return 0;
    }
    const uint id = vk_table_add(&vk_programs);
    vk_program* program = vk_table_get(&vk_programs, id);
    program->live = 1;
    zox_gpu_programs_live++;
    if (!vulkan_link(program, stages)) {
        zox_gpu_dispose_program(id);
        return 0;
    }
    for (int b = 0; b < program->blocks_count; b++) {
        program->blocks[b].point = -1;
    }
    return id;
}

// a uniform's name without an array's [0]
int vulkan_uniform_index(const vk_program* program, const char* name) {
    char trimmed[48];
    snprintf(trimmed, 48, "%s", name);
    char* bracket = strchr(trimmed, '[');
    if (bracket) {
        *bracket = 0;
    }
    for (int i = 0; program && i < program->uniforms_count; i++) {
        if (strcmp(program->uniforms[i].name, trimmed) == 0) {
            return i;
        }
    }
    return -1;
}

static inline gint zox_gpu_get_material_property(guint id, const char* name) {
    return vulkan_uniform_index(vk_program_of(id), name);
}

// the vertex input's location (from the vertex shader's spir-v)
static inline gint zox_gpu_get_material_attribute(guint id, const char* name) {
    const vk_program* program = vk_program_of(id);
    for (int i = 0; program && i < program->inputs_count; i++) {
        if (strcmp(program->inputs[i].name, name) == 0) {
            return program->inputs[i].value;
        }
    }
    return -1;
}

static inline guint zox_gpu_get_material_uniform(guint id, const char* name) {
    const vk_program* program = vk_program_of(id);
    for (int i = 0; program && i < program->blocks_count; i++) {
        if (strcmp(program->blocks[i].name, name) == 0) {
            return i;
        }
    }
    return 0xFFFFFFFF;
}

static inline void zox_gpu_material(guint material) {
    vk_ctx->program_bound = material;
}

static inline void zox_disable_material() {
    vk_ctx->program_bound = 0;
}

// this context's values for a program, made on first use; a worker's start as the main thread's are at each job
//  (a split system draws on from the uniforms the main thread left, as gl would)
vk_program_state* vulkan_program_state(uint id, const vk_program* program) {
    vk_context* context = vk_ctx;
    if (id >= context->programs_count) {
        const uint count = id + 32;
        context->programs = realloc(context->programs, sizeof(vk_program_state) * count);
        memset(context->programs + context->programs_count, 0, sizeof(vk_program_state) * (count - context->programs_count));
        context->programs_count = count;
    }
    vk_program_state* state = &context->programs[id];
    const vk_program_state* main = context != &vk_contexts[0] && id < vk_contexts[0].programs_count ?
        &vk_contexts[0].programs[id] : NULL;
    const byte sync = main && state->job != vk_job;
    for (byte s = 0; s < 2; s++) {
        if (!program->data_size[s]) {
            continue;
        }
        if (!state->data[s]) {
            state->data[s] = calloc(1, program->data_size[s] + 16);
            state->data_dirty[s] = 1;
        }
        if (sync && main->data[s]) {
            memcpy(state->data[s], main->data[s], program->data_size[s]);
            state->data_dirty[s] = 1;
        }
    }
    state->job = vk_job;
    return state;
}

// gl's glUniform: count elements of bytes each into every stage block the uniform is in
void vulkan_uniform_write(uint id, gint index, const void* data, int bytes, int count) {
    const vk_program* program = vk_program_of(id);
    if (!program || index < 0 || index >= program->uniforms_count) {
        return;
    }
    const vk_uniform* uniform = &program->uniforms[index];
    // a sampler's: its texture unit
    if (uniform->sampler >= 0) {
        if (bytes == 4 && count == 1) {
            ((vk_program*) program)->sampler_units[uniform->sampler] = *(const int*) data;
        }
        return;
    }
    vk_program_state* state = vulkan_program_state(id, program);
    for (byte s = 0; s < 2; s++) {
        if (uniform->offset[s] < 0) {
            continue;
        }
        const int stride = uniform->stride[s] ? uniform->stride[s] : bytes;
        for (int k = 0; k < count; k++) {
            const int at = uniform->offset[s] + k * stride;
            if (at + bytes > program->data_size[s]) {
                break;
            }
            memcpy(state->data[s] + at, (const byte*) data + k * bytes, bytes);
        }
        state->data_dirty[s] = 1;
    }
}

static inline void vulkan_uniform_set(gint index, const void* data, int bytes, int count) {
    vulkan_uniform_write(vk_ctx->program_bound, index, data, bytes, count);
}

static inline void zox_gpu_int(gint index, int value) {
    vulkan_uniform_set(index, &value, 4, 1);
}

static inline void zox_gpu_float(gint index, float value) {
    vulkan_uniform_set(index, &value, 4, 1);
}

static inline void zox_gpu_float2(gint index, float2 value) {
    vulkan_uniform_set(index, &value, 8, 1);
}

static inline void zox_gpu_float3(gint index, float3 value) {
    vulkan_uniform_set(index, &value, 12, 1);
}

static inline void zox_gpu_float4(gint index, float4 value) {
    vulkan_uniform_set(index, &value, 16, 1);
}

static inline void zox_gpu_float4x4(gint index, float4x4 value) {
    vulkan_uniform_set(index, &value, 64, 1);
}

static inline void zox_gpu_float4x4_array(gint index, const float4x4* values, int count) {
    vulkan_uniform_set(index, values, 64, count);
}

static inline void zox_gpu_float3_array(gint index, const float3* values, gsizei count) {
    vulkan_uniform_set(index, values, 12, count);
}

static inline void zox_gpu_color_rgb(gint index, color_rgb value) {
    const float3 f = color_rgb_to_float3(value);
    zox_gpu_float4(index, (float4) { f.x, f.y, f.z, 1 });
}

static inline void zox_gpu_color(gint index, color value) {
    zox_gpu_float4(index, color_to_float4(value));
}

static inline void vulkan_program_set(guint id, const char* name, const void* data, int bytes, int count) {
    vulkan_uniform_write(id, vulkan_uniform_index(vk_program_of(id), name), data, bytes, count);
}

static inline void zox_gpu_program_int(guint program, const char* name, int value) {
    vulkan_program_set(program, name, &value, 4, 1);
}

static inline void zox_gpu_program_float4(guint program, const char* name, float4 value) {
    vulkan_program_set(program, name, &value, 16, 1);
}

static inline void zox_gpu_program_float4_array(guint program, const char* name, const float4* values, int count) {
    vulkan_program_set(program, name, values, 16, count);
}

static inline void zox_gpu_program_float4x4(guint program, const char* name, float4x4 value) {
    vulkan_program_set(program, name, &value, 64, 1);
}

// the program's uniforms and inputs, when one's asked for and isn't there
static inline void zox_gpu_log_program_inputs(guint id, const char* shader_name) {
    const vk_program* program = vk_program_of(id);
    if (!program) {
        return;
    }
    zox_log("(%s) uniforms [%i]", shader_name, program->uniforms_count);
    for (int i = 0; i < program->uniforms_count; i++) {
        zox_log("   - uniform [%s] at [%i %i]", program->uniforms[i].name,
            program->uniforms[i].offset[0], program->uniforms[i].offset[1]);
    }
}
