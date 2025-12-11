entity meta_item_block_dirt;
entity meta_item_block_dark;
entity meta_item_block_obsidian;
entity meta_item_block_sand;
entity meta_item_block_stone;
entity meta_item_block_dungeon_core;

void spawn_realm_items(ecs *world, const entity realm) {
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
    }
    if (blocks->length == 0 || blocks->value == NULL) {
        zox_log_error("No blocks to spawn items from")
        return;
    }
    // i should make a BlockItemLinks perhaps? nah  that overcomplicates
    // clear previous
    zox_geter(realm, ItemLinks, old);
    // if (old->value) return; // TODO: Temp; Remove when crashes gone

    if (old) {
        for (int i = 0; i < old->length; i++) {
            if (old->value[i]) {
                zox_delete(old->value[i])
            }
        }
    }

    ItemLinks items = (ItemLinks) { 0 };
    initialize_ItemLinks(&items, blocks->length);
    for (int i = 0; i < blocks->length; i++) {
        const entity block = blocks->value[i];
        if (!zox_valid(block)) {
            zox_log_error("Block Invalid at Realm [%i] - e [%lu] %s", i, block, zox_get_name(block));
            items.value[i] = 0;
            continue;
        }
        items.value[i] = spawn_block_item(world, block);
        if (i == zox_block_dirt_grass - 1) {
            const entity item_block_dirt = items.value[zox_block_dirt - 1];
            zox_set(block, ItemLink, { item_block_dirt });
        }
    }
    zox_set_ptr(realm, ItemLinks, items);

    meta_item_block_dirt = items.value[zox_block_dirt - 1];
    meta_item_block_obsidian = items.value[zox_block_obsidian - 1];
    meta_item_block_dark = items.value[zox_block_dark - 1];
    meta_item_block_sand = items.value[zox_block_sand - 1];
    meta_item_block_stone = items.value[zox_block_stone - 1];
    meta_item_block_dungeon_core = items.value[zox_block_dungeon_core - 1];

    zox_logv("At [%f] Realm [items] [%i] spawned.", zox_current_time, items.length);
}
