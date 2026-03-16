entity spawn_block_grass(ecs *world, byte index, color block_color, entity model) {

    // use instanced mesh prefab
    SpawnBlock data = {
        .name = "grass",
        .prefab = prefab_block_vox_meta,
        .prefab_world_block = prefab_block_vox_instanced,
        .tag = zox_id(BlockVox),
        .model = zox_block_vox,
        .color = block_color,
        .index = index,
        .seed = generate_voxel_seed(index),
        .disable_collision = 1,
        .vox = model
    };

    // thats all fokes
    process_disabled_block_vox(world, &data, 1);

    entity e = spawn_block_vox_meta(world, data);
    zox_prefab_set(e, BlockSound, { 1 });

    if (disable_block_voxes) {
        return e;
    }

    // link a texture to it
    zox_geter(model, ModelLinks, models);   // model group
    entity vox = models->value[0];  // use first model
    zox_geter_value(vox, MaxRenderDepth, byte, max_render_depth);
    zox_geter(vox, ModelLods, mlods);
    entity vox_lod = mlods->value[max_render_depth];

    // Spawn Item Texture
    entity texture = spawn_texture(world, prefab_vox_texture, int2_single(powers_of_two[block_vox_depth]));
    zox_set_name_e(texture, "texture_grass");
    zox_set(texture, VoxLink, { vox_lod });
    zox_set(texture, VoxBakeSide, { direction_left });
    zox_set(texture, GenerateTexture, { zox_dirty_trigger });
    zox_set(e, TextureLink, { texture });

    return e;
}
