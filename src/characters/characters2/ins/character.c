entity spawn_character2(ecs *world, entity prefab, float2 position) {
    zox_instance(prefab);
    zox_set(e, Position2, { position });
    // zox_set(e, Scale1D, { 0.4f + ((rand() % 101) / 100.0f) * 0.2f  });
    zox_set(e, Scale1D, { 1 });
    zox_set(e, Brightness, { 0.8f + ((rand() % 101) / 100.0f) * 0.6f });
    // zox_set(e, AnimateTexture, { (((rand() % 100) / 100.0f) * noise_animation_speed) })

    // TODO: Just use global material here
    uint2 shader = zox_get_value(shader_textured2D, ShaderGPULink);

    uint material = spawn_gpu_material(world, e, shader);
    if (!material) {
        zox_log_error("character2D material failed to initialize");
    }

    spawn_gpu_texture(world, e);
    clone_texture_file_to_entity(world, e, "taskbar_body");

    return e;
}