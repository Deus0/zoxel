// todo: Different meshes, linking, and then use a stack
//  also LODing support
entity shader_vox_instance = 0;
entity material_vox_instance = 0;
#define zox_instance_matrices_binding 0    // the InstanceMatrices ubo's binding, both vox instance materials read it

// the material's InstanceMatrices block reads the instance ubo, set again after a relink (a gpu restore)
static inline void bind_instance_matrices(guint material) {
#ifndef zox_disable_ubos
    zox_gpu_ubo_block_binding(material, zox_gpu_get_material_uniform(material, "InstanceMatrices"), zox_instance_matrices_binding);
#else
    (void) material;
#endif
}

typedef struct {
    gint vertex_position;
    gint vertex_color;
    gint matrices;
    guint brightness;
    guint fog_data;
    guint camera_matrix;
} MaterialVoxInstance;
zoxc_custom(MaterialVoxInstance);

MaterialVoxInstance create_MaterialVoxInstance(const uint material) {
    return (MaterialVoxInstance) {
        zox_gpu_get_material_attribute(material, "vertex_position"),
        zox_gpu_get_material_attribute(material, "vertex_color"),
#ifndef zox_disable_ubos
        zox_gpu_get_material_uniform(material, "InstanceMatrices"),
#else
        0,
#endif
        zox_gpu_get_material_property(material, "brightness"),
        zox_gpu_get_material_property(material, "fog_data"),
        zox_gpu_get_material_property(material, "camera_matrix")
    };
}

uint generate_ubo(gint binding_point) {
#ifndef zox_disable_ubos

    guint ubo = zox_gpu_create_buffer();
    zox_gpu_ubo_initialize_matricies(ubo, binding_point, zox_get_safe_ubo_size());
    zox_gpu_ubo_reset();

    // glGenBuffers(1, &ubo);
    // glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    // glBufferData(GL_UNIFORM_BUFFER, sizeof(float4x4) * zox_get_safe_ubo_size(), NULL, GL_DYNAMIC_DRAW);
    // glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);
    // glBindBuffer(GL_UNIFORM_BUFFER, 0); // Unbind after allocation

    zox_logv(" + spawned ubo: %i bound at [%i]", ubo, binding_point);
    return ubo;
#else
    return 0;
#endif
}

/*uint spawn_ubo(ecs *world, const entity material) {
    const MaterialVoxInstance* materialVoxInstance = zox_get(material, MaterialVoxInstance);
    const gint binding_point = materialVoxInstance->matrices;
    if (binding_point == GL_INVALID_INDEX) {
        zox_loge("InstanceMatrices block index not found in shader")
    }
    return generate_ubo(binding_point);
}*/

entity spawn_material_vox_instance(ecs* world) {
    const byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "vox_instance.vert");
    char* frag = get_shader_source(world, "vox_instance.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    entity shader = spawn_shader(world, shader_index);
    if (!shader) {
        zox_loge("[shader_vox_instance] failed to spawn")
        return 0;
    }
    guint material;
    entity e = spawn_material(world, shader, &material);
    zox_link(world, e, ShaderLink, shader);
    if (!material) {
        zox_loge("vox instance material failed to initialize")
        return 0;
    }
    const MaterialVoxInstance materialVoxInstance =  create_MaterialVoxInstance(material);
    zox_set_data(e, MaterialVoxInstance, materialVoxInstance)
    bind_instance_matrices(material);
    guint ubo = generate_ubo(zox_instance_matrices_binding);
    zox_setv(e, UboGPULink, ubo);
    material_vox_instance = e;
    shader_vox_instance = shader;
    return e;
}
