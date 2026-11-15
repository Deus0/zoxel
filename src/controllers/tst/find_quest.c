// tst: find_quest, the starting quest: find a town, talk to someone there, 3 cookies (quests/fun/reward.c)
//  - the player starts with it, the tracker says what to do (it said Find [???])
//  - talking outside a town (quests_found_town) doesn't finish it
//  - inside one (InTown) it's done and handed in, and the character has 3 more cookies
//  - run: bash bsh/tst.sh find_quest
byte zox_tst_find_stage = 0;
int zox_tst_find_frames = 0;
entity zox_tst_find_quest = 0;
int zox_tst_find_cookies = 0;
byte zox_tst_find_outside_done = 0;
char zox_tst_find_tracker[256];

// the character's find quest (its user quest)
static inline entity zox_tst_find_user_quest(ecs* world, entity character) {
    entity found = 0;
    iter it = zox_children(world, character);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            if (zox_has(it.entities[i], Quest) && zox_has(it.entities[i], FindQuest)) {
                found = it.entities[i];
            }
        }
    }
    return found;
}

// how many cookies its actionbar and inventory hold
static inline int zox_tst_find_count_cookies(ecs* world, entity character) {
    int count = 0;
    const entity managers[2] = { zox_get_child_by_id(world, character, zox_id(Actionbar)),
        zox_get_child_by_id(world, character, zox_id(Inventory)) };
    for (int m = 0; m < 2; m++) {
        iter it = zox_children(world, managers[m]);
        while (zox_children_next(it)) {
            for (int i = 0; i < it.count; i++) {
                // a slot's item is its DataLink, an instance of the realm's item
                const entity slot_item = zox_get_link(world, it.entities[i], DataLink);
                const entity item = zox_valid(slot_item) ? zox_get_prefab(world, slot_item) : 0;
                if (zox_valid(item) && zox_has(item, ZoxName) && !strcmp(zox_getv(item, ZoxName), "cokie")) {
                    count += zox_has(slot_item, Quantity) ? zox_getv(slot_item, Quantity) : 1;
                }
            }
        }
    }
    return count;
}

byte zox_tst_find_quest_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    zox_tst_find_frames++;
    zox_tst_progress(zox_tst_find_stage);
    if (zox_tst_find_stage == 0) {
        zox_tst_waiting = "the find quest";
        zox_tst_find_quest = zox_tst_find_user_quest(world, character);
        if (!zox_valid(zox_tst_find_quest)) {
            return zox_tst_find_frames > 300;
        }
        char* text = (char*) get_quest_tracker_text(world, zox_tst_find_quest);
        snprintf(zox_tst_find_tracker, sizeof(zox_tst_find_tracker), "%s", text);
        if (text[0]) {
            free(text);
        }
        zox_tst_find_cookies = zox_tst_find_count_cookies(world, character);
        // outside a town
        zox_remove(character, InTown);
        zox_tst_find_stage = 1;
        zox_tst_find_frames = 0;
        return 0;
    }
    if (zox_tst_find_stage == 1) {
        if (zox_tst_find_frames < 3) {
            return 0;
        }
        // only when it's really outside one (in_town_system adds it back each frame in a town)
        if (!zox_has(character, InTown)) {
            quests_found_town(world, character);
        }
        zox_add(character, InTown);
        zox_tst_find_stage = 2;
        zox_tst_find_frames = 0;
        return 0;
    }
    if (zox_tst_find_stage == 2) {
        if (zox_tst_find_frames < 3) {
            return 0;
        }
        zox_tst_find_outside_done = zox_has(zox_tst_find_quest, QuestDone);
        quests_found_town(world, character);
        zox_tst_find_stage = 3;
        zox_tst_find_frames = 0;
        return 0;
    }
    return zox_tst_find_frames > 5;
}

void zox_tst_find_quest_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    zox_tst_check(zox_valid(zox_tst_find_quest), "the player didn't start with the find quest");
    if (!zox_valid(zox_tst_find_quest)) {
        return;
    }
    zox_log("[tst] find quest tracker: %s", zox_tst_find_tracker);
    zox_tst_check(!strstr(zox_tst_find_tracker, "???"), "the tracker shows [%s]", zox_tst_find_tracker);
    zox_tst_check(!zox_tst_find_outside_done, "talking outside a town finished it");
    zox_tst_check(zox_has(zox_tst_find_quest, QuestDone) && zox_has(zox_tst_find_quest, QuestHandedin),
        "talking in a town didn't finish and hand it in");
    const int cookies = zox_tst_find_count_cookies(world, character);
    zox_tst_check(cookies == zox_tst_find_cookies + 3, "[%i] cookies after, [%i] before, not 3 more", cookies, zox_tst_find_cookies);
}
