zox_sys2(ItemsRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(BlockLinks);
    zox_sys_out(ItemLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(BlockLinks, blocks);
        zox_sys_o(ItemLinks, items);
        if (state->value != zox_generate_realm_items) {
            continue;
        }
        if (!blocks->length) {
            zox_log_error("No blocks to spawn items from")
            continue;
        }
        entity soil_item = 0;
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            if (!zox_valid(block)) {
                zox_log_error("Block Invalid at Realm [%i] - e [%lu] %s", j, block, zox_get_name(block));
                continue;
            }
            entity item = spawn_block_item(world, block);
            add_to_ItemLinks(items, item);
            // NOTE: If soil grass, just use last soil item in list (should be before it)
            if (zox_has(block, BlockSoil)) {
                soil_item = item;
            } else if (zox_has(block, BlockSoilGrass)) {
                zox_set(block, ItemLink, { soil_item });
            }
        }
        zox_logv("At [%f] Realm [items] [%i] spawned.", zox_current_time, items->length);
    }
} zox_sys_end(ItemsRealmSpawnSystem);
