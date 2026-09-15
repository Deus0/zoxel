entity shader_colored3D;
entity material_colored3D;

typedef struct {
    gint vertex_position;
    gint vertex_color;
    guint transform_matrix;
    guint camera_matrix;
    guint brightness;
    guint fog_data;
} MaterialColored3D;
zoxc_custom(MaterialColored3D);

MaterialColored3D create_MaterialColored3D(guint material) {
    return (MaterialColored3D) {
        .vertex_position = zox_gpu_get_material_attribute(material, "vertex_position"),
        .vertex_color = zox_gpu_get_material_attribute(material, "vertex_color"),
        .transform_matrix = zox_gpu_get_material_property(material, "transform_matrix"),
        .camera_matrix = zox_gpu_get_material_property(material, "camera_matrix"),
        .brightness = zox_gpu_get_material_property(material, "brightness"),
        .fog_data = zox_gpu_get_material_property(material, "fog_data")
    };
}

entity spawn_shader_colored3D(ecs *world) {
    byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "colored3D.vert");
    char* frag = get_shader_source(world, "colored3D.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_log_error("[shader_colored3D] failed to spawn")
        return 0;
    }
    zox_name("shader_colored3D")
    return e;
}

entity spawn_material_colored3D(ecs *world) {
    entity shader = spawn_shader_colored3D(world);
    if (!shader) {
        return 0;
    }
    uint material;
    entity e = spawn_material(world, shader, &material);
    zox_link(world, e, ShaderLink, shader);
    const MaterialColored3D attributes = create_MaterialColored3D(material);
    zox_set_data(e, MaterialColored3D, attributes)
    material_colored3D = e;
    shader_colored3D = shader;
    return e;
}
