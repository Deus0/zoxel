// NOTE: Theres an issue when we optimize, the light nodes can be set by the bigger nodes, so it was hard to debug

byte sunbeam(LightQueue* floodlight_queue, SunlightQueue* chunk_below_queue, LightNode* root_lights, const VoxelNode* root_vnode, byte depth, byte3 pos, byte light, const VoxelNode* n_root_vnodes[6], const LightNode* n_root_lightss[6], byte min_light, byte air_decay, const byte* solidity) {
    // hmm
    byte dirty = 0;
    short length = octree_size(depth);
    if (pos.y >= length) {
        zox_logw("position too high [%i]", pos.y);
        return dirty;
    }
    byte max_y = pos.y;
    byte flood_start = 0;
    byte flood_end = 0;
    byte beam_started = 0;
    byte beam_stopped = 0;
    for (byte y = 0; y <= max_y; y++) {
        pos.y = max_y - y;
        byte voxel = getv_VoxelNode(root_vnode, depth, pos);
        if (voxel && solidity[voxel - 1]) {
            // zox_log("sunbeam stopped v at [%ix%ix%i] v[%i]",  pos.x, pos.y, pos.z, voxel);
            beam_stopped = 1;
            break;
        }
        // set light in LightNode
        set_LightNode(root_lights, depth, pos, light);
        dirty = 1;
        beam_started = 1;
        if (y == 0) {
            flood_end = pos.y;
        }
        flood_start = pos.y;
        zox_logv("+ SunLight [%i] Set at [%ix%ix%i]", light, pos.x, pos.y, pos.z);
    }
    if (!beam_started) {
        return dirty;
    }
    zox_logv(" * light beam y: [%i] to [%i]", flood_start, (flood_end));
    for (byte y = flood_start; y <= flood_end; y++) {
        pos.y = y;
        zox_logv(" - Light Beam Spreads [%ix%ix%i]", pos.x, pos.y, pos.z);
        if (locks_enabled) spin_lock(&floodlight_queue->lock);
        a_LightQueue(floodlight_queue,
            (LightUpdate) {
                .light = light,
                .pos = pos,
                .depth = depth,
                .distance = light_propogation_distance
            });
        if (locks_enabled) spin_unlock(&floodlight_queue->lock);
    }
    if (chunk_below_queue && !beam_stopped) {
        if (locks_enabled) spin_lock(&chunk_below_queue->lock);
        a_SunlightQueue(chunk_below_queue, (SunlightUpdate) {
            .pos = (byte3) {
                pos.x,
                length - 1,
                pos.z
            },
            .light = light,
            .depth = depth
        });
        if (locks_enabled) spin_unlock(&chunk_below_queue->lock);
    }
    return dirty;
}


