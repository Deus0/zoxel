// an emitter the dark meets beside the voxel it darkened (position) lights it again: its light came from the
//  emitter, the dark took it (a hole over it covered, the sun's dark flooding the room), and nothing else lit it back
static inline void dark_relight_emitter(LightQueue* light_queue, const byte* glows, byte voxel, byte3 position, byte depth) {
    if (!light_queue || !voxel || !glows[voxel - 1]) {
        return;
    }
    spin_lock(&light_queue->lock);
    a_LightQueue(light_queue, (LightUpdate) {
        .light = emitter_side_light(glows[voxel - 1]),
        .distance = light_propogation_distance,
        .pos = position,
        .depth = depth
    });
    spin_unlock(&light_queue->lock);
}

// Move darkbeam and dark propogation here
static inline byte dark_flood_light(
    spinlock* lightlock,
    const VoxelNode* root_vnode,          // (READ)
    const BlockDatas* datas,
    LightOctree* root_lnode,                // (WRITE)
    const VoxelNode* n_root_vnodes[6],    // (READ-ONLY)
    const BlockDatas* n_datas[6],
    const LightOctree* n_root_lnodes[6],    // (READ-ONLY)
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
    const byte* solidity,
    const byte* glows)
{
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
            const LightOctree* n_root_lnode = n_root_lnodes[dir];
            if (!n_root_lnode) {
                continue;
            }
            if (n_root_vnode) {
                byte voxel = getv_VoxelNode(n_root_vnode, depth, pos);
                if (light_stops(solidity, n_datas[dir], n_root_vnode, depth, voxel, pos)) {
                    dark_relight_emitter(light_queue, glows, voxel, positionl, depth);
                    continue; // solid wall
                }
            }
            byte ncurrent_light = getv_LightOctree(n_root_lnode, depth, pos);
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
                    spin_lock(&nqueue->lock);
                    a_DarkQueue(nqueue, (DarkUpdate) {
                        .type  = 0,
                        .light = old_light,
                        .distance = distance - 1,
                        .pos   = pos,
                        .depth = depth
                    });
                    spin_unlock(&nqueue->lock);
                }
            } else {
                LightQueue* nqueue = n_light_queues[dir];
                if (nqueue) {
                    spin_lock(&nqueue->lock);
                    a_LightQueue(nqueue, (LightUpdate) {
                        .light = darklight,     // floods from its own light then, not this (it may be dark by then)
                        .distance = light_propogation_distance,
                        .pos   = pos,
                        .depth = depth
                    });
                    spin_unlock(&nqueue->lock);
                }
            }
            continue;
        }
        // --- in-chunk ---
        byte voxel = getv_VoxelNode(root_vnode, depth, pos);
        if (light_stops(solidity, datas, root_vnode, depth, voxel, pos)) {
            dark_relight_emitter(light_queue, glows, voxel, positionl, depth);
            continue; // solid → stop (light passes water, so does its dark)
        }
        byte current_light = getv_LightOctree(root_lnode, depth, pos);
        if (current_light <= min_light) {
            continue;   // omg this wasn't here
        }
        if (current_light < old_light) {
            zox_logv("     - Light Banished at [%ix%ix%i] l[%i] dist[%i]",
                pos.x,
                pos.y,
                pos.z,
                old_light,
                distance);
            // extinguish here and continue removing
            spin_lock(lightlock);
            set_LightOctree(
                root_lnode,
                depth,
                pos,
                min_light);
            spin_unlock(lightlock);
            dirty = 1;
            dark_flood_light(
                lightlock,
                root_vnode,
                datas,
                root_lnode,
                n_root_vnodes,
                n_datas,
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
                solidity,
                glows
            );
        } else {
            if (light_queue) {
                byte decayed_light =
                    (current_light > light_air_decay) ?
                        (byte) (current_light - light_air_decay) :
                        darklight;
                zox_logv("     + Dark Flood Light Source [%ix%ix%i] new [%i] old [%i] decayed [%i]",
                    pos.x,
                    pos.y,
                    pos.z,
                    current_light,
                    old_light,
                    decayed_light);
                spin_lock(&light_queue->lock);
                a_LightQueue(light_queue, (LightUpdate) {
                    // .type  = zox_light_type_flood,
                    .light = darklight,     // floods from its own light then: a beam beside it can go dark first (a roof placed at once), its stale light relit the room
                    .distance = light_propogation_distance,
                    .pos   = pos,
                    .depth = depth
                });
                spin_unlock(&light_queue->lock);
            }
        }
    }
    return dirty;
}


