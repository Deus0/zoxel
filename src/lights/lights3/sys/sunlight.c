// NOTE: Theres an issue when we optimize, the light nodes can be set by the bigger nodes, so it was hard to debug

byte sunbeam(LightQueue* floodlight_queue, SunlightQueue* chunk_below_queue, LightNode* root_lnode, const VoxelNode* root_vnode, byte depth, byte3 pos, byte light, const VoxelNode* n_root_vnodes[6], const LightNode* n_root_lnodes[6], byte min_light, byte air_decay, const byte* solidity) {
    // hmm
    byte dirty = 0;
    byte length = powers_of_two[depth];
    if (pos.y >= length) {
        zox_logw("position too high [%i]", pos.y);
        return dirty;
    }
    byte max_y = pos.y;
    byte flood_start = 0;
    byte flood_end = 0;
    byte beam_stopped = 0;
    for (byte y = 0; y <= max_y; y++) {
        pos.y = max_y - y;
        byte voxel = get_value_VoxelNode(root_vnode, depth, pos, 0);
        if (voxel && solidity[voxel - 1]) {
            // zox_log("sunbeam stopped v at [%ix%ix%i] v[%i]",  pos.x, pos.y, pos.z, voxel);
            beam_stopped = 1;
            break;
        }
        // set light in LightNode
        zox_logv("+ SunLight [%i] Set at [%ix%ix%i]", light, pos.x, pos.y, pos.z);
        set_LightNode(root_lnode, depth, pos, light, 0);
        dirty = 1;
        if (y == 0) {
            flood_end = pos.y;
        }
        flood_start = pos.y;
    }
    zox_logv(" * light beam y: [%i] to [%i]", flood_start, (flood_end));
    for (byte y = flood_start; y <= flood_end; y++) {
        pos.y = y;
        zox_logv(" - Light Beam Spreads [%ix%ix%i]", pos.x, pos.y, pos.z);
        // TODO: when we change light, we can save light to array, and reuse here
        // Simpler to just add to queue here for flood lighting?
        /*byte floodlight_dirty = flood_light(root_vnode, root_lnode, n_root_vnodes, n_root_lnodes, n_light_queues, depth, pos, light, light_propogation_distance, darklight, light_air_decay, solidity);
        dirty |= floodlight_dirty;*/
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
    if (disable_lights) {
        return;
    }
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateLights);
    zox_sys_in(NodeDepth);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxLink);
    zox_sys_out(LightQueue);
    zox_sys_out(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    byte solidity[255];
    for (int j = 0; j < 255; j++) {
        solidity[j] = 1;
    }
    fetch_first_solidity(world, it, VoxLink_, solidity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateLights, state);
        zox_sys_i(NodeDepth, depthr);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightQueue, floodlight_queue);
        zox_sys_o(LightNode, lnode);
        zox_sys_o(LightNodeDepth, depthl);
        zox_sys_o(LightNodeDirty, dirty);
        if (state->value != zox_dirty_active) {
            continue;
        }
        // we skip if already at right depth
        if (depthl->value >= depthr->value) {
            // zox_logw("Skip Updating lights, light depth already updated");
            continue;
        }
        entity chunkd = neighbors->value[direction_down];
        // For now we skip unless bottom chunk - due to loading timing
        if (!zox_valid(chunkd)) {
            // Delay a frame!
            if (!zox_has(e, BottomChunk)) {
                zox_set(e, GenerateLights, { zox_dirty_trigger });
            }
            continue;
        }
        SunlightQueue* chunk_below_sunlight_queue = zox_gett_mut(chunkd, SunlightQueue);
        depthl->value = depthr->value;
        byte length = powers_of_two[depthl->value];
        const VoxelNode* n_root_vnodes[6];
        fetch_neightbor_voxel_nodes(world, neighbors, n_root_vnodes);
        const LightNode* n_root_lnodes[6];
        fetch_neightbor_light_nodes(world, neighbors, n_root_lnodes);
        //SunlightQueue* neighbor_sunlight_queues[6];
        //fetch_neighbors_sunlight_queues(world, neighbors->value, neighbor_sunlight_queues);
        if (dbg_log) {
            zox_log("[%s] Topmost Sunbeams l[%i] d[%i]", zox_get_name(e), sunlight, depthl->value);
        }
        // now for all XZ places we go through
        byte3 pos;
        pos.y = length - 1;
        for (pos.x = 0; pos.x < length; pos.x++) {
            for (pos.z = 0; pos.z < length; pos.z++) {
                if (dbg_log >= 2) {
                    zox_log("   - [%s] Begin Topmost Sunbeam [%ix%ix%i] l[%i]", zox_get_name(e), pos.x, pos.y, pos.z, sunlight);
                }
                if (sunbeam(floodlight_queue, chunk_below_sunlight_queue, lnode, vnode, depthl->value, pos, sunlight, n_root_vnodes, n_root_lnodes, darklight, light_air_decay, solidity)) {
                    dirty->value = zox_dirty_trigger;
                }
            }
        }
        zox_mut_end(chunkd, LightQueue);
    }
} zox_sys_end(SunlightSystem);

