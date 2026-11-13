// basic3's uniforms (MaterialBasic3D, drawn by basic3_render_system), shaded by face and a lighter fog
entity spawn_shader_cloud(ecs* world) {
    byte shader_index = get_new_shader_source_index();
    shader_verts[shader_index] = get_shader_source(world, "cloud.vert");
    shader_frags[shader_index] = get_shader_source(world, "cloud.frag");
    entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_loge("[shader_cloud] failed to spawn");
        return 0;
    }
    zox_name("shader_cloud");
    return e;
}
