byte tst_all_items = 0;

void give_character_all_items(ecs* world, entity e) {
    entity realm = zox_getv(e, RealmLink);
    if (!zox_valid(realm)) {
        return;
    }
    entity inventory = zox_get_child_by_id(world, e, zox_id(Inventory));
    zox_geter(realm, ItemLinks, realm_items);
    zox_log("Giving [%s] [%i] Items.", zox_get_name(e), realm_items->length);
    for (int j = 0; j < realm_items->length; j++) {
        entity realm_item = realm_items->value[j];
        if (!zox_valid(realm_item)) {
            zox_log_error("Item invalid [%i]", j);
            continue;
        }
        entity slot = zox_get_empty_slot(world, inventory);
        if (!zox_valid(slot)) {
            zox_logw("[Inventory] Out of empty slots.");
            zox_print_slots(world, inventory);
            break;
        }
        byte quantity = 1;
        entity e2 = spawn_user_item(world, e, realm_item);
        if (zox_has(realm_item, Quantity)) {
            quantity = rand_range(4, 16);
            zox_set(e2, Quantity, { quantity });
        }
        zox_muter(slot, DataLink, slot_data);
        zox_muter(slot, DataDirty, dirty);
        slot_data->value = e2;
        dirty->value = zox_dirty_trigger;
        zox_log("   + [%s] x%i", zox_get_name(realm_item), quantity);
    }
}

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
    if (!zox_valid(character)) {
        return;
    }
    // TODO: Automatically add to slot when picking up a new item
    //      - make event for picking up item, if fails, just drops into world?
    // Get our Slots
    // Add more slots - new grid length is 7
    entity inventory = zox_get_child_by_id(world, character, zox_id(Inventory));
    add_more_slots(world, inventory, (7 * 7) - 25);
    delay_event(world, &give_character_all_items, character, 1.0f);
    tst_all_items = 1;
}

        // find slot
        /*uint empty_slot_index = 255;
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
        entity slot = slots[empty_slot_index];*/


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
