zox_sys2(CharacterBodySpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    zox_sys_out(BodyLinks);
    zox_sys_out(BodyDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(BodyLinks, bodys);
        zox_sys_o(BodyDirty, dirty);

        if (state->value != zox_dirty_active) {
            continue;
        }

        zox_geter(realm->value, ItemLinks, realm_items);

        // TODO: Randomly find a "hat" tag equip item from realm
        entity ritem = 0;

        for (uint j = 0; j < realm_items->length; j++) {
            entity item = realm_items->value[j];

            if (zox_has(item, BodyItem)) {
                ritem = item;
                break;
            }
        }


        if (!zox_valid(ritem)) {
            continue;
        }

        entity item = spawn_user_item(
                world,
                ritem,
                e
            );

        add_to_BodyLinks(bodys, item);

        dirty->value = zox_dirty_trigger;

    }
} zox_sys_end(CharacterBodySpawnSystem);
