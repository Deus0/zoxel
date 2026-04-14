entity shader_render_texture;
entity material_render_texture;

MaterialAttributesRenderTexture create_MaterialAttributesRenderTexture(uint material) {
    return (MaterialAttributesRenderTexture) {
        .vertex_position = glGetAttribLocation(material, "vertex_position"),
        .vertex_uv = glGetAttribLocation(material, "vertex_uv"),
        .camera_matrix = glGetUniformLocation(material, "camera_matrix"),
        .transform_matrix = glGetUniformLocation(material, "transform_matrix"),
        .texture = glGetUniformLocation(material, "tex"),
        .blur_strength = glGetUniformLocation(material, "blur_strength")
    };
}

entity spawn_shader_render_texture(ecs *world) {

    byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "render_texture.vert");
    char* frag = get_shader_source(world, "render_texture.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;

    entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_log_error("[shader_render_texture] failed to spawn");
        return 0;
    }

    zox_name("shader_render_texture");
    return e;
}

entity spawn_material_render_texture(ecs* world) {

    entity shader = spawn_shader_render_texture(world);
    if (!shader) {
        return 0;
    }
    shader_render_texture = shader;

    uint material;

    entity e = spawn_material(world, shader, &material);
    zox_set(e, ShaderLink, { shader });

    // Shader properties
    const MaterialAttributesRenderTexture attributes = create_MaterialAttributesRenderTexture(material);

    zox_set_data(e, MaterialAttributesRenderTexture, attributes);
    zox_set(e, CameraBlur, { 0 });

    return e;
}

void spawn_materials_render_textures(ecs* world) {
    material_render_texture = spawn_material_render_texture(world);
}
