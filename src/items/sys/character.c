int character_inventory_count = 8; // 8 | 16; // having blank items seems to b reak it
byte test_give_npcs_blocks = 1;
// NOTE: For NPC item drops
zox_sys2(CharacterItemsSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity inventory = zox_get_child_by_id(world, e, zox_id(Inventory));
        // NOTE: Grabs a random block item and gives it to character
        zox_geter(realm->value, BlockLinks, blocks);
        entity block = blocks->value[rand() % blocks->length];
        if (zox_valid(block) && zox_has(block, ItemLink)) {
            zox_geter_value(block, ItemLink, entity, prefab);
            if (zox_valid(prefab)) {
                byte quantity = rand_range(1, 3); // 1 + rand() % 3;
                entity e2 = spawn_user_item(world, e, prefab);
                zox_set(e2, Quantity, { quantity });
                if (zox_valid(inventory)) {
                    entity slot = zox_get_empty_slot(world, inventory);
                    if (zox_valid(slot)) {
                        zox_muter(slot, DataLink, slot_data);
                        slot_data->value = e2;
                    }
                }
            }
        }
    }
} zox_sys_end(CharacterItemsSpawnSystem);
