int character_inventory_count = 8; // 8 | 16; // having blank items seems to b reak it
byte test_give_npcs_blocks = 1;

void spawn_character_items(
    ecs *world,
    spawned_character3D_data *data
) {
    if (!data->p) {
        // todo: give them mushroom! or simple block for now
        if (test_give_npcs_blocks) {
            ItemLinks items = (ItemLinks) { 0 };
            // get voxels
            zox_geter(data->realm, VoxelLinks, voxels);
            // give npc random voxel
            const ecs_entity_t block = voxels->value[rand() % voxels->length];
            if (zox_valid(block) && zox_has(block, ItemLink)) {
                zox_geter(block, ItemLink, itemLink)
                if (zox_valid(itemLink->value)) {
                    const ecs_entity_t item = spawn_user_item(world, itemLink->value, data->e);
                    zox_set(item, Quantity, { 1 + rand() % 3 })
                    add_to_ItemLinks(&items, item);
                    zox_set_ptr(data->e, ItemLinks, items);
                }
            }
        }
        return;
    }

    int inventory_count = character_inventory_count;
    if (test_items_blocks) {
        inventory_count = 16;
    }

    ItemLinks items = (ItemLinks) { 0 };
    initialize_ItemLinks(&items, inventory_count);
    if (!items.value) {
        zox_log_error("Failed allocating memory for items")
        return;
    }
    for (int i = 0; i < items.length; i++) {
        items.value[i] = 0; // blanks are item slots
    }

    int place_index = 0;
    if (test_items_blocks && data->p) {
        // get voxels
        zox_geter(data->realm, VoxelLinks, blocks);
        for (int i = 0; i < blocks->length; i++) {
            if (i >= items.length) {
                break;
            }
            const entity block = blocks->value[i];
            if (!zox_valid(block)) {
                zox_log_error("block invalid [%i]", i);
                continue;
            }
            if (!zox_has(block, ItemLink)) {
                zox_log_error("block has no item [%i]", i);
                continue;
            }
            zox_geter_value(block, ItemLink, entity, block_item);
            if (!zox_valid(block_item)) {
                zox_log_error("block item invalid [%i]", i);
                continue;
            }
            entity item = spawn_user_item(world, block_item, data->e);
            zox_set(item, Quantity, { 64 });
            items.value[place_index++] = item;
        }
    }

    zox_set_ptr(data->e, ItemLinks, items);
}