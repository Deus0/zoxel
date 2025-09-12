entity spawn_block_flower(
    ecs *world,
    const byte index
) {
    entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("flower"));
    if (!model) {
        zox_log_error("[flower] model file not found.");
        return 0;
    }

    // for instancing
    zox_prefab_child_named(prefab_block_vox_instanced, prefab_world_block);
    zox_set(prefab_world_block, InstanceLink, { model });

    SpawnBlock spawn_data = {
        .name = "flower",
        .prefab = prefab_block_vox_meta,
        .prefab_texture = prefab_vox_texture,
        .prefab_world_block = prefab_world_block,
        .tag = zox_id(BlockVox),
        .index = index,
        .seed = generate_voxel_seed(index),
        .color = generate_random_voxel_color(),
        .model = zox_block_vox,
        .disable_collision = 1,
        .vox_offset = 1,
        .vox = model
    };

    // our block!
    process_disabled_block_vox(world, &spawn_data, 0);

    const entity e = spawn_block_vox_meta(world, spawn_data);
    if (disable_block_voxes) {
        return e;
    }

    zox_geter(spawn_data.vox, ModelLods, modelLods);
    entity vox_lod = modelLods->value[0];
    // link a texture to it
    const entity texture = spawn_texture(
        world,
        prefab_vox_texture,
        voxel_texture_size
    );
    zox_set_name_e(texture, "grass_texture");
    zox_set(texture, VoxLink, { vox_lod });
    zox_set(texture, VoxBakeSide, { direction_left });
    zox_set(texture, GenerateTexture, { zox_dirty_trigger });
    zox_set(e, TextureLink, { texture });

    return e;
}