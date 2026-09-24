zox_sys2(ItemsRealmSpawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(BlockLinks, blocks);
        if (state->value != zox_generate_realm_items) {
            continue;
        }
        if (!blocks->length) {
            zox_loge("No blocks to spawn items from")
            continue;
        }
        if (dbg_log) {
            zox_log("Spawning [%i] Block Items on Realm", blocks->length);
        }
        entity previous_soil_item = 0;
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            if (!zox_valid(block)) {
                zox_loge("Block Invalid at Realm [%i] - e [%lu] %s",
                    j,
                    block,
                    zox_get_name(block));
                continue;
            }
            entity item = spawn_block_item(
                world,
                e,
                block,
                dbg_log);
            // NOTE: If soil grass, just use last soil item in list (should be before it)
            entity drop_item = item;
            if (zox_has(block, BlockSoilGrass)) {
                drop_item = previous_soil_item;
            }
            // link to drop item
            zox_link(world, block, ItemLink, drop_item);
            if (zox_has(block, BlockSoil)) {
                previous_soil_item = item;
            }
        }
        zox_logv("Realm [items] spawned");
    }
} zox_sys_end(ItemsRealmSpawnSystem);
