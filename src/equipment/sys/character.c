entity get_equip_slot_in_children(ecs *world, entity e, entity id) {
    if (zox_has(e, EquipSlot) && zox_has_id(e, id)) {
        // check slot type
        return e;
    }
    entity slots[zox_children_capacity];
    uint length = zox_get_children_by_id(world, e, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        entity slot = slots[k];
        entity equip = get_equip_slot_in_children(world, slot, id);
        if (equip) {
            return equip;
        }
    }
    return 0;
}

zox_sys2(CharacterPlayerEquipsSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    zox_sys_out(BodyDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(BodyDirty, dirty);
        if (state->value != zox_dirty_end) { // active) {
            continue;
        }
        zox_geter(realm->value, ItemLinks, realm_items);
        entity chest_slot = zox_get_child_by_id(world, e, zox_id(Body));
        entity hat_slot = get_equip_slot_in_children(world, chest_slot, zox_id(HatSlot));
        if (zox_valid(hat_slot)) {
            entity realm_hat = find_slot_type(world, realm_items->value, realm_items->length, zox_slot_hat);
            if (zox_valid(realm_hat)) {
                entity e2 = spawn_user_item(world, e, realm_hat);
                zox_muter(hat_slot, DataLink, slot_data);
                slot_data->value = e2;
                if (dbg_log) {
                    zox_log("Added Hat [%s] to Character [%s]", zox_get_name(realm_hat), zox_get_name(e));
                }
            } else {
                zox_loge("Could not find Hat in Realm");
            }
        } else {
            zox_loge("Could not find Hat Slot on Character [%s]", zox_get_name(e));
        }
        // TODO: Add Shirt
        // Add Second Hand to inventory
        entity inventory = zox_get_child_by_id(world, e, zox_id(Inventory));
        if (!zox_valid(inventory)) {
            continue;
        }
        entity inventory_slot = zox_get_empty_slot(world, inventory);
        if (!zox_valid(inventory_slot)) {
            entity realm_hat = find_slot_type_index(world, realm_items->value, realm_items->length, zox_slot_hat, 1);
            if (zox_valid(realm_hat)) {
                entity e2 = spawn_user_item(world, e, realm_hat);
                zox_muter(inventory_slot, DataLink, slot_data);
                slot_data->value = e2;
                dirty->value = zox_generate_body_start;
                if (dbg_log) {
                    zox_log("Added Hat [%s] to Character [%s]", zox_get_name(realm_hat), zox_get_name(e));
                }
            } else {
                zox_loge("Could not find Hat in Realm");
            }
        }
    }
} zox_sys_end(CharacterPlayerEquipsSystem);
