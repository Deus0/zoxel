entity spawn_block_grass(
    ecs *world,
    const byte index,
    const color block_color,
    const entity model
) {
    // zox_log("+ spawning realm_block with model [%s]", zox_get_name(model))
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
    if (disable_block_voxes) {
        return e;
    }

    // link a texture to it
    zox_geter(model, ModelLinks, models);   // model group
    entity vox = models->value[0];  // use first model
    zox_geter_value(vox, MaxRenderDepth, byte, max_render_depth);
    zox_geter(vox, ModelLods, modelLods);
    entity vox_lod = modelLods->value[max_render_depth];

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