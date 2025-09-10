void spawn_character_equipment(
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
}