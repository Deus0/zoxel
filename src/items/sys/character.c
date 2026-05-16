int character_inventory_count = 8; // 8 | 16; // having blank items seems to b reak it
byte test_give_npcs_blocks = 1;
// NOTE: For NPC item drops
zox_sys2(CharacterItemsSystem) {
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
        // Collect Realm Stats
        zox_geter(realm->value, BlockLinks, blocks);
        // give npc random voxel
        entity block = blocks->value[rand() % blocks->length];
        if (zox_valid(block) && zox_has(block, ItemLink)) {
            zox_geter_value(block, ItemLink, entity, prefab);
            if (zox_valid(prefab)) {
                byte quantity =  1 + rand() % 3;
                entity item = spawn_user_item(world, e, prefab);
                zox_set(item, Quantity, { quantity });
            }
        }
    }
} zox_sys_end(CharacterItemsSystem);
