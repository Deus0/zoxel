// Cheat: gives the player character every realm quest
void zox_tst_all_quests(ecs* world, ClickEventData data) {
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(character)) {
        return;
    }
    entity game = zox_get_link(world, player, GameLink);
    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        return;
    }
    zox_log("Giving [%s] all Quests.",
        zox_getn(character));
    uint given_count = 0;
    iter it2 = zox_children(world, realm);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity quest = it2.entities[j];
            if (!zox_has(quest, Quest)) {
                continue;
            }
            // skip quests we already have
            if (get_user_quest(world, character, quest)) {
                continue;
            }
            spawn_quest_doing(world, character, quest);
            given_count++;
            zox_log("   + [%s]",
                zox_getn(quest));
        }
    }
    if (!given_count) {
        zox_log("Already have All Quests.");
    }
}
