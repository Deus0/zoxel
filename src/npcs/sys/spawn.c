extern void on_spawned_character3_npc(ecs*, entity);
uint total_spawned_npcs_count = 0;
// npcs a frame at most: each brings its children (stats, body, skeleton, label), chunks streaming in together
//  spawned many in one frame (lag spikes), the rest of the chunks spawn theirs next frame
#define npcs_spawned_per_frame_max 4
uint npcs_spawned_frame = UINT32_MAX;
uint npcs_spawned_this_frame = 0;

// we need to check if chunk has generated yet - is there a component for this?
void terrain_characters_spawn_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    float dbg_length = 0;
    if (disable_npcs || !character_spawn_rate_max) {
        return;
    }
    uint spawned_count = 0;
    if (npcs_spawned_frame != ecs_run_count) {
        npcs_spawned_frame = ecs_run_count;
        npcs_spawned_this_frame = 0;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NpcSpawnZoneDirty);
    zox_sys_in(NpcSpawnZone);
    zox_sys_in(Seed);
    zox_sys_in(VoxelNode);
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(RenderDistance);
    zox_sys_in(RenderDisabled);
    zox_sys_in(ChunkPosition);
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NpcSpawnZoneDirty, dirty);
        zox_sys_i(NpcSpawnZone, active);
        zox_sys_i(Seed, seed);
        zox_sys_i(VoxelNode, voctree);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(RenderDistance, render_distance);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(ChunkPosition, chunk_position);
        zox_sys_i(Position3D, positionf);
        zox_sys_i(BlockScale, cscale);
        // Only spawn if fully loaded
        if (!active->value || dirty->value) {
            continue;
        }
        // this frame's share spawned, the chunk spawns its npcs next frame
        if (npcs_spawned_this_frame >= npcs_spawned_per_frame_max) {
            break;
        }
        entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("Terrain missing on chunk3_terrain");
            continue;
        }
#endif
        // getters
        entity realm = zox_get_parent(world, terrain);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            zox_loge("Terrain missing on chunk3_terrain");
            continue;
        }
#endif
        // NOTE: If has No children or all air, we avoid
        if (!has_children_VoxelNode(voctree) || !voctree->value) {
            continue;
        }
        entity realm_characters[zox_realm_characters_max];
        const uint realm_characters_count = zox_get_links(
            world,
            realm,
            CharacterLinks,
            realm_characters,
            zox_realm_characters_max);
        if (!realm_characters_count) {
            continue;
        }
        entity chunk_above = neighbors->value[direction_up];
        const VoxelNode* voctree_above =
            zox_valid(chunk_above) ?
                zox_get(chunk_above, VoxelNode) :
                NULL;
        // calcs
        // its column's population on its region's map scales the rolls (fun/population.c), towns busy, the wild
        //  quiet, all of them until the map is made
        const entity tunk = zox_get_link(world, e, TunkLink);
        const int chunk_length = octree_size(depth->value);
        const int population = region_population_at(world, zox_valid(tunk) ? zox_get_link(world, tunk, RegionLink) : 0,
            (int2) { chunk_position->value.x * chunk_length, chunk_position->value.z * chunk_length }, chunk_length);
        byte character_spawn_rate = seed_range(seed->value, character_spawn_rate_min, character_spawn_rate_max);
        if (max_npcs) {
            character_spawn_rate = character_spawn_rate_max;
        } else {
            character_spawn_rate = npc_spawn_count(character_spawn_rate, population, seed->value);
        }
        // each type's weight here: villagers in towns, monsters in the wild (fun/population.c)
        const float town = population_town_share(population);
        float weights[zox_realm_characters_max];
        float weights_total = 0;
        for (uint k = 0; k < realm_characters_count; k++) {
            const entity type = realm_characters[k];
            weights[k] = npc_spawn_weight(zox_getv(type, SpawnChance), zox_has(type, Villager), zox_has(type, Monster), town);
            weights_total += weights[k];
        }
        if (weights_total <= 0) {
            continue;
        }
        // its npcs still alive (out walking), by home index: the same rolls are made, they aren't spawned again
        uint alive = 0;
        zox_each_link(world, e, ChunkCharacters, npc) {
            if (zox_valid(npc) && zox_has(npc, NpcHome)) {
                alive |= 1u << (zox_getv(npc, NpcHome) & 31);
            }
        }
        srand(seed->value);
        byte spawned = 0;
        for (byte j = 0; j < character_spawn_rate; j++) {
            // Find Position First
            byte3 in_chunk_position;
            if (!find_random_position_on_ground(voctree, voctree_above, depth->value, 32, &in_chunk_position)) {
                if (dbg_log) {
                    zox_log("Failed find Position for NPC at [%ix%ix%i]:%i", chunk_position->value.x, chunk_position->value.y, chunk_position->value.z, j);
                }
                break;
            }
            // a random character type, by its weight here
            float rolled = weights_total * (rand() / (RAND_MAX + 1.0f));
            entity meta = 0;
            for (uint k = 0; k < realm_characters_count && !meta; k++) {
                rolled -= weights[k];
                if (rolled < 0) {
                    meta = realm_characters[k];
                }
            }
            if (!meta) {
                meta = realm_characters[realm_characters_count - 1];   // float rounding at the very end
            }
            if (!meta) {
                zox_loge("failed to find a spawn character_meta");
                continue;
            }
            lint npc_seed = rand_range(0, 10000);
            float3 position = byte3_to_float3(in_chunk_position);
            float3_scale_p(&position, cscale->value);
            float3_add_float3_p(&position, positionf->value); // chunk
            float3_add_float3_p(&position, float3_single(cscale->value * 0.5f));
            float4 rotation = quaternion_from_euler((float3) { 0, (rand() % 361) * degreesToRadians, 0 });
            if (alive & (1u << (j & 31))) {
                continue;   // its rolls made (rand), it's still out there
            }
            entity e2 = spawn_character3_npc(
                world,
                meta,
                realm,
                terrain,
                npc_seed,
                render_distance->value,
                render_disabled->value,
                position,
                rotation,
                NULL);
            if (!e2) {
                zox_loge("Spawning NPC Failed");
                continue;
            }
            zox_add(e2, DisableMovement);
            zox_link(world, e, ChunkCharacters, e2);
            zox_setv(e2, NpcHome, j);
            zox_setv(e2, HomePosition, position);
            spawned++;
            npcs_spawned_this_frame++;
            if (dbg_length) {
                spawn_arrow3D(
                    world,
                    position,
                    (float3) {
                        0,
                        cscale->value * dbg_length,
                        0
                    },
                    0.1f,
                    6,
                    20);
            }
            if (dbg_log) {
                zox_log("Chunk %s Spawned NPC [%i / %i] at [%fx%fx%f]",
                    zox_getn(e),
                    spawned_count,
                    total_spawned_npcs_count,
                    position.x,
                    position.y,
                    position.z);
            }
            spawned_count++;
            total_spawned_npcs_count++;
        }
        zox_add(e, CharactersSpawned);
    }
    zox_sys_on_end();
} zoxd_system(terrain_characters_spawn_system);