// removes a sunbeam down from pos (light: the beam's light at pos, or for a beam continued from the chunk above, the
//  light coming into its top): each voxel lit as the beam would light it (light_dim'd in liquids) goes dark, then the
//  light around floods back in
//  - beam_start: a block placed at pos (already dark), the beam under it goes
//  - dim_start: water placed at pos, the beam from pos down goes and is relit a liquid dimmer (a flood at each voxel)
static inline byte is_light_type_dim(byte type) {
    return type == zox_light_type_dim_start || type == zox_light_type_dim;
}

byte dark_sunbeam(
    spinlock* lightlock,
    DarkQueue* queued,
    const VoxelNode* root_vnode,
    const BlockDatas* datas,
    LightOctree* root_lnode,
    const VoxelNode* n_root_vnodes[6],
    const BlockDatas* n_datas[6],
    const LightOctree* n_root_lnodes[6],
    LightQueue* n_light_queues[6],
    LightQueue* light_queue,
    DarkQueue* n_dark_queues[6],
    DarkQueue* dark_queue,
    byte depth,
    byte3 pos,
    byte light,
    byte min_light,
    byte air_decay,
    byte type,
    const byte* solidity,
    const byte* liquids,
    const byte* glows)
{
    byte dirty = 0;
    const int length = octree_size(depth);
    const byte started = type == zox_light_type_beam_start || type == zox_light_type_dim_start;
    if (started && pos.y >= length) {
        return dirty;
    }
    const int top = started ? pos.y : length - 1;
    byte expected = light;
    byte removed[256];      // the beam's light taken from each y, its dark floods from that
    int flood_start = -1;
    int flood_end = -1;
    byte beam_stopped = 0;
    for (int y = top; y >= 0; y--) {
        pos.y = y;
        if (!started || y != top) {
            const byte voxel = getv_VoxelNode(root_vnode, depth, pos);
            if (light_stops(solidity, datas, root_vnode, depth, voxel, pos)) {
                beam_stopped = 1;
                break;
            }
            if (voxel && liquids[voxel - 1]) {
                expected = light_dim(expected, min_light);
            }
        } else if (type == zox_light_type_beam_start) {
            flood_end = y;      // the placed block, darkened as it was placed
            removed[y] = expected;
            continue;
        }
        // stops where it doesn't see the beam (another light, or relit already)
        if (expected <= min_light || getv_LightOctree(root_lnode, depth, pos) != expected) {
            beam_stopped = 1;
            break;
        }
        spin_lock(lightlock);
        set_LightOctree(root_lnode, depth, pos, min_light);
        spin_unlock(lightlock);
        removed[y] = expected;
        dirty = 1;
        if (flood_end < 0) {
            flood_end = y;
        }
        flood_start = y;
    }
    // flood after, otherwise is refills with sunlight: what each lit went dark (lit a step dimmer than it), what's
    //  as bright as it stays (another beam beside it, as deep in water)
    if (flood_start < 0) {
        flood_start = flood_end;
    }
    for (int y = flood_start; flood_start >= 0 && y <= flood_end; y++) {
        pos.y = y;
        if (dark_flood_light(
            lightlock,
            root_vnode,
            datas,
            root_lnode,
            n_root_vnodes,
            n_datas,
            n_root_lnodes,
            n_light_queues,
            light_queue,
            n_dark_queues,
            dark_queue,
            depth,
            pos,
            removed[y] > air_decay ? removed[y] - air_decay + 1 : min_light,
            darklight_propogation_distance,
            min_light,
            air_decay,
            solidity,
            glows))
        {
            dirty = 1;
        }
    }
    // relit a liquid dimmer, flooded in after the dark: the placed water dims the beam from it down, a beam continued
    //  from above comes in a liquid dimmer than it was
    if (is_light_type_dim(type) && flood_start >= 0) {
        byte relit = type == zox_light_type_dim_start ? light : light_dim(light, min_light);
        for (int y = top; y >= flood_start; y--) {
            pos.y = y;
            const byte voxel = getv_VoxelNode(root_vnode, depth, pos);
            if (voxel && liquids[voxel - 1]) {
                relit = light_dim(relit, min_light);
            }
            spin_lock(&light_queue->lock);
            a_LightQueue(light_queue, (LightUpdate) {
                .light = relit,
                .pos = pos,
                .depth = depth,
                .distance = light_propogation_distance
            });
            spin_unlock(&light_queue->lock);
        }
    }
    // pass downward into chunk below since we survived until the end
    if (!beam_stopped && queued) {
        spin_lock(&queued->lock);
        a_DarkQueue(queued, (DarkUpdate) {
            .type = is_light_type_dim(type) ? zox_light_type_dim : zox_light_type_beam,
            .pos = (byte3) { pos.x, length - 1, pos.z },
            .depth = depth,
            .light = expected
        });
        spin_unlock(&queued->lock);
    }
    return dirty;
}

