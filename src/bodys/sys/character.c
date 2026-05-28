byte zox_dbg_extraheads = 0; // 16;

entity find_slot_type(ecs* world, const entity* items, uint length, byte slot) {
    for (uint j = 0; j < length; j++) {
        entity e = items[j];
        if (!zox_has(e, BodyItem)) {
            continue;
        }
        if (zox_getv(e, SlotType) != slot) {
            continue;
        }
        return e;
    }
    return 0;
}
// TODO: Randomly find a "hat" tag equip item from realm
// TODO: use items slot type and add that - spawn_body_item(world, rcore, e);
// TODO: Add Shoulders, hips here
// TODO: Spawn location of head: Half chest + half head sizes, minus half head size (cornered spawn location)
// TODO: Calculate bone positions from this
// NOTE: Spawns in body parts from Realm
zox_sys2(CharacterBodySpawnSystem) {
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
        if (state->value != zox_dirty_active) {
            continue;
        }
        zox_geter(realm->value, ItemLinks, ritems);
        // pick core
        //bsize->value = byte3_zero;
        // We can add first to the character then set body dirty
        entity realm_chest = find_slot_type(world, ritems->value, ritems->length, zox_slot_core);
        entity realm_head = find_slot_type(world, ritems->value, ritems->length, zox_slot_head);
        entity realm_hips = find_slot_type(world, ritems->value, ritems->length, zox_slot_hips);
        entity realm_thigh = find_slot_type(world, ritems->value, ritems->length, zox_slot_lthigh);
        entity realm_calf = find_slot_type(world, ritems->value, ritems->length, zox_slot_lcalf);
        entity realm_foot = find_slot_type(world, ritems->value, ritems->length, zox_slot_lfoot);
        entity realm_shoulder = find_slot_type(world, ritems->value, ritems->length, zox_slot_lshoulder);
        entity realm_bicep = find_slot_type(world, ritems->value, ritems->length, zox_slot_lbicep);
        entity realm_forearm = find_slot_type(world, ritems->value, ritems->length, zox_slot_lforearm);
        entity realm_hand = find_slot_type(world, ritems->value, ritems->length, zox_slot_lhand);
        if (!realm_chest) {
            zox_logw("Realm Chest Part not found.");
            continue;
        }
        // Debug our parts
        if (dbg_log) {
            zox_dbg_body_part(world, realm_chest);
            zox_dbg_body_part(world, realm_head);
            zox_dbg_body_part(world, realm_shoulder);
            zox_dbg_body_part(world, realm_bicep);
            zox_dbg_body_part(world, realm_forearm);
            zox_dbg_body_part(world, realm_hand);
            zox_dbg_body_part(world, realm_hips);
            zox_dbg_body_part(world, realm_thigh);
            zox_dbg_body_part(world, realm_calf);
            zox_dbg_body_part(world, realm_foot);
        }
        // Get our Core Slot
        entity chest_slot = zox_get_child_by_id(world, e, zox_id(Body));
        // now add them all sequentially to the body
        if (!zox_valid(chest_slot) || !zox_has(chest_slot, DataLink)) {
            zox_logw("No chest slot on character [%s]", zox_get_name(e));
            continue;
        }
        // Spawn our user body
        // Attach Core Part
        zox_set(chest_slot, DataLink, { spawn_user_item_body(world, e, realm_chest, zox_slot_core) });
        // Trigger early, incase we cannot keep growing body
        dirty->value = zox_dirty_trigger;
        // Given our chest spawned, we can spawn slots now
        // Sub Slots
        entity eslot_head = spawn_body_slot(world, chest_slot, body_anchor_top);
        entity eslot_hips = spawn_body_slot(world, chest_slot, body_anchor_bottom);
        entity eslot_lshoulder = spawn_body_slot(world, chest_slot, body_anchor_left);
        entity eslot_rshoulder = spawn_body_slot(world, chest_slot, body_anchor_right);
        // TODO: Base offsets on a float3 anchor of Chest data
        // Slot Offsets
        zox_set(eslot_head, PartOffset, { 0, 0, 0 }); // -1
        zox_set(eslot_hips, PartOffset, { 0, 2, 0 });
        zox_set(eslot_lshoulder, PartOffset, { 1, 5, 0 });
        zox_set(eslot_rshoulder, PartOffset, { -1, 5, 0 });
        // Now add parts to those slots
        // NOTE: This is head branch
        if (realm_head)
        {
            entity head = spawn_user_item_body(world, e, realm_head, zox_slot_head);
            zox_set(eslot_head, DataLink, { head });
            zox_add_tag(head, Head);
        }
        // Test slot systems
        // byte positions are limited
        // TODO: FIx Glitch when out of bounds
        // NOTE: This is legs branch
        if (realm_hips) {
            zox_set(eslot_hips, DataLink, { spawn_user_item_body(world, e, realm_hips, zox_slot_hips) });
            // Sub Slots
            entity eslot_lthigh = spawn_body_slot(world, eslot_hips, body_anchor_bottom);
            entity eslot_rthigh = spawn_body_slot(world, eslot_hips, body_anchor_bottom);
            // Slot Offsets
            byte chest_size = 16;
            byte thigh_offset = chest_size / 4;
            zox_set(eslot_lthigh, PartOffset, { thigh_offset, 1, 0 });
            zox_set(eslot_rthigh, PartOffset, { -thigh_offset, 1, 0 });
            if (realm_thigh) {
                // Attach Parts [Thighs]
                zox_set(eslot_lthigh, DataLink, { spawn_user_item_body(world, e, realm_thigh, zox_slot_lthigh) });
                zox_set(eslot_rthigh, DataLink, { spawn_user_item_body(world, e, realm_thigh, zox_slot_rthigh) });
                // Sub Slots [Calfs]
                entity eslot_lcalf = spawn_body_slot(world, eslot_lthigh, body_anchor_bottom);
                entity eslot_rcalf = spawn_body_slot(world, eslot_rthigh, body_anchor_bottom);
                // Slot Offsets
                zox_set(eslot_lcalf, PartOffset, { 1, 0, 1 });
                zox_set(eslot_rcalf, PartOffset, { 1, 0, 1 });
                if (realm_calf) {
                    // Attach Parts [Calfs]
                    zox_set(eslot_lcalf, DataLink, { spawn_user_item_body(world, e, realm_calf, zox_slot_lthigh) });
                    zox_set(eslot_rcalf, DataLink, { spawn_user_item_body(world, e, realm_calf, zox_slot_rthigh) });
                    // Sub Slots [Calfs]
                    entity eslot_lfoot = spawn_body_slot(world, eslot_lcalf, body_anchor_bottom);
                    entity eslot_rfoot = spawn_body_slot(world, eslot_rcalf, body_anchor_bottom);
                    // Slot Offsets
                    zox_set(eslot_lfoot, PartOffset, { 0, 1, 2 });
                    zox_set(eslot_rfoot, PartOffset, { 0, 1, 2 });
                    if (realm_foot) {
                        // Attach Parts [Calfs]
                        zox_set(eslot_lfoot, DataLink, { spawn_user_item_body(world, e, realm_foot, zox_slot_lfoot) });
                        zox_set(eslot_rfoot, DataLink, { spawn_user_item_body(world, e, realm_foot, zox_slot_rfoot) });
                    }
                }
            }
        }
        // NOTE: This is Arms Branch
        if (realm_shoulder) {
            // Attach Parts
            entity lshoulder = spawn_user_item_body(world, e, realm_shoulder, zox_slot_lshoulder);
            entity rshoulder = spawn_user_item_body(world, e, realm_shoulder, zox_slot_rshoulder);
            zox_set(eslot_lshoulder, DataLink, { lshoulder });
            zox_set(eslot_rshoulder, DataLink, { rshoulder });
            zox_add_tag(lshoulder, Shoulder);
            // Sub Slots [Biceps]
            entity eslot_lbicep = spawn_body_slot(world, eslot_lshoulder, body_anchor_bottom);
            entity eslot_rbicep = spawn_body_slot(world, eslot_rshoulder, body_anchor_bottom);
            // Slot Offsets
            zox_set(eslot_lbicep, PartOffset, { -1, 1, 0 });
            zox_set(eslot_rbicep, PartOffset, { 1, 1, 0 });
            if (realm_bicep) {
                // Attach Parts
                zox_set(eslot_lbicep, DataLink, { spawn_user_item_body(world, e, realm_bicep, zox_slot_lbicep) });
                zox_set(eslot_rbicep, DataLink, { spawn_user_item_body(world, e, realm_bicep, zox_slot_rbicep) });
                // Sub Slots
                entity eslot_lforearm = spawn_body_slot(world, eslot_lbicep, body_anchor_bottom);
                entity eslot_rforearm = spawn_body_slot(world, eslot_rbicep, body_anchor_bottom);
                if (realm_forearm) {
                    // Attach Parts
                    zox_set(eslot_lforearm, DataLink, { spawn_user_item_body(world, e, realm_forearm, zox_slot_lforearm) });
                    zox_set(eslot_rforearm, DataLink, { spawn_user_item_body(world, e, realm_forearm, zox_slot_rforearm) });
                    // Sub Slots
                    entity eslot_lhand = spawn_body_slot(world, eslot_lforearm, body_anchor_bottom);
                    entity eslot_rhand = spawn_body_slot(world, eslot_rforearm, body_anchor_bottom);
                    // Slot Offsets
                    zox_set(eslot_lhand, PartOffset, { 0, 1, 1 });
                    zox_set(eslot_rhand, PartOffset, { 0, 1, 1 });
                    if (realm_hand) {
                        // Attach Parts
                        entity lhand = spawn_user_item_body(world, e, realm_hand, zox_slot_lhand);
                        zox_set(eslot_lhand, DataLink, { lhand });
                        entity rhand = spawn_user_item_body(world, e, realm_hand, zox_slot_rhand);
                        zox_set(eslot_rhand, DataLink, { rhand });
                        zox_add_tag(lhand, Hand);
                    }
                }
            }
        }
        // Testing
        entity pslot1 = eslot_head;
        entity pslot2 = pslot1;
        entity pslot3 = pslot1;
        entity pslot4 = pslot1;
        entity pslot5 = pslot1;
        for (byte j = 0; j < zox_dbg_extraheads; j++) {
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
            entity e2 = spawn_user_item_body(world, e, realm_head, zox_slot_head);
            zox_set(pslot, DataLink, { e2 });
        }
        // TODO: Once we attach chest to this slot, we should spawn sub slots of chest onto the slot
    }
} zox_sys_end(CharacterBodySpawnSystem);
