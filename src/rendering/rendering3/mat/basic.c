typedef struct {
    gint vertex_position;
    gint transform_matrix;
    gint camera_matrix;
    gint fog_data;
    gint color;
} MaterialBasic3D;
zoxc_custom(MaterialBasic3D);

MaterialBasic3D create_MaterialBasic3D(guint material) {
    return (MaterialBasic3D) {
        .vertex_position = zox_gpu_get_material_attribute(material, "vertex_position"),
        .transform_matrix = zox_gpu_get_material_property(material, "transform_matrix"),
        .camera_matrix = zox_gpu_get_material_property(material, "camera_matrix"),
        .color = zox_gpu_get_material_property(material, "color"),
        .fog_data = zox_gpu_get_material_property(material, "fog_data")
    };
}

entity spawn_shader_basic3(ecs *world) {
    byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "basic3D.vert");
    char* frag = get_shader_source(world, "basic3D.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_log_error("[shader_basic3] failed to spawn");
        return 0;
    }
    zox_name("shader_basic3");
    return e;
}

entity spawn_material_basic3D(
    ecs* world,
    entity shader)
{
    if (!shader) {
        return 0;
    }
    guint material;
    entity e = spawn_material(world, shader, &material);
    zox_name("material_basic3D");
    zox_link(world, e, ShaderLink, shader);
    MaterialBasic3D attributes = create_MaterialBasic3D(material);
    zox_set_data(e, MaterialBasic3D, attributes);
    return e;
}

entity spawn_shader_basic3_overlay(ecs *world) {
    byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "basic3D.vert");
    char* frag = get_shader_source(world, "basic3_overlay.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_loge("[shader_basic3_overlay] failed to spawn");
        return 0;
    }
    zox_name("shader_basic3_overlay");
    return e;
}
