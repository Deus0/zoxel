// -> for each neighbor, we have 6
//      -> If out of bounds:
//          ->wrap position, add to queue, we can get the light and voxel though to check if meets conditions
//      -> otherwise
//          -> check if air
//              -> if air we check light
//                  -> if light less than current (decayed light) we set it and propogate there
//          -> if not air, i.e. solid, of course light doesnt go through solid you twat
// NOTE: Stop setting neighbor data, thats const, you are literally corrupting memory
static inline byte flood_light(const VoxelNode* voxels, LightNode* lights, const VoxelNode* n_root_vnodes[6], const LightNode* n_root_lnodes[6], LightQueue* n_queues[6], byte depth, byte3 positionl, byte light, byte distance, byte min_light, byte air_decay, const byte* solidity) {
    byte dirty = 0;
    if (!voxels || !lights || distance == 0 || light <= min_light) {
        return dirty;
    }
    byte length = octree_size(depth) - 1;
    // byte length = (byte)((1u << depth) - 1u);
    for (byte dir = 0; dir < 6; dir++) {
        byte3 pos = positionl;
        byte oob = 0;
        // move with wrap into neighbor-local coords
        switch (dir) {
            case 0: if (pos.x > 0) pos.x--; else { pos.x = length; oob = 1; } break;
            case 1: if (pos.x < length) pos.x++; else { pos.x = 0; oob = 1; } break;
            case 2: if (pos.y > 0) pos.y--; else { pos.y = length; oob = 1; } break;
            case 3: if (pos.y < length) pos.y++; else { pos.y = 0;    oob = 1; } break;
            case 4: if (pos.z > 0) pos.z--; else { pos.z = length; oob = 1; } break;
            case 5: if (pos.z < length) pos.z++; else { pos.z = 0;    oob = 1; } break;
        }
        if (oob) {
            // --- Cross-chunk: READ neighbor if present, never write it. Queue only. ---
            const VoxelNode* neighbor_voxels  = n_root_vnodes[dir];
            const LightNode* neighbor_lights = n_root_lnodes[dir];
            // solid → no propagation
            if (neighbor_voxels) {
                byte neighbor_voxel = getv_VoxelNode(neighbor_voxels, depth, pos);
                if (neighbor_voxel && solidity[neighbor_voxel - 1]) {
                    continue; // solid: hard stop
                }
            }
            // air decay
            byte decayed_light = (light > air_decay) ? (byte)(light - air_decay) : 0;
            if (decayed_light <= min_light) {
                continue;
            }
            // only queue if it improves neighbor
            byte current_light = neighbor_lights ? getv_LightNode(neighbor_lights, depth, pos) : 0;
            if (decayed_light <= current_light) {
                continue;
            }
            LightQueue* nqueue = n_queues[dir];
            // Add to neighbor queue
            if (nqueue) {
                if (locks_enabled) spin_lock(&nqueue->lock);
                a_LightQueue(nqueue,
                    (LightUpdate) {
                        .light = decayed_light,
                        .pos = pos,
                        .depth = depth,
                        .distance = distance - 1
                    });
                if (locks_enabled) spin_unlock(&nqueue->lock);
            }
            continue;
        }
        // --- In-chunk: READ voxel, WRITE light in our own chunk only. ---
        byte voxel = getv_VoxelNode(voxels, depth, pos);
        if (voxel && solidity[voxel - 1]) {
            continue;
        }
        byte decayed_light = (light > air_decay) ? (byte) (light - air_decay) : min_light;
        byte current_light = getv_LightNode(lights, depth, pos);
        if (decayed_light <= current_light) {
            continue;
        }
        zox_logv("     + Light Flooded [%ix%ix%i] l[%i] dist[%i]", pos.x, pos.y, pos.z, decayed_light, distance);
        set_LightNode(lights, depth, pos, decayed_light);
        dirty = 1;
        flood_light(voxels, lights, n_root_vnodes, n_root_lnodes, n_queues, depth, pos, decayed_light, distance - 1, min_light, air_decay, solidity);
    }
    return dirty;
}

