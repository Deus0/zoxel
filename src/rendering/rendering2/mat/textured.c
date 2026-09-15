entity shader_textured2D = 0;
entity material_textured2D = 0;

entity spawn_shader_textured2D(ecs *world) {
    byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, "textured2D.vert");
    char* frag = get_shader_source(world, "textured2D.frag");
    // char* frag = get_shader_source(world, "uv_debug.frag");
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
    zox_link(world, e, ShaderLink, shader);
    MaterialTextured2D attributes = create_MaterialTextured2D(material);
    zox_set_data(e, MaterialTextured2D, attributes);
    material_textured2D = e;
    shader_textured2D = shader;
    return e;
}
