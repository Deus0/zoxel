// Shaders: the engine's gl source compiled to spir-v by shaderc, then read (reflected) for what the gl calls ask
//  - relaxed vulkan rules put loose uniforms in a default block (gl_DefaultUniformBlock), each stage its own
//  - vulkan's clip space: each vertex shader ends by moving its depth from gl's -1 to 1 to vulkan's 0 to 1 (its
//    main renamed, wrapped); y is flipped by the viewport (vulkan_viewport)

shaderc_compiler_t vk_shaderc;

// spir-v's numbers this reads
#define spv_name 5
#define spv_member_name 6
#define spv_decorate 71
#define spv_member_decorate 72
#define spv_type_int 21
#define spv_type_float 22
#define spv_type_vector 23
#define spv_type_matrix 24
#define spv_type_sampled_image 27
#define spv_type_array 28
#define spv_type_struct 30
#define spv_type_pointer 32
#define spv_constant 43
#define spv_variable 59
#define spv_block 2
#define spv_array_stride 6
#define spv_builtin 11
#define spv_location 30
#define spv_binding 33
#define spv_offset 35
#define spv_uniform_constant 0
#define spv_input 1
#define spv_uniform 2
#define spv_output 3

// the gl source as vulkan glsl: version 450, gl's vertex and instance ids, the vertex depth moved
char* vulkan_shader_source(const char* source, byte fragment) {
    if (strncmp(source, "#version", 8) == 0) {
        const char* line_end = strchr(source, '\n');
        source = line_end ? line_end + 1 : source;
    }
    const char* prefix = "#version 450\n#define gl_VertexID gl_VertexIndex\n#define gl_InstanceID gl_InstanceIndex\n";
    const char* suffix = "\nvoid main() {\n    zox_main();\n    gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5;\n}\n";
    const char* main = fragment ? NULL : strstr(source, "void main");
    const size_t length = strlen(prefix) + strlen(source) + strlen(suffix) + 8;
    char* result = malloc(length);
    if (!main) {
        snprintf(result, length, "%s%s", prefix, source);
        return result;
    }
    snprintf(result, length, "%s%.*svoid zox_main%s%s", prefix, (int) (main - source), source,
        main + strlen("void main"), suffix);
    return result;
}

// a literal string at words, as a name (cut at 47)
void spv_read_name(const uint* words, char* name) {
    snprintf(name, 48, "%s", (const char*) words);
}

// the bytes a type takes in a block: scalars, vectors, matrices (std140 columns of 16), arrays by their stride
int spv_type_size(const uint* words, const uint* types, const int* strides, const uint* constants, uint id) {
    const uint* type = &words[types[id]];
    switch (type[0] & 0xFFFF) {
        case spv_type_int:
        case spv_type_float:
            return type[2] / 8;
        case spv_type_vector:
            return type[3] * spv_type_size(words, types, strides, constants, type[2]);
        case spv_type_matrix:
            return type[3] * 16;
        case spv_type_array:
            return words[constants[type[3]] + 3] * strides[id];
    }
    return 0;
}

