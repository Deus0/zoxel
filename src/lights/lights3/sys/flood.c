// -> for each neighbor, we have 6
//      -> If out of bounds:
//          ->wrap position, add to queue, we can get the light and voxel though to check if meets conditions
//      -> otherwise
//          -> check if air
//              -> if air we check light
//                  -> if light less than current (decayed light) we set it and propogate there
//          -> if not air, i.e. solid, of course light doesnt go through solid you twat
// NOTE: Stop setting neighbor data, thats const, you are literally corrupting memory
static inline byte flood_light(const VoxelNode* root_vnode, LightNode* root_lnode, const VoxelNode* n_root_vnodes[6], const LightNode* n_root_lnodes[6], LightQueue* n_queues[6], byte depth, byte3 positionl, byte light, byte distance, byte min_light, byte air_decay, const byte* solidity) {
    byte dirty = 0;
    if (!root_vnode || !root_lnode || distance == 0 || light <= min_light) {
        return dirty;
    }
    byte length = (byte)((1u << depth) - 1u);
    for (byte dir = 0; dir < 6; dir++) {
        byte3 pos = positionl;
        byte oob = 0;
        // move with wrap into neighbor-local coords
        switch (dir) {
            case 0: if (pos.x > 0) pos.x--; else { pos.x = length; oob = 1; } break;
            case 1: if (pos.x < length) pos.x++; else { pos.x = 0;    oob = 1; } break;
            case 2: if (pos.y > 0) pos.y--; else { pos.y = length; oob = 1; } break;
            case 3: if (pos.y < length) pos.y++; else { pos.y = 0;    oob = 1; } break;
            case 4: if (pos.z > 0) pos.z--; else { pos.z = length; oob = 1; } break;
            case 5: if (pos.z < length) pos.z++; else { pos.z = 0;    oob = 1; } break;
        }
        if (oob) {
            // --- Cross-chunk: READ neighbor if present, never write it. Queue only. ---
            const VoxelNode* nvox_root  = n_root_vnodes[dir];
            const LightNode* nlight_root= n_root_lnodes[dir];
            // solid → no propagation
            if (nvox_root) {
                byte nvoxel = get_value_VoxelNode(nvox_root, depth, pos, 0);
                if (nvoxel && solidity[nvoxel - 1]) {
                    continue; // solid: hard stop
                }
            }
            // air decay
            byte decayed_light = (light > air_decay) ? (byte)(light - air_decay) : 0;
            if (decayed_light <= min_light) {
                continue;
            }
            // only queue if it improves neighbor
            byte current_light = nlight_root ? get_value_LightNode(nlight_root, depth, pos, 0) : 0;
            if (decayed_light <= current_light) {
                continue;
            }
            LightQueue* nqueue = n_queues[dir];
            // Add to neighbor queue
            if (nqueue) {
                if (locks_enabled) spin_lock(&nqueue->lock);
                a_LightQueue(nqueue,
                    (LightUpdate) {
                        // .type = zox_light_type_flood,
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
        byte voxel = get_value_VoxelNode(root_vnode, depth, pos, 0);
        if (voxel && solidity[voxel - 1]) {
            continue;
        }
        byte decayed_light = (light > air_decay) ? (byte) (light - air_decay) : min_light;
        byte current_light = get_value_LightNode(root_lnode, depth, pos, 0);
        if (decayed_light <= current_light) {
            continue;
        }
        zox_logv("     + Light Flooded [%ix%ix%i] l[%i] dist[%i]", pos.x, pos.y, pos.z, decayed_light, distance);
        set_LightNode(root_lnode, depth, pos, decayed_light, 0);
        dirty = 1;
        flood_light(root_vnode, root_lnode, n_root_vnodes, n_root_lnodes, n_queues, depth, pos, decayed_light, distance - 1, min_light, air_decay, solidity);
    }
    return dirty;
}

zox_sys2(LightFloodSystem) {
    byte dbg_log = 0;
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
    for (int j = 0; j < 255; j++) {
        solidity[j] = 1;
    }
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
        if (zox_getv(e, Busy)) {
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
            byte length = powers_of_two[update.depth];
            if (pos.x >= length || pos.z >= length || pos.y >= length) {
                //  zox_log_error("[r_LightQueue] position oob [%ix%ix%i]", pos.x, pos.y, pos.z);
                continue;
            }
            byte voxel = get_value_VoxelNode(root_vnode, update.depth, update.pos, 0);
            if (voxel && solidity[voxel - 1]) {
                if (dbg_log) {
                    zox_log("[%s]: Light Flood Canceled at [%ix%ix%i] l[%i] q [%i]", zox_get_name(e), update.pos.x, update.pos.y, update.pos.z, update.light,  light_queue->count);
                }
                continue;
            }
            byte current_light = get_value_LightNode(root_lnode, update.depth, update.pos, 0);
            byte spread_light = update.light;
            if (dbg_log) {
                zox_log("[%s]: [%s] ^ Light Flooding at [%ix%ix%i] l[%i] spread [%i] q [%i]", current_light > spread_light ? "Skip" : "Run", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, current_light, spread_light, light_queue->count);
            }
            if (current_light > spread_light) {
                spread_light = current_light;
            } else if (current_light < spread_light) {
                set_LightNode(root_lnode, update.depth, update.pos, spread_light, 0);
                dirty = 1;
            }
            if (flood_light(root_vnode, root_lnode, n_root_vnodes, n_root_lnodes, n_light_queues, update.depth, update.pos, spread_light, update.distance, darklight, light_air_decay, solidity)) {
                dirty = 1;
            }
        }
        if (dirty) {
            light_node_dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(LightFloodSystem);
