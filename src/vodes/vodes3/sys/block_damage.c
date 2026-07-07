// NOTE: This is for chunks handling incoming damages
// TODO: Refactor Health to remove from chunk instead of here
zox_sys2(BlockDamageQueueSystem) {
    byte dbg_log = 0;
    float overlay_buffer = 1.04f;
    color popup_color = (color) { 255, 0, 0, 255 };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkPosition);
    zox_sys_in(VoxelNode);
    zox_sys_in(NodeDepth);
    zox_sys_in(BlockManagerLink);
    zox_sys_out(BlockDamageQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e()
        zox_sys_i(ChunkPosition, chunk_position);
        zox_sys_i(VoxelNode, voxel_octree);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(BlockManagerLink, manager);
        zox_sys_o(BlockDamageQueue, queue);
        if (!queue->count) {
            continue;
        }
        if (!zox_valid(manager->value)) {
            continue;
        }
        zox_geter(manager->value, BlockLinks, blocks);
        while (queue->count) {
            BlockDamageUpdate update = remove_BlockDamageQueue(queue);
            byte index = getv_VoxelNode(voxel_octree, update.position, depth->value);
            if (!index || index - 1 >= blocks->length) {
                continue;
            }
            entity meta = blocks->value[index - 1];
            // TODO: Check the depth matches MaxDepth
            // TODO: Grab meta again here using lookup table, reduce cache
            if (zox_has(meta, BlockInvinsible)) {
                // cannot destroy voxel sound
                spawn_sound_generated(world, prefab_sound_generated,  instrument_violin, note_frequencies[42 + rand() % 6], 0.26, 1.4f * get_volume_sfx());
                continue;
            }
            // effect our terrain here
            byte3 position = update.position;
            entity chunk = e;
            // First check Vode:
            zox_mut_begin(chunk, VoxelNode, root);
            VoxelNode* leaf = open_at_VoxelNode(root, depth->value, position, 0); //  getm
            // TODO: Subdivide Octree if not lowest level! we need it at lowest level for destruction
            if (!leaf) {
                zox_loge("Leaf is null in melee system");
                continue;
            }
            byte linked = is_linked_VoxelNode(leaf);
            entity world_block = 0;
            if (linked) {
                world_block = get_entity_VoxelNode(leaf);
            }
            // NOTE: Calculate the positionf for popup and overlays
            entity terrain = zox_get_parent(world, e);
            byte terrain_depth = zox_getv(terrain, NodeDepth);
            float terrain_block_scale = zox_getv(terrain, BlockScale);
            float3 positionf = local_block_position_to_real_position(update.position, chunk_position->value, terrain_depth, terrain_block_scale);
            // TODO: Refactor this damage into a damage system against blocks
            byte did_destroy_block = 0;
            // Create new block health
            float block_health;
            float start_health = 0;
            if (!zox_valid(world_block) || !zox_has(world_block, StatValue)) {
                float2 health_start_range = zox_has(meta, BlockHealth) ? zox_getv(meta, BlockHealth) : (float2) { 1, 1 };
                start_health = randf_range(health_start_range.x, health_start_range.y);
                block_health = start_health;
                // TODO: Add Destruction Overlay for Voxes
            } else {
                block_health = zox_getv(world_block, StatValue);
            }
            block_health -= update.damage;
            if (block_health <= 0) {
                // Only dispose block if had to spawn health entity
                if (zox_valid(world_block)) {
                    if (dbg_log) {
                        zox_log("Disposing of Block [%s]", zox_get_name(world_block));
                    }
                    // Detatch overlay first and fadeout destroy
                    entity overlay = zox_get_child_by_id(world, world_block, zox_id(Cube));
                    if (zox_valid(overlay)) {
                        zox_set_parent(world, overlay, 0);
                        zox_set(overlay, AnimationStart, { zox_current_time });
                        zox_set(overlay, FadeOutTime, { 1 });
                        zox_set(overlay, DestroyInTime, { 1 });
                    } else {
                        zox_loge("Overlay was missing from world block [%s]", zox_get_name(world_block));
                    }
                    // Now destroy our health block
                    zox_delete(world_block);
                    dispose_node_link_VoxelNode(leaf);
                    zox_mut_end(chunk, VoxelNode);
                } else {
                    if (dbg_log) {
                        zox_log("Chunk has no Block for [%ix%ix%i]", update.position.x, update.position.y, update.position.z);
                    }
                }
                // finally remove from chunk
                zox_muter(chunk, VoxelNodeQueue, queue);
                a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = 0, .pos = position });
                did_destroy_block = 1;
            } else {
                if (!zox_valid(world_block)) {
                    // NOTE: Here we spawn Block Health
                    // TODO: Spawn a simple cube here as child
                    // create health entity
                    // TODO: Get health off meta or use prefab
                    world_block = zox_ins(prefab_block_health);
                    zox_set(world_block, StatValue, { block_health });
                    zox_set(world_block, StatValueMax, { start_health });
                    // world_block = zox_new();
                    // zox_set_unique_name(world_block, "block_health");
                    entity overlay = spawn_cube(world, prefab_cube, positionf, terrain_block_scale * overlay_buffer);
                    zox_set_parent(world, overlay, world_block);
                    // TODO: Get this to work
                    // zox_set(overlay, Alpha, { 0.3f });
                    link_node_VoxelNode(leaf, world_block);
                    zox_mut_end(chunk, VoxelNode);
                    if (dbg_log) {
                        zox_log("- New Block Health [%s]:%f", zox_get_name(world_block), block_health);
                    }
                } else if (!zox_has(world_block, StatValue)) {
                    zox_set(world_block, StatValue, { block_health });
                    if (dbg_log) {
                        zox_log("- Grass Block Health [%s]:%f", zox_get_name(world_block), block_health);
                    }
                } else {
                    zox_muter(world_block, StatValue, health);
                    health->value = block_health;
                    if (dbg_log) {
                        zox_log("- Block Health [%s]:%f", zox_get_name(world_block), block_health);
                    }
                }
            }
            // add health to block child
            // destroy voxel sound
            if (did_destroy_block) {
                spawn_sound_generated(world, prefab_sound_generated, instrument_piano, note_frequencies[rand_range(28, 36)], 0.7, 1.4f * get_volume_sfx());
            } else {
                spawn_sound_generated(world, prefab_sound_generated, instrument_piano, note_frequencies[rand_range(20, 26)], 0.4, 1.1f * get_volume_sfx());
            }
            // hit block popup
            // float3 positionf = raycast->positionf;
            float range_xz = terrain_block_scale / 4.0f;
            positionf.x += randf_range(-range_xz, range_xz);
            positionf.z += randf_range(-range_xz, range_xz);
            positionf.y += randf_range(terrain_block_scale / 2.0f, terrain_block_scale);
            char popup_text[64];
            sprintf(popup_text, "%i", (int) floor(update.damage));
            spawn_popup3_easy(world, popup_text, popup_color, positionf, 2.5f, randf_range(4, 8));
            if (dbg_log) {
                // zox_log("User [%s] hit block at []", zox_get_name(user));
            }
        }
    }
} zox_sys_end(BlockDamageQueueSystem);
