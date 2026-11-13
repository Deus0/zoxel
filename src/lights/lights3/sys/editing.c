//  Using the VoxelUpdateQueue, we add to the sunbeams and propogation
// for now: check above voxel, if sun, create beam
// TODO: Convert local position to terrain position (positionv)
// Shouuld we use VoxelNodeDepth here? since we are removing/adding at that depth
void voxel_light_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeQueue);
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(LightLock);
    zox_sys_out(SunlightQueue);
    zox_sys_out(LightQueue);
    zox_sys_out(DarkQueue);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    entity realm = 0;
    byte solidity[zox_light_solidity_length];
    memset(solidity, 1, zox_light_solidity_length);
    // light each block gives off, a removed emitter darkens what it lit, a placed one lights around it
    entity emission_realm = 0;
    byte emission[zox_light_solidity_length];
    memset(emission, 0, zox_light_solidity_length);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNodeQueue, input_queue);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightLock, lightlock);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(SunlightQueue, sun_queue);
        zox_sys_o(LightQueue, light_queue);
        zox_sys_o(DarkQueue, dark_queue);
        zox_sys_o(LightNodeDirty, light_node_dirty);
        if (!input_queue->count) {
            continue;
        }
        // NOTE: Check Blocks Caches
        update_light_solidity(
            world,
            zox_get_link(world, e, BlockManagerLink),
            &realm,
            solidity);
        update_light_emission(world, zox_get_link(world, e, BlockManagerLink), &emission_realm, emission);
        const LightNode* nnodesl[6];
        fetch_neightbor_light_nodes(world, neighbors, nnodesl);
        for (size_t j = 0; j < input_queue->count; j++) {
            VoxelNodeUpdate update = input_queue->ptr[j];
            // once an edit, after it's set (old_value known): each was handled every frame it queued, before it was
            //  set too, a removed glow stone was seen as nothing and its neighbours refilled it
            if (update.state != zox_voxel_queue_post) {
                continue;
            }
            // Removing Voxel - Spreads Light
            if (!update.value) {
                // a removed emitter (glow stone): darkness out from it at its light, the dark pass then refills from
                //  the light left, not its own (its neighbours' light came from it, a flood from them kept it lit)
                const byte emitted = update.old_value ? emission[update.old_value - 1] : 0;
                if (emitted) {
                    a_DarkQueue(dark_queue, (DarkUpdate) {
                        .type = zox_light_type_flood,
                        .pos = update.position,
                        .depth = depth->value,
                        .light = emitted,
                        .distance = darklight_propogation_distance
                    });
                }
                const LightNode* above = get_neighbor_LightNode(root_lnode, nnodesl, direction_up, update.position, depth->value);
                byte light_above = above ? above->value : 0;
                if (light_above == sunlight) {
                    // if y, we do y + 1
                    a_SunlightQueue(sun_queue, (SunlightUpdate) {
                        .pos = update.position,
                        .light = sunlight,
                        .depth = depth->value
                    });
                    if (dbg_log) {
                        zox_log("[%s] Removed Block: + Sunbeam at [%ix%ix%i] sunlight [%i]",
                            zox_sys_e_name,
                            update.position.x,
                            update.position.y,
                            update.position.z,
                            sunlight);
                    }
                    // set dark light, as it was filled up
                    // set_LightNode(root_lnode, depth->value, update.position, sunlight, 0);
                } else if (!emitted) {
                    byte max_nlight = light_above;
                    for (byte dir = 0; dir < 6; dir++) {
                        if (dir == direction_up) {
                            continue;
                        }
                        const LightNode* lnnode = get_neighbor_LightNode(
                            root_lnode,
                            nnodesl,
                            dir,
                            update.position,
                            depth->value
                        );
                        byte nlight = lnnode ? lnnode->value : 0;
                        if (nlight > max_nlight) {
                            max_nlight = nlight;
                        }
                    }
                    byte decayed_light = (max_nlight > light_air_decay) ? (byte) (max_nlight - light_air_decay) : darklight;
                    if (dbg_log) {
                        zox_log("[%s] Removed Block: + Light Flood at [%ix%ix%i] max nlight [%i]",
                            zox_sys_e_name,
                            update.position.x,
                            update.position.y,
                            update.position.z,
                            max_nlight);
                    }
                    a_LightQueue(
                        light_queue,
                        (LightUpdate) {
                            // .type = zox_light_type_flood,
                            .pos = update.position,
                            .depth = depth->value,
                            .light = decayed_light,
                            .distance = light_propogation_distance
                    });
                }
            }
            // Placing Voxel - Spreads Darkness!
            else { // if (update.value) {
                // a placed emitter lights the air on its sides a step down (solid, the flood skips its own voxel), a step
                //  down so a removal's darkness (at its light) clears them, equal light counts as another source
                if (emission[update.value - 1]) {
                    const int length = octree_size(depth->value);
                    for (byte d = 0; d < 6; d++) {
                        const sbyte3 o = neighbor_offsets[d];
                        const int3 p = { update.position.x + o.x, update.position.y + o.y, update.position.z + o.z };
                        if (p.x < 0 || p.y < 0 || p.z < 0 || p.x >= length || p.y >= length || p.z >= length) {
                            continue;
                        }
                        a_LightQueue(light_queue, (LightUpdate) {
                            .pos = int3_to_byte3(p), .depth = depth->value,
                            .light = emission[update.value - 1] > light_air_decay ? emission[update.value - 1] - light_air_decay : darklight,
                            .distance = light_propogation_distance });
                    }
                }
                if (!solidity[update.value - 1]) {
                    continue;
                }
                // TODO: We can add a dark beam here instead
                //          - It needs to call dark floodfill on all points along
                // NOTE: no matter what we need to dark flood surroundings - for example on corner parts
                // Dark Flood
                const LightNode* removed_lnode = get_LightNode(root_lnode, depth->value, update.position);
                byte removed_light = removed_lnode ? removed_lnode->value : 0;
                if (removed_light > darklight) {
                    if (dbg_log) {
                        zox_log("[%s] Placed Block: + Dark Flood at [%ix%ix%i] removed light [%i]",
                            zox_sys_e_name,
                            update.position.x,
                            update.position.y,
                            update.position.z,
                            removed_light);
                    }
                    a_DarkQueue(dark_queue,
                        (DarkUpdate) {
                            .type = zox_light_type_flood,
                            .pos = update.position,
                            .depth = depth->value,
                            .light = removed_light,
                            .distance = darklight_propogation_distance
                        });
                }
                // NOTE: DarkBeam needs to Process first (so we add last), otherwise its stomped by nearby sun? idk but it works!
                const LightNode* above = get_neighbor_LightNode(
                    root_lnode,
                    nnodesl,
                    direction_up,
                    update.position,
                    depth->value
                );
                byte light_above = above ? above->value : 0;
                if (light_above == sunlight) {
                    // if y, we do y + 1
                    if (dbg_log) {
                        zox_log("[%s] Placed Block: + Darkbeam [%ix%ix%i] l[%i]",
                            zox_sys_e_name,
                            update.position.x,
                            update.position.y,
                            update.position.z,
                            darklight);
                    }
                    a_DarkQueue(dark_queue,
                        (DarkUpdate) {
                            .type = zox_light_type_beam_start,
                            .pos = update.position,
                            .depth = depth->value,
                            .light = light_above
                        });
                }
                // set dark light, as it was filled up
                // TODO: Move this to the light system itself
                spin_lock(&lightlock->value);
                set_LightNode(root_lnode, depth->value, update.position, darklight);
                spin_unlock(&lightlock->value);
                light_node_dirty->value = zox_dirty_trigger;
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(voxel_light_system);
