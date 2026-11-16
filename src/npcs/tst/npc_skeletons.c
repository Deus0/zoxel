// tst: npc_skeletons, humanoid npcs get their bodies (they were invisible)
//  - a realm of its own (world_only.c, no game, no player), spawns one in the void from the realm's humanoid prefab (as streaming does), one from the base
//      skeleton prefab
//  - each: a chest slot (Body), body parts attached (its DataLink), a skeleton (SkeletonLink)
//  - run: bash bsh/tst.sh npc_skeletons
int zox_tst_skeletons_stage = 0;
int zox_tst_skeletons_frames = 0;
entity zox_tst_skeletons[2];

// the realm's humanoid character prefab (Boney, a villager)
static inline entity zox_tst_realm_humanoid(ecs* world, entity realm) {
    entity found = 0;
    zox_each_link(world, realm, CharacterLinks, meta) {
        if (zox_has(meta, Villager)) {
            found = meta;
        }
    }
    return found;
}

// once their chest part and skeleton are there
static inline byte zox_tst_skeletons_built(ecs* world, entity e) {
    const entity chest = zox_valid(e) ? zox_get_child_by_id(world, e, zox_id(Body)) : 0;
    return zox_valid(chest) && zox_valid(zox_get_link(world, chest, DataLink)) && zox_valid(zox_get_link(world, e, SkeletonLink));
}

// a fixture: a realm of its own (world_only.c, no game, no player) to spawn npcs into, in the void (nothing streams
//  there): 1 once its characters are made; register the test with zox_tst_set_setup(name, NULL, 1)
#define zox_tst_npc_void ((float3) { 0, 1000, 0 })
static inline byte zox_tst_npc_world(ecs* world, entity* realm, entity* terrain) {
    zox_tst_waiting = "a realm of its own and its characters";
    if (!zox_tst_world_ready(world)) {
        return 0;
    }
    *realm = zox_tst_world_realm;
    *terrain = zox_tst_world_terrain;
    return zox_valid(zox_tst_realm_humanoid(world, *realm));
}

byte zox_tst_npc_skeletons_ready(ecs* world) {
    // a realm of its own (world_only.c), no game, no player: the npcs spawn in the void, nothing streams there
    zox_tst_skeletons_frames++;
    zox_tst_progress(zox_tst_skeletons_stage * 1000 + zox_tst_skeletons_frames);
    if (!zox_tst_world_ready(world)) {
        return 0;
    }
    const entity realm = zox_tst_world_realm;
    const entity terrain = zox_tst_world_terrain;
    if (zox_tst_skeletons_stage == 0) {
        zox_tst_waiting = "the realm's humanoid";
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        if (!zox_valid(humanoid)) {
            if (zox_tst_skeletons_frames > 600) {
                zox_tst_check(0, "the realm has no humanoid character prefab");
                return 1;
            }
            return 0;
        }
        float3 position = { 0, 1000, 0 };
        zox_tst_skeletons[0] = spawn_character3_npc(world, humanoid, realm, terrain, 1, 0, 0, position, quaternion_identity, NULL);
        position.x += 2;
        zox_tst_skeletons[1] = spawn_character3_npc(world, prefab_character3_skeleton_npc, realm, terrain, 2, 0, 0, position, quaternion_identity, NULL);
        zox_tst_skeletons_stage = 1;
        zox_tst_skeletons_frames = 0;
        return 0;
    }
    zox_tst_waiting = "their bodies";
    return (zox_tst_skeletons_built(world, zox_tst_skeletons[0]) && zox_tst_skeletons_built(world, zox_tst_skeletons[1])) ||
        zox_tst_skeletons_frames > 600;
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

void zox_tsts_npc_skeletons(ecs* world) {
    zox_tst_add("npc_skeletons", zox_tst_npc_skeletons_ready, zox_tst_npc_skeletons_check, 120);
    zox_tst_set_setup("npc_skeletons", NULL, 1);
}
