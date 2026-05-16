zox_sys2(CharacterPlayerEquipsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    zox_sys_out(EquipLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(EquipLinks, equips);
        if (state->value != zox_dirty_active) {
            continue;
        }
        zox_geter(realm->value, ItemLinks, realm_items);
        // TODO: Randomly find a "hat" tag equip item from realm
        entity ritem = 0;
        for (uint j = 0; j < realm_items->length; j++) {
            entity item = realm_items->value[j];
            if (zox_has(item, EquipItem)) {
                ritem = item;
                break;
            }
        }
        if (zox_valid(ritem)) {
            entity new_equip = spawn_user_item(world, e, ritem);
            add_to_EquipLinks(equips, new_equip);
        }
    }
} zox_sys_end(CharacterPlayerEquipsSystem);