zox_sys2(LightFloodSystem) {
    byte dbg_log = 0;
    byte max_process = !zox_disable_process_skips ? 1 : 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(LightQueue);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    entity realm = 0;
    byte solidity[255];
    memset(solidity, 1, 255);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BlockManagerLink, manager);
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightQueue, light_queue);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(LightNodeDirty, light_node_dirty);
        if (!light_queue->count) {
            continue;
        }
        if (zox_has(e, GenerateChunk)) {
            continue;
        }
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            continue;
        }
        // NOTE: Check Blocks Caches
        if (realm != manager->value) {
            realm = manager->value;
            zox_geter(realm, BlockLinks, blocks);
            for (int j = 0; j < blocks->length; j++) {
                entity block = blocks->value[j];
                solidity[j] = zox_valid(block) && zox_has(block, BlockLightPass) ? !zox_getv(block, BlockLightPass) : 1;
            }
        }
        const VoxelNode* n_root_vnodes[6];
        fetch_neightbor_voxel_nodes(world, neighbors, n_root_vnodes);
        const LightNode* n_root_lnodes[6];
        fetch_neightbor_light_nodes(world, neighbors, n_root_lnodes);
        LightQueue* n_light_queues[6];
        fetch_neightbor_propogation_queues(world, neighbors, n_light_queues);
        byte dirty = 0;
        byte voxel_octree_depth = zox_getv(e, NodeDepth);
        while (light_queue->count) {
            LightUpdate update = remove_LightQueue(light_queue);
            if (zox_disable_flood_fill) {
                continue;
            }
            // TODO: Handle Depth Flood Light Propogation
            if (update.depth != voxel_octree_depth) {
                continue;
            }
            byte3 pos = update.pos;
            short length = octree_size(update.depth);
            if (pos.x >= length || pos.z >= length || pos.y >= length) {
                //  zox_log_error("[r_LightQueue] position oob [%ix%ix%i]", pos.x, pos.y, pos.z);
                continue;
            }
            byte voxel = getv_VoxelNode(root_vnode, update.depth, update.pos);
            if (voxel && solidity[voxel - 1]) {
                if (dbg_log) {
                    zox_log("[%s]: Light Flood Canceled at [%ix%ix%i] l[%i] q [%i]", zox_get_name(e), update.pos.x, update.pos.y, update.pos.z, update.light,  light_queue->count);
                }
                continue;
            }
            byte current_light = getv_LightNode(root_lnode, update.depth, update.pos);
            byte spread_light = update.light;
            if (dbg_log) {
                zox_log("[%s]: [%s] ^ Light Flooding at [%ix%ix%i] l[%i] spread [%i] q [%i]", current_light > spread_light ? "Skip" : "Run", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, current_light, spread_light, light_queue->count);
            }
            if (current_light > spread_light) {
                spread_light = current_light;
            } else if (current_light < spread_light) {
                set_LightNode(root_lnode, update.depth, update.pos, spread_light);
                dirty = 1;
            }
            if (flood_light(root_vnode, root_lnode, n_root_vnodes, n_root_lnodes, n_light_queues, update.depth, update.pos, spread_light, update.distance, darklight, light_air_decay, solidity)) {
                dirty = 1;
            }
        }
        if (dirty) {
            light_node_dirty->value = zox_dirty_trigger;
        }
        zox_sys_increment();
    }
} zox_sys_end(LightFloodSystem);


