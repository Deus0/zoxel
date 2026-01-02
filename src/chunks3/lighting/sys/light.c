// Queued side updates for propogation
void LightSystem(iter *it) {
    zox_ts_begin(light_propogate);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxLink);
    zox_sys_out(LightNode);
    zox_sys_out(LightQueue);
    zox_sys_out(LightNodeDirty);

    byte solidity[255];
    for (int j = 0; j < 255; j++) solidity[j] = 1;
    fetch_first_solidity(world, it, VoxLink_, solidity);

    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(LightQueue, light_queue);
        zox_sys_o(LightNodeDirty, light_node_dirty);

        if (!light_queue->count) {
            continue;
        }

        const VoxelNode* n_root_vnodes[6];
        fetch_neightbor_voxel_nodes(
            world,
            neighbors,
            n_root_vnodes);
        const LightNode* n_root_lnodes[6];
        fetch_neightbor_light_nodes(
            world,
            neighbors,
            n_root_lnodes);
        LightQueue* n_light_queues[6];
        fetch_neightbor_propogation_queues(
            world,
            neighbors,
            n_light_queues);

        entity chunkd = neighbors->value[direction_down];

        // For now we skip for bottom chunk - due to loading timing
        if (!zox_valid(chunkd)) {
            continue;
        }

        LightQueue* sun_queued = zox_valid(chunkd) ? zox_gett_mut(chunkd, LightQueue) : NULL;

        byte dirty = 0;
        // int count = light_queue->count;
        while (light_queue->count) {

            LightUpdate update = r_LightQueue(light_queue);
            byte3 pos = update.pos;
            byte length = powers_of_two[update.depth];

            //if (depthl->value < update.depth) { // depthr->value) {
                //depthl->value = update.depth; // depthr->value;
            //}

            if (pos.x >= length || pos.z >= length || pos.y >= length) {
               //  zox_log_error("[r_LightQueue] position oob [%ix%ix%i]", pos.x, pos.y, pos.z);
                continue;
            }

            // TODO: Add type for light/dark beams
            // TODO: we should probably make this byte3, with y, since we are gonna be used that now
            zox_log_lighting_light("Sunbeam [%ix%ix%i]", pos.x, pos.y, pos.y);

            if (update.type == zox_light_type_beam) {

                zox_log_lighting_light("[%s] ^ Sunbeam [%ix%ix%i] l[%i] q [%i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, update.light, light_queue->count);

                if (sunbeam(
                    sun_queued,
                    root_lnode,
                    root_vnode,
                    update.depth,
                    update.pos,
                    update.light,
                    n_root_vnodes,
                    n_root_lnodes,
                    n_light_queues,
                    darklight,
                    light_air_decay,
                    solidity
                )) {
                    dirty = 1;
                }

            } else if (update.type == zox_light_type_flood) {

                byte voxel = get_value_VoxelNode(root_vnode, update.depth, update.pos, 0);

                if (voxel && solidity[voxel - 1]) {
                    zox_log_lighting_light("[%s]: Light Flood Canceled at [%ix%ix%i] l[%i] q [%i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, update.light,  light_queue->count);
                    continue;
                }

                byte current_light = get_value_LightNode(root_lnode, update.depth, update.pos, 0);
                byte spread_light = update.light;

                zox_log_lighting_light("[%s]: [%s] ^ Light Flooding at [%ix%ix%i] l[%i] spread [%i] q [%i]", current_light > spread_light ? "Skip" : "Run", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, current_light, spread_light, light_queue->count);

                if (current_light > spread_light) {
                    spread_light = current_light;
                } else if (current_light < spread_light) {
                    set_LightNode(
                        root_lnode,
                        update.depth,
                        update.pos,
                        spread_light,
                        0
                    );
                    dirty = 1;
                }

                if (flood_light(
                    root_vnode,
                    root_lnode,
                    n_root_vnodes,
                    n_root_lnodes,
                    n_light_queues,
                    update.depth,
                    update.pos,
                    spread_light,
                    update.distance,
                    darklight,
                    light_air_decay,
                    solidity
                )) {
                    dirty = 1;
                }

            }
        }
        zox_mut_end(chunkd, LightQueue);

        if (dirty) {
            light_node_dirty->value = zox_dirty_trigger;
            /*zox_sys_world();
            zox_sys_e();
            zox_log("LightSystem %s - %i", zox_get_name(e), count);*/
        }
    }
    zox_ts_end(light_propogate, 3, zox_profile_light_propogate);
} zoxd_system2(LightSystem);