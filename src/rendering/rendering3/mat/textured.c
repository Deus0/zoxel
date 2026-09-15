entity shader_textured3D;
entity material_textured3D;

typedef struct {
    gint vertex_position;
    gint vertex_uv;
    gint vertex_color;
    gint transform_matrix;
    gint camera_matrix;
    gint texture;
    gint fog_data;
    gint brightness;
} MaterialTextured3D;
zoxc_custom(MaterialTextured3D);

MaterialTextured3D create_MaterialTextured3D(guint material) {
    return (MaterialTextured3D) {
        .vertex_position = zox_gpu_get_material_attribute(material, "vertex_position"),
        .vertex_uv = zox_gpu_get_material_attribute(material, "vertex_uv"),
        .vertex_color = zox_gpu_get_material_attribute(material, "vertex_color"),
        .transform_matrix = zox_gpu_get_material_property(material, "transform_matrix"),
        .camera_matrix = zox_gpu_get_material_property(material, "camera_matrix"),
        .texture = zox_gpu_get_material_property(material, "zexture"),
        .fog_data = zox_gpu_get_material_property(material, "fog_data"),
        .brightness = zox_gpu_get_material_property(material, "brightness")
    };
}

entity spawn_shader_textured3D(ecs *world) {
    byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "textured3D.vert");
    char* frag = get_shader_source(world, "textured3D.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_log_error("[shader_textured3D] failed to spawn")
        return 0;
    }
    zox_name("shader_textured3");
    return e;
}

entity spawn_material_textured3D(ecs *world) {
    entity shader = spawn_shader_textured3D(world);
    if (!shader) {
        return 0;
    }
    guint material;
    entity e = spawn_material(world, shader, &material);
    zox_set_unique_name(e, "material_textured3");
    zox_link(world, e, ShaderLink, shader);
    MaterialTextured3D attributes = create_MaterialTextured3D(material);
    zox_set_data(e, MaterialTextured3D, attributes)
    material_textured3D = e;
    shader_textured3D = shader;
    return e;
}
