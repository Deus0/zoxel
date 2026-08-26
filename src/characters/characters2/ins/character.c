entity spawn_character2(
    ecs *world,
    entity prefab,
    float2 position)
{
    zox_instance(prefab);
    zox_add(e, Initialize);
    zox_set(e, Position2, { position });
    zox_set(e, Scale1, { 1 });
    zox_set(e, Brightness, { 0.8f + ((rand() % 101) / 100.0f) * 0.6f });
    clone_texture_file_to_entity(world, e, "taskbar_body");
    // TODO: Just use global material here
    guint2 shader = zox_getv(shader_textured2D, ShaderGPULink);
    guint gpu_material = spawn_gpu_material_program(shader);
    if (gpu_material) {
        zox_setv(e, MaterialGPULink, gpu_material);
    }
    return e;
}
