// unique mesh, need to instance these!
entity spawn_cube_textured(ecs* world, entity prefab, float3 position, entity texture, float scale1) {

    entity e = spawn_cube(world, prefab, position, scale1);
    zox_name("cube_textured");

    if (zox_valid(texture)) {
        clone_texture_data(world, e, texture);
    }

    if (!zox_valid(shader_textured3D)) {
        return e;
    }

    spawn_gpu_mesh(world, e);
    spawn_gpu_uvs(world, e);
    spawn_gpu_colors(world, e);
    spawn_gpu_texture(world, e);

    uint2 shader = zox_get_value(shader_textured3D, ShaderGPULink);
    zox_set(e, ShaderLink, { shader_textured3D });

    uint material = spawn_gpu_material(world, e, shader);
    if (!material) {
        zox_log_error("=> [spawn_cube_textured] Failed");
    } else {
        MaterialTextured3D attributes = create_MaterialTextured3D(material);
        zox_set_data(e, MaterialTextured3D, attributes);
    }

    return e;
}