// Queued side updates for propogation
zox_sys2(LightBeamSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxLink);
    zox_sys_out(SunlightQueue);
    zox_sys_out(LightQueue);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    byte solidity[255];
    for (int j = 0; j < 255; j++) {
        solidity[j] = 1;
    }
    fetch_first_solidity(world, it, VoxLink_, solidity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(SunlightQueue, sunlight_queue);
        zox_sys_o(LightQueue, floodlight_queue);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(LightNodeDirty, dirty);
        if (!sunlight_queue->count) {
            continue;
        }
        entity chunkd = neighbors->value[direction_down];
        // For now we skip for bottom chunk - due to loading timing
        if (!zox_valid(chunkd) && !zox_has(e, BottomChunk)) {
            // NOTE: We wait for below chunk to load
            continue;
        }
        SunlightQueue* chunk_below_sunlight_queue = zox_valid(chunkd) ? zox_gett_mut(chunkd, SunlightQueue) : NULL;
        const VoxelNode* n_root_vnodes[6];
        fetch_neightbor_voxel_nodes(world, neighbors, n_root_vnodes);
        const LightNode* n_root_lnodes[6];
        fetch_neightbor_light_nodes(world, neighbors, n_root_lnodes);
        if (dbg_log) {
            zox_log("[%s] Extended Sunbeams l[%i]", zox_get_name(e), sunlight);
        }
        while (sunlight_queue->count) {
            SunlightUpdate update = r_SunlightQueue(sunlight_queue);
            // NOTE: For bottom chunk we just remove queue for beaming to bottom of earth
            byte3 pos = update.pos;
            byte length = powers_of_two[update.depth];
            if (pos.x >= length || pos.z >= length || pos.y >= length) {
                //  zox_log_error("[r_LightQueue] position oob [%ix%ix%i]", pos.x, pos.y, pos.z);
                continue;
            }
            // TODO: Add type for light/dark beams
            // TODO: we should probably make this byte3, with y, since we are gonna be used that now
            if (dbg_log >= 2) {
                zox_log(" - Beaming [%ix%ix%i] l[%i] q [%i]", update.pos.x, update.pos.y, update.pos.z, update.light, sunlight_queue->count);
            }
            if (sunbeam(floodlight_queue, chunk_below_sunlight_queue, root_lnode, root_vnode, update.depth, update.pos, update.light, n_root_vnodes, n_root_lnodes, darklight, light_air_decay, solidity)) {
                if (!dirty->value) {
                    if (dbg_log) {
                        zox_log("[%s] Chunk Now Dirty at [%ix%ix%i] l[%i] q [%i]", zox_get_name(e) , update.pos.x, update.pos.y, update.pos.z, update.light, sunlight_queue->count);
                    }
                }
                dirty->value = zox_dirty_trigger;
            }
        }
        if (zox_valid(chunkd)) {
            zox_mut_end(chunkd, LightQueue);
        }
    }
} zox_sys_end(LightBeamSystem);
