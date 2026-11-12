// tst: npc_skeletons, humanoid npcs get their bodies (they were invisible)
//  - spawns one next to the player from the realm's humanoid prefab (as streaming does), one from the base
//      skeleton prefab
//  - each: a chest slot (Body), body parts attached (its DataLink), a skeleton (SkeletonLink)
//  - run: bash bsh/tst.sh npc_skeletons
int zox_tst_skeletons_stage = 0;
int zox_tst_skeletons_frames = 0;
entity zox_tst_skeletons[2];

// the realm's humanoid character prefab (Boney, a quest giver)
static inline entity zox_tst_realm_humanoid(ecs* world, entity realm) {
    entity found = 0;
    zox_each_link(world, realm, CharacterLinks, meta) {
        if (zox_has(meta, QuestGiver)) {
            found = meta;
        }
    }
    return found;
}

byte zox_tst_npc_skeletons_ready(ecs* world) {
    const entity player = dbg_player;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    if (!zox_valid(character)) {
        zox_tst_waiting = "the player's character";
        return 0;
    }
    zox_tst_skeletons_frames++;
    if (zox_tst_skeletons_stage == 0) {
        zox_tst_waiting = "playing";
        if (zox_getv(player, PlayerState) != zox_player_state_playing || zox_tst_skeletons_frames < 10) {
            return 0;
        }
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity terrain = zox_get_link(world, character, TerrainLink);
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        if (!zox_valid(humanoid)) {
            zox_tst_check(0, "the realm has no humanoid character prefab");
            return 1;
        }
        float3 position = zox_getv(character, Position3D);
        position.x += 2;
        zox_tst_skeletons[0] = spawn_character3_npc(world, humanoid, realm, terrain, 1, 0, 0, position, quaternion_identity, NULL);
        position.x += 2;
        zox_tst_skeletons[1] = spawn_character3_npc(world, prefab_character3_skeleton_npc, realm, terrain, 2, 0, 0, position, quaternion_identity, NULL);
        zox_tst_skeletons_stage = 1;
        zox_tst_skeletons_frames = 0;
        return 0;
    }
    zox_tst_waiting = "their bodies";
    return zox_tst_skeletons_frames > 120;
}

void zox_tst_npc_skeletons_check(ecs* world) {
    const char* names[2] = { "from the realm's humanoid", "from the skeleton prefab" };
    for (int i = 0; i < 2; i++) {
        const entity e = zox_tst_skeletons[i];
        zox_tst_check(zox_valid(e), "the npc %s is gone", names[i]);
        if (!zox_valid(e)) {
            continue;
        }
        const entity chest = zox_get_child_by_id(world, e, zox_id(Body));
        const entity part = zox_valid(chest) ? zox_get_link(world, chest, DataLink) : 0;
        const entity skeleton = zox_get_link(world, e, SkeletonLink);
        zox_log("[tst] npc %s: chest slot [%i] body part [%i] skeleton [%i] render disabled [%i]", names[i],
            zox_valid(chest), zox_valid(part), zox_valid(skeleton), zox_has(e, RenderDisabled) ? zox_getv(e, RenderDisabled) : -1);
        zox_tst_check(zox_valid(chest), "the npc %s has no chest slot", names[i]);
        zox_tst_check(zox_valid(part), "the npc %s has no body part on its chest", names[i]);
        zox_tst_check(zox_valid(skeleton), "the npc %s has no skeleton", names[i]);
    }
}
