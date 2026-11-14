// tst: npcs_after_load, a loaded game has npcs round the player (they were gone after loading, outside towns)
//  - continues a saved game (continue_game), waits after the player spawns, counts npcs and the chunks that spawn them
//  - run: bash bsh/tst.sh npcs_after_load
byte zox_tst_continue_game_ready(ecs* world);  // tst/_.c, below
double zox_tst_nal_spawned = 0;
int zox_tst_nal_npcs = 0, zox_tst_nal_zones = 0, zox_tst_nal_spawned_chunks = 0, zox_tst_nal_near = 0, zox_tst_nal_population = 0;

byte zox_tst_npcs_after_load_ready(ecs* world) {
    if (!zox_tst_nal_spawned) {
        if (!zox_tst_continue_game_ready(world)) {
            return 0;
        }
        zox_tst_nal_spawned = zox_current_time;
    }
    zox_tst_waiting = "npcs after loading";
    zox_tst_progress(zox_current_time - zox_tst_nal_spawned);
    if (zox_current_time - zox_tst_nal_spawned < 8) {
        return 0;
    }
    zox_tst_nal_npcs = zox_count_types(Npc);
    // near the player (32 blocks), and the population there
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float scale = zox_getv(terrain, BlockScale);
    const float3 p = zox_getv(character, Position3D);
    ecs_iter_t nit = ecs_each_id(world, zox_id(Npc));
    while (ecs_each_next(&nit)) {
        for (int i = 0; i < nit.count; i++) {
            if (zox_has(nit.entities[i], Position3D)) {
                zox_tst_nal_near += float3_distance(zox_getv(nit.entities[i], Position3D), p) < 32 * scale;
            }
        }
    }
    const int chunk_length = octree_size(zox_getv(terrain, NodeDepth));
    const int3 chunk = real_position_to_chunk_position(p, chunk_length, scale);
    const entity region = zox_tst_player_region(world);
    zox_tst_nal_population = region_population_at(world, region, (int2) { chunk.x * chunk_length, chunk.z * chunk_length }, chunk_length);
    ecs_iter_t it = ecs_each_id(world, zox_id(NpcSpawnZone));
    while (ecs_each_next(&it)) {
        const NpcSpawnZone* zones = ecs_field(&it, NpcSpawnZone, 0);
        for (int i = 0; i < it.count; i++) {
            zox_tst_nal_zones += zones[i].value != 0;
            zox_tst_nal_spawned_chunks += zox_has(it.entities[i], CharactersSpawned);
        }
    }
    return 1;
}

void zox_tst_npcs_after_load_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] npcs_after_load [%i] npcs, [%i] chunks in the spawn zone, [%i] spawned theirs",
        zox_tst_nal_npcs, zox_tst_nal_zones, zox_tst_nal_spawned_chunks);
    zox_log("[tst] [metric] npcs_after_load [%i] npcs within 32 blocks, the population there [%i] of 255", zox_tst_nal_near, zox_tst_nal_population);
    zox_tst_check(zox_tst_nal_zones > 0, "no chunks in the npc spawn zone after loading");
    zox_tst_check(zox_tst_nal_npcs > 0, "no npcs after loading");
}
