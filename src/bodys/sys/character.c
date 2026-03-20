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

// Currently adds parts from realm
zox_sys2(CharacterBodySpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    zox_sys_out(BodyLinks);
    zox_sys_out(BoneLinks);
    zox_sys_out(BodyDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(BodyLinks, bodys);
        zox_sys_o(BoneLinks, bones);
        zox_sys_o(BodyDirty, dirty);

        if (state->value != zox_dirty_active) {
            continue;
        }

        zox_geter(realm->value, ItemLinks, ritems);

        // TODO: Randomly find a "hat" tag equip item from realm
        // byte found = 0;

        // pick core
        entity rcore = find_slot_type(world, ritems, zox_slot_core);

        if (!rcore) {
            continue;
        }

        entity bcore = spawn_user_item(world, rcore, e);
        add_to_BodyLinks(bodys, bcore);

        // TODO: use items slot type and add that - spawn_body_item(world, rcore, e);
        zox_set(bcore, SlotType, { zox_slot_core });

        zox_log("+ Body Core %s", zox_get_name(rcore));
        // Link up parts together
        PartLinks core_parts = (PartLinks) { };

        // TODO: Add Shoulders, hips here
        {
            entity rhead = find_slot_type(world, ritems, zox_slot_head);
            if (rhead) {
                entity bhead = spawn_user_item(world, rhead, e);
                zox_set(bhead, SlotType, { zox_slot_head });
                zox_set(bhead, AttachLink, { bcore });

                add_to_BodyLinks(bodys, bhead);
                add_to_PartLinks(&core_parts, bhead);

                zox_log("+ Body Core %s", zox_get_name(rhead));
            }
        }

        zox_set_ptr(bcore, PartLinks, core_parts);

        // how to get head position
        spawn_skeleton_bones(world, e, bones, 0.06f);

        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(CharacterBodySpawnSystem);
