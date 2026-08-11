entity spawn_prefab_character2(ecs *world) {
    zox_prefab();
    zox_add_tag(e, Character2D);
     // UIs
    zox_prefab_set(e, ElementLinks, { 0 });
    // Physics
    zox_add_tag(e, Frictioned);
    zox_prefab_set(e, Position2, { float2_zero });
    // zox_prefab_set(e, Rotation2, { 0 });
    zox_prefab_set(e, Rotation2, { - 90 * degreesToRadians });
    zox_prefab_set(e, Scale1, { 1 });
    add_physics2D(world, e, world_grid2D_size);
    // GPU
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, MaterialGPULink, { 0 });
    zox_prefab_set(e, ShaderLink, { 0 });
    zox_prefab_set(e, TextureGPULink, { 0 });

    /*zox_add_tag(e, Texture)
    zox_prefab_set(e, TextureData, { 0, NULL })
    zox_prefab_set(e, TextureSize, { texture_size })
    zox_prefab_set(e, TextureDirty, { 0 })
    zox_prefab_set(e, Seed, { 666 })
    zox_prefab_set(e, GenerateTexture, { zox_dirty_trigger })*/
    // add_animated_noise_texture(world, e);

    return e;
}
