/*void spawn_character_equipment(
    ecs *world,
    spawned_character3D_data *data
) {
    if (!data->p) {
        return;
    }

    zox_geter(data->realm, ItemLinks, items);

    // test - Spawn Tophat!
    EquipLinks equips = (EquipLinks) { 0 };

    // get voxels
    entity add_item = items->value[items->length - 1];
    if (zox_valid(add_item)) {
        const entity new_equip = spawn_user_item(
            world,
            add_item,
            data->e
        );
        add_to_EquipLinks(&equips, new_equip);
    }

    zox_set_ptr(data->e, EquipLinks, equips);
}*/

void CharacterPlayerEquipsSystem(iter *it) {
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
        entity add_item = realm_items->value[realm_items->length - 1];

        if (zox_valid(add_item)) {
            const entity new_equip = spawn_user_item(
                world,
                add_item,
                e
            );
            add_to_EquipLinks(equips, new_equip);
        }

    }
} zoxd_system2(CharacterPlayerEquipsSystem);