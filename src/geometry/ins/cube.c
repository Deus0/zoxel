entity spawn_cube(
    ecs* world,
    entity prefab,
    float3 position,
    float scale1)
{
    if (!zox_valid(shader_basic3D)) {
        zox_loge("spawn_cube failed [shader_basic3D] invalid");
        return 0;
    }
    zox_instance(prefab);
    zox_name("cube");
    zox_setv(e, Position3D, position);
    zox_setv(e, Scale1, scale1);
    zox_add(e, MeshBasic3D);
    guint2 shader = zox_getv(shader_basic3D, ShaderGPULink);
    zox_setv(e, ShaderLink, shader_basic3D );
    guint gpu_material = spawn_gpu_material_program(shader);
    if (gpu_material) {
        zox_setv(e, MaterialGPULink, gpu_material);
        MaterialBasic3D attributes = create_MaterialBasic3D(gpu_material);
        zox_set_data(e, MaterialBasic3D, attributes);
    }
    return e;
}
