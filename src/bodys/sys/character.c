byte zox_dbg_extraheads = 0; // 16;

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
            // entity mchest = get_item_model(world, rchest);
            // zox_geter_value(mchest, ChunkSize, int3, schest);
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
            entity head_slot = spawn_body_slot(world, chest_slot, body_anchor_top);
            entity hips_slot = spawn_body_slot(world, chest_slot, body_anchor_bottom);
            entity left_shoulder_slot = spawn_body_slot(world, chest_slot, body_anchor_left);
            entity right_shoulder_slot = spawn_body_slot(world, chest_slot, body_anchor_right);
            // TODO: Base offsets on a float3 anchor of Chest data
            zox_set(left_shoulder_slot, PartOffset, { 2, 5, 0 });
            zox_set(right_shoulder_slot, PartOffset, { -2, 5, 0 });
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
            entity realm_hips = find_slot_type(world, ritems->value, ritems->length, zox_slot_hips);
            if (realm_hips) {
                entity e2 = spawn_user_item_body(world, e, realm_hips, zox_slot_hips);
                zox_set(hips_slot, DataLink, { e2 });
            }
            entity realm_shoulder = find_slot_type(world, ritems->value, ritems->length, zox_slot_lshoulder);
            if (realm_shoulder) {
                {
                    entity e2 = spawn_user_item_body(world, e, realm_shoulder, zox_slot_lshoulder);
                    zox_set(left_shoulder_slot, DataLink, { e2 });
                }
                {
                    entity e2 = spawn_user_item_body(world, e, realm_shoulder, zox_slot_rshoulder);
                    zox_set(right_shoulder_slot, DataLink, { e2 });
                }
                entity left_bicep_slot = spawn_body_slot(world, left_shoulder_slot, body_anchor_bottom);
                entity right_bicep_slot = spawn_body_slot(world, right_shoulder_slot, body_anchor_bottom);
                entity realm_bicep = find_slot_type(world, ritems->value, ritems->length, zox_slot_lbicep);
                if (realm_bicep) {
                    {
                        entity e2 = spawn_user_item_body(world, e, realm_bicep, zox_slot_lbicep);
                        zox_set(left_bicep_slot, DataLink, { e2 });
                    }
                    {
                        entity e2 = spawn_user_item_body(world, e, realm_bicep, zox_slot_rbicep);
                        zox_set(right_bicep_slot, DataLink, { e2 });
                    }
                }
            }
            // Testing
            entity pslot1 = head_slot;
            entity pslot2 = head_slot;
            entity pslot3 = head_slot;
            entity pslot4 = head_slot;
            entity pslot5 = head_slot;
            for (byte j = 0; j < zox_dbg_extraheads; j++)
            {
                entity pslot;
                int slot_rand = rand_range(1, 100);
                if (slot_rand >= 80) {
                    pslot1 = spawn_body_slot(world, pslot1, body_anchor_top);
                    pslot = pslot1;
                } else if (slot_rand >= 60) {
                    pslot2 = spawn_body_slot(world, pslot2, body_anchor_left);
                    pslot = pslot2;
                } else if (slot_rand >= 40) {
                    pslot3 = spawn_body_slot(world, pslot3, body_anchor_right);
                    pslot = pslot3;
                } else if (slot_rand >= 20) {
                    pslot4 = spawn_body_slot(world, pslot4, body_anchor_forward);
                    pslot = pslot4;
                } else {
                    pslot5 = spawn_body_slot(world, pslot5, body_anchor_back);
                    pslot = pslot5;
                }
                entity e2 = spawn_user_item_body(world, e, rhead, zox_slot_head);
                zox_set(pslot, DataLink, { e2 });
            }
        }
        // TODO: Once we attach chest to this slot, we should spawn sub slots of chest onto the slot
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(CharacterBodySpawnSystem);
