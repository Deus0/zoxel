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
    zox_sys_out(BodySize);
    zox_sys_out(BodyDirty);
    zox_sys_out(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(BodySize, bsize);
        zox_sys_o(BodyDirty, dirty);
        zox_sys_o(BlockScale, bscale);
        if (state->value != zox_dirty_active) {
            continue;
        }
        zox_geter(realm->value, ItemLinks, ritems);
        // pick core
        entity rchest = find_slot_type(world, ritems->value, ritems->length, zox_slot_core);
        entity rhips = find_slot_type(world, ritems->value, ritems->length, zox_slot_hips);
        entity rhead = find_slot_type(world, ritems->value, ritems->length, zox_slot_head);
        if (!rchest || !rhips || !rhead) {
            continue;
        }
        entity mchest = get_item_model(world, rchest);
        entity mhips = get_item_model(world, rhips);
        entity mhead = get_item_model(world, rhead);
        zox_geter_value(mchest, ChunkSize, int3, schest);
        zox_geter_value(mhips, ChunkSize, int3, ships);
        zox_geter_value(mhead, ChunkSize, int3, shead);
        byte3 body_size = int3_to_byte3(schest);
        body_size.y += ships.y;
        body_size.y += shead.y;
        byte3 pchest = (byte3) { 0, ships.y, 0 };
        byte3 cpchest = byte3_add(pchest, byte3_half(int3_to_byte3(schest)));

        entity chest_slot = zox_get_child_by_id(world, e, zox_id(Body));
        if (!zox_valid(chest_slot) || !zox_has(chest_slot, DataLink)) {
            zox_logw("No chest slot on character [%s]", zox_get_name(e));
            continue;
        }

        // Spawn Chest on character
        // Then set to body slot link
        entity chest = spawn_user_item_body(world, e, rchest, e, zox_slot_core, pchest, cpchest);
        zox_muter(chest_slot, DataLink, chest_slotd);
        chest_slotd->value = chest;

        // TODO: Once we attach chest to this slot, we should spawn sub slots of chest onto the slot
        // This is the harder part

        // zox_log("+ Body Chest %s: s[%ix%ix%i]", zox_get_name(rchest), schest.x, schest.y, schest.z);
        // Head
        if (rhead) {
            byte3 phead = (byte3) {
                (schest.x - shead.x) / 2,
                ships.y + schest.y,
                (schest.z - shead.z) / 2
            };
            byte3 cposition = byte3_add(phead, byte3_half(int3_to_byte3(shead)));
            entity part = spawn_user_item_body(world, e, rhead, chest, zox_slot_head, phead, cposition);
            zox_set_parent(world, part, chest);
            // zox_log("+ Spawned Body Head %s: p[%ix%ix%i] - s[%ix%ix%i]", zox_get_name(rhead), phead.x, phead.y, phead.z, shead.x, shead.y, shead.z);
        }
        // Hips
        if (rhips) {
            byte3 phips = (byte3) {
                (schest.x - ships.x) / 2,
                0,
                (schest.z - ships.z) / 2
            };
            byte3 cposition = byte3_add(phips, byte3_half(int3_to_byte3(ships)));
            entity part = spawn_user_item_body(world, e, rhips, chest, zox_slot_hips, phips, cposition);
            zox_set_parent(world, part, chest);
            // zox_log("+ Spawned Body Hips %s: p[%ix%ix%i] - s[%ix%ix%i]", zox_get_name(rhips), phips.x, phips.y, phips.z, ships.x, ships.y, ships.z);
        }
        bsize->value = body_size;
        dirty->value = zox_dirty_trigger;
        byte mdepth = block_vox_depth + 2;
        bscale->value = (1.0f / (powers_of_two_byte[mdepth]));
    }
} zox_sys_end(CharacterBodySpawnSystem);
