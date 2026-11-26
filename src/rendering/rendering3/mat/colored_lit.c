// a flat shaded colored mesh lit by the sun (colored3D_lit): the meshes linking it (MaterialLink) draw with it
entity material_colored3D_lit;

typedef struct {
    MaterialColored3D colored;  // the same mesh data and uniforms as colored3D
    guint model_matrix;
    guint camera_position;
    guint sun_direction;
    guint sun_light;
    guint ambient_light;
} MaterialColoredLit3D;
zoxc_custom(MaterialColoredLit3D);

MaterialColoredLit3D create_MaterialColoredLit3D(guint material) {
    return (MaterialColoredLit3D) {
        .colored = create_MaterialColored3D(material),
        .model_matrix = zox_gpu_get_material_property(material, "model_matrix"),
        .camera_position = zox_gpu_get_material_property(material, "camera_position"),
        .sun_direction = zox_gpu_get_material_property(material, "sun_direction"),
        .sun_light = zox_gpu_get_material_property(material, "sun_light"),
        .ambient_light = zox_gpu_get_material_property(material, "ambient_light")
    };
}

entity spawn_material_colored3D_lit(ecs *world) {
    const byte shader_index = get_new_shader_source_index();
    shader_verts[shader_index] = get_shader_source(world, "colored3D_lit.vert");
    shader_frags[shader_index] = get_shader_source(world, "colored3D_lit.frag");
    const entity shader = spawn_shader(world, shader_index);
    if (!shader) {
        zox_loge("[shader_colored3D_lit] failed to spawn")
        return 0;
    }
    uint material;
    const entity e = spawn_material(world, shader, &material);
    zox_link(world, e, ShaderLink, shader);
    const MaterialColoredLit3D attributes = create_MaterialColoredLit3D(material);
    zox_set_data(e, MaterialColoredLit3D, attributes)
    material_colored3D_lit = e;
    return e;
}
