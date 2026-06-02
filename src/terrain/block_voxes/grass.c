extern void delayed_texture_generate(ecs* world, entity e);

entity spawn_block_grass(ecs *world, entity realm, byte index, color block_color, entity model, entity texture_vox) {
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
        .index = index,
        .seed = generate_voxel_seed(index),
        .disable_collision = 1,
        .model = model
    };
    // thats all fokes
    process_disabled_block_vox(world, &data, 1);
    entity e = spawn_block_vox_meta(world, data);
    zox_set_unique_name(e, "block_grass");
    zox_set_parent(world, e, realm);
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
        zox_set_parent(world, texture, realm);
        zox_set(texture, GenerateTexture, { zox_dirty_trigger });
        zox_set(texture, VoxBakeSide, { direction_front }); // direction_left });
        zox_set(texture, VoxLink, { texture_vox });
        zox_set(e, TextureLink, { texture });
    }
    // zox_set(texture, GenerateTexture, { 0 });
    // This should wait for block to finish generating... but how if its a node?
    // Nodegraph Runners should lock a entity its working on?
    // NOTE: Why ??!?!? TIMIEMEIEME
    // delay_event(world, &delayed_texture_generate, texture, 0.5f);
    return e;
}

// link a texture to it
/*zox_geter(model, ModelLinks, models);   // model group
if (!models->length) {
    zox_loge("Model Group [%s] has no sub models.", zox_get_name(model));
    return e;
}*/
// entity vox = models->value[0];  // use first model
// zox_geter_value(vox, MaxRenderDepth, byte, max_render_depth);
// zox_geter(vox, ModelLods, mlods);
// entity vox_lod = mlods->value[max_render_depth];
