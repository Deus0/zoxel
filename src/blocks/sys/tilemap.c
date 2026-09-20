// NOTE: Pushes Texture data into tilemap from realm
// NOTE: Updates on dirty end state
zox_sys2(RealmTilemapSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockLinks);
    // zox_sys_out(BlocksDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BlockLinks, blocks);
        /*zox_sys_o(BlocksDirty, dirty);
        if (dirty->value != zox_blocks_dirty_tilemaps) {
            continue;
        }*/
        entity tilemap = zox_get_link(world, e, Tilemap);
        if (!zox_valid(tilemap)) {
            continue;
        }
        byte generating = 0;
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            if (!zox_valid(block) ||
                !zox_has(block, BlockBaked)
            ) {
                continue;
            }
            iter it2 = zox_children(world, block);
            while (zox_children_next(it2)) {
                for (int k = 0; k < it2.count; k++) {
                    entity texture = it2.entities[k];
                    if (!zox_has(texture, Texture)) {
                        continue;
                    }
                    if (zox_has(texture, GenerateTexture) &&
                        zox_getv(texture, GenerateTexture))
                    {
                        // zox_logw("Block [%s:%i]'s Texture [%s] still generating...", zox_get_name(block), j, zox_get_name(texture));
                        generating = 1;
                        break;
                    }
                }
            }
        }
        if (generating) {
            continue;
        }
        // UNlink all old links
        // NOTE: This must be done immediately due to the unlink vs link race conditinos
        ecs_defer_suspend(world);
        zox_unlink_all(world, tilemap, TextureLink);
        ecs_defer_resume(world);
        uint tilemap_textures_length = 0;
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            if (!zox_valid(block) ||
                !zox_has(block, BlockBaked)
            ) {
                continue;
            }
            iter it2 = zox_children(world, block);
            while (zox_children_next(it2)) {
                for (int k = 0; k < it2.count; k++) {
                    entity texture = it2.entities[k];
                    if (!zox_has(texture, Texture)) {
                        continue;
                    }
                    zox_link(
                        world,
                        tilemap,
                        TextureLink,
                        texture);
                    // zox_setv(texture, TilemapIndex, tilemap_textures_length);
                    tilemap_textures_length++;
                    if (dbg_log >= 2) {
                        zox_log("Tilemap linked to block [%s] texture [%s] at [%i]",
                            zox_getn(block),
                            zox_getn(texture),
                            tilemap_textures_length);
                    }
                }
            }
        }
        int length = next_power_of_two_root(tilemap_textures_length);
        zox_setv(tilemap, TilemapSize, int2_single(length));
        zox_setv(tilemap, GenerateTexture, zox_generate_texture_run);
        zox_remove(e, BlocksTilemapUpdate);
        if (dbg_log) {
            zox_log("Tilemap linked [%i] Textures",
                tilemap_textures_length);
        }
    }
} zox_sys_end(RealmTilemapSystem);
