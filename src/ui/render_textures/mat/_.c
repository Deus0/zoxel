entity material_render_texture_rgb;
entity material_render_texture_rgba;

entity spawn_material_render_texture(ecs* world, byte is_alpha) {
    char* frag_filename = is_alpha ?
        "render_texture_rgba.frag" :
        "render_texture_rgb.frag";
    entity shader;
    guint2 shader_ids;
    byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "matrixui.vert");
    char* frag = get_shader_source(world, frag_filename);
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    shader = spawn_shader_new(world, shader_index, &shader_ids);
    if (!shader) {
        zox_loge("[spawn_material_render_texture] failed to spawn [shader]");
        return 0;
    }
    zox_set_unique_name(shader, "shader_render_texture");
    guint material_id;
    entity e = spawn_material_from_ids(world, shader, shader_ids, &material_id);
    if (!e) {
        zox_loge("[spawn_material_render_texture] failed to spawn e");
        return 0;
    }
    zox_set_unique_name(e, "material_render_texture");
    zox_set_parent(world, shader, material_id);
    zox_link(world, e, ShaderLink, shader);
    zox_setv(e, CameraBlur, 0);
    // Set material properties
    attributes_matrixui base_attributes = create_attributes_matrixui(material_id);
    zox_set_data(e, attributes_matrixui, base_attributes);
    zox_setv(e, MaterialBlur,
        zox_gpu_get_material_property(material_id, "blur"));
    zox_setv(e, MaterialVignette,
        zox_gpu_get_material_property(material_id, "vignette"));
    return e;
}

void spawn_materials_render_textures(ecs* world) {
    material_render_texture_rgb = spawn_material_render_texture(
        world,
        0);
    material_render_texture_rgba = spawn_material_render_texture(
        world,
        1);
}
