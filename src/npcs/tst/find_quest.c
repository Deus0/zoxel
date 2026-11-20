// tst: find_quest, the starting quest: find a town, talk to someone there, 3 cookies (quests/fun/reward.c)
//  - the player starts with it tracked (it wasn't), the tracker says what to do (it said Find [???]), a panel behind
//    its text (play test: none at the start)
//  - talking (quests_found_town) to a villager outside a town, or to a critter in one, doesn't finish it
//  - to a villager inside one (wherever the player stands) it's done and handed in, the character has 3 more cookies
//  - run: bash bsh/tst.sh find_quest
byte zox_tst_find_stage = 0;
int zox_tst_find_frames = 0;
entity zox_tst_find_quest = 0;
int zox_tst_find_cookies = 0;
byte zox_tst_find_outside_done = 0;
char zox_tst_find_tracker[256];
entity zox_tst_find_tracker_e = 0;
byte zox_tst_find_panel = 0;
int2 zox_tst_find_panel_size;

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

// the realm's first town
static inline entity zox_tst_find_town(ecs* world) {
    entity town = 0;
    ecs_iter_t it = ecs_each_id(world, zox_id(Town));
    while (ecs_each_next(&it)) {
        if (!town && it.count) {
            town = it.entities[0];
        }
    }
    return town;
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
    const entity character = zox_tst_player_character(world);
    zox_tst_find_frames++;
    zox_tst_progress(zox_tst_find_stage);
    if (zox_tst_find_stage == 0) {
        zox_tst_waiting = "the find quest";
        zox_tst_find_quest = zox_tst_find_user_quest(world, character);
        if (!zox_valid(zox_tst_find_quest)) {
            return zox_tst_find_frames > 300;
        }
        zox_tst_waiting = "the find quest tracked";
        const entity player = zox_get_link(world, character, PlayerLink);
        const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
        const entity tracker = zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(QuestTracker)) : 0;
        if (!zox_valid(tracker) || !zox_get(tracker, TextData)->length) {
            if (zox_tst_find_frames > 300) {
                zox_tst_check(0, "the quest tracker shows nothing at the start");
                return 1;
            }
            return 0;
        }
        zox_tst_check(zox_get_link(world, character, QuestTracked) == zox_tst_find_quest, "the find quest isn't tracked at the start");
        zox_tst_check(zox_get_link(world, zox_tst_find_quest, QuestTrackerLink) == tracker, "the find quest isn't linked to the tracker");
        char* text = get_zext_text((TextData*) zox_get(tracker, TextData));
        snprintf(zox_tst_find_tracker, sizeof(zox_tst_find_tracker), "%s", text);
        free(text);
        zox_tst_find_cookies = zox_tst_find_count_cookies(world, character);
        zox_tst_find_tracker_e = tracker;
        zox_tst_find_stage = 1;
        zox_tst_find_frames = 0;
        return 0;
    }
    if (zox_tst_find_stage == 1) {
        if (zox_tst_find_frames < 3) {
            return 0;
        }
        // its panel, a few frames after its text (its mesh)
        const entity tracker = zox_tst_find_tracker_e;
        if (zox_valid(tracker)) {
            zox_tst_find_panel_size = zox_getv(tracker, LayoutSize);
            // its mesh as big as its size (it stayed at 0x0: a LayoutSizeDirty lost, mcr/dirty.c)
            const MeshVertices2D* vertices = zox_has(tracker, MeshVertices2D) ? zox_get(tracker, MeshVertices2D) : NULL;
            float width = 0;
            for (int k = 0; vertices && k < vertices->length; k++) {
                width = fmaxf(width, fabsf(vertices->value[k].x - vertices->value[0].x));
            }
            zox_tst_find_panel = !zox_getv(tracker, RenderDisabled) && zox_getv(tracker, MeshGPULink).x &&
                zox_tst_find_panel_size.x > 0 && zox_tst_find_panel_size.y > 0 && width > 0;
        }
        // a stand in npc: a villager far from every town, then a critter (no Villager) at a town's middle
        const entity terrain = zox_get_link(world, character, TerrainLink);
        const entity town = zox_tst_find_town(world);
        zox_tst_check(zox_valid(town), "no town in the realm");
        if (!zox_valid(town) || !zox_valid(terrain)) {
            return 1;
        }
        const float scale = zox_getv(terrain, BlockScale);
        const int2 center = zox_getv(town, BlockPosition2);
        const float3 inside = { (center.x + 0.5f) * scale, 0, (center.y + 0.5f) * scale };
        const entity npc = zox_new();
        zox_set_link(world, npc, TerrainLink, terrain);
        zox_add(npc, Villager);
        zox_setv(npc, Position3D, float3_add(inside, (float3) { 100000, 0, 0 }));
        quests_found_town(world, character, npc);
        zox_remove(npc, Villager);
        zox_setv(npc, Position3D, inside);
        quests_found_town(world, character, npc);
        zox_tst_find_outside_done = zox_has(zox_tst_find_quest, QuestDone);
        // the villager in the town
        zox_add(npc, Villager);
        quests_found_town(world, character, npc);
        zox_delete(npc);
        zox_tst_find_stage = 2;
        zox_tst_find_frames = 0;
        return 0;
    }
    return zox_tst_find_frames > 5;
}

void zox_tst_find_quest_check(ecs* world) {
    const entity character = zox_tst_player_character(world);
    zox_tst_check(zox_valid(zox_tst_find_quest), "the player didn't start with the find quest");
    if (!zox_valid(zox_tst_find_quest)) {
        return;
    }
    zox_log("[tst] find quest tracker: %s", zox_tst_find_tracker);
    zox_tst_check(!strstr(zox_tst_find_tracker, "???"), "the tracker shows [%s]", zox_tst_find_tracker);
    zox_tst_check(zox_tst_find_panel, "the tracker's text has no panel behind it at the start, size [%ix%i]",
        zox_tst_find_panel_size.x, zox_tst_find_panel_size.y);
    zox_tst_check(!zox_tst_find_outside_done, "talking to a villager outside a town, or a critter in one, finished it");
    zox_tst_check(zox_has(zox_tst_find_quest, QuestDone) && zox_has(zox_tst_find_quest, QuestHandedin),
        "talking to a villager in a town didn't finish and hand it in");
    const int cookies = zox_tst_find_count_cookies(world, character);
    zox_tst_check(cookies == zox_tst_find_cookies + 3, "[%i] cookies after, [%i] before, not 3 more", cookies, zox_tst_find_cookies);
}

void zox_tsts_find_quest(ecs* world) {
    zox_tst_add("find_quest", zox_tst_find_quest_ready, zox_tst_find_quest_check, 120);
}
