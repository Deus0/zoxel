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

// every equip slot with a tag under a slot (a shoe slot on each foot), depth first
uint get_equip_slots_in_children(ecs* world, entity e, entity id, entity* found, uint found_max) {
    uint count = 0;
    if (zox_has(e, EquipSlot) && zox_has_id(e, id) && count < found_max) {
        found[count++] = e;
    }
    entity slots[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, e, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length && count < found_max; k++) {
        count += get_equip_slots_in_children(world, slots[k], id, found + count, found_max - count);
    }
    return count;
}

// gives a character the realm's first item with a tag (Hat, Shirt, Shoes), in each of its body's slots with a tag (HatSlot)
byte equip_realm_item(ecs* world, entity character, entity realm, entity chest_slot, entity slot_tag, entity item_tag) {
    entity slots[8];
    const uint length = get_equip_slots_in_children(world, chest_slot, slot_tag, slots, 8);
    if (!length) {
        zox_loge("No [%s] on character [%s]", zox_get_name(slot_tag), zox_get_name(character));
        return 0;
    }
    const entity realm_item = zox_get_child_by_id(world, realm, item_tag);
    if (!zox_valid(realm_item)) {
        zox_loge("No [%s] in the realm", zox_get_name(item_tag));
        return 0;
    }
    for (uint k = 0; k < length; k++) {
        zox_set_link(world, slots[k], DataLink, spawn_user_item(world, character, realm_item));
    }
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
        body_updated |= equip_realm_item(world, e, realm, chest_slot, zox_id(ShortsSlot), zox_id(Shorts));
        body_updated |= equip_realm_item(world, e, realm, chest_slot, zox_id(ShoeSlot), zox_id(Shoes));
        if (body_updated) {
            dirty->value = zox_generate_body_start;
        }
        // spares in the inventory: the realm's second hat and second shirt (the vest), to swap in
        entity inventory = zox_get_child_by_id(world, e, zox_id(Inventory));
        if (!zox_valid(inventory)) {
            continue;
        }
        const entity spares[] = {
            find_slot_type_index(world, realm_items, realm_items_length, zox_slot_hat, 1),
            find_slot_type_index(world, realm_items, realm_items_length, zox_slot_shirt, 1),
        };
        uint placed = 0;
        for (byte k = 0; k < 2; k++) {
            // the nth empty slot, the links made this frame aren't seen yet
            const entity slot = zox_get_empty_slot_n(world, inventory, placed);
            if (!zox_valid(spares[k]) || zox_valid(slot)) {
                continue;
            }
            zox_set_link(world, slot, DataLink, spawn_user_item(world, e, spares[k]));
            placed++;
            if (dbg_log) {
                zox_log("Added [%s] to [%s]'s inventory", zox_get_name(spares[k]), zox_get_name(e));
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(character_player_equips_system);
