// Monsters attack the players they see (Monster: slimes): a player in its view cone and sight, no block between their
//  eyes, starts a battle (combat_on_hit) and keeps it going while it sees them, so it chases till it loses them
//  - players in its chunk and its 6 neighbours (glance_in_chunk), the line sampled block by block in its terrain
//  - close by it senses them all round (npc_sense_distance), behind it too
#define npc_sight_distance 8.0f
#define npc_sight_cone 0.5f             // cos of its half angle, 60 degrees either side
#define npc_sense_distance 2.0f
#define npc_eye_height 0.25f            // of its bounds above its centre

// no block between two points, sampled every half block
static inline byte npc_line_clear(ecs* world, entity terrain, float3 a, float3 b) {
    const float scale = zox_getv(terrain, BlockScale);
    const float distance = float3_distance(a, b);
    const int steps = (int) (distance / (0.5f * scale));
    for (int i = 1; i < steps; i++) {
        const float3 p = float3_add(a, float3_scale(float3_subtract(b, a), (float) i / steps));
        if (terrain_voxel_at(world, terrain, real_position_to_block_position(p, scale)) > 0) {
            return 0;
        }
    }
    return 1;
}

void npc_sight_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(CombatState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(CombatState, combat);
        const byte at_peace = combat->value == zox_combat_peace;
        if (!at_peace && combat->value != zox_combat_battle) {
            continue;
        }
        const entity chunk = zox_get_link(world, e, ChunkLink);
        const entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(chunk) || !zox_valid(terrain)) {
            continue;
        }
        float closest = npc_sight_distance;
        entity seen = glance_in_chunk(world, e, chunk, position->value, &closest);
        const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
        for (byte j = 0; neighbors && j < chunk_neighbors_length; j++) {
            const entity found = glance_in_chunk(world, e, neighbors->value[j], position->value, &closest);
            if (found) {
                seen = found;
            }
        }
        if (!zox_valid(seen) || zox_has(seen, Dead)) {
            continue;
        }
        // in battle it only keeps sight of its attacker
        if (!at_peace && zox_get_link(world, e, AttackerLink) != seen) {
            continue;
        }
        const float3 target = zox_getv(seen, Position3D);
        float3 to = float3_subtract(target, position->value);
        to.y = 0;
        const float distance = float3_length(to);
        if (distance > npc_sense_distance &&
            float3_dot(quaternion_to_normal(rotation->value), float3_scale(to, 1 / distance)) < npc_sight_cone) {
            continue;
        }
        float3 eye = position->value;
        eye.y += npc_eye_height * zox_getv(e, Bounds3D).y;
        if (!npc_line_clear(world, terrain, eye, target)) {
            continue;
        }
        if (at_peace) {
            combat_on_hit(world, e, seen);
        } else {
            zox_setv(e, LastCombatTime, zox_current_time);
        }
    }
    zox_sys_on_end();
} zoxd_system(npc_sight_system);
