// tst: all_items, the cheat's All Items gives every realm item, a slot each (items/tst/all.c)
//  - the player's inventory, slots added as the cheat adds them, then give_character_all_items
//  - every realm item (blocks' items: oil, the last block, among them) is in a slot
//  - was: 64 slots, fewer than the realm's items: the last (oil) never given
//  - run: bash bsh/tst.sh all_items
byte zox_tst_ai_stage = 0;
int zox_tst_ai_realm = 0, zox_tst_ai_given = 0;
byte zox_tst_ai_oil = 0;

byte zox_tst_all_items_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity inventory = zox_get_child_by_id(world, character, zox_id(Inventory));
    const entity realm = zox_get_link(world, character, RealmLink);
    if (!zox_valid(inventory) || !zox_valid(realm)) {
        zox_tst_waiting = "the player's inventory";
        return 0;
    }
    if (zox_tst_ai_stage == 0) {
        all_items_add_slots(world, character);
        give_character_all_items(world, character);
        zox_tst_ai_stage = 1;
        return 0;
    }
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            zox_tst_ai_realm += zox_has(it.entities[j], Item);
        }
    }
    iter it2 = zox_children(world, inventory);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity item = zox_has(it2.entities[j], Slot) ? zox_get_link(world, it2.entities[j], DataLink) : 0;
            if (zox_valid(item)) {
                zox_tst_ai_given++;
                zox_tst_ai_oil |= !strncmp(zox_get_name(item), "oil_", 4);
            }
        }
    }
    return 1;
}

void zox_tst_all_items_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] all_items: realm items [%i], in slots [%i], oil [%i]", zox_tst_ai_realm, zox_tst_ai_given, zox_tst_ai_oil);
    zox_tst_check(zox_tst_ai_realm && zox_tst_ai_given >= zox_tst_ai_realm, "[%i] of [%i] realm items given",
        zox_tst_ai_given, zox_tst_ai_realm);
    zox_tst_check(zox_tst_ai_oil, "oil wasn't given");
}

void zox_tsts_all_items(ecs* world) {
    (void) world;
    zox_tst_add("all_items", zox_tst_all_items_ready, zox_tst_all_items_check, 120);
}
