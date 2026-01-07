// Move darkbeam and dark propogation here

// Queued side updates for propogation
// NOTE: We need to account for depth differences

void DarkLightSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_in(VoxLink);
    zox_sys_out(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(DarkQueue);
    zox_sys_out(LightQueue);
    zox_sys_out(LightNodeDirty);

    byte solidity[255];
    for (int j = 0; j < 255; j++) solidity[j] = 1;
    fetch_first_solidity(world, it, VoxLink_, solidity);

    for (int i = 0; i < it->count; i++) {
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

        const VoxelNode* nnodesv[6];
        fetch_neightbor_voxel_nodes(
            world,
            neighbors,
            nnodesv);
        const LightNode* nnodesl[6];
        fetch_neightbor_light_nodes(
            world,
            neighbors,
            nnodesl);
        LightQueue* n_light_queues[6];
        fetch_neightbor_propogation_queues(
            world,
            neighbors,
            n_light_queues);
        DarkQueue* n_dark_queues[6];
        fetch_neightbor_dark_queues(
            world,
            neighbors,
            n_dark_queues);

        byte dirty = 0;
        entity chunkd = neighbors->value[direction_down];
        DarkQueue* dark_queued = zox_valid(chunkd) ? zox_gett_mut(chunkd, DarkQueue) : NULL;

        // int count = dark_queue->count;
        while (dark_queue->count) {

            DarkUpdate update = r_DarkQueue(dark_queue);

            if (depthl->value < update.depth) {
                depthl->value = update.depth;
            }

            // if (depthl->value != update.depth) continue;   // for now

            if (update.type == zox_light_type_flood) {

                zox_log_lighting_dark("[%s] Begin Dark Flooding [%ix%ix%i] l[%i] distance [%i] q [%i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, update.light, update.distance, dark_queue->count)

                byte current_light = get_value_LightNode(root_lnode, depthl->value, update.pos, 0);
                if (current_light > darklight) {
                    set_LightNode(
                        root_lnode,
                        depthl->value,
                        update.pos,
                        darklight,
                        0
                    );
                    dirty = 1;
                }

                if (dark_flood_light(
                    root_vnode,
                    root_lnode,
                    nnodesv,
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
                    solidity
                )) {
                    dirty = 1;
                }

            } else if (update.type == zox_light_type_beam_start || update.type == zox_light_type_beam) {

                zox_log_lighting_dark("[%s] Begin Darkbeam [%ix%ix%i] l[%i] q [%i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, update.light, dark_queue->count);

                if (dark_sunbeam(
                    dark_queued,
                    root_vnode,
                    root_lnode,
                    nnodesv,
                    nnodesl,
                    n_light_queues,
                    light_queue,
                    n_dark_queues,
                    dark_queue,
                    depthl->value,
                    update.pos,
                    sunlight,
                    darklight,
                    light_air_decay,
                    update.type,
                    solidity
                )) {
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
} zoxd_system2(DarkLightSystem);
