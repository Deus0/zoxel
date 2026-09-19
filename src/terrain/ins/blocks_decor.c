void process_disabled_block_vox(
    ecs *world,
    SpawnBlock *data,
    byte is_unique_vox)
{
    if (!disable_block_voxes) {
        return;
    }
    if (data->model && is_unique_vox) {
        zox_delete(data->model)
    }
    if (data->prefab_world_block) {
        zox_delete(data->prefab_world_block)
        data->prefab_world_block = 0;
    }
    entity vox = spawn_vox_generated_invisible(
        world,
        prefab_vox_generated,
        color_gray);
    zox_set_unique_name(vox, "unknown");
    zox_setv(vox, VoxType, vox_type_soil);
    // settings!
    data->model = vox;
    data->prefab_texture = prefab_vox_texture;
    data->model = 0;
    data->tag = 0;
    data->bake_vox = 1;
    data->color = color_gray;
}

entity spawn_realm_block_model(
    ecs *world,
    entity parent,
    lint seed,
    char* name,
    color block_color,
    byte is_collision,
    entity vox,
    entity texture_vox,
    byte texture_direction)
{
    // Spawn our model block
    SpawnBlock spawn_data = {
        .seed = seed,
        .name = name,
        .model = vox,
        .color = block_color,
        .disable_collision = !is_collision,
        .prefab = prefab_block_base,
        .prefab_world_block = prefab_block_vox_instanced,
        .tag = zox_id(BlockVox),
        .model_type = zox_block_vox,
    };
    // TODO: test non instanced voxes
    process_disabled_block_vox(world, &spawn_data, 1);
    entity block = spawn_block_vox_meta(
        world,
        spawn_data);
    zox_set_parent(world, block, parent);
    // NOTE: Spawns a VoxTexture for the Items!
    {
        byte length = octree_size(block_depth);
        int2 texture_size = int2_single(length);
        entity texture = spawn_texture(
            world,
            prefab_vox_texture,
            texture_size);
        zox_set_unique_name(texture, name);
        zox_setv(texture, GenerateTexture, zox_generate_texture_run);
        zox_setv(texture, VoxBakeSide, texture_direction);
        zox_setv(texture, ModelLink, texture_vox);
        // zox_setv(e, TextureLink, texture);
        zox_set_parent(world, texture, block);
        zox_link(world, block, TextureLink, texture);
    }
    return block;
}
