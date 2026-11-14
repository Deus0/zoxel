// tst: npcs_after_load, a loaded game has npcs round the player (they were gone after loading, outside towns)
//  - continues a saved game (continue_game), waits after the player spawns, counts npcs and the chunks that spawn them
//  - then flies the character (held high up) to spots out in the wild, away from towns, waits at each, and logs the
//      npcs near it (across the ground) and the population there; out of towns, where its chunks spawned theirs, some
//      npcs (the wild rounded its spawn rolls down to none)
//  - run: bash bsh/tst.sh npcs_after_load (zox_tst_save=<realm folder> for a real save)
#define zox_tst_nal_spots 4

byte zox_tst_continue_game_ready(ecs* world);  // tst/_.c, below
double zox_tst_nal_spawned = 0, zox_tst_nal_arrived = 0, zox_tst_nal_streamed = 0;
int zox_tst_nal_npcs = 0, zox_tst_nal_zones = 0, zox_tst_nal_spawned_chunks = 0, zox_tst_nal_near = 0, zox_tst_nal_population = 0;
int zox_tst_nal_spot = -1;
float3 zox_tst_nal_start;
int zox_tst_nal_spot_near[zox_tst_nal_spots], zox_tst_nal_spot_population[zox_tst_nal_spots];
byte zox_tst_nal_spot_town[zox_tst_nal_spots];
int zox_tst_nal_spot_tunk[zox_tst_nal_spots], zox_tst_nal_spot_region[zox_tst_nal_spots], zox_tst_nal_spot_chunks[zox_tst_nal_spots];

// npcs within distance blocks of p across the ground (any height)
static inline int zox_tst_nal_count_near(ecs* world, float3 p, float distance) {
    int near = 0;
    ecs_iter_t it = ecs_each_id(world, zox_id(Npc));
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            if (zox_has(it.entities[i], Position3D)) {
                const float3 n = zox_getv(it.entities[i], Position3D);
                near += float2_distance((float2) { n.x, n.z }, (float2) { p.x, p.z }) < distance;
            }
        }
    }
    return near;
}

// the population (0 to 255) of the chunk column at p
static inline int zox_tst_nal_population_at(ecs* world, entity terrain, float3 p) {
    const float scale = zox_getv(terrain, BlockScale);
    const int chunk_length = octree_size(zox_getv(terrain, NodeDepth));
    const int3 chunk = real_position_to_chunk_position(p, chunk_length, scale);
    const entity region = zox_tst_player_region(world);
    return region_population_at(world, region, (int2) { chunk.x * chunk_length, chunk.z * chunk_length }, chunk_length);
}

