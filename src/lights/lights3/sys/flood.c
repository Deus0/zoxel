// -> for each neighbor, we have 6
//      -> If out of bounds:
//          ->wrap position, add to queue, we can get the light and voxel though to check if meets conditions
//      -> otherwise
//          -> check if air
//              -> if air we check light
//                  -> if light less than current (decayed light) we set it and propogate there
//          -> if not air, i.e. solid, of course light doesnt go through solid you twat
// NOTE: Stop setting neighbor data, thats const, you are literally corrupting memory
static inline byte flood_light(
    spinlock* lightlock,
    spinlock** neighbor_locks,
    const VoxelNode* voxels,
    LightNode* lights,
    const VoxelNode* neighbor_voxels[6],
    const LightNode* neighbor_lights[6],
    LightQueue* n_queues[6],
    byte depth,
    byte3 positionl,
    byte light,
    byte distance,
    byte min_light,
    byte air_decay,
    const byte* solidity,
    uint* flooded)
{
    byte dirty = 0;
    if (!voxels || !lights || distance == 0 || light <= min_light) {
        return dirty;
    }
    (*flooded)++;
    byte length = octree_size(depth) - 1;
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
            // Cross Chunk: READ neighbor if present, never write it
            const VoxelNode* neighbor_voxels2 = neighbor_voxels[dir];
            const LightNode* neighbor_lights2 = neighbor_lights[dir];
            spinlock* neighbor_lock = neighbor_locks[dir];
            LightQueue* nqueue = n_queues[dir];
            if (!neighbor_voxels2 ||
                !neighbor_lights2 ||
                !neighbor_lock ||
                !nqueue)
            {
                continue;
            }
            // solid → no propagation
            byte neighbor_voxel = getv_VoxelNode(
                neighbor_voxels2,
                depth,
                pos);
            if (neighbor_voxel && solidity[neighbor_voxel - 1]) {
                continue; // solid: hard stop
            }
            // air decay
            byte decayed_light = (light > air_decay) ? (byte)(light - air_decay) : 0;
            if (decayed_light <= min_light) {
                continue;
            }
            // only queue if it improves neighbor
            spin_lock(neighbor_lock);
            byte current_light = getv_LightNode(
                neighbor_lights2,
                depth,
                pos);
            spin_unlock(neighbor_lock);
            if (decayed_light <= current_light) {
                continue;
            }
            // Add to neighbor queue
            spin_lock(&nqueue->lock);
            a_LightQueue(nqueue,
                (LightUpdate) {
                    .light = decayed_light,
                    .pos = pos,
                    .depth = depth,
                    .distance = distance - 1
                });
            spin_unlock(&nqueue->lock);
            continue;
        }
        // --- In-chunk: READ voxel, WRITE light in our own chunk only. ---
        byte voxel = getv_VoxelNode(voxels, depth, pos);
        if (voxel && solidity[voxel - 1]) {
            continue;
        }
        byte decayed_light = (light > air_decay) ?
            (byte) (light - air_decay) :
            min_light;
        spin_lock(lightlock);
        byte current_light = getv_LightNode(
            lights,
            depth,
            pos);
        spin_unlock(lightlock);
        if (decayed_light <= current_light) {
            continue;
        }
        zox_logv("     + Light Flooded [%ix%ix%i] l[%i] dist[%i]",
            pos.x,
            pos.y,
            pos.z,
            decayed_light,
            distance);
        spin_lock(lightlock);
        set_LightNode(lights, depth, pos, decayed_light);
        spin_unlock(lightlock);
        dirty = 1;
        flood_light(
            lightlock,
            neighbor_locks,
            voxels,
            lights,
            neighbor_voxels,
            neighbor_lights,
            n_queues,
            depth,
            pos,
            decayed_light,
            distance - 1,
            min_light,
            air_decay,
            solidity,
            flooded);
    }
    return dirty;
}

zox_sys2(LightFloodSystem) {
    byte dbg_log = 0;
    const uint max_flooding = 256;
    uint flooded = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(LightLock);
    zox_sys_out(LightQueue);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    entity realm = 0;
    byte solidity[255];
    memset(solidity, 1, 255);
    for (int i = 0; i < it->count && flooded < max_flooding; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightLock, lightlock);
        zox_sys_o(LightQueue, light_queue);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(LightNodeDirty, light_node_dirty);
        if (!light_queue->count) {
            continue;
        }
        // NOTE: Check Blocks Caches
        entity manager = zox_get_link(world, e, BlockManagerLink);
        if (realm != manager) {
            realm = manager;
            zox_geter(realm, BlockLinks, blocks);
            for (int j = 0; j < blocks->length; j++) {
                entity block = blocks->value[j];
                solidity[j] = zox_valid(block) &&
                    zox_has(block, BlockLightPass) ?
                        !zox_getv(block, BlockLightPass) :
                        1;
            }
        }
        const VoxelNode* neighbor_voxels[6];
        const LightNode* neighbor_lights[6];
        spinlock* neighbor_locks[6];
        fetch_neightbor_voxel_nodes(
            world,
            neighbors,
            neighbor_voxels);
        fetch_neightbor_light_nodes(
            world,
            neighbors,
            neighbor_lights);
        fetch_neighbors_light_locks(
            world,
            neighbors->value,
            neighbor_locks);
        LightQueue* n_light_queues[6];
        fetch_neightbor_propogation_queues(world, neighbors, n_light_queues);
        byte dirty = 0;
        byte voxel_octree_depth = zox_getv(e, NodeDepth);
        while (light_queue->count && flooded < max_flooding) {
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
                zox_log("[%s]: [%s] ^ Light Flooding at [%ix%ix%i] l[%i] spread [%i] q [%i]",
                    current_light > spread_light ? "Skip" : "Run",
                    zox_sys_e_name,
                    update.pos.x,
                    update.pos.y,
                    update.pos.z,
                    current_light,
                    spread_light,
                    light_queue->count);
            }
            if (current_light > spread_light) {
                spread_light = current_light;
            } else if (current_light < spread_light) {
                set_LightNode(root_lnode, update.depth, update.pos, spread_light);
                dirty = 1;
            }
            if (flood_light(
                &lightlock->value,
                neighbor_locks,
                root_vnode,
                root_lnode,
                neighbor_voxels,
                neighbor_lights,
                n_light_queues,
                update.depth,
                update.pos,
                spread_light,
                update.distance,
                darklight,
                light_air_decay,
                solidity,
                &flooded))
            {
                dirty = 1;
            }
        }
        if (dirty) {
            light_node_dirty->value = zox_dirty_trigger;
        }
        zox_sys_increment();
    }
} zox_sys_end(LightFloodSystem);
