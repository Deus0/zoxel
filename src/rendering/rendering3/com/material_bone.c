typedef struct {
    gint vertex_position;
    gint vertex_color;
    gint bone_index;
    gint transform_matrix;
    gint camera_matrix;
    gint bone_matrix;
    gint bone_positions;
    gint brightness;
    gint fog_data;
} MaterialBone;
zoxc_custom(MaterialBone);

MaterialBone create_MaterialBone(guint material) {
    return (MaterialBone) {
        zox_gpu_get_material_attribute(material, "vertex_position"),
        zox_gpu_get_material_attribute(material, "vertex_color"),
        zox_gpu_get_material_attribute(material, "bone_index"),
        zox_gpu_get_material_property(material, "transform_matrix"),
        zox_gpu_get_material_property(material, "camera_matrix"),
        zox_gpu_get_material_property(material, "bone_matrix"),
        zox_gpu_get_material_property(material, "bone_positions"),
        zox_gpu_get_material_property(material, "brightness"),
        zox_gpu_get_material_property(material, "fog_data")
    };
}

entity spawn_material_bone(ecs *world, byte transparent) {
    // entity shader = spawn_shader_bone(world);
    entity shader = transparent ? spawn_shader_source(world, "shader_bone", "bone.vert", "bonet.frag") : spawn_shader_source(world, "shader_bone", "bone.vert", "bone.frag");
    if (!shader) {
        zox_log(" ! failed spawning bone shader")
        return 0;
    }
    uint material; // link to gpu
    entity e = spawn_material(world, shader, &material);
    if (!e) {
        zox_log(" ! failed spawning bone material")
        return 0;
    }
    zox_set(e, ShaderLink, { shader });
    const MaterialBone attributes = create_MaterialBone(material);
    zox_set_data(e, MaterialBone, attributes);
    return e;
}
