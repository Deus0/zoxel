// Npcs' sight (their Target): the closest player an npc sees, 0 none; what it does with it is up to others
//  - humanoids look at them (sys/glance.c), monsters attack them (monster_aggro_system)
//  - seen: in its view cone (npc_sight_cone) within npc_sight_distance, or close by (npc_sense_distance) all round, with
//    no block between its eye and them, sampled every half block in its terrain
//  - players in its chunk and its 6 neighbours (ChunkNeighbors), so it sees across a chunk's edge
//  - chunks' ChunkEntities change on workers (chunk_link_system), read under their lock (it reallocates them)
#define npc_sight_distance 8.0f
#define npc_sight_cone 0.5f             // cos of its half angle, 60 degrees either side
#define npc_sense_distance 2.0f
#define npc_eye_height 0.25f            // of its bounds above its centre

// no block between two points, sampled every half block
static inline byte npc_line_clear(ecs* world, entity terrain, float3 a, float3 b) {
    const float scale = zox_getv(terrain, BlockScale);
    const int steps = (int) (float3_distance(a, b) / (0.5f * scale));
    for (int i = 1; i < steps; i++) {
        const float3 p = float3_add(a, float3_scale(float3_subtract(b, a), (float) i / steps));
        if (terrain_voxel_at(world, terrain, real_position_to_block_position(p, scale)) > 0) {
            return 0;
        }
    }
    return 1;
}

// in its cone or close by, and in a clear line from its eye
static inline byte npc_can_see(ecs* world, entity terrain, float3 eye, float3 forward, float3 target) {
    float3 to = float3_subtract(target, eye);
    to.y = 0;
    const float distance = float3_length(to);
    if (distance > npc_sight_distance) {
        return 0;
    }
    if (distance > npc_sense_distance && float3_dot(forward, float3_scale(to, 1 / distance)) < npc_sight_cone) {
        return 0;
    }
    return npc_line_clear(world, terrain, eye, target);
}

// the closest player it sees in a chunk, closer than closest (which it lowers)
static inline entity sight_in_chunk(ecs* world, entity e, entity terrain, entity chunk, float3 eye, float3 forward, float* closest) {
    if (!zox_valid(chunk) || !zox_has(chunk, ChunkEntities)) {
        return 0;
    }
    entity found = 0;
    ChunkEntities* entities = (ChunkEntities*) zox_get(chunk, ChunkEntities);
    spin_lock(&entities->lock);
    for (int j = 0; j < entities->length; j++) {
        const entity e2 = entities->value[j];
        if (e2 == e || !zox_valid(e2) || !zox_valid(zox_get_link(world, e2, PlayerLink)) || zox_has(e2, Dead)) {
            continue;
        }
        const float3 target = zox_getv(e2, Position3D);
        const float distance = float3_distance(eye, target);
        if (distance < *closest && npc_can_see(world, terrain, eye, forward, target)) {
            *closest = distance;
            found = e2;
        }
    }
    spin_unlock(&entities->lock);
    return found;
}

void npc_sight_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(Bounds3D);
    zox_sys_out(Target);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_o(Target, sight);
        const entity chunk = zox_get_link(world, e, ChunkLink);
        const entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(chunk) || !zox_valid(terrain)) {
            sight->value = 0;
            continue;
        }
        float3 eye = position->value;
        eye.y += npc_eye_height * bounds->value.y;
        const float3 forward = quaternion_to_normal(rotation->value);
        float closest = npc_sight_distance + 1;
        entity seen = sight_in_chunk(world, e, terrain, chunk, eye, forward, &closest);
        const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
        for (byte j = 0; neighbors && j < chunk_neighbors_length; j++) {
            const entity found = sight_in_chunk(world, e, terrain, neighbors->value[j], eye, forward, &closest);
            if (found) {
                seen = found;
            }
        }
        sight->value = seen;
    }
    zox_sys_on_end();
} zoxd_system(npc_sight_system);

// Monsters (slimes) attack a player they see: a battle with them (combat_on_hit), kept going while it sees its attacker
//  (it chases till it loses them, then calms after exit_combat_time)
void monster_aggro_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Target);
    zox_sys_in(CombatState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Target, sight);
        zox_sys_i(CombatState, combat);
        if (!zox_valid(sight->value)) {
            continue;
        }
        if (combat->value == zox_combat_peace) {
            combat_on_hit(world, e, sight->value);
        } else if (combat->value == zox_combat_battle && zox_get_link(world, e, AttackerLink) == sight->value) {
            zox_setv(e, LastCombatTime, zox_current_time);
        }
    }
    zox_sys_on_end();
} zoxd_system(monster_aggro_system);
