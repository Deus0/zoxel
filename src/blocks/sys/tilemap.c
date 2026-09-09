// NOTE: Pushes Texture data into tilemap from realm
// NOTE: Updates on dirty end state
zox_sys2(RealmTilemapSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockLinks);
    zox_sys_out(BlocksDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BlockLinks, blocks);
        zox_sys_o(BlocksDirty, dirty);
        if (dirty->value != zox_blocks_dirty_tilemaps) {
            continue;
        }
        entity tilemap = zox_get_link(world, e, Tilemap);
        if (!zox_valid(tilemap)) {
            continue;
        }
        if (!zox_has(tilemap, TextureLinks)) {
            zox_loge("Tilemap has no textures [%s]", zox_get_name(tilemap));
            continue;
        }
        byte generating = 0;
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            if (!zox_valid(block)) {
                continue;
            }
            zox_geter(block, TextureLinks, block_textures);
            for (int k = 0; k < block_textures->length; k++) {
                entity texture = block_textures->value[k];
                if (!zox_valid(texture)) {
                    zox_logw("invalid block texture [%s:%i]", zox_get_name(block), k);
                    continue;
                }
                if (zox_has(texture, GenerateTexture) && zox_getv(texture, GenerateTexture)) {
                    // zox_logw("Block [%s:%i]'s Texture [%s] still generating...", zox_get_name(block), j, zox_get_name(texture));
                    generating = 1;
                    break;
                }
            }
        }
        if (generating) {
            continue;
        }
        zox_muter(tilemap, TextureLinks, tilemap_textures);
        resize_TextureLinks(tilemap_textures, 0);
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            if (!zox_valid(block)) {
                continue;
            }
            zox_geter(block, TextureLinks, block_textures);
            for (int k = 0; k < block_textures->length; k++) {
                entity texture =  block_textures->value[k];
                if (!zox_valid(texture)) {
                    continue;
                }
                add_to_TextureLinks(tilemap_textures, texture);
            }
        }
        int length = next_power_of_two_root(tilemap_textures->length);
        dirty->value = zox_blocks_dirty_end;
        zox_setv(tilemap, TilemapSize, int2_single(length));
        zox_setv(tilemap, GenerateTexture, zox_generate_texture_run);
    }
} zox_sys_end(RealmTilemapSystem);
