extern void on_spawned_character3_npc(ecs*, entity);
uint total_spawned_npcs_count = 0;

// we need to check if chunk has generated yet - is there a component for this?
zox_sys2(TerrainCharactersSpawnSystem) {
    byte dbg_log = 0;
    float dbg_length = 0;
    if (disable_npcs || !character_spawn_rate_max) {
        return;
    }
    uint spawned_count = 0;
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
    zox_sys_out(ChunkCharacters);
    zox_sys_out(CharactersSpawned);
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
        zox_sys_o(ChunkCharacters, characters);
        zox_sys_o(CharactersSpawned, spawned);
        // Only spawn if fully loaded
        if (!active->value || dirty->value) {
            continue;
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
        if (spawned->value) {
            // if (spawned->value) zox_log("- already_spawned [%i]", entities->length)
            continue;
        }
        // NOTE: If has No children or all air, we avoid
        if (!has_children_VoxelNode(voctree) || !voctree->value) {
            continue;
        }
        zox_geter(realm, CharacterLinks, realm_characters);
        zox_geter_value(realm, CharactersChanceMax, byte, max_chance);
        entity chunk_above = neighbors->value[direction_up];
        const VoxelNode* voctree_above = zox_valid(chunk_above) ? zox_gett(chunk_above, VoxelNode) : NULL;
        // calcs
        byte character_spawn_rate = seed_range(seed->value, character_spawn_rate_min, character_spawn_rate_max);
        srand(seed->value);
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
            for (byte k = 0; k < realm_characters->length; k++) {
                entity e2 = realm_characters->value[k];
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
            // entity model = zox_getv(meta, ModelLink);
            /*if (!model || !meta) {
                zox_loge("failed to find a spawn character_meta");
                continue;
            }
            // if model group
            if (zox_has(model, ModelLinks)) {
                zox_geter(model, ModelLinks, models);
                if (models->length) {
                    model = models->value[rand() % models->length];
                }
            }
            if (!zox_valid(model) || !zox_has(model, MaxRenderDepth)) {
                zox_loge("Model Invalid [%s]", zox_get_name(model));
                continue;
            }*/
            // byte max_depth = zox_valid(model) && zox_has(model, MaxRenderDepth) ? zox_getv(model, MaxRenderDepth) : 0;
            // byte render_depth = camera_distance_to_npc_render_depth(render_distance->value, max_depth);
            lint npc_seed = rand_range(0, 10000);
            float3 position = byte3_to_float3(in_chunk_position);
            float3_scale_p(&position, cscale->value);
            float3_add_float3_p(&position, positionf->value); // chunk
            float3_add_float3_p(&position, float3_single(cscale->value * 0.5f));
            float4 rotation = quaternion_from_euler((float3) { 0, (rand() % 361) * degreesToRadians, 0 });
            entity e2 = spawn_character3_npc(world, meta, realm, terrain, npc_seed, render_distance->value, render_disabled->value, position, rotation, NULL);
            if (!e2) {
                zox_loge("Spawning NPC Failed");
                continue;
            }
            characters->value[spawned->value] = e2;
            spawned->value++;
            // zox_set(e2, ChunkPosition, { chunk_position->value });
            // Link together
            // zox_set(e2, ChunkLink, { e });
            // add_to_ChunkEntities(entities, e2);
            if (disable_npc_movement) {
                zox_set(e2, DisableMovement, { 1 });
            }
            if (dbg_length) {
                spawn_arrow3D(world, position, (float3) { 0, cscale->value * dbg_length, 0 }, 0.1f, 6, 20);
            }
            if (dbg_log) {
                zox_log("Chunk %s Spawned NPC [%i / %i] at [%fx%fx%f]", zox_getn(e), spawned_count, total_spawned_npcs_count, position.x, position.y, position.z);
            }
            spawned_count++;
            total_spawned_npcs_count++;
        }
    }
} zox_sys_end(TerrainCharactersSpawnSystem);
