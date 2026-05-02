entity spawn_shader_skybox(ecs *world) {

    byte shader_index = get_new_shader_source_index();

    char* vert = get_shader_source(world, "skybox.vert");
    char* frag = get_shader_source(world, "skybox.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;

    entity e = spawn_shader(world, shader_index);

    if (!e) {
        zox_log_error("[shader_skybox] failed to spawn")
        return 0;
    }

    zox_name("shader_skybox");

    return e;
}

void set_skybox_material_color(uint material, color_rgb top_color, color_rgb bottom_color) {

    if (!skybox) {
        return;
    }

    float3 top_colorf = color_rgb_to_float3(top_color);
    float3 bottom_colorf = color_rgb_to_float3(bottom_color);

    zox_gpu_material(material);
    zox_gpu_float3(zox_gpu_get_material_property(material, "sky_top_color"), top_colorf);
    zox_gpu_float3(zox_gpu_get_material_property(material, "sky_bottom_color"), bottom_colorf);

    zox_gpu_material(0);
}
