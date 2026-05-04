entity shader_textured2D = 0;
entity material_textured2D = 0;

typedef struct {
    gint vertex_position;
    gint vertex_uv;
    uint position;
    uint angle;
    uint scale;
    uint camera_matrix;
    uint texture;
    uint brightness;
    uint alpha;
} MaterialTextured2D;
zoxc_custom(MaterialTextured2D);

MaterialTextured2D create_MaterialTextured2D(uint material) {
    return (MaterialTextured2D) {
        zox_gpu_get_material_attribute(material, "vertex_position"),
        zox_gpu_get_material_attribute(material, "vertex_uv"),
        zox_gpu_get_material_property(material, "position"),
        zox_gpu_get_material_property(material, "angle"),
        zox_gpu_get_material_property(material, "scale"),
        zox_gpu_get_material_property(material, "camera_matrix"),
        zox_gpu_get_material_property(material, "texture"),
        zox_gpu_get_material_property(material, "brightness"),
        zox_gpu_get_material_property(material, "alpha") };
}

entity spawn_shader_textured2D(ecs *world) {
    byte shader_index = get_new_shader_source_index();

    char* vert = get_shader_source(world, "textured2D.vert");
    char* frag = get_shader_source(world, "textured2D.frag");

    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;

    entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_log_error("=> [spawn_shader_textured2D] Failed:\n%s", vert);
        return 0;
    }

    zox_name("shader_textured2D");

    return e;
}

entity spawn_material_textured2D(ecs *world) {

    entity shader = spawn_shader_textured2D(world);
    if (!shader) {
        return 0;
    }

    uint material;
    entity e = spawn_material(world, shader, &material);

    if (!e) {
        return 0;
    }

    zox_set(e, ShaderLink, { shader });

    MaterialTextured2D attributes = create_MaterialTextured2D(material);
    zox_set_data(e, MaterialTextured2D, attributes);

    material_textured2D = e;
    shader_textured2D = shader;

    return e;
}