// Move darkbeam and dark propogation here
static inline byte dark_flood_light(
    const VoxelNode* root_vnode,          // (READ)
    LightNode* root_lnode,                // (WRITE)
    const VoxelNode* n_root_vnodes[6],    // (READ-ONLY)
    const LightNode* n_root_lnodes[6],    // (READ-ONLY)
    LightQueue* n_light_queues[6],
    LightQueue* light_queue,
    DarkQueue* n_dark_queues[6],
    DarkQueue* dark_queue,
    byte depth,
    byte3 positionl,
    byte old_light,                       // the light we’re extinguishing
    byte distance,
    byte min_light,
    byte air_decay,
    const byte* solidity
) {
    byte dirty = 0;
    if (!root_vnode || !root_lnode || distance == 0 || old_light <= min_light) {
        return dirty;
    }
    const byte SIZE = (byte)((1u << depth) - 1u);
    for (byte dir = 0; dir < 6; dir++) {
        byte3 pos = positionl;
        byte oob = 0;

        // move with wrap
        switch (dir) {
            case 0: if (pos.x > 0) pos.x--; else { pos.x = SIZE; oob = 1; } break;
            case 1: if (pos.x < SIZE) pos.x++; else { pos.x = 0;    oob = 1; } break;
            case 2: if (pos.y > 0) pos.y--; else { pos.y = SIZE; oob = 1; } break;
            case 3: if (pos.y < SIZE) pos.y++; else { pos.y = 0;    oob = 1; } break;
            case 4: if (pos.z > 0) pos.z--; else { pos.z = SIZE; oob = 1; } break;
            case 5: if (pos.z < SIZE) pos.z++; else { pos.z = 0;    oob = 1; } break;
        }
        if (oob) {
            // --- neighbor chunk ---
            const VoxelNode* n_root_vnode   = n_root_vnodes[dir];
            const LightNode* n_root_lnode = n_root_lnodes[dir];
            if (!n_root_lnode) {
                continue;
            }
            if (n_root_vnode) {
                byte voxel = getv_VoxelNode(n_root_vnode, depth, pos);
                if (voxel && solidity[voxel - 1]) {
                    continue; // solid wall
                }
            }
            byte ncurrent_light = getv_LightNode(n_root_lnode, depth, pos);
            if (ncurrent_light <= min_light) {
                continue;   // omg this wasn't here
            }
            // wait we never set light for dark flood on neighbors
            // enqueue only
            byte is_darkness = (ncurrent_light < old_light);
            // zox_log("dark flood at border [%ix%ix%i] dist [%i] d?[%i]", pos.x, pos.y, pos.z, distance, is_darkness);
            if (is_darkness) {
                DarkQueue* nqueue = n_dark_queues[dir];
                if (nqueue) {
                    if (locks_enabled) spin_lock(&nqueue->lock);
                    a_DarkQueue(nqueue, (DarkUpdate) {
                        .type  = 0,
                        .light = old_light,
                        .distance = distance - 1,
                        .pos   = pos,
                        .depth = depth
                    });
                    if (locks_enabled) spin_unlock(&nqueue->lock);
                }
            } else {
                LightQueue* nqueue = n_light_queues[dir];
                if (nqueue) {
                    if (locks_enabled) spin_lock(&nqueue->lock);
                    a_LightQueue(nqueue, (LightUpdate) {
                        // .type  = zox_light_type_flood,
                        .light = ncurrent_light,
                        .distance = light_propogation_distance,
                        .pos   = pos,
                        .depth = depth
                    });
                    if (locks_enabled) spin_unlock(&nqueue->lock);
                }
            }
            continue;
        }
        // --- in-chunk ---
        byte voxel = getv_VoxelNode(root_vnode, depth, pos);
        if (voxel) {
            continue; // solid → stop
        }
        byte current_light = getv_LightNode(root_lnode, depth, pos);
        if (current_light <= min_light) {
            continue;   // omg this wasn't here
        }
        if (current_light < old_light) {
            zox_logv("     - Light Banished at [%ix%ix%i] l[%i] dist[%i]", pos.x, pos.y, pos.z, old_light, distance);
            // extinguish here and continue removing
            set_LightNode(root_lnode, depth, pos, min_light);
            dirty = 1;
            dark_flood_light(
                root_vnode,
                root_lnode,
                n_root_vnodes,
                n_root_lnodes,
                n_light_queues,
                light_queue,
                n_dark_queues,
                dark_queue,
                depth,
                pos,
                old_light,
                distance - 1,
                min_light,
                air_decay,
                solidity
            );
        } else {
            if (light_queue) {
                byte decayed_light = (current_light > light_air_decay) ? (byte) (current_light - light_air_decay) : darklight;
                zox_logv("     + Dark Flood Light Source [%ix%ix%i] new [%i] old [%i] decayed [%i]", pos.x, pos.y, pos.z, current_light, old_light, decayed_light);
                if (locks_enabled) spin_lock(&light_queue->lock);
                a_LightQueue(light_queue, (LightUpdate) {
                    // .type  = zox_light_type_flood,
                    .light = decayed_light,
                    .distance = light_propogation_distance,
                    .pos   = pos,
                    .depth = depth
                });
                if (locks_enabled) spin_unlock(&light_queue->lock);
            }
        }
    }
    return dirty;
}


