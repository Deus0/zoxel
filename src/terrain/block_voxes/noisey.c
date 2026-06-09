/*entity spawn_realm_block_noisey(ecs *world, entity parent, char* name, color block_color, lint seed) {
    zox_prefab_child_named(prefab_block_vox_instanced, prefab_world_block);
    entity vox;
    {
        byte mdepth = block_vox_depth_limits.y;
        vox = spawn_vox_basic(world, prefab_vox, block_vox_depth, mdepth);
        zox_set_unique_name(vox, "dirt_pile");
        zox_set_parent(world, vox, parent);
        zox_set(vox, VoxType, { vox_type_noisey });
        zox_set(vox, Color, { block_color });
        zox_set(vox, RenderDepth, { block_vox_depth });
        zox_set(vox, Generate, { zox_dirty_trigger });
        zox_set(prefab_world_block, InstanceLink, { vox });
    }
    SpawnBlock spawn_data = {
        .name = name,
        .prefab = prefab_block_vox_meta,
        .prefab_world_block = prefab_world_block,
        .model = vox,
        .tag = zox_id(BlockVox),
        .model_type = zox_block_vox,
        .seed = seed,
        .color = block_color,
    };
    // disable
    process_disabled_block_vox(world, &spawn_data, 1);
    entity e = spawn_block_vox_meta(world, spawn_data);
    zox_set_parent(world, e, parent);
    if (disable_block_voxes) {
        return e;
    }
    {
        int2 texture_size = int2_single(powers_of_two[block_vox_depth]);
        entity texture = spawn_texture(world, prefab_vox_texture, texture_size);
        zox_set_unique_name(texture, "block_texture");
        zox_set_parent(world, texture, parent);
        zox_set(texture, GenerateTexture, { zox_dirty_trigger });
        zox_set(texture, VoxBakeSide, { direction_front });
        zox_set(texture, VoxLink, { vox });
        zox_set(e, TextureLink, { texture });
    }
    return e;
}*/
