// Queued side updates for propogation
// NOTE: We need to account for depth differences

void LightFloodSystem(ecs_iter_t *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LightNodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_in(DarkQueue);
    zox_sys_out(LightNode);
    zox_sys_out(PropogateQueue);
    zox_sys_out(LightNodeDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(LightNodeDepth, depthl);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(DarkQueue, dark_queue);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(PropogateQueue, propogate_queue);
        zox_sys_o(LightNodeDirty, dirty);

        if (!propogate_queue->count) {
            continue;
        }

        if (dark_queue->count) {
            continue;
        }

        // skip when darkfloodexists
        DarkQueue* n_dark_queues[6];
        fetch_neightbor_dark_queues(
            world,
            neighbors,
            n_dark_queues);

        byte queued_darkness = 0;
        for (int j = 0; j < 6; j++) {
            if (n_dark_queues[j] && n_dark_queues[j]->count) {
                queued_darkness = 1;
                break;
            }
        }
        if (queued_darkness) {
            continue;
        }

        PropogateQueue* nqueues[6];
        fetch_neightbor_propogation_queues(
            world,
            neighbors,
            nqueues);

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

        byte updated = 0;

        // while (propogate_queue->count) {
        for (size_t j = propogate_queue->count; j-- > 0; ) {
            if (propogate_queue->ptr[j].type != 0) continue;

            PropogateUpdate update = r_PropogateQueue(propogate_queue);

            if (depthl->value != update.depth) continue;   // for now

            if (update.type == 0) {

                byte voxel = get_value_VoxelNode(root_vnode, update.depth, update.pos, 0);

                if (voxel) {
                    zox_log_lighting_light("[%s]: Light Flood Canceled at [%ix%ix%i] l[%i] q [%i/%i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, update.light, j, propogate_queue->count);
                    continue;
                }

                byte current_light = get_value_LightNode(root_lnode, update.depth, update.pos, 0);
                byte spread_light = update.light;

                zox_log_lighting_light("[%s]: [%s] Begin Light Flooding at [%ix%ix%i] l[%i] spread [%i] q [%i/%i]", current_light > spread_light ? "Skip" : "Run", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, current_light, spread_light, j, propogate_queue->count);


                if (current_light > spread_light) {
                    spread_light = current_light;
                    // continue;
                } else if (current_light < spread_light) {
                    set_LightNode(
                        root_lnode,
                        depthl->value,
                        update.pos,
                        spread_light,
                        0
                    );
                }

                flood_light(
                    root_vnode,
                    root_lnode,
                    nnodesv,
                    nnodesl,
                    nqueues,
                    depthl->value,
                    update.pos,
                    spread_light,
                    update.distance,
                    darklight,
                    light_air_decay
                );
            }

            updated = 1;
        }

        if (updated) {
            dirty->value = zox_dirty_trigger;
        }
    }
} zoxd_system2(LightFloodSystem);
