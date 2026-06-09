entity spawn_realm_block_model(ecs *world, entity parent, lint seed, char* name, color block_color, byte collision, entity vox, entity texture_vox) {
    // For Instancing?
    // zox_prefab_child_named(prefab_block_vox_instanced, prefab_world_block);
    // zox_set(prefab_world_block, InstanceLink, { vox });
    // Spawn our model block
    SpawnBlock spawn_data = {
        .seed = seed,
        .name = name,
        .model = vox,
        .color = block_color,
        .disable_collision = !collision,
        .prefab = prefab_block_vox_meta,
        .prefab_world_block = prefab_block_vox_instanced, // prefab_world_block,
        .tag = zox_id(BlockVox),
        .model_type = zox_block_vox,
    };
    // TODO: test non instanced voxes
    process_disabled_block_vox(world, &spawn_data, 1);
    entity e = spawn_block_vox_meta(world, spawn_data);
    zox_set_parent(world, e, parent);
    if (disable_block_voxes) {
        return e;
    }
    // NOTE: Spawns a VoxTexture for the Items!
    {
        int2 texture_size = int2_single(powers_of_two[block_vox_depth]);
        entity texture = spawn_texture(world, prefab_vox_texture, texture_size);
        zox_set_unique_name(texture, "block_texture");
        zox_set_parent(world, texture, parent);
        zox_set(texture, GenerateTexture, { zox_dirty_trigger });
        zox_set(texture, VoxBakeSide, { direction_front });
        zox_set(texture, VoxLink, { texture_vox });
        zox_set(e, TextureLink, { texture });
    }
    return e;
}
