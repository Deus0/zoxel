entity shader_colored3D;
entity material_colored3D;

typedef struct {
    gint vertex_position;
    gint vertex_color;
    uint transform_matrix;
    uint camera_matrix;
    uint brightness;
    uint fog_data;
} MaterialColored3D;
zoxc_custom(MaterialColored3D);

MaterialColored3D create_MaterialColored3D(const uint material) {
    return (MaterialColored3D) {
        zox_gpu_get_material_attribute(material, "vertex_position"),
        zox_gpu_get_material_attribute(material, "vertex_color"),
        zox_gpu_get_material_property(material, "transform_matrix"),
        zox_gpu_get_material_property(material, "camera_matrix"),
        zox_gpu_get_material_property(material, "brightness"),
        zox_gpu_get_material_property(material, "fog_data")
    };
}

entity spawn_shader_colored3D(ecs *world) {
    const byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "colored3D.vert");
    char* frag = get_shader_source(world, "colored3D.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    const entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_log_error("[shader_colored3D] failed to spawn")
        return 0;
    }
    zox_name("shader_colored3D")
    return e;
}

entity spawn_material_colored3D(ecs *world) {
    const entity shader = spawn_shader_colored3D(world);
    if (!shader) {
        return 0;
    }
    uint material;
    const entity e = spawn_material(world, shader, &material);
    zox_set(e, ShaderLink, { shader })
    const MaterialColored3D attributes = create_MaterialColored3D(material);
    zox_set_data(e, MaterialColored3D, attributes)
    material_colored3D = e;
    shader_colored3D = shader;
    return e;
}
