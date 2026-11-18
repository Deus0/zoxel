// tst: quest_marker, a quest giver's marker shows the player's state of its quest, and keeps it when it respawns
//  - ! yellow: it has a quest to give, ? gray: the player has it, ? yellow: the player can hand it in
//  - spawns seeded villagers by the player, takes a giver's quest (like a give quest node, zox_event_give_quest)
//  - deletes and respawns the giver from its seed (like a chunk unloading and loading), the marker keeps the state
//      (it showed ! again)
//  - run: bash bsh/tst.sh quest_marker
#define zox_tst_qm_count 24
int zox_tst_qm_stage = 0;
int zox_tst_qm_frames = 0;
entity zox_tst_qm_npcs[zox_tst_qm_count];
entity zox_tst_qm_npc = 0;
uint zox_tst_qm_seed = 0;
float3 zox_tst_qm_position;
entity zox_tst_qm_quest = 0;

static inline uint zox_tst_qm_seed_of(int i) {
    return 1000 + i * 7919;
}

static inline entity zox_tst_qm_spawn(ecs* world, entity character, uint seed, float3 position) {
    const entity realm = zox_get_link(world, character, RealmLink);
    return spawn_character3_npc(world, zox_tst_realm_humanoid(world, realm), realm,
        zox_get_link(world, character, TerrainLink), seed, 1, 0, position, quaternion_identity, NULL);
}

// checks the npc's marker shows text in fill
static inline void zox_tst_qm_expect(ecs* world, entity npc, const char* text, color fill, const char* when) {
    const entity marker = zox_valid(npc) ? zox_get_link(world, npc, MarkerLink) : 0;
    const entity label = zox_valid(marker) ? zox_get_child_by_id_recursive(world, marker, zox_id(Text)) : 0;
    if (!zox_valid(label)) {
        zox_tst_check(0, "%s: the quest giver has no marker", when);
        return;
    }
    const byte shows = is_zext((TextData*) zox_get(label, TextData), text);
    const color shown = zox_getv(label, FontFillColor);
    zox_log("[tst] %s: marker [%s] fill [%i %i %i]", when, shows ? text : "other", shown.r, shown.g, shown.b);
    zox_tst_check(shows, "%s: the marker doesn't show [%s]", when, text);
    zox_tst_check(color_equal(shown, fill), "%s: the marker [%s] is the wrong colour", when, text);
}

// respawns the quest giver from its seed, where it was
static inline void zox_tst_qm_respawn(ecs* world, entity character) {
    zox_delete(zox_tst_qm_npc);
    zox_tst_qm_npc = zox_tst_qm_spawn(world, character, zox_tst_qm_seed, zox_tst_qm_position);
}

byte zox_tst_quest_marker_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    zox_tst_qm_frames++;
    zox_tst_progress(zox_tst_qm_stage * 1000 + zox_tst_qm_frames);
    if (zox_tst_qm_stage == 0) {
        const entity realm = zox_get_link(world, character, RealmLink);
        if (!zox_valid(zox_tst_realm_humanoid(world, realm))) {
            zox_tst_check(0, "the realm has no humanoid character prefab");
            return 1;
        }
        const float3 at = zox_getv(character, Position3D);
        for (int i = 0; i < zox_tst_qm_count; i++) {
            const float3 position = { at.x + 3 + (i % 6), at.y, at.z + 3 + i / 6 };
            zox_tst_qm_npcs[i] = zox_tst_qm_spawn(world, character, zox_tst_qm_seed_of(i), position);
        }
    } else if (zox_tst_qm_frames < 60) {
        zox_tst_waiting = "the villagers' markers";
        return 0;
    } else if (zox_tst_qm_stage == 1) {
        // the first quest giver with a marker, the rest go
        for (int i = 0; i < zox_tst_qm_count; i++) {
            const entity npc = zox_tst_qm_npcs[i];
            if (!zox_tst_qm_npc && zox_valid(npc) && zox_has(npc, QuestGiver) && zox_get_link(world, npc, MarkerLink)) {
                zox_tst_qm_npc = npc;
                zox_tst_qm_seed = zox_tst_qm_seed_of(i);
                zox_tst_qm_position = zox_getv(npc, Position3D);
            } else if (zox_valid(npc)) {
                zox_delete(npc);
            }
        }
        if (!zox_tst_qm_npc) {
            zox_tst_check(0, "no villager is a quest giver with a marker");
            return 1;
        }
        zox_tst_qm_expect(world, zox_tst_qm_npc, quest_marker_available, quest_marker_fill, "has a quest");
        // take its quest
        const entity quest = get_quest_giver_quest(world, zox_tst_qm_npc);
        zox_tst_qm_quest = spawn_quest_doing(world, character, quest);
        const entity marker = zox_get_link(world, zox_tst_qm_npc, MarkerLink);
        link_quest_marker(world, zox_tst_qm_quest, marker);
        set_quest_marker(world, marker, zox_tst_qm_quest);
    } else if (zox_tst_qm_stage == 2) {
        zox_tst_qm_expect(world, zox_tst_qm_npc, quest_marker_doing, quest_marker_doing_fill, "took the quest");
        zox_tst_qm_respawn(world, character);
    } else if (zox_tst_qm_stage == 3) {
        zox_tst_qm_expect(world, zox_tst_qm_npc, quest_marker_doing, quest_marker_doing_fill, "respawned, doing");
        const entity marker = zox_get_link(world, zox_tst_qm_npc, MarkerLink);
        zox_tst_check(zox_get_link(world, zox_tst_qm_quest, MarkerLink) == marker,
            "the quest isn't linked to the respawned giver's marker");
        // the objectives are done
        zox_add(zox_tst_qm_quest, QuestDone);
        zox_add(zox_tst_qm_quest, Dirty);
    } else if (zox_tst_qm_stage == 4) {
        zox_tst_qm_expect(world, zox_tst_qm_npc, quest_marker_done, quest_marker_fill, "can hand in");
        zox_tst_qm_respawn(world, character);
    } else {
        zox_tst_qm_expect(world, zox_tst_qm_npc, quest_marker_done, quest_marker_fill, "respawned, can hand in");
        return 1;
    }
    zox_tst_qm_stage++;
    zox_tst_qm_frames = 0;
    return 0;
}

void zox_tst_quest_marker_check(ecs* world) {
    (void) world;
}

void zox_tsts_quest_marker(ecs* world) {
    zox_tst_add("quest_marker", zox_tst_quest_marker_ready, zox_tst_quest_marker_check, 120);
    zox_tst_set_setup("quest_marker", zox_tst_level_setup, 0);
}
