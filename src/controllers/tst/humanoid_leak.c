// tst: humanoid_leak, humanoid npcs leave no memory behind once deleted (end_game_cleanup's heap grows with villagers)
//  - spawns 20 humanoids by the player (their bodies, bones, part models), deletes them, three rounds
//  - logs the heap in use after each round, the last ones barely grow (the first warms flecs' pools): the array and
//    queue components' move hooks dropped their old memory (~700kb a humanoid, its skeleton meshes, end_game_cleanup)
//  - monster_leak: the same with monsters (slimes), to compare
//  - run: bash bsh/tst.sh humanoid_leak
#define zox_tst_hleak_count 20
#define zox_tst_hleak_rounds 4
byte zox_tst_hleak_monster = 0;     // monster_leak: slimes instead, to compare
int zox_tst_hleak_round = 0;
int zox_tst_hleak_frames = 0;
entity zox_tst_hleak_npcs[zox_tst_hleak_count];
double zox_tst_hleak_heap[zox_tst_hleak_rounds];

byte zox_tst_humanoid_leak_ready(ecs* world) {
    const entity player = dbg_player;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    if (!zox_valid(character) || zox_getv(player, PlayerState) != zox_player_state_playing) {
        zox_tst_waiting = "playing";
        return 0;
    }
    zox_tst_hleak_frames++;
    zox_tst_progress(zox_tst_hleak_round * 1000 + zox_tst_hleak_frames);
    zox_tst_waiting = "humanoids spawned and deleted";
    if (zox_tst_hleak_frames == 30) {
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity terrain = zox_get_link(world, character, TerrainLink);
        // a humanoid, or a monster (slime) to compare
        entity humanoid = zox_tst_realm_humanoid(world, realm);
        if (zox_tst_hleak_monster) {
            zox_each_link(world, realm, CharacterLinks, meta) {
                if (zox_has(meta, Monster)) {
                    humanoid = meta;
                }
            }
        }
        float3 position = zox_getv(character, Position3D);
        for (int i = 0; i < zox_tst_hleak_count; i++) {
            position.x += (i % 5) * 0.5f;
            zox_tst_hleak_npcs[i] = spawn_character3_npc(world, humanoid, realm, terrain, i + 1, 0, 0, position,
                quaternion_identity, NULL);
        }
    } else if (zox_tst_hleak_frames == 150) {
        for (int i = 0; i < zox_tst_hleak_count; i++) {
            if (zox_valid(zox_tst_hleak_npcs[i])) {
                zox_delete(zox_tst_hleak_npcs[i]);
            }
        }
    } else if (zox_tst_hleak_frames == 210) {
        zox_tst_hleak_heap[zox_tst_hleak_round] = zox_tst_heap_mb();
        zox_log("[tst] [metric] humanoid_leak round [%i] heap [%.1fmb] entities [%i] tables [%i]", zox_tst_hleak_round + 1,
            zox_tst_hleak_heap[zox_tst_hleak_round], ecs_get_entities(world).alive_count, zox_get_tables_count(world));
        zox_tst_hleak_round++;
        zox_tst_hleak_frames = 0;
    }
    return zox_tst_hleak_round >= zox_tst_hleak_rounds;
}

byte zox_tst_monster_leak_ready(ecs* world) {
    zox_tst_hleak_monster = 1;
    return zox_tst_humanoid_leak_ready(world);
}

void zox_tst_humanoid_leak_check(ecs* world) {
    (void) world;
    const double grew = (zox_tst_hleak_heap[zox_tst_hleak_rounds - 1] - zox_tst_hleak_heap[1]) / (zox_tst_hleak_rounds - 2);
    zox_log("[tst] [metric] humanoid_leak [%.2fmb] a round of [%i] humanoids, [%.0fkb] each", grew, zox_tst_hleak_count,
        grew * 1024 / zox_tst_hleak_count);
    // ~100kb each is left: flecs keeps a table for each humanoid's unique children (bones), the leak was ~700kb each
    zox_tst_check(grew < 3, "the heap grew [%.2fmb] a round of [%i] humanoids spawned and deleted", grew, zox_tst_hleak_count);
}
