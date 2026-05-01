// todo: Different meshes, linking, and then use a stack
//  also LODing support
entity shader_vox_instance = 0;
entity material_vox_instance = 0;

typedef struct {
    gint vertex_position;
    gint vertex_color;
    gint matrices;
    uint camera_matrix;
    uint brightness;
    uint fog_data;
} MaterialVoxInstance;
zoxc_custom(MaterialVoxInstance);

MaterialVoxInstance create_MaterialVoxInstance(const uint material) {
    return (MaterialVoxInstance) {
        zox_gpu_get_material_attribute(material, "vertex_position"),
        zox_gpu_get_material_attribute(material, "vertex_color"),
#ifndef zox_disable_ubos
        glGetUniformBlockIndex(material, "InstanceMatrices"),
#else
        0,
#endif
        zox_gpu_get_material_property(material, "camera_matrix"),
        zox_gpu_get_material_property(material, "brightness"),
        zox_gpu_get_material_property(material, "fog_data")
    };
}

uint generate_ubo(gint binding_point) {
#ifndef zox_disable_ubos

    uint ubo = zox_gpu_create_buffer();
    zox_gpu_ubo_initialize_matricies(ubo, binding_point, zox_get_safe_ubo_size());
    zox_gpu_ubo_reset();

    // glGenBuffers(1, &ubo);
    // glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    // glBufferData(GL_UNIFORM_BUFFER, sizeof(float4x4) * zox_get_safe_ubo_size(), NULL, GL_DYNAMIC_DRAW);
    // glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);
    // glBindBuffer(GL_UNIFORM_BUFFER, 0); // Unbind after allocation

    zox_log_shader(" + spawned ubo: %i binded to block index %i", ubo, binding_point);
    return ubo;
#else
    return 0;
#endif
}

/*uint spawn_ubo(ecs *world, const entity material) {
    zox_geter(material, MaterialVoxInstance, materialVoxInstance)
    const gint binding_point = materialVoxInstance->matrices;
    if (binding_point == GL_INVALID_INDEX) {
        zox_log_error("InstanceMatrices block index not found in shader")
    }
    return generate_ubo(binding_point);
}*/

entity spawn_material_vox_instance(ecs *world) {
    const byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "vox_instance.vert");
    char* frag = get_shader_source(world, "vox_instance.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    const entity shader = spawn_shader(world, shader_index);
    if (!shader) {
        zox_log_error("[shader_vox_instance] failed to spawn")
        return 0;
    }
    uint material;
    const entity e = spawn_material(world, shader, &material);
    zox_set(e, ShaderLink, { shader });
    if (!material) {
        zox_log_error("vox instance material failed to initialize")
        return 0;
    }
    const MaterialVoxInstance materialVoxInstance =  create_MaterialVoxInstance(material);
    zox_set_data(e, MaterialVoxInstance, materialVoxInstance)
    uint ubo = generate_ubo(materialVoxInstance.matrices);
    zox_set(e, UboGPULink, { ubo })
    material_vox_instance = e;
    shader_vox_instance = shader;
    return e;
}
