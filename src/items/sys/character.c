int character_inventory_count = 8; // 8 | 16; // having blank items seems to b reak it
byte test_give_npcs_blocks = 1;

void add_item_to_inventory(
    ecs* world,
    entity user,
    entity inventory,
    entity item)
{
    if (!zox_valid(item)) {
        return;
    }
    byte quantity = rand_range(1, 3);
    entity user_item = spawn_user_item(
        world,
        user,
        item);
    zox_setv(user_item, Quantity, quantity);
    if (zox_valid(inventory)) {
        entity slot = zox_get_empty_slot(
            world,
            inventory);
        if (zox_valid(slot)) {
            zox_muter(slot, DataLink, slot_data);
            slot_data->value = user_item;
        }
    }
}

extern entity Hat;

// NOTE: For NPC item drops
zox_sys2(CharacterItemsSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity inventory = zox_get_child_by_id(world, e, zox_id(Inventory));
        entity realm = zox_get_link(world, e, RealmLink);
        // cookie
        entity drop_item;
        byte drop_chance = rand_range(0, 100);
        if (drop_chance <= 70) {
            drop_item = zox_get_child_by_id(world, realm, ItemConsumable);
        } else if (drop_chance <= 95) {
            zox_geter(realm, BlockLinks, blocks);
            entity block = blocks->value[rand() % blocks->length];
            if (!zox_valid(block)) {
                continue;
            }
            drop_item = zox_get_link(world, block, ItemLink);
        } else {
            drop_item = zox_get_child_by_id(world, realm, Hat);
        }
        add_item_to_inventory(world, e, inventory, drop_item);
    }
} zox_sys_end(CharacterItemsSpawnSystem);