byte zox_tst_npcs_after_load_ready(ecs* world) {
    if (!zox_tst_nal_spawned) {
        if (!zox_tst_continue_game_ready(world)) {
            return 0;
        }
        zox_tst_nal_spawned = zox_current_time;
    }
    zox_tst_waiting = "npcs after loading";
    zox_tst_progress((int) (zox_current_time - zox_tst_nal_spawned));
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float scale = zox_getv(terrain, BlockScale);
    if (zox_tst_nal_spot < 0) {
        if (zox_current_time - zox_tst_nal_spawned < 8) {
            return 0;
        }
        zox_tst_nal_npcs = zox_count_types(Npc);
        const float3 p = zox_getv(character, Position3D);
        zox_tst_nal_near = zox_tst_nal_count_near(world, p, 32 * scale);
        zox_tst_nal_population = zox_tst_nal_population_at(world, terrain, p);
        ecs_iter_t it = ecs_each_id(world, zox_id(NpcSpawnZone));
        while (ecs_each_next(&it)) {
            const NpcSpawnZone* zones = ecs_field(&it, NpcSpawnZone, 0);
            for (int i = 0; i < it.count; i++) {
                zox_tst_nal_zones += zones[i].value != 0;
                zox_tst_nal_spawned_chunks += zox_has(it.entities[i], CharactersSpawned);
            }
        }
        zox_tst_nal_start = p;
        zox_tst_nal_spot = 0;
        zox_tst_nal_arrived = zox_current_time;
    }
    // the spot: 120 blocks out each way from the start, held high up while the land round it streams in
    const float2 ways[zox_tst_nal_spots] = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };
    const float out = 120 * scale;
    const float3 spot = { zox_tst_nal_start.x + ways[zox_tst_nal_spot].x * out, 90 * scale,
        zox_tst_nal_start.z + ways[zox_tst_nal_spot].y * out };
    zox_setv(character, Position3D, spot);
    zox_setv(character, Velocity3D, float3_zero);
    // until its land has streamed in (its tunk, up to 30s), then a while for its npcs
    {
        const int chunk_length = octree_size(zox_getv(terrain, NodeDepth));
        const int3 chunk = real_position_to_chunk_position(spot, chunk_length, scale);
        const entity tunk = int2_hashmap_get(zox_get(terrain, TunkLinks)->value, (int2) { chunk.x, chunk.z });
        if (!zox_valid(tunk) && zox_current_time - zox_tst_nal_arrived < 30) {
            return 0;
        }
        if (zox_valid(tunk) && !zox_tst_nal_streamed) {
            zox_tst_nal_streamed = zox_current_time;
        }
        if (zox_tst_nal_streamed && zox_current_time - zox_tst_nal_streamed < 8) {
            return 0;
        }
    }
    zox_log("[tst] spot [%i] at block [%.0fx%.0f], streamed after [%.1fs]", zox_tst_nal_spot, spot.x / scale, spot.z / scale,
        zox_tst_nal_streamed ? zox_tst_nal_streamed - zox_tst_nal_arrived : -1.0);
    zox_tst_nal_streamed = 0;
    const int s = zox_tst_nal_spot;
    zox_tst_nal_spot_near[s] = zox_tst_nal_count_near(world, spot, 48 * scale);
    zox_tst_nal_spot_population[s] = zox_tst_nal_population_at(world, terrain, spot);
    zox_tst_nal_spot_town[s] = town_at(world, (int2) { (int) floorf(spot.x / scale), (int) floorf(spot.z / scale) }) != 0;
    // its tunk, the tunk's region, and its chunks that spawned their npcs
    const int chunk_length = octree_size(zox_getv(terrain, NodeDepth));
    const int3 chunk = real_position_to_chunk_position(spot, chunk_length, scale);
    const entity tunk = int2_hashmap_get(zox_get(terrain, TunkLinks)->value, (int2) { chunk.x, chunk.z });
    zox_tst_nal_spot_tunk[s] = zox_valid(tunk);
    zox_tst_nal_spot_region[s] = zox_valid(tunk) && zox_valid(zox_get_link(world, tunk, RegionLink));
    zox_tst_nal_spot_chunks[s] = 0;
    if (zox_valid(tunk)) {
        const Chunk3Stack* stack = zox_get(tunk, Chunk3Stack);
        for (int k = 0; k < chunk3_stack_length; k++) {
            zox_tst_nal_spot_chunks[s] += zox_valid(stack->value[k]) && zox_has(stack->value[k], CharactersSpawned);
        }
    }
    zox_tst_nal_spot++;
    zox_tst_nal_arrived = zox_current_time;
    return zox_tst_nal_spot >= zox_tst_nal_spots;
}

void zox_tst_npcs_after_load_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] npcs_after_load [%i] npcs, [%i] chunks in the spawn zone, [%i] spawned theirs",
        zox_tst_nal_npcs, zox_tst_nal_zones, zox_tst_nal_spawned_chunks);
    zox_log("[tst] [metric] npcs_after_load [%i] npcs within 32 blocks, the population there [%i] of 255", zox_tst_nal_near, zox_tst_nal_population);
    for (int s = 0; s < zox_tst_nal_spots; s++) {
        zox_log("[tst] [metric] npcs_after_load spot [%i]: [%i] npcs within 48 blocks, population [%i] of 255, in a town [%i], tunk [%i] region [%i] chunks spawned [%i]",
            s, zox_tst_nal_spot_near[s], zox_tst_nal_spot_population[s], zox_tst_nal_spot_town[s], zox_tst_nal_spot_tunk[s], zox_tst_nal_spot_region[s], zox_tst_nal_spot_chunks[s]);
    }
    zox_tst_check(zox_tst_nal_zones > 0, "no chunks in the npc spawn zone after loading");
    zox_tst_check(zox_tst_nal_npcs > 0, "no npcs after loading");
    // out in the wild (no town), where its chunks have spawned theirs: some npcs (the wild rounded its rolls down to 0)
    int wild_spots = 0, wild_npcs = 0;
    for (int s = 0; s < zox_tst_nal_spots; s++) {
        if (!zox_tst_nal_spot_town[s] && zox_tst_nal_spot_chunks[s]) {
            wild_spots++;
            wild_npcs += zox_tst_nal_spot_near[s];
        }
    }
    // (14 at the wild spot on the play test save, 1 before: one wandered from a town)
    zox_tst_check(wild_npcs >= 4 * wild_spots, "[%i] npcs at [%i] spots out in the wild, under 4 a spot", wild_npcs, wild_spots);
}