// Progress ray in a sunlight direction, stops when solid, decreases when liquid
// TODO: Optimize LightNode System - group same values
// Triggers: VoxelNodeGenerated
zox_sys2(SunlightSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    zox_sys_in(NodeDepth);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(LightQueue);
    zox_sys_out(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    entity realm = 0;
    byte solidity[255];
    memset(solidity, 1, 255);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BlockManagerLink, manager);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightQueue, floodlight_queue);
        zox_sys_o(LightNode, lights);
        zox_sys_o(LightNodeDepth, light_depth);
        zox_sys_o(LightNodeDirty, dirty);
        entity chunkd = neighbors->value[direction_down];
        // For now we skip unless bottom chunk - due to loading timing
        if (!zox_valid(chunkd)) {
            if (dbg_log) {
                zox_log("Delaying Topmost Chunk [%s] as below chunk not valid", zox_get_name(e));
            }
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
        SunlightQueue* chunk_below_sunlight_queue = zox_gett_mut(chunkd, SunlightQueue);
        light_depth->value = depth->value;
        short length = octree_size(depth->value);
        const VoxelNode* n_root_vnodes[6];
        fetch_neightbor_voxel_nodes(world, neighbors, n_root_vnodes);
        const LightNode* n_root_lightss[6];
        fetch_neightbor_light_nodes(world, neighbors, n_root_lightss);
        if (dbg_log) {
            zox_log("[%s] Topmost Sunbeams Light [%i] Depth [%i]", zox_get_name(e), sunlight, depth->value);
        }
        // now for all XZ places we go through
        byte3 pos;
        pos.y = length - 1;
        for (pos.x = 0; pos.x < length; pos.x++) {
            for (pos.z = 0; pos.z < length; pos.z++) {
                if (dbg_log >= 2) {
                    zox_log("   - [%s] Begin Topmost Sunbeam [%ix%ix%i] l[%i]", zox_get_name(e), pos.x, pos.y, pos.z, sunlight);
                }
                if (sunbeam(floodlight_queue, chunk_below_sunlight_queue, lights, vnode, depth->value, pos, sunlight, n_root_vnodes, n_root_lightss, darklight, light_air_decay, solidity)) {
                    dirty->value = zox_dirty_trigger;
                }
            }
        }
        zox_remove(e, GenerateSunlight);
    }
} zox_sys_end(SunlightSystem);

// Queued side updates for propogation
zox_sys2(LightBeamSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(SunlightQueue);
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
        zox_sys_o(SunlightQueue, sunlight_queue);
        zox_sys_o(LightQueue, floodlight_queue);
        zox_sys_o(LightNode, root_lights);
        zox_sys_o(LightNodeDirty, dirty);
        if (!sunlight_queue->count) {
            continue;
        }
        entity chunkd = neighbors->value[direction_down];
        if (!zox_has(e, BottomChunk)) {
            if (!zox_valid(chunkd)) {
                // NOTE: We wait for below chunk to load
                if (dbg_log) {
                    zox_log("Delaying Chunk [%s] as below chunk not valid", zox_get_name(e));
                }
                continue;
            }
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
        SunlightQueue* chunk_below_sunlight_queue = zox_valid(chunkd) ? zox_gett_mut(chunkd, SunlightQueue) : NULL;
        const VoxelNode* n_root_vnodes[6];
        fetch_neightbor_voxel_nodes(world, neighbors, n_root_vnodes);
        const LightNode* n_root_lightss[6];
        fetch_neightbor_light_nodes(world, neighbors, n_root_lightss);
        if (dbg_log) {
            zox_log("[%s] Extended Sunbeams l[%i]", zox_get_name(e), sunlight);
        }
        while (sunlight_queue->count) {
            SunlightUpdate update = remove_SunlightQueue(sunlight_queue);
            // NOTE: For bottom chunk we just remove queue for beaming to bottom of earth
            byte3 pos = update.pos;
            short length = octree_size(update.depth);
            if (pos.x >= length || pos.z >= length || pos.y >= length) {
                //  zox_log_error("[r_LightQueue] position oob [%ix%ix%i]", pos.x, pos.y, pos.z);
                continue;
            }
            if (dbg_log >= 2) {
                zox_log(" - Beaming [%ix%ix%i] l[%i] q [%i]", update.pos.x, update.pos.y, update.pos.z, update.light, sunlight_queue->count);
            }
            if (sunbeam(floodlight_queue, chunk_below_sunlight_queue, root_lights, root_vnode, update.depth, update.pos, update.light, n_root_vnodes, n_root_lightss, darklight, light_air_decay, solidity)) {
                if (!dirty->value) {
                    if (dbg_log) {
                        zox_log("[%s] ChunkLights Updated at [%ix%ix%i] l[%i] q [%i] Depth [%i]", zox_get_name(e) , update.pos.x, update.pos.y, update.pos.z, update.light, sunlight_queue->count, update.depth);
                    }
                }
                dirty->value = zox_dirty_trigger;
            }
        }
    }
} zox_sys_end(LightBeamSystem);
