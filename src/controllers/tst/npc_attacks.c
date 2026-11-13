// tst: npc_attacks, an npc the player hits fights back and hurts the player
//  - spawns a humanoid npc beside the player (not a coward), hits it as the player would (combat_on_hit)
//  - it goes into battle, attacks (Behaviour), triggers its skill, and the player's health drops
//  - logs where the chain stops: battle, behaviour, skill, its ray's hit, the player's health
//  - run: bash bsh/tst.sh npc_attacks
byte zox_tst_player_spawn_ready(ecs* world);   // in _.c, after this is included
byte zox_tst_attacks_stage = 0;
int zox_tst_attacks_frames = 0;
double zox_tst_attacks_time = 0;
entity zox_tst_attacks_npc = 0;
float zox_tst_attacks_health = 0;
#define zox_tst_attacks_seconds 15.0

static inline float zox_tst_attacks_player_health(ecs* world, entity character) {
    const entity health = zox_get_child_by_id(world, character, zox_id(StatHealth));
    return zox_valid(health) ? zox_getv(health, StatValue) : -1;
}

byte zox_tst_npc_attacks_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity player = dbg_player;
    const entity character = zox_get_link(world, player, CharacterLink);
    zox_tst_attacks_frames++;
    if (zox_tst_attacks_stage == 0) {
        zox_tst_waiting = "playing";
        if (zox_getv(player, PlayerState) != zox_player_state_playing || zox_tst_attacks_frames < 10) {
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
        position.x += 1.5f;
        zox_tst_attacks_npc = spawn_character3_npc(world, humanoid, realm, terrain, 7, 0, 0, position, quaternion_identity, NULL);
        zox_remove(zox_tst_attacks_npc, Coward);
        zox_tst_attacks_stage = 1;
        zox_tst_attacks_frames = 0;
        return 0;
    }
    if (zox_tst_attacks_stage == 1) {
        zox_tst_waiting = "the npc to initialize";
        const entity npc = zox_tst_attacks_npc;
        if (zox_tst_attacks_frames < 30 || !zox_valid(npc) || zox_has(npc, PreInitialize) || zox_has(npc, Initialize)) {
            return 0;
        }
        zox_tst_attacks_health = zox_tst_attacks_player_health(world, character);
        combat_on_hit(world, npc, character);
        zox_tst_attacks_time = zox_current_time;
        zox_tst_attacks_stage = 2;
        return 0;
    }
    zox_tst_waiting = "the npc to hurt the player";
    const entity npc = zox_tst_attacks_npc;
    const float health = zox_tst_attacks_player_health(world, character);
    if (zox_tst_attacks_frames % 60 == 0 && zox_valid(npc)) {
        const entity action = zox_has(npc, ActiveAction) ? zox_getv(npc, ActiveAction) : 0;
        const RaycastVoxelData* ray = zox_get(npc, RaycastVoxelData);
        zox_log("[tst] npc combat [%i] behaviour [%i] action [%s] ray hit [%s] at [%.2f] player health [%.1f] of [%.1f]",
            zox_getv(npc, CombatState), zox_getv(npc, Behaviour), zox_get_name(action),
            zox_get_name(ray->chunk), ray->distance, health, zox_tst_attacks_health);
    }
    return health < zox_tst_attacks_health || zox_current_time - zox_tst_attacks_time > zox_tst_attacks_seconds;
}

void zox_tst_npc_attacks_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const float health = zox_tst_attacks_player_health(world, character);
    zox_log("[tst] player health [%.1f] from [%.1f] after [%.1fs]", health, zox_tst_attacks_health,
        zox_current_time - zox_tst_attacks_time);
    zox_tst_check(zox_valid(zox_tst_attacks_npc), "the npc is gone");
    zox_tst_check(health < zox_tst_attacks_health, "the npc didn't hurt the player in [%.0fs]", zox_tst_attacks_seconds);
}

// tst: region_population, the player's region has its population map, busy at its towns (npcs/sys/population.c)
//  - one cell a chunk column, every cell at least the base, a town's column full
//  - run: bash bsh/tst.sh region_population
byte zox_tst_region_population_ready(ecs* world) {
    return zox_tst_player_spawn_ready(world);
}