// Queued side updates for propogation
// NOTE: We need to account for depth differences
void dark_light_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(LightLock);
    zox_sys_out(LightOctreeDepth);
    zox_sys_out(LightOctree);
    zox_sys_out(DarkQueue);
    zox_sys_out(LightQueue);
    zox_sys_out(LightOctreeDirty);
    entity realm = 0;
    byte solidity[zox_light_solidity_length];
    memset(solidity, 1, zox_light_solidity_length);
    entity liquids_realm = 0;
    byte liquids[zox_light_solidity_length];
    memset(liquids, 0, zox_light_solidity_length);
    // emitters the dark meets light again (dark_relight_emitter)
    entity emission_realm = 0;
    byte emission[zox_light_solidity_length];
    byte3 emission_offset[zox_light_solidity_length];
    byte glows[zox_light_solidity_length];
    memset(glows, 0, zox_light_solidity_length);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightLock, lightlock);
        zox_sys_o(LightOctreeDepth, depthl);
        zox_sys_o(LightOctree, root_lnode);
        zox_sys_o(DarkQueue, dark_queue);
        zox_sys_o(LightQueue, light_queue);
        zox_sys_o(LightOctreeDirty, light_node_dirty);
        if (!dark_queue->count) {
            continue;
        }
        // NOTE: Check Blocks Caches
        update_light_solidity(
            world,
            zox_get_link(world, e, BlockManagerLink),
            &realm,
            solidity);
        update_light_liquids(world, zox_get_link(world, e, BlockManagerLink), &liquids_realm, liquids);
        const entity manager = zox_get_link(world, e, BlockManagerLink);
        if (emission_realm != manager) {
            update_light_emission(world, manager, &emission_realm, emission, emission_offset);
            emission_from_voxel(emission, emission_offset, glows);
        }
        const VoxelNode* nnodesv[6];
        fetch_neightbor_voxel_nodes(world, neighbors, nnodesv);
        const BlockDatas* datas = zox_has(e, BlockDatas) ? zox_get(e, BlockDatas) : NULL;
        const BlockDatas* ndatas[6];
        fetch_neighbor_block_datas(world, neighbors, ndatas);
        const LightOctree* nnodesl[6];
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
                if (dbg_log) {
                    zox_log("[%s] Begin Dark Flooding [%ix%ix%i] l[%i] distance [%i] q [%i]",
                        zox_sys_e_name,
                        update.pos.x,
                        update.pos.y,
                        update.pos.z,
                        update.light,
                        update.distance,
                        dark_queue->count);
                }
                byte current_light = getv_LightOctree(root_lnode, depthl->value, update.pos);
                if (current_light > darklight) {
                    spin_lock(&lightlock->value);
                    set_LightOctree(
                        root_lnode,
                        depthl->value,
                        update.pos,
                        darklight);
                    spin_unlock(&lightlock->value);
                    dirty = 1;
                }
                if (dark_flood_light(
                    &lightlock->value,
                    root_vnode,
                    datas,
                    root_lnode,
                    nnodesv,
                    ndatas,
                    nnodesl,
                    n_light_queues,
                    light_queue,
                    n_dark_queues,
                    dark_queue,
                    depthl->value,
                    update.pos,
                    update.light,
                    update.distance,
                    darklight,
                    light_air_decay,
                    solidity,
                    glows))
                {
                    dirty = 1;
                }
            } else {   // the beams
                if (dbg_log) {
                    zox_log("[%s] Begin Darkbeam [%ix%ix%i] l[%i] q [%i]",
                        zox_sys_e_name,
                        update.pos.x,
                        update.pos.y,
                        update.pos.z,
                        update.light,
                        dark_queue->count);
                }
                if (dark_sunbeam(
                    &lightlock->value,
                    dark_queued,
                    root_vnode,
                    datas,
                    root_lnode,
                    nnodesv,
                    ndatas,
                    nnodesl,
                    n_light_queues,
                    light_queue,
                    n_dark_queues,
                    dark_queue,
                    depthl->value,
                    update.pos,
                    update.light,
                    darklight,
                    light_air_decay,
                    update.type,
                    solidity,
                    liquids,
                    glows))
                {
                    dirty = 1;
                }
            }
        }
        zox_mut_end(chunkd, DarkQueue);
        if (dirty) {
            light_node_dirty->value = zox_dirty_trigger;
            /*zox_sys_world();
             *            zox_sys_e();
             *            zox_log("DarkSystem %s - %i", zox_get_name(e), count);*/
        }
    }
    zox_sys_on_end();
} zoxd_system(dark_light_system);

