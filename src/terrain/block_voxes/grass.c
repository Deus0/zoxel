/*extern void delayed_texture_generate(ecs* world, entity e);

entity spawn_block_grass(ecs *world, entity parent, color block_color, entity model, entity texture_vox, lint seed) {
    if (!zox_valid(model)) {
        zox_loge("Grass Model does not exist");
        return 0;
    }
    // use instanced mesh prefab
    SpawnBlock data = {
        .name = "grass",
        .prefab = prefab_block_vox_meta,
        .prefab_world_block = prefab_block_vox_instanced,
        .tag = zox_id(BlockVox),
        .model_type = zox_block_vox,
        .color = block_color,
        .seed = seed,
        .disable_collision = 1,
        .model = model
    };
    // thats all fokes
    process_disabled_block_vox(world, &data, 1);
    entity e = spawn_block_vox_meta(world, data);
    zox_set_unique_name(e, "block_grass");
    zox_set_parent(world, e, parent);
    zox_prefab_set(e, BlockSound, { 1 });
    if (disable_block_voxes) {
        return e;
    }
    // Spawn Item Texture
    // zox_log("Created Item Texture for [%s]", zox_get_name(vox));
    if (!zox_valid(texture_vox)) {
        zox_loge("texture_vox is invalid in grass block spawning.");
        return e;
    }
    {
        int2 texture_size = int2_single(powers_of_two[block_vox_depth]);
        entity texture = spawn_texture(world, prefab_vox_texture, texture_size);
        zox_set_unique_name(texture, "texture_grass");
        zox_set_parent(world, texture, parent);
        zox_set(texture, GenerateTexture, { zox_dirty_trigger });
        zox_set(texture, VoxBakeSide, { direction_front }); // direction_left });
        zox_set(texture, VoxLink, { texture_vox });
        zox_set(e, TextureLink, { texture });
    }
    return e;
}*/