void zox_tst_region_population_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const int chunk_length = octree_size(zox_getv(terrain, NodeDepth));
    // the region under the player's tunk
    const float3 position = zox_getv(character, Position3D);
    const int3 chunk = real_position_to_chunk_position(position, chunk_length, zox_getv(terrain, BlockScale));
    const TunkLinks* tunks = zox_get(terrain, TunkLinks);
    const entity tunk = int2_hashmap_get(tunks->value, (int2) { chunk.x, chunk.z });
    const entity region = zox_valid(tunk) ? zox_get_link(world, tunk, RegionLink) : 0;
    zox_tst_check(zox_valid(region), "no region under the player");
    if (!zox_valid(region)) {
        return;
    }
    const PopulationMap* map = zox_get(region, PopulationMap);
    const int2 size = population_map_size(zox_getv(region, BlockSize2), chunk_length);
    zox_log("[tst] [metric] region_population [%ix%i] cells, map [%i]", size.x, size.y, map->length);
    zox_tst_check(map->length && map->length == size.x * size.y, "the map is [%i], not [%ix%i]", map->length, size.x, size.y);
    int below = 0;
    for (int i = 0; i < map->length; i++) {
        below += map->value[i] < population_base;
    }
    zox_tst_check(!below, "[%i] cells under the base", below);
    entity towns[zox_children_capacity];
    const uint towns_count = zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town));
    for (uint i = 0; i < towns_count; i++) {
        const int p = region_population_at(world, region, zox_getv(towns[i], BlockPosition2), chunk_length);
        zox_tst_check(p == population_town, "town [%i]'s column is [%i], not full", i, p);
    }
}

// tst: npc_leash, an npc wandering far from home walks back towards it (ai/sys/wander_system.c)
//  - its home set 30 units away (+x), wandering 6s it walks towards it (at peace, half a unit a second)
//  - run: bash bsh/tst.sh npc_leash
entity zox_tst_leash_npc = 0;
double zox_tst_leash_time = 0;
float zox_tst_leash_start = 0;

static inline float zox_tst_leash_distance(ecs* world, entity npc) {
    const float3 p = zox_getv(npc, Position3D), h = zox_getv(npc, HomePosition);
    return sqrtf((p.x - h.x) * (p.x - h.x) + (p.z - h.z) * (p.z - h.z));
}

byte zox_tst_npc_leash_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    if (!zox_tst_leash_npc) {
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        if (!zox_valid(humanoid)) {
            zox_tst_check(0, "the realm has no humanoid character prefab");
            return 1;
        }
        float3 position = zox_getv(character, Position3D);
        position.x += 2;
        zox_tst_leash_npc = spawn_character3_npc(world, humanoid, realm, zox_get_link(world, character, TerrainLink), 9, 0, 0,
            position, quaternion_identity, NULL);
        zox_remove(zox_tst_leash_npc, Coward);
        return 0;
    }
    const entity npc = zox_tst_leash_npc;
    zox_tst_waiting = "the npc to wander home";
    if (!zox_tst_leash_time) {
        if (zox_has(npc, PreInitialize) || zox_has(npc, Initialize)) {
            return 0;
        }
        float3 home = zox_getv(npc, Position3D);
        home.x += 30;
        zox_setv(npc, HomePosition, home);
        zox_setv(npc, DefaultBehaviour, zox_behaviour_wander);
        zox_setv(npc, Behaviour, zox_behaviour_wander);
        zox_tst_leash_start = zox_tst_leash_distance(world, npc);
        zox_tst_leash_time = zox_current_time;
        return 0;
    }
    static int logged = 0;
    if ((int) (zox_current_time - zox_tst_leash_time) > logged) {
        logged = (int) (zox_current_time - zox_tst_leash_time);
        const float3 v = zox_getv(npc, Velocity3D);
        zox_log("[tst] leash [%is] home [%.1f] behaviour [%i] move [%i] velocity [%.2f %.2f %.2f]", logged,
            zox_tst_leash_distance(world, npc), zox_getv(npc, Behaviour), zox_getv(npc, MoveForwards), v.x, v.y, v.z);
    }
    return zox_current_time - zox_tst_leash_time > 6;
}

void zox_tst_npc_leash_check(ecs* world) {
    const entity npc = zox_tst_leash_npc;
    zox_tst_check(zox_valid(npc), "the npc is gone");
    if (!zox_valid(npc)) {
        return;
    }
    const float now = zox_tst_leash_distance(world, npc);
    zox_log("[tst] [metric] npc_leash home [%.1f] away, now [%.1f]", zox_tst_leash_start, now);
    // npcs walk about half a unit a second at peace
    zox_tst_check(now < zox_tst_leash_start - 1.5f, "the npc didn't head home, [%.1f] to [%.1f] away", zox_tst_leash_start, now);
    const float3 v = zox_getv(npc, Velocity3D);
    zox_tst_check(v.x > 0, "the npc isn't walking towards its home (+x), velocity x [%.2f]", v.x);
}
