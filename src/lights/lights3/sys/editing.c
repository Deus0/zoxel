//  Using the VoxelUpdateQueue, we add to the sunbeams and propogation
// for now: check above voxel, if sun, create beam
// TODO: Convert local position to terrain position (positionv)
// Shouuld we use VoxelNodeDepth here? since we are removing/adding at that depth
zox_sys2(VoxelLightSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeQueue);
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxLink);
    zox_sys_out(SunlightQueue);
    zox_sys_out(LightQueue);
    zox_sys_out(DarkQueue);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    byte solidity[255];
    for (int j = 0; j < 255; j++) solidity[j] = 1;
    fetch_first_solidity(world, it, VoxLink_, solidity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxelNodeQueue, input_queue);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(SunlightQueue, sun_queue);
        zox_sys_o(LightQueue, light_queue);
        zox_sys_o(DarkQueue, dark_queue);
        zox_sys_o(LightNodeDirty, light_node_dirty);
        if (!input_queue->count) {
            continue;
        }
        const LightNode* nnodesl[6];
        fetch_neightbor_light_nodes(world, neighbors, nnodesl);
        for (size_t j = 0; j < input_queue->count; j++) {
            VoxelNodeUpdate update = input_queue->ptr[j];
            // Removing Voxel - Spreads Light
            if (!update.value) {
                const LightNode* above = get_neighbor_LightNode(root_lnode, nnodesl, direction_up, update.pos, depth->value);
                byte light_above = above ? above->value : 0;
                if (light_above == sunlight) {
                    // if y, we do y + 1
                    /*a_LightQueue(light_queue, (LightUpdate) {
                        // .type = zox_light_type_beam,
                        .pos = update.pos,
                        .light = sunlight,
                        .depth = depth->value
                    });*/
                    a_SunlightQueue(sun_queue, (SunlightUpdate) {
                        .pos = update.pos,
                        .light = sunlight,
                        .depth = depth->value
                    });
                    zox_logv("[%s] Removed Block: + Sunbeam at [%ix%ix%i] sunlight [%i]", zox_get_name(it->entities[i]),  update.pos.x, update.pos.y, update.pos.z, sunlight);
                    // set dark light, as it was filled up
                    // set_LightNode(root_lnode, depth->value, update.pos, sunlight, 0);
                } else {
                    byte max_nlight = light_above;
                    for (byte dir = 0; dir < 6; dir++) {
                        if (dir == direction_up) {
                            continue;
                        }
                        const LightNode* lnnode = get_neighbor_LightNode(
                            root_lnode,
                            nnodesl,
                            dir,
                            update.pos,
                            depth->value
                        );
                        byte nlight = lnnode ? lnnode->value : 0;
                        if (nlight > max_nlight) {
                            max_nlight = nlight;
                        }
                    }
                    byte decayed_light = (max_nlight > light_air_decay) ? (byte) (max_nlight - light_air_decay) : darklight;
                    zox_logv("[%s] Removed Block: + Light Flood at [%ix%ix%i] max nlight [%i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, max_nlight);
                    a_LightQueue(
                        light_queue,
                        (LightUpdate) {
                            // .type = zox_light_type_flood,
                            .pos = update.pos,
                            .depth = depth->value,
                            .light = decayed_light,
                            .distance = light_propogation_distance
                    });
                }
            }
            // Placing Voxel - Spreads Darkness!
            else { // if (update.value) {
                if (!solidity[update.value - 1]) {
                    continue;
                }
                // TODO: We can add a dark beam here instead
                //          - It needs to call dark floodfill on all points along
                // NOTE: no matter what we need to dark flood surroundings - for example on corner parts
                // Dark Flood
                const LightNode* removed_lnode = get_LightNode(root_lnode, depth->value, update.pos, 0);
                byte removed_light = removed_lnode ? removed_lnode->value : 0;
                if (removed_light > darklight) {
                    zox_logv("[%s] Placed Block: + Dark Flood at [%ix%ix%i] removed light [%i]", zox_get_name(it->entities[i]),  update.pos.x, update.pos.y, update.pos.z, removed_light);
                    a_DarkQueue(
                        dark_queue,
                        (DarkUpdate) {
                            .type = zox_light_type_flood,
                            .pos = update.pos,
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
                    update.pos,
                    depth->value
                );
                byte light_above = above ? above->value : 0;
                if (light_above == sunlight) {
                    // if y, we do y + 1
                    zox_logv("[%s] Placed Block: + Darkbeam [%ix%ix%i] l[%i]", zox_get_name(it->entities[i]),  update.pos.x, update.pos.y, update.pos.z, darklight);
                    a_DarkQueue(
                        dark_queue,
                        (DarkUpdate) {
                            .type = zox_light_type_beam_start,
                            .pos = update.pos,
                            .depth = depth->value,
                            .light = light_above
                    });
                }
                // set dark light, as it was filled up
                set_LightNode(root_lnode, depth->value, update.pos, darklight, 0);
                light_node_dirty->value = zox_dirty_trigger;
            }
        }
    }
} zox_sys_end(VoxelLightSystem);
