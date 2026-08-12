// removed references to block vox
entity spawn_block(ecs *world, SpawnBlock data) {
    int2 vox_texture_size = int2_single(powers_of_two[block_vox_depth]);
    zox_instance(data.prefab);
    zox_set_unique_name(e, "block");
    set_ZoxName(world, e, data.name);
    zox_set(e, BlockIndex, { data.index });
    zox_set(e, Color, { data.color });
    if (data.model) {
        zox_set(e, BlockModel, { data.model });
    }
    if (data.tag) {
        zox_add_tag_id(e, data.tag);
    }
    if (data.disable_collision) {
        zox_set(e, BlockCollider, { zox_block_air });
    }
    if (data.textures) {
        TextureLinks textures = (TextureLinks) { 0 };
        initialize_TextureLinks(&textures, data.textures);
        for (int i = 0; i < data.textures; i++) {
            entity e2;
            if (data.texture_filename) {
                e2 = spawn_texture_filename(world, data.texture_filename, vox_texture_size);
                zox_set_unique_name(e2, data.texture_filename);
            } else {
                e2 = spawn_texture_dirt(world, prefab_texture_noise, data.seed + i, data.color);
                zox_set_unique_name(e2, "block_dirt_texture");
            }
            zox_set_parent(world, e2, e);
            zox_set(e2, BlockLink, { e });
            if (data.texture_tag) {
                zox_add_tag_id(e2, data.texture_tag);
            }
            textures.value[i] = e2;
        }
        zox_set_ptr(e, TextureLinks, textures);
    }
    return e;
}
