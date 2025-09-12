entity spawn_realm_block_rubble(
    ecs *world,
    const byte index,
    char* name,
    const color block_color,
    const byte vox_type
) {
    const entity vox = spawn_vox_basic(
        world,
        prefab_vox,
        block_vox_depth,
        block_vox_depth
    );
    zox_set_unique_name(vox, "block_rubble");
    zox_set(vox, VoxType, { vox_type });
    zox_set(vox, Color, { block_color });
    zox_set(vox, GenerateVox, { zox_dirty_trigger });
    zox_set(vox, RenderDepth, { block_vox_depth });

    // for instancing
    zox_prefab_child_named(prefab_block_vox_instanced, prefab_world_block);
    zox_set(prefab_world_block, InstanceLink, { vox });

    SpawnBlock spawn_data = {
        .name = name,
        .prefab = prefab_block_vox_meta,
        .prefab_world_block = prefab_world_block,
        .vox = vox,
        .tag = zox_id(BlockVox),
        .model = zox_block_vox,
        .index = index,
        .seed = generate_voxel_seed(index),
        .color = block_color,
        .disable_collision = 1,
    };

    // TODO: test non instanced voxes
    process_disabled_block_vox(world, &spawn_data, 1);

    const entity e = spawn_block_vox_meta(world, spawn_data);
    if (disable_block_voxes) {
        return e;
    }

    // zox_geter(spawn_data.vox, ModelLods, modelLods);
    // entity vox_lod = modelLods->value[0];
    // link a texture to it
    /*const entity texture = spawn_texture(
        world,
        prefab_vox_texture,
        voxel_texture_size
    );
    zox_set_name_e(texture, "grass_texture");
    zox_set(texture, VoxLink, { vox });
    zox_set(texture, VoxBakeSide, { direction_up });
    zox_set(texture, GenerateTexture, { zox_dirty_trigger });

    zox_set(e, TextureLink, { texture });*/

    return e;
}