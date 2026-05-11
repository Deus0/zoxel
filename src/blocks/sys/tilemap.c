// Push Texture data into tilemap from realm
zox_sys2(TilemapRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TilemapLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TilemapLink, tilemap);
        if (!zox_valid(tilemap->value)) {
            tilemap->value = spawn_tilemap(world, prefab_tilemap);
            zox_set(tilemap->value, RealmLink, { e });
        }
    }
} zox_sys_end(TilemapRealmSpawnSystem);

zox_sys2(RealmTilemapSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(BlocksDirty);
    zox_sys_in(BlockLinks);
    zox_sys_in(TilemapLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(BlocksDirty, state);
        zox_sys_i(BlockLinks, blocks);
        zox_sys_i(TilemapLink, tilemap);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(tilemap->value)) {
            continue;
        }
        byte generating = 0;
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            if (!zox_valid(block)) {
                continue;
            }
            zox_geter(block, TextureLinks, textures);
            for (int k = 0; k < textures->length; k++) {
                entity texture =  textures->value[k];
                if (!zox_valid(texture)) {
                    zox_logw("invalid block texture [%s:%i]", zox_get_name(block), k);
                    continue;
                }
                if (zox_has(texture, GenerateTexture) && zox_gett_value(texture, GenerateTexture)) {
                    zox_logw("Block [%s:%i]'s Texture [%s] still generating...", zox_get_name(block), j, zox_get_name(texture));
                    generating = 1;
                    break;
                }
            }
        }
        if (generating) {
            state->value = zox_dirty_trigger;
            continue;
        }
        zox_muter(tilemap->value, TextureLinks, textures);
        resize_TextureLinks(textures, 0);
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
                add_to_TextureLinks(textures, texture);
            }
        }
        int tilemap_length = next_power_of_two_root(textures->length);
        zox_set(tilemap->value, TilemapSize, { int2_single(tilemap_length) });
        zox_set(tilemap->value, GenerateTexture, { zox_dirty_trigger });
    }
} zox_sys_end(RealmTilemapSystem);

/*zox_sys2(TerrainTextureSetSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TilemapDirty);
    zox_sys_in(RealmLink);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TilemapSize);
    zox_sys_out(TextureLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TilemapDirty, state);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TilemapSize, size);
        zox_sys_o(TextureLinks, textures);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(realm->value) || !zox_has(realm->value, BlockLinks)) {
            zox_log_error("invalid realm in [TerrainTextureSetSystem]");
            continue;
        }
        zox_geter(realm->value, BlocksDirty, blocks_dirty);
        if (blocks_dirty->value != zox_dirty_active) {
            continue;
        }
        // Refresh Texture Links for Tilemap
        zox_geter(realm->value, BlockLinks, blocks);
        clear_memory_component(TextureLinks, textures);
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            if (!zox_valid(block)) {
                continue;
            }
            zox_geter(block, TextureLinks, block_textures);
            for (int k = 0; k < block_textures->length; k++) {
                entity block_texture =  block_textures->value[k];
                if (!zox_valid(block_texture)) {
                    zox_logw("invalid block texture [%s:%i]", zox_get_name(block), k);
                    continue;
                }

                if (zox_has(block_texture, GenerateTexture) && zox_gett_value(block_texture, GenerateTexture)) {
                    zox_logw("Texture [%s] still generating...", zox_get_name(block_texture));
                }
                add_to_TextureLinks(textures, block_texture);
            }
        }
        int tilemap_length = next_power_of_two_root(textures->length);
        size->value = int2_single(tilemap_length);
        generate->value = zox_dirty_trigger;
        // zox_log("Generated Tilemap: count [%i]  length [%i]", textures->length, tilemap_length);
    }
} zox_sys_end(TerrainTextureSetSystem);
*/
