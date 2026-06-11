extern void on_spawned_character3_npc(ecs*, entity);
// we need to check if chunk has generated yet - is there a component for this?
zox_sys2(Characters3SpawnSystem) {
    float dbg_length = 0;
    if (disable_npcs || !character_spawn_rate_max) {
        return;
    }
    // TODO: Use Bounds to place into terrain
    // float3 bounds = (float3) { 0.22f, 0.44f, 0.22f };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CharacterSpawnZone);
    zox_sys_in(VoxelNode);
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(RenderDistance);
    zox_sys_in(RenderDisabled);
    zox_sys_in(ChunkPosition);
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    zox_sys_out(CharactersSpawned);
    zox_sys_out(ChunkEntities);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CharacterSpawnZone, active);
        zox_sys_i(VoxelNode, voctree);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(RenderDistance, render_distance);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(Position3D, positionf);
        zox_sys_i(BlockScale, cscale);
        zox_sys_o(CharactersSpawned, spawned);
        zox_sys_o(ChunkEntities, entities);
        // Only spawn if fully loaded
        if (!active->value) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            zox_loge("Terrain missing on chunk3_terrain");
            continue;
        }
        if (spawned->value) {
            // if (spawned->value) zox_log("- already_spawned [%i]", entities->length)
            continue;
        }
        // NOTE: If has No children or all air, we avoid
        if (!has_children_VoxelNode(voctree) || !voctree->value) {
            continue;
        }
        // getters
        zox_geter_value(terrain, RealmLink, entity, realm);
        if (!zox_valid(realm)) {
            continue;
        }
        zox_geter(realm, CharacterLinks, characters);
        zox_geter_value(realm, CharactersChanceMax, byte, max_chance);
        entity chunk_above = neighbors->value[direction_up];
        const VoxelNode* voctree_above = zox_valid(chunk_above) ? zox_gett(chunk_above, VoxelNode) : NULL;
        // calcs
        // byte character_spawn_rate = character_spawn_rate_min + rand() % (character_spawn_rate_max - character_spawn_rate_min + 1);
        byte character_spawn_rate = rand_range(character_spawn_rate_min, character_spawn_rate_max);
        for (byte j = 0; j < character_spawn_rate; j++) {
            // 1) Find a npc to place
            // find random from realm characters
            byte chance_current = 0;
            byte chance_rolled = rand() % max_chance;
            entity meta = 0;
            for (byte k = 0; k < characters->length; k++) {
                entity e2 = characters->value[k];
                zox_geter_value(e2, SpawnChance, byte, chance);
                chance_current += chance;
                if (chance_rolled <= chance_current) {
                    meta = e2;
                    break;
                }
            }
            if (!meta) {
                zox_log_error("failed to find a spawn character_meta");
                continue;
            }
            zox_geter_value_non_const(meta, ModelLink, entity, model);
            if (!model || !meta) {
                zox_log_error("failed to find a spawn character_meta");
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
                zox_log_error("Model Invalid [%s]", zox_get_name(model));
                continue;
            }
            zox_geter_value(model, MaxRenderDepth, byte, mdepth);
            byte character_depth = camera_distance_to_npc_render_depth(render_distance->value, mdepth);
            byte3 in_chunk_position;
            if (!find_random_position_on_ground(voctree, voctree_above, depth->value, 32, &in_chunk_position)) {
                // zox_loge("Failed find Position for NPC at [%ix%ix%i]:%i", cposition->value.x, cposition->value.y, cposition->value.z, j);
                break;
            }
            lint npc_seed = rand_range(0, 10000);
            float3 position = byte3_to_float3(in_chunk_position);
            float3_scale_p(&position, cscale->value);
            float3_add_float3_p(&position, positionf->value); // chunk
            float3_add_float3_p(&position, float3_single(cscale->value * 0.5f));
            float4 rotation = quaternion_from_euler((float3) { 0, (rand() % 361) * degreesToRadians, 0 });
            char* name = generate_name();
            entity e2 = spawn_character3_npc(world, meta, realm, terrain, npc_seed, model, character_depth, render_disabled->value, position, rotation, name);
            if (!zox_valid(e2)) {
                zox_loge("spawn_character3 failed");
                continue;
            }
            zox_set(e2, ChunkLink, { e });
            zox_set(e2, ChunkPosition, { cposition->value });
            add_to_ChunkEntities(entities, e2);
            if (disable_npc_movement) {
                zox_set(e2, DisableMovement, { 1 });
            }
            if (dbg_length) {
                spawn_arrow3D(world, position, (float3) { 0, cscale->value * dbg_length, 0 }, 0.1f, 6, 20);
            }
        }
        spawned->value = 1;
        // ever_spawned->value = 1;
    }
} zox_sys_end(Characters3SpawnSystem);
