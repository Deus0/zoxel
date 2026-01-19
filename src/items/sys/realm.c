// TODO: Remove Global Items
/*entity meta_item_block_dark;
entity meta_item_block_obsidian;
entity meta_item_block_sand;
entity meta_item_block_stone;
entity meta_item_block_dungeon_core;*/

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

        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];

            if (!zox_valid(block)) {
                zox_log_error("Block Invalid at Realm [%i] - e [%lu] %s", j, block, zox_get_name(block));
                continue;
            }

            entity item = spawn_block_item(world, block);
            add_to_ItemLinks(items, item);

            /*if (i == zox_block_dirt_grass - 1) {
                const entity item_block_dirt = items.value[zox_block_dirt - 1];
                zox_set(block, ItemLink, { item_block_dirt });
            }*/
        }

        /*meta_item_block_obsidian = items->value[zox_block_obsidian - 1];
        meta_item_block_dark = items->value[zox_block_dark - 1];
        meta_item_block_sand = items->value[zox_block_sand - 1];
        meta_item_block_stone = items->value[zox_block_stone - 1];
        meta_item_block_dungeon_core = items->value[zox_block_dungeon_core - 1];*/

        zox_logv("At [%f] Realm [items] [%i] spawned.", zox_current_time, items->length);
    }
} zox_sys_end(ItemsRealmSpawnSystem);


    /*void spawn_realm_items(ecs *world, const entity realm) {
        if (!zox_has(realm, ItemLinks)) {
            zox_log_error("Realm does not have ItemLinks [%lu]", realm)
            return;
        }
        if (!zox_has(realm, BlockLinks)) {
            zox_log_error("Realm does not have BlockLinks [%lu]", realm)
            return;
        }
        zox_geter(realm, BlockLinks, blocks);
        if (!blocks) {
            zox_log_error("Realm blocks was null [%lu]", realm);
            return;
        }*/