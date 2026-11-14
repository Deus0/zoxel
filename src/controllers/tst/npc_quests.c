// tst: npc_quests, some villagers give quests, not all, and not all the same one
//  - spawns villagers by the player (the realm's humanoid, as towns do), each its own seed
//  - checks: some are quest givers (QuestGiver), not all; the givers give more than one quest; each has the quest its
//      dialogue gives (its QuestLink) to give (every villager gave the same one before)
//  - run: bash bsh/tst.sh npc_quests
#define zox_tst_nq_count 24
int zox_tst_nq_frames = 0;
entity zox_tst_nq_npcs[zox_tst_nq_count];

byte zox_tst_npc_quests_ready(ecs* world) {
    const entity player = dbg_player;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    if (!zox_valid(character) || zox_getv(player, PlayerState) != zox_player_state_playing) {
        zox_tst_waiting = "playing";
        return 0;
    }
    zox_tst_nq_frames++;
    zox_tst_progress(zox_tst_nq_frames);
    zox_tst_waiting = "the villagers' dialogues";
    if (zox_tst_nq_frames == 10) {
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        const float3 at = zox_getv(character, Position3D);
        for (int i = 0; i < zox_tst_nq_count; i++) {
            const float3 position = { at.x + 2 + (i % 6), at.y, at.z + 2 + i / 6 };
            zox_tst_nq_npcs[i] = spawn_character3_npc(world, humanoid, realm, zox_get_link(world, character, TerrainLink),
                1000 + i * 7919, 1, 0, position, quaternion_identity, NULL);
        }
    }
    return zox_tst_nq_frames > 60;
}

void zox_tst_npc_quests_check(ecs* world) {
    int givers = 0, talkers = 0, kinds = 0, mismatched = 0;
    entity quests[zox_tst_nq_count];
    for (int i = 0; i < zox_tst_nq_count; i++) {
        const entity npc = zox_tst_nq_npcs[i];
        const entity tree = zox_valid(npc) ? zox_get_link(world, npc, Dialogue) : 0;
        if (!zox_valid(tree)) {
            continue;
        }
        talkers++;
        if (!zox_has(npc, QuestGiver)) {
            continue;
        }
        givers++;
        const entity quest = zox_get_link(world, tree, QuestLink);
        mismatched += !zox_valid(quest) || !get_user_quest(world, npc, quest);
        byte seen = 0;
        for (int j = 0; j < kinds && !seen; j++) {
            seen = quests[j] == quest;
        }
        if (!seen) {
            quests[kinds++] = quest;
        }
    }
    zox_log("[tst] [metric] npc_quests [%i] villagers talk, [%i] give quests, [%i] kinds of quest", talkers, givers, kinds);
    zox_tst_check(talkers == zox_tst_nq_count, "only [%i] of [%i] villagers have a dialogue", talkers, zox_tst_nq_count);
    zox_tst_check(givers > 0, "no villager gives a quest");
    zox_tst_check(givers < talkers, "every villager gives a quest");
    zox_tst_check(kinds > 1, "every quest giver gives the same quest");
    zox_tst_check(!mismatched, "[%i] quest givers don't have the quest their dialogue gives", mismatched);
}
