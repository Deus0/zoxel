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

// gives a character the realm's first item with a tag (Hat, Shirt), in its body's slot with a tag (HatSlot)
byte equip_realm_item(ecs* world, entity character, entity realm, entity chest_slot, entity slot_tag, entity item_tag) {
    const entity slot = get_equip_slot_in_children(world, chest_slot, slot_tag);
    if (!zox_valid(slot)) {
        zox_loge("No [%s] on character [%s]", zox_get_name(slot_tag), zox_get_name(character));
        return 0;
    }
    const entity realm_item = zox_get_child_by_id(world, realm, item_tag);
    if (!zox_valid(realm_item)) {
        zox_loge("No [%s] in the realm", zox_get_name(item_tag));
        return 0;
    }
    zox_set_link(world, slot, DataLink, spawn_user_item(world, character, realm_item));
    return 1;
}

// NOTE: Has to run after body parts spawn for slots attachment
void character_player_equips_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_out(BodyDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_o(BodyDirty, dirty);
        if (state->value != zox_dirty_end) {
            continue;
        }
        entity realm = zox_get_link(world, e, RealmLink);
        byte body_updated = 0;
        entity realm_items[256];
        uint realm_items_length = zox_get_children_by_id(
            world,
            realm,
            realm_items,
            256,
            zox_id(Item));
        entity chest_slot = zox_get_child_by_id(
            world,
            e,
            zox_id(Body));
        body_updated |= equip_realm_item(world, e, realm, chest_slot, zox_id(HatSlot), zox_id(Hat));
        body_updated |= equip_realm_item(world, e, realm, chest_slot, zox_id(ShirtSlot), zox_id(Shirt));
        if (body_updated) {
            dirty->value = zox_generate_body_start;
        }
        // Add Second Hand to inventory
        entity inventory = zox_get_child_by_id(world, e, zox_id(Inventory));
        if (!zox_valid(inventory)) {
            continue;
        }
        // NOTE: Adds second hat into inventory for fun
        entity inventory_slot = zox_get_empty_slot(world, inventory);
        if (!zox_valid(inventory_slot)) {
            entity realm_hat = find_slot_type_index(
                world,
                realm_items,
                realm_items_length,
                zox_slot_hat,
                1);
            if (zox_valid(realm_hat)) {
                entity e2 = spawn_user_item(world, e, realm_hat);
                zox_set_link(world, inventory_slot, DataLink, e2);
                // dirty->value = zox_generate_body_start;
                if (dbg_log) {
                    zox_log("Added Hat [%s] to Character [%s]", zox_get_name(realm_hat), zox_get_name(e));
                }
            } else {
                zox_loge("Could not find Hat in Realm");
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(character_player_equips_system);
