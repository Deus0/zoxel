entity shader_textured3D;      // the world's textured meshes (terrain's tilemaps, textured cubes): in the sky's light
entity material_element3;      // the 3D uis' (element3): textured3D unlit, the same at night

typedef struct {
    gint vertex_position;
    gint vertex_uv;
    gint vertex_color;
    gint mvp_matrix;
    gint texture;
    gint brightness;
    gint alpha_cutoff;
} MaterialTextured3D;
zoxc_custom(MaterialTextured3D);

MaterialTextured3D create_MaterialTextured3D(guint material) {
    return (MaterialTextured3D) {
        .vertex_position = zox_gpu_get_material_attribute(material, "vertex_position"),
        .vertex_uv = zox_gpu_get_material_attribute(material, "vertex_uv"),
        .vertex_color = zox_gpu_get_material_attribute(material, "vertex_color"),
        .mvp_matrix = zox_gpu_get_material_property(material, "mvp_matrix"),
        .texture = zox_gpu_get_material_property(material, "zexture"),
        .brightness = zox_gpu_get_material_property(material, "brightness"),
        .alpha_cutoff = zox_gpu_get_material_property(material, "alpha_cutoff")
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
        zox_loge("[shader_textured3D] failed to spawn")
        return 0;
    }
    zox_name("shader_textured3");
    return e;
}

// the 3D uis' material: textured3D's fragment, an unlit vertex (no sky light: they're not in the world)
entity spawn_material_element3(ecs *world) {
    const byte shader_index = get_new_shader_source_index();
    shader_verts[shader_index] = get_shader_source(world, "textured3D_unlit.vert");
    shader_frags[shader_index] = get_shader_source(world, "textured3D.frag");
    entity shader = spawn_shader(world, shader_index);
    if (!shader) {
        zox_loge("[shader_element3] failed to spawn")
        return 0;
    }
    guint material;
    entity e = spawn_material(world, shader, &material);
    zox_set_unique_name(e, "material_element3");
    zox_link(world, e, ShaderLink, shader);
    MaterialTextured3D attributes = create_MaterialTextured3D(material);
    zox_set_data(e, MaterialTextured3D, attributes)
    material_element3 = e;
    return e;
}
