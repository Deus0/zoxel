byte tst_all_items = 0;

// TODO: Work Item Slots better
void zox_tst_all_items(ecs* world, ClickEventData data) {
    if (tst_all_items) {
        zox_log("Already have All Items.");
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    if (!zox_valid(character) || !zox_valid(realm)) {
        return;
    }
    zox_geter(realm, ItemLinks, ritems);
    zox_muter(character, ItemLinks, items);
    zox_log("Giving [%s] [%i] Items.", zox_get_name(character), ritems->length);
    for (int j = 0; j < ritems->length; j++) {
        entity ritem = ritems->value[j];
        if (!zox_valid(ritem)) {
            zox_log_error("Item invalid [%i]", j)
            continue;
        }
        byte quantity =  1 + rand() % 3;
        entity item = spawn_user_item(world, ritem, character);
        zox_set(item, Quantity, { quantity });
        add_to_ItemLinks(items, item);
        zox_log("   + [%s] x%i", zox_get_name(ritem), quantity);
    }
    tst_all_items = 1;
}

/*

// zox_geter(realm->value, ItemLinks, realm_items);
zox_geter(realm->value, BlockLinks, blocks);
// give npc random voxel
entity block = blocks->value[rand() % blocks->length];
if (zox_valid(block) && zox_has(block, ItemLink)) {
    zox_geter(block, ItemLink, itemLink)
    if (zox_valid(itemLink->value)) {
        byte quantity =  1 + rand() % 3;

        entity item = spawn_user_item(world, itemLink->value, e);
        zox_set(item, Quantity, { quantity });
        add_to_ItemLinks(items, item);
    }
}

 * */