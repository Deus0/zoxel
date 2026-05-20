entity find_slot_type(ecs* world, const entity* items, uint length, byte slot) {
    entity e = 0;
    for (uint j = 0; j < length; j++) {
        entity ritem = items[j];
        if (!zox_has(ritem, BodyItem)) {
            continue;
        }
        if (zox_gett_value(ritem, SlotType) != slot) {
            continue;
        }
        e = ritem;
        break;
    }
    return e;
}
// TODO: Randomly find a "hat" tag equip item from realm
// TODO: use items slot type and add that - spawn_body_item(world, rcore, e);
// TODO: Add Shoulders, hips here
// TODO: Spawn location of head: Half chest + half head sizes, minus half head size (cornered spawn location)
// TODO: Calculate bone positions from this
// NOTE: Spawns in body parts from Realm
zox_sys2(CharacterBodySpawnSystem) {
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
        if (state->value != zox_dirty_active) {
            continue;
        }
        zox_geter(realm->value, ItemLinks, ritems);
        // pick core
        //bsize->value = byte3_zero;
        // We can add first to the character then set body dirty
        entity rchest = find_slot_type(world, ritems->value, ritems->length, zox_slot_core);
        if (rchest) {
            entity mchest = get_item_model(world, rchest);
            zox_geter_value(mchest, ChunkSize, int3, schest);
            entity chest_slot = zox_get_child_by_id(world, e, zox_id(Body));
            if (!zox_valid(chest_slot) || !zox_has(chest_slot, DataLink)) {
                zox_logw("No chest slot on character [%s]", zox_get_name(e));
                continue;
            }
            // Spawn our user body
            entity chest = spawn_user_item_body(world, e, rchest, zox_slot_core);
            zox_muter(chest_slot, DataLink, chest_slotd);
            chest_slotd->value = chest;
            // Given our chest spawned, we can spawn slots now
            entity head_slot = spawn_slot(world, chest_slot);
            zox_set(head_slot, SlotAnchor, { 1 });
            entity hips_slot = spawn_slot(world, chest_slot);
            zox_set(hips_slot, SlotAnchor, { 2 });
            entity left_shoulder_slot = spawn_slot(world, chest_slot);
            zox_set(left_shoulder_slot, SlotAnchor, { 3 });
            entity right_shoulder_slot = spawn_slot(world, chest_slot);
            zox_set(right_shoulder_slot, SlotAnchor, { 4 });
            // Now add parts to those slots
            entity rhead = find_slot_type(world, ritems->value, ritems->length, zox_slot_head);
            if (rhead)
            {
                entity head = spawn_user_item_body(world, e, rhead, zox_slot_head);
                zox_set(head_slot, DataLink, { head });
            }
            // Test slot systems
            // byte positions are limited
            // TODO: FIx Glitch when out of bounds
            /*
            entity pslot = head_slot;*
            for (byte j = 0; j < 4; j++)
            {
                pslot = spawn_slot(world, pslot);
                zox_set(pslot, SlotAnchor, { 1 });
                entity e2 = spawn_user_item_body(world, e, rhead, zox_slot_head);
                zox_set(pslot, DataLink, { e2 });
            }*/
            entity rhips = find_slot_type(world, ritems->value, ritems->length, zox_slot_hips);
            if (rhips) {
                entity e2 = spawn_user_item_body(world, e, rhips, zox_slot_hips);
                zox_set(hips_slot, DataLink, { e2 });
            }
        }
        // TODO: Once we attach chest to this slot, we should spawn sub slots of chest onto the slot
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(CharacterBodySpawnSystem);
