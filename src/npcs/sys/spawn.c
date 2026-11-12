extern void on_spawned_character3_npc(ecs*, entity);
uint total_spawned_npcs_count = 0;
// npcs a frame at most: each brings its children (stats, body, skeleton, label), chunks streaming in together
//  spawned many in one frame (lag spikes), the rest of the chunks spawn theirs next frame
#define npcs_spawned_per_frame_max 4
uint npcs_spawned_frame = UINT32_MAX;
uint npcs_spawned_this_frame = 0;

// we need to check if chunk has generated yet - is there a component for this?
void TerrainCharactersSpawnSystem(iter* it) {
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
        zox_geter_value(realm, CharactersChanceMax, byte, max_chance);
        entity chunk_above = neighbors->value[direction_up];
        const VoxelNode* voctree_above =
            zox_valid(chunk_above) ?
                zox_get(chunk_above, VoxelNode) :
                NULL;
        // calcs
        byte character_spawn_rate = seed_range(seed->value, character_spawn_rate_min, character_spawn_rate_max);
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
            // Use a random character type
            byte chance_current = 0;
            byte chance_rolled = rand() % max_chance;
            entity meta = 0;
            for (uint k = 0; k < realm_characters_count; k++) {
                entity e2 = realm_characters[k];
                zox_geter_value(e2, SpawnChance, byte, chance);
                chance_current += chance;
                if (chance_rolled <= chance_current) {
                    meta = e2;
                    break;
                }
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
} zoxd_system(TerrainCharactersSpawnSystem);
