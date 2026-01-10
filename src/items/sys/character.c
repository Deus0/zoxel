int character_inventory_count = 8; // 8 | 16; // having blank items seems to b reak it
byte test_give_npcs_blocks = 1;

zox_sys2(CharacterItemsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    zox_sys_out(ItemLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(ItemLinks, items);

        if (state->value != zox_dirty_active) {
            continue;
        }

        if (!test_give_npcs_blocks) {
            continue;
        }

        // Collect Realm Stats
        // zox_geter(realm->value, ItemLinks, realm_items);
        zox_geter(realm->value, BlockLinks, blocks);
        // give npc random voxel
        const entity block = blocks->value[rand() % blocks->length];
        if (zox_valid(block) && zox_has(block, ItemLink)) {
            zox_geter(block, ItemLink, itemLink)
            if (zox_valid(itemLink->value)) {
                const entity item = spawn_user_item(world, itemLink->value, e);
                zox_set(item, Quantity, { 1 + rand() % 3 });
                add_to_ItemLinks(items, item);
            }
        }

    }
} zox_sys_end(CharacterItemsSystem);