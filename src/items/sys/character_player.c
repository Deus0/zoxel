zox_sys2(CharacterPlayerItemsSystem) {
    // zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    //zox_sys_in(RealmLink);
    zox_sys_out(ItemLinks);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        //zox_sys_i(RealmLink, realm);
        zox_sys_o(ItemLinks, items);

        if (state->value != zox_dirty_active) {
            continue;
        }

        int count = character_inventory_count; // !test_items_blocks ? character_inventory_count : 16;

        initialize_ItemLinks(items, count);
        if (!items->value) {
            zox_log_error("Failed allocating memory for items")
            continue;
        }

        for (int i = 0; i < count; i++) {
            items->value[i] = 0; // blanks are item slots
        }

        /*if (!test_items_blocks) {
            continue;
        }

        int place_index = 0;
        zox_geter(realm->value, BlockLinks, blocks);
        for (int j = 0; j < blocks->length; j++) {
            if (j >= count) {
                break;
            }

            entity block = blocks->value[j];
            if (!zox_valid(block)) {
                zox_log_error("block invalid [%i]", j);
                continue;
            }

            if (!zox_has(block, ItemLink)) {
                zox_log_error("block has no item [%i]", j);
                continue;
            }

            zox_geter_value(block, ItemLink, entity, block_item);
            if (!zox_valid(block_item)) {
                zox_log_error("block item invalid [%i]", j);
                continue;
            }

            byte quantity =  4 + rand() % 60;

            entity item = spawn_user_item(world, block_item, e);
            zox_set(item, Quantity, { quantity });

            items->value[place_index++] = item;
        }*/

    }
} zox_sys_end(CharacterPlayerItemsSystem);