// reads what the gl calls need from a stage's spir-v (vk_shader's comments)
void vulkan_reflect(vk_shader* shader) {
    const uint* words = shader->words;
    const uint count = shader->count;
    const uint bound = words[3];
    // where each id's type, name, decorations are
    uint* types = calloc(bound, sizeof(uint));          // a type or constant's instruction
    uint* constants = types;
    uint* names = calloc(bound, sizeof(uint));          // its OpName's string
    int* locations = calloc(bound, sizeof(int));        // the word of its Location literal
    int* bindings = calloc(bound, sizeof(int));
    int* strides = calloc(bound, sizeof(int));
    byte* blocks = calloc(bound, 1);
    byte* builtins = calloc(bound, 1);
    for (uint i = 5; i < count; i += words[i] >> 16) {
        const uint op = words[i] & 0xFFFF;
        if (words[i] >> 16 == 0) {
            break;
        }
        if (op == spv_name) {
            names[words[i + 1]] = i + 2;
        } else if (op == spv_decorate) {
            const uint target = words[i + 1];
            const uint decoration = words[i + 2];
            if (decoration == spv_location) {
                locations[target] = i + 3;
            } else if (decoration == spv_binding) {
                bindings[target] = i + 3;
            } else if (decoration == spv_array_stride) {
                strides[target] = words[i + 3];
            } else if (decoration == spv_block) {
                blocks[target] = 1;
            } else if (decoration == spv_builtin) {
                builtins[target] = 1;
            }
        } else if ((op >= spv_type_int && op <= spv_type_pointer) || op == spv_constant) {
            types[words[i + 1]] = i;
        }
    }
    shader->block_word = -1;
    for (uint i = 5; i < count; i += words[i] >> 16) {
        if (words[i] >> 16 == 0) {
            break;
        }
        if ((words[i] & 0xFFFF) != spv_variable) {
            continue;
        }
        const uint id = words[i + 2];
        const uint storage = words[i + 3];
        const uint pointee = words[types[words[i + 1]] + 3];
        const uint* type = &words[types[pointee]];
        vk_slot slot = { .word = -1 };
        if (names[id]) {
            spv_read_name(&words[names[id]], slot.name);
        }
        if ((storage == spv_input || storage == spv_output) && !builtins[id] && !builtins[pointee] && locations[id]) {
            slot.word = locations[id];
            slot.value = words[locations[id]];
            if (storage == spv_input && shader->inputs_count < vk_slots_max) {
                shader->inputs[shader->inputs_count++] = slot;
            } else if (storage == spv_output && shader->outputs_count < vk_slots_max) {
                shader->outputs[shader->outputs_count++] = slot;
            }
        } else if (storage == spv_uniform_constant && (type[0] & 0xFFFF) == spv_type_sampled_image) {
            slot.word = bindings[id];
            if (shader->samplers_count < vk_slots_max) {
                shader->samplers[shader->samplers_count++] = slot;
            }
        } else if (storage == spv_uniform && blocks[pointee]) {
            char block_name[48] = "";
            if (names[pointee]) {
                spv_read_name(&words[names[pointee]], block_name);
            }
            // its members: names and offsets from the struct's member decorations
            int size = 0;
            const int members = (type[0] >> 16) - 2;
            vk_member list[vk_members_max];
            memset(list, 0, sizeof(list));
            for (uint j = 5; j < count; j += words[j] >> 16) {
                const uint op = words[j] & 0xFFFF;
                if ((words[j] >> 16) == 0) {
                    break;
                }
                if ((op != spv_member_name && op != spv_member_decorate)
                    || words[j + 1] != pointee || words[j + 2] >= vk_members_max)
                {
                    continue;
                }
                const uint member = words[j + 2];
                if (op == spv_member_name) {
                    spv_read_name(&words[j + 3], list[member].name);
                } else if (op == spv_member_decorate && words[j + 3] == spv_offset) {
                    const uint member_type = type[2 + member];
                    list[member].offset = words[j + 4];
                    list[member].stride = strides[member_type];
                    const int end = words[j + 4] + spv_type_size(words, types, strides, constants, member_type);
                    size = end > size ? end : size;
                }
            }
            if (strcmp(block_name, "gl_DefaultUniformBlock") == 0) {
                shader->block_word = bindings[id];
                shader->block_size = size;
                shader->members_count = members < vk_members_max ? members : vk_members_max;
                memcpy(shader->members, list, sizeof(vk_member) * shader->members_count);
            } else if (shader->blocks_count < vk_slots_max) {
                snprintf(slot.name, 48, "%s", block_name);
                slot.word = bindings[id];
                slot.value = size;
                shader->blocks[shader->blocks_count++] = slot;
            }
        }
    }
    free(types);
    free(names);
    free(locations);
    free(bindings);
    free(strides);
    free(blocks);
    free(builtins);
}

// a stage compiled and reflected, its id (0 if it failed, logged)
uint vulkan_compile_stage(const char* source, byte fragment) {
    if (!vk_shaderc) {
        vk_shaderc = shaderc_compiler_initialize();
    }
    char* glsl = vulkan_shader_source(source, fragment);
    shaderc_compile_options_t options = shaderc_compile_options_initialize();
    shaderc_compile_options_set_vulkan_rules_relaxed(options, 1);
    shaderc_compile_options_set_auto_bind_uniforms(options, 1);
    shaderc_compile_options_set_auto_map_locations(options, 1);
    shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
    shaderc_compilation_result_t result = shaderc_compile_into_spv(vk_shaderc, glsl, strlen(glsl),
        fragment ? shaderc_fragment_shader : shaderc_vertex_shader, fragment ? "frag" : "vert", "main", options);
    shaderc_compile_options_release(options);
    uint id = 0;
    if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
        zox_loge("[vulkan] %s shader failed:\n%s\n%s", fragment ? "fragment" : "vertex",
            shaderc_result_get_error_message(result), glsl);
    } else {
        id = vk_table_add(&vk_shaders);
        vk_shader* shader = vk_table_get(&vk_shaders, id);
        shader->live = 1;
        shader->fragment = fragment;
        shader->count = shaderc_result_get_length(result) / 4;
        shader->words = malloc(shader->count * 4);
        memcpy(shader->words, shaderc_result_get_bytes(result), shader->count * 4);
        vulkan_reflect(shader);
        zox_gpu_shaders_live++;
    }
    shaderc_result_release(result);
    free(glsl);
    return id;
}

static inline void zox_gpu_dispose_shader(guint id) {
    vk_shader* shader = vk_shader_of(id);
    if (shader) {
        free(shader->words);
        shader->live = 0;
        vk_table_remove(&vk_shaders, id);
        zox_gpu_shaders_live--;
    }
}

guint2 zox_gpu_compile_shader(const gchar* vert_buffer, const gchar* frag_buffer) {
    if (!vert_buffer || !frag_buffer) {
        return guint2_zero;
    }
    const uint vert = vulkan_compile_stage(vert_buffer, 0);
    const uint frag = vert ? vulkan_compile_stage(frag_buffer, 1) : 0;
    if (!frag) {
        zox_gpu_dispose_shader(vert);
        return guint2_zero;
    }
    return (guint2) { vert, frag };
}

int zox_get_safe_ubo_size() {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(vk_physical, &properties);
    const uint bytes = properties.limits.maxUniformBufferRange < 65536 ? properties.limits.maxUniformBufferRange : 65536;
    return bytes / sizeof(float4x4);
}
