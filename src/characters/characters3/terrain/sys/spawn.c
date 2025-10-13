extern void on_spawned_character3_npc(ecs*, const entity);
// we need to check if chunk has generated yet - is there a component for this?

void Characters3SpawnSystem(iter *it) {
    if (disable_npcs || !character_spawn_rate_max) {
        return;
    }
    zox_ts_begin(npc_spawns);
    // todo: dynamically check bounds
    const float3 bounds = (float3) { 0.22f, 0.44f, 0.22f };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(VoxelNode);
    zox_sys_in(NodeDepth);
    zox_sys_in(VoxelNodeLoaded);
    zox_sys_in(ChunkPosition);
    zox_sys_in(RenderDistance);
    zox_sys_in(RenderDisabled);
    zox_sys_in(VoxLink);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(CharactersSpawned);
    zox_sys_out(CharactersEverSpawned);
    zox_sys_out(ChunkEntities);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDistanceDirty, state);
        zox_sys_i(VoxelNode, voxel_node);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(VoxelNodeLoaded, loaded);
        zox_sys_i(RenderDistance, render_distance);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(CharactersSpawned, spawned);
        zox_sys_o(CharactersEverSpawned, ever_spawned);
        zox_sys_o(ChunkEntities, entities);

        const byte is_in_spawn_range = render_distance->value <= terrain_lod_near;
        const byte is_first_spawn = is_in_spawn_range && !ever_spawned->value;

        // if already spawned, skip spawning, only update LODs
        // if basically all air, no need to spawn
        if (!has_children_VoxelNode(voxel_node) && !voxel_node->value) {
            continue;
        }

        if (!is_first_spawn && state->value != zox_dirty_active) {
            continue;
        }

        if (spawned->value || !is_in_spawn_range) {
            // if (spawned->value) zox_log("- already_spawned [%i]", entities->length)
            // if (!is_in_spawn_range) zox_log("- out of range")
            continue;
        }

        // valid check for spawning
        if (!depth->value) {
            continue;
        }
        if (!loaded->value) {
            continue;
        }

        // getters
        zox_geter_value(terrain->value, RealmLink, entity, realm);
        zox_geter_value(terrain->value, BlockScale, float, terrain_scale);
        zox_geter(realm, CharacterLinks, characters);
        zox_geter_value(realm, CharactersChanceMax, byte, max_chance);
        const entity chunk_above = neighbors->value[direction_up];
        const VoxelNode* voxel_node_above = zox_valid(chunk_above) ? zox_gett(chunk_above, VoxelNode) : NULL;

        // calcs
        const int chunk_length = powers_of_two[depth->value];
        const int3 chunk_dimensions = int3_single(chunk_length);
        int3 chunk_voxel_position = get_chunk_positionv(cposition->value, chunk_dimensions);
        byte found_position = 0;
        float3 position;

        const byte character_spawn_rate = character_spawn_rate_min + rand() % (character_spawn_rate_max - character_spawn_rate_min + 1);
        for (byte j = 0; j < character_spawn_rate; j++) {

            // 1) Find a npc to place
            // find random from realm characters
            byte chance_current = 0;
            byte chance_rolled = rand() % max_chance;
            entity meta = 0;
            for (byte k = 0; k < characters->length; k++) {
                entity e2 = characters->value[k];
                zox_geter_value(e2, SpawnChance, byte, chance)
                chance_current += chance;
                if (chance_rolled <= chance_current) {
                    meta = e2;
                    break;
                }
            }
            if (!meta) {
                zox_log_error("failed to find a spawn character_meta")
                continue;
            }

            zox_geter_value_non_const(meta, ModelLink, entity, model)
            zox_geter_value(meta, Character3PrefabLink, entity, prefab_character)
            if (!model || !prefab_character) {
                zox_log_error("failed to find a spawn character_meta")
                continue;
            }

            // if model group
            if (zox_has(model, ModelLinks)) {
                zox_geter(model, ModelLinks, models)
                if (models->length) {
                    model = models->value[rand() % models->length];
                }
            }
            if (!zox_valid(model) || !zox_has(model, MaxRenderDepth)) {
                zox_log_error("Model Invalid [%s]", zox_get_name(model));
                continue;
            }

            zox_geter_value(model, MaxRenderDepth, byte, max_render_depth);
            const byte render_depth = camera_distance_to_npc_render_depth(render_distance->value, max_render_depth);

            // 2) find a place for our new npc
            // sometimes cannot find a position
            // many spawn checks
            if (!found_position) {
                if (disable_npc_positioner) {
                    found_position = 1;
                }
                byte3 local_position;
                for (byte k = 0; k < chunk_length; k++) {
                    local_position = find_random_position_on_ground(
                        voxel_node,
                        voxel_node_above,
                        depth->value
                    );
                    if (!byte3_equals(byte3_full, local_position)) {
                        break;
                    }/* else {
                        zox_log_error("pos out of bounds [%ix%ix%i]", local_position.x, local_position.y, local_position.z)
                    }*/
                }
                if (byte3_equals(byte3_full, local_position)) {
                    zox_log_spawning("! failed to spawn npc")
                    continue;
                }
                position = local_to_real_position_character(
                    local_position,
                    chunk_voxel_position,
                    bounds,
                    terrain_scale // 1
                );
            }
            float4 rotation = quaternion_from_euler( (float3) { 0, (rand() % 361) * degreesToRadians, 0 });

            // 3) Finally we spawn and link
            const spawn_character3D_data spawn_data = {
                .prefab = prefab_character,
                .position = position,
                .rotation = rotation,
                .meta = meta,
                .model = model,
                .scale = vox_model_scale,
                .render_depth = render_depth,
                .render_disabled = render_disabled->value,
                .realm = realm,
                .terrain = terrain->value,
                .terrain_chunk = e,
                .chunk_position = cposition->value,
            };

            if (only_single_npc && zox_valid(single_npc)) {
                spawned->value = 1;
                ever_spawned->value = 1;
                continue;
            }

            const entity character = spawn_character3(world, spawn_data);

            if (only_single_npc) {
                single_npc = character;
            }

            if (character) {
                on_spawned_character3_npc(world, character);
                add_to_ChunkEntities(entities, character);

                zox_logv("+ [%s] npc at [%fx%fx%f] [%i of %i] => %lu", zox_get_name(meta), position.x, position.y, position.z, (j + 1), (character_spawn_rate), character);
                zox_stats_characters++;

            } else {
                zox_logw("! [%s] npc at [%fx%fx%f] [%i of %i]", zox_get_name(meta), position.x, position.y, position.z, (j + 1), (character_spawn_rate));
            }

        }

        if (entities->length) {
            zox_logv("=> characters spawned [%i] at c[%ix%ix%i] v[%ix%ix%i]", entities->length, cposition->value.x, cposition->value.y, cposition->value.z, chunk_voxel_position.x, chunk_voxel_position.y, chunk_voxel_position.z);
        }

        spawned->value = 1;
        ever_spawned->value = 1;
    }

    zox_ts_end(npc_spawns, 3, zox_profile_system_npc_spawns);

} zoxd_system2(Characters3SpawnSystem);