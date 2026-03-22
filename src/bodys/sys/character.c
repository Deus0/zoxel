entity find_slot_type(ecs* world, const ItemLinks* ritems, byte slot) {
    entity e = 0;
    for (uint j = 0; j < ritems->length; j++) {
        entity ritem = ritems->value[j];

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

// Currently adds parts from realm
zox_sys2(CharacterBodySpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    zox_sys_out(PartLinks);
    zox_sys_out(BodyDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(PartLinks, parts);
        zox_sys_o(BodyDirty, dirty);

        if (state->value != zox_dirty_active) {
            continue;
        }

        zox_geter(realm->value, ItemLinks, ritems);

        // pick core
        entity base_chest = find_slot_type(world, ritems, zox_slot_core);

        if (!base_chest) {
            continue;
        }

        entity chest_model = get_item_model(world, base_chest);
        zox_geter_value(chest_model, ChunkSize, int3, chest_size);

        entity chest = spawn_user_item_body(world, e, base_chest, e, zox_slot_core, byte3_zero, byte3_half(int3_to_byte3(chest_size)));
        add_to_PartLinks(parts, chest);

        zox_log("+ Body Chest %s: s[%ix%ix%i]", zox_get_name(base_chest), chest_size.x, chest_size.y, chest_size.z);

        byte3 body_size = int3_to_byte3(chest_size);

        // Link up parts together
        PartLinks chest_parts = (PartLinks) { };
        {
            entity base_head = find_slot_type(world, ritems, zox_slot_head);
            if (base_head) {
                entity head_model = get_item_model(world, base_head);
                zox_geter_value(head_model, ChunkSize, int3, head_size);
                byte3 head_position = (byte3) {
                    (chest_size.x - head_size.x) / 2,
                    chest_size.y,
                    (chest_size.z - head_size.z) / 2
                };
                // Center Position
                byte3 head_center_position = byte3_add(head_position, byte3_half(int3_to_byte3(head_size)));
                // byte3 head_position = byte3_scale((byte3) { 4, 21, 4 }, position_mul);

                // atm its based on the realm body items
                entity head = spawn_user_item_body(world, e, base_head, chest, zox_slot_head, head_position, head_center_position);
                add_to_PartLinks(&chest_parts, head);

                /*entity bhead = spawn_user_item(world, base_head, e);
                zox_set(bhead, SlotType, { zox_slot_head });
                zox_set(bhead, AttachLink, { chest });
                zox_set(bhead, VoxelPosition, { byte3_to_int3(head_position) });*/

                zox_log("+ Spawned Body Head %s: p[%ix%ix%i] - s[%ix%ix%i]", zox_get_name(base_head), head_position.x, head_position.y, head_position.z, head_size.x, head_size.y, head_size.z);

                body_size.y += head_size.y;
            }
        }
        zox_set_ptr(chest, PartLinks, chest_parts);

        zox_set(e, BodySize, { body_size });

        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(CharacterBodySpawnSystem);
