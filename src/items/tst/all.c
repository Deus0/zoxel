byte tst_all_items = 0;

// TODO: Work Item Slots better
void zox_tst_all_items(ecs* world, ClickEventData data) {
    if (tst_all_items) {
        zox_log("Already have All Items.");
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    if (!zox_valid(character) || !zox_valid(realm)) {
        return;
    }
    // TODO: Automatically add to slot when picking up a new item
    //      - make event for picking up item, if fails, just drops into world?
    // Get our Slots
    entity inventory = zox_get_child_by_id(world, character, zox_id(Inventory));
    entity slots[layouts2_children_capacity];
    uint slots_length = zox_get_children_by_id(world, inventory, slots, layouts2_children_capacity, zox_id(Slot));
    zox_geter(realm, ItemLinks, ritems);
    zox_log("Giving [%s] [%i] Items.", zox_get_name(character), ritems->length);
    for (int j = 0; j < ritems->length; j++) {
        entity ritem = ritems->value[j];
        if (!zox_valid(ritem)) {
            zox_log_error("Item invalid [%i]", j);
            continue;
        }
        // find slot
        uint empty_slot_index = 255;
        for (uint k = 0; k < slots_length; k++) {
            entity slot = slots[k];
            entity slot_data = zox_gett_value(slot, DataLink);
            if (!slot_data) {
                empty_slot_index = k;
                break;
            }
        }
        if (empty_slot_index == 255) {
            zox_logw("Out of empty slots.");
            for (uint k = 0; k < slots_length; k++) {
                entity slot_data = zox_gett_value(slots[k], DataLink);
                zox_log("Slot [%i] Data: %s", k, zox_get_name(slot_data));
            }
            continue;
        }
        entity slot = slots[empty_slot_index];
        byte quantity = rand_range(1, 10);
        entity item = spawn_user_item(world, character, ritem);
        zox_set(item, Quantity, { quantity });
        zox_muter(slot, DataLink, slot_data);
        slot_data->value = item;
        zox_log("   + [%s] x%i", zox_get_name(ritem), quantity);
    }
    tst_all_items = 1;
}

/*

// zox_geter(realm->value, ItemLinks, realm_items);
zox_geter(realm->value, BlockLinks, blocks);
// give npc random voxel
entity block = blocks->value[rand() % blocks->length];
if (zox_valid(block) && zox_has(block, ItemLink)) {
    zox_geter(block, ItemLink, itemLink)
    if (zox_valid(itemLink->value)) {
        byte quantity =  1 + rand() % 3;

        entity item = spawn_user_item(world, itemLink->value, e);
        zox_set(item, Quantity, { quantity });
    }
}

 * */
