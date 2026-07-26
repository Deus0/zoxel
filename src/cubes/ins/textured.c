// unique mesh, need to instance these!
entity spawn_cube_textured(ecs* world, entity prefab, entity texture, float3 position, float scale1) {
    zox_instance(prefab);
    zox_name("cube_textured");
    zox_add_tag(e, TexturedMesh3D);
    zox_set(e, Position3D, { position });
    zox_set(e, Scale1, { scale1 });
    if (zox_valid(texture)) {
        clone_texture_data(world, e, texture);
    }
    if (!zox_valid(shader_textured3D)) {
        return e;
    }
    guint2 shader = zox_get_value(shader_textured3D, ShaderGPULink);
    zox_set(e, ShaderLink, { shader_textured3D });
    guint material = spawn_gpu_material(world, e, shader);
    if (!material) {
        zox_loge("=> [spawn_cube_textured] Failed");
    } else {
        MaterialTextured3D attributes = create_MaterialTextured3D(material);
        zox_set_data(e, MaterialTextured3D, attributes);
    }
    return e;
}
