entity spawn_cube_with_shader(
    ecs* world,
    entity prefab,
    float3 position,
    float scale1,
    entity shader)
{
    if (!zox_valid(shader)) {
        zox_loge("spawn_cube failed [shader] invalid");
        return 0;
    }
    guint2 shader_link = zox_getv(shader, ShaderGPULink);
    guint gpu_material = spawn_gpu_material_program(shader_link);
    if (!gpu_material) {
        zox_loge("Failed [gpu_material_program] for spawn_cube");
        return 0;
    }
    zox_instance(prefab);
    zox_name("cube");
    zox_setv(e, Position3D, position);
    zox_setv(e, Scale1, scale1);
    zox_add(e, MeshBasic3);
    zox_link(world, e, ShaderLink, shader);
    zox_setv(e, MaterialGPULink, gpu_material);
    MaterialBasic3D attributes = create_MaterialBasic3D(gpu_material);
    zox_set_data(e, MaterialBasic3D, attributes);
    return e;
}

entity spawn_cube(
    ecs* world,
    entity prefab,
    float3 position,
    float scale1)
{
    return spawn_cube_with_shader(
        world,
        prefab,
        position,
        scale1,
        shader_basic3);
}
