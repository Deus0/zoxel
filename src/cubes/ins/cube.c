entity spawn_cube(ecs* world, entity prefab, float3 position, float scale1) {
    zox_instance(prefab);
    zox_name("cube");
    zox_set(e, Position3D, { position });
    zox_set(e, Scale1D, { scale1 });
    zox_add_tag(e, MeshBasic3D);
    guint2 shader = zox_get_value(shader_basic3D, ShaderGPULink);
    zox_set(e, ShaderLink, { shader_basic3D });
    guint material = spawn_gpu_material(world, e, shader);
    if (!material) {
        zox_log_error("=> [spawn_cube] Failed");
    } else {
        MaterialBasic3D attributes = create_MaterialBasic3D(material);
        zox_set_data(e, MaterialBasic3D, attributes);
    }
    return e;
}
