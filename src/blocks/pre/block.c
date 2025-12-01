entity spawn_prefab_block(ecs *world) {
    zox_prefab();
    zox_prefab_name("block");
    zox_add_tag(e, Voxel);
    zox_add_tag(e, Block);
    zox_prefab_set(e, BlockIndex, { 0 });
    zox_prefab_set(e, Color, { color_white });
    zox_prefab_set(e, BlockModel, { zox_block_solid });
    zox_prefab_set(e, BlockCollider, { zox_block_solid });
    zox_prefab_set(e, TextureLinks, { 0 });
    zox_prefab_set(e, ZoxName, { 0 });
    return e;
}

// removed references to block vox
entity spawn_block(ecs *world, const SpawnBlock *data) {
    zox_instance(data->prefab);
    // zox_name("block")
    zox_set(e, BlockIndex, { data->index });
    zox_set(e, Color, { data->color });
    if (data->model != zox_block_air) {
        zox_set(e, BlockModel, { data->model });
    }
    if (data->tag) {
        zox_add_tag_id(e, data->tag);
    }
    if (data->disable_collision) {
        zox_set(e, BlockCollider, { zox_block_air });
    }
    if (data->textures) {
        TextureLinks textures = (TextureLinks) { 0 };
        initialize_TextureLinks(&textures, data->textures);
        for (int i = 0; i < data->textures; i++) {
            entity e2;
            if (data->texture_filename) {
                e2 = spawn_texture_filename(world, data->texture_filename);
                // zox_log("   + dark block [%i] texture [%i] %lu\n", data->index, i, e2)
            } else {
                // todo: base grass off dirt, as well as sand, using HSV color contrasts
                e2 = spawn_texture_dirt(world, prefab_texture_noise, data->seed + i, data->color);
            }
            if (data->texture_tag) {
                zox_add_tag_id(e2, data->texture_tag);
            }
            textures.value[i] = e2;
        }
        zox_set_ptr(e, TextureLinks, textures);
    }
    set_ZoxName(world, e, data->name);
    return e;
}