byte dark_sunbeam(DarkQueue* queued, const VoxelNode* root_vnode, LightNode* root_lnode, const VoxelNode* n_root_vnodes[6], const LightNode* n_root_lnodes[6], LightQueue* n_light_queues[6], LightQueue* light_queue, DarkQueue* n_dark_queues[6], DarkQueue* dark_queue, byte depth, byte3 pos, byte sunlight, byte min_light, byte air_decay, byte type, const byte* solidity) {
    byte dirty = 0;
    short length = octree_size(depth);
    if (pos.y > length) {
        return dirty;
    }
    byte max_y = pos.y;
    byte flood_start = 0;
    byte flood_end = 0;
    byte beam_stopped = 0;
    for (byte y = 0; y < max_y; y++) {
        pos.y = max_y - y;
        if (type == zox_light_type_beam_start && y == 0) {
            flood_end = pos.y;
            continue;
        }
        byte voxel = getv_VoxelNode(root_vnode, depth, pos);
        if (voxel && solidity[voxel - 1]) {
            // zox_log("sunbeam stopped v at [%ix%ix%i] v[%i]",  pos.x, pos.y, pos.z, voxel);
            beam_stopped = 1;
            break;
        }
        // extinguish sunlight here
        byte current_light = getv_LightNode(root_lnode, depth, pos);
        if (current_light != sunlight) {
            // zox_log("sunbeam stopped l at %i", pos.y);
            beam_stopped = 1;
            break;
        }
        zox_logv("     - Light Banished at [%ix%ix%i] l[%i]", pos.x, pos.y, pos.z, current_light);
        set_LightNode(root_lnode, depth, pos, min_light);
        dirty = 1;
        if (y == 0) {
            flood_end = pos.y;
        }
        flood_start = pos.y;
    }
    // flood after, otherwise is refills with sunlight
    // QUESTION: Dark DarkFlood use neighbor.. Probably?!?!
    //      can we delay these until its gone? test sunlight between chunks, sunlight comes through chunk + check it dissapears
    zox_logv(" * dark beam y: [%i] to [%i]", flood_start, flood_end);
    for (byte y = flood_start; y <= flood_end; y++) {
        pos.y = y;
        zox_logv(" - Dark Beam Spreads [%ix%ix%i]", pos.x,  pos.y, pos.z);
        if (dark_flood_light(root_vnode, root_lnode, n_root_vnodes, n_root_lnodes, n_light_queues, light_queue, n_dark_queues, dark_queue, depth, pos, sunlight - air_decay + 1, darklight_propogation_distance, min_light, air_decay, solidity)) {
            dirty = 1;
        }
    }
    // pass downward into chunk below since we survived until the end
    if (!beam_stopped && queued) {
        zox_logv(" - Dark Beam Continues [%ix%ix%i]", pos.x, length, pos.z);
        if (locks_enabled) spin_lock(&queued->lock);
        a_DarkQueue(queued,
            (DarkUpdate) {
                .type = zox_light_type_beam,
                .pos = (byte3) {
                    pos.x,
                    length,   // y = bottom edge
                    pos.z
                },
                .depth = depth,
                .light = sunlight
            });
        if (locks_enabled) spin_unlock(&queued->lock);
    }
    return dirty;
}

