entity spawn_prefab_character2_player(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("character2_player");
    zox_add_tag(e, PlayerCharacter);
    zox_add_tag(e, PlayerCharacter2);
    zox_prefab_set(e, CameraLink, { 0 });
    zox_prefab_set(e, DisableMovement, { 0 });
    zox_prefab_set(e, Movement2, { float2_zero });
    return e;
}

entity spawn_character2_player(ecs *world, const entity prefab) {
    entity e = spawn_character2(world, prefab, float2_zero);
    // zox_instance(prefab_player_character2D) // child prefabs don't seem to inherit tags ??
    zox_name("character2_player");
    // zox_set(e, Scale1, { 0.4f + ((rand() % 101) / 100.0f) * 0.2f  })
    // zox_set(e, Brightness, { 0.8f + ((rand() % 101) / 100.0f) * 0.6f })
    // const uint2 shader = zox_get_value(shader_textured2D, ShaderGPULink)
    // spawn_gpu_material(world, e, shader);
    // linking camera
    return e;
}
