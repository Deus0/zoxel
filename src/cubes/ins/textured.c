// unique mesh, need to instance these!
entity spawn_cube_textured(ecs* world, entity prefab, entity texture, float3 position, float scale) {
    zox_instance(prefab);
    zox_name("cube_textured");
    zox_add(e, TexturedMesh3D);
    zox_setv(e, Position3D, position);
    zox_setv(e, Scale1, scale);
    if (zox_valid(texture)) {
        clone_texture_data(world, e, texture);
    }
    if (!zox_valid(shader_textured3D)) {
        return e;
    }
    guint2 shader = zox_getv(shader_textured3D, ShaderGPULink);
    zox_set(e, ShaderLink, { shader_textured3D });
    guint gpu_material = spawn_gpu_material_program(shader);
    if (gpu_material) {
        zox_setv(e, MaterialGPULink, gpu_material);
        MaterialTextured3D attributes = create_MaterialTextured3D(gpu_material);
        zox_set_data(e, MaterialTextured3D, attributes);
    }
    return e;
}
