byte tst_all_items = 0;

void give_character_all_items(ecs* world, entity e) {
    entity realm = zox_get_link(world, e, RealmLink);
    if (!zox_valid(realm)) {
        return;
    }
    entity inventory = zox_get_child_by_id(
        world,
        e,
        zox_id(Inventory));
    if (!zox_valid(inventory)) {
        zox_loge("Character has no Inventory");
        return;
    }
    zox_log("Giving [%s] [X] Items.",
        zox_getn(e));
    iter it2 = zox_children(world, realm);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity realm_item = it2.entities[j];
            if (!zox_has(realm_item, Item)) {
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
            slot_data->value = e2;
            zox_add(slot, DataDirty);
            zox_log("   + [%s] x%i", zox_get_name(realm_item), quantity);
        }
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
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        return;
    }
    // TODO: Automatically add to slot when picking up a new item
    //      - make event for picking up item, if fails, just drops into world?
    // Get our Slots
    // Add more slots - new grid length is 7
    entity inventory = zox_get_child_by_id(world, character, zox_id(Inventory));
    add_more_slots(world, inventory, (8 * 8) - 16);
    delay_event(world, &give_character_all_items, character, 1.0f);
    tst_all_items = 1;
}