// Queued side updates for propogation
// NOTE: We need to account for depth differences
zox_sys2(DarkLightSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(DarkQueue);
    zox_sys_out(LightQueue);
    zox_sys_out(LightNodeDirty);
    entity realm = 0;
    byte solidity[255];
    memset(solidity, 1, 255);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(BlockManagerLink, manager);
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightNodeDepth, depthl);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(DarkQueue, dark_queue);
        zox_sys_o(LightQueue, light_queue);
        zox_sys_o(LightNodeDirty, light_node_dirty);
        if (!dark_queue->count) {
            continue;
        }
        // NOTE: Check Blocks Caches
        if (realm != manager->value) {
            realm = manager->value;
            zox_geter(realm, BlockLinks, blocks);
            for (int j = 0; j < blocks->length; j++) {
                entity block = blocks->value[j];
                solidity[j] = zox_valid(block) && zox_has(block, BlockLightPass) ? !zox_getv(block, BlockLightPass) : 1;
            }
        }
        const VoxelNode* nnodesv[6];
        fetch_neightbor_voxel_nodes(world, neighbors, nnodesv);
        const LightNode* nnodesl[6];
        fetch_neightbor_light_nodes(world, neighbors, nnodesl);
        LightQueue* n_light_queues[6];
        fetch_neightbor_propogation_queues(world, neighbors, n_light_queues);
        DarkQueue* n_dark_queues[6];
        fetch_neightbor_dark_queues(world, neighbors, n_dark_queues);
        byte dirty = 0;
        entity chunkd = neighbors->value[direction_down];
        DarkQueue* dark_queued = zox_valid(chunkd) ? zox_gett_mut(chunkd, DarkQueue) : NULL;
        while (dark_queue->count) {
            DarkUpdate update = remove_DarkQueue(dark_queue);
            if (depthl->value < update.depth) {
                depthl->value = update.depth;
            }
            // if (depthl->value != update.depth) continue;   // for now
            if (update.type == zox_light_type_flood) {
                zox_logv("[%s] Begin Dark Flooding [%ix%ix%i] l[%i] distance [%i] q [%i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, update.light, update.distance, dark_queue->count);
                byte current_light = getv_LightNode(root_lnode, depthl->value, update.pos);
                if (current_light > darklight) {
                    set_LightNode(root_lnode, depthl->value, update.pos, darklight);
                    dirty = 1;
                }
                if (dark_flood_light(root_vnode, root_lnode, nnodesv, nnodesl, n_light_queues, light_queue, n_dark_queues, dark_queue, depthl->value, update.pos, update.light, update.distance, darklight, light_air_decay, solidity)) {
                    dirty = 1;
                }
            } else if (update.type == zox_light_type_beam_start || update.type == zox_light_type_beam) {
                zox_logv("[%s] Begin Darkbeam [%ix%ix%i] l[%i] q [%i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, update.light, dark_queue->count);
                if (dark_sunbeam(dark_queued, root_vnode, root_lnode, nnodesv, nnodesl, n_light_queues, light_queue, n_dark_queues, dark_queue, depthl->value, update.pos, sunlight, darklight, light_air_decay, update.type, solidity)) {
                    dirty = 1;
                }
            }
        }
        zox_mut_end(chunkd, DarkQueue);
        if (dirty) {
            light_node_dirty->value = zox_dirty_trigger;
            /*zox_sys_world();
            zox_sys_e();
            zox_log("DarkSystem %s - %i", zox_get_name(e), count);*/
        }
    }
} zox_sys_end(DarkLightSystem);

