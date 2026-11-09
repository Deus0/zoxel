// Swaying vox instances (grass) - shares the vox_instance UBO and frag shader
entity shader_vox_instance_sway = 0;
entity material_vox_instance_sway = 0;

typedef struct {
    gint vertex_position;
    gint vertex_color;
    gint matrices;
    guint brightness;
    guint fog_data;
    guint camera_matrix;
    guint sway_data;
} MaterialVoxInstanceSway;
zoxc_custom(MaterialVoxInstanceSway);

MaterialVoxInstanceSway create_MaterialVoxInstanceSway(const uint material) {
    return (MaterialVoxInstanceSway) {
        zox_gpu_get_material_attribute(material, "vertex_position"),
        zox_gpu_get_material_attribute(material, "vertex_color"),
#ifndef zox_disable_ubos
        zox_gpu_get_material_uniform(material, "InstanceMatrices"),
#else
        0,
#endif
        zox_gpu_get_material_property(material, "brightness"),
        zox_gpu_get_material_property(material, "fog_data"),
        zox_gpu_get_material_property(material, "camera_matrix"),
        zox_gpu_get_material_property(material, "sway_data")
    };
}

entity spawn_material_vox_instance_sway(ecs* world) {
    const byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "vox_instance_sway.vert");
    char* frag = get_shader_source(world, "vox_instance.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    entity shader = spawn_shader(world, shader_index);
    if (!shader) {
        zox_log_error("[shader_vox_instance_sway] failed to spawn")
        return 0;
    }
    guint material;
    entity e = spawn_material(world, shader, &material);
    zox_link(world, e, ShaderLink, shader);
    if (!material) {
        zox_log_error("vox instance sway material failed to initialize")
        return 0;
    }
    const MaterialVoxInstanceSway data = create_MaterialVoxInstanceSway(material);
    zox_set_data(e, MaterialVoxInstanceSway, data)
    material_vox_instance_sway = e;
    shader_vox_instance_sway = shader;
    return e;
}
