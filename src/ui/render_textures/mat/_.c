entity shader_render_texture;
entity material_render_texture;

MaterialAttributesRenderTexture create_MaterialAttributesRenderTexture(guint material) {
    return (MaterialAttributesRenderTexture) {
        .vertex_position = zox_gpu_get_material_attribute(material, "vertex_position"),
        .vertex_uv = zox_gpu_get_material_attribute(material, "vertex_uv"),
        .camera_matrix = zox_gpu_get_material_property(material, "camera_matrix"),
        .transform_matrix = zox_gpu_get_material_property(material, "matrix"),
        .texture = zox_gpu_get_material_property(material, "tex"),
    };
}

entity spawn_shader_render_texture(ecs *world) {
    byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "matrixui.vert");
    char* frag = get_shader_source(world, "render_texture.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_loge("[shader_render_texture] failed to spawn");
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
    guint material;
    entity e = spawn_material(world, shader, &material);
    zox_set_unique_name(e, "render_texture2");
    zox_set(e, ShaderLink, { shader });
    const MaterialAttributesRenderTexture attributes = create_MaterialAttributesRenderTexture(material);
    zox_set_data(e, MaterialAttributesRenderTexture, attributes);
    zox_set(e, CameraBlur, { 0 });
    zox_set(e, MaterialBlur, { zox_gpu_get_material_property(material, "blur") });
    zox_set(e, MaterialVignette, { zox_gpu_get_material_property(material, "vignette") });
    // const MaterialTextured2D base_attributes = create_MaterialTextured2D(material);
    // zox_set_data(e, MaterialTextured2D, base_attributes);
    attributes_matrixui base_attributes = create_attributes_matrixui(material);
    zox_set_data(e, attributes_matrixui, base_attributes);
    // attributes_matrixui base_attributes = ;
    // zox_set(e, attributes_matrixui, create_attributes_matrixui(material));
    return e;
}

void spawn_materials_render_textures(ecs* world) {
    material_render_texture = spawn_material_render_texture(world);
}
