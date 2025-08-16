// Queued side updates for propogation
// NOTE: We need to account for depth differences

void LightFloodSystem(ecs_iter_t *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LightNodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(LightNode);
    zox_sys_out(PropogateQueue);
    zox_sys_out(LightNodeDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(LightNodeDepth, depthl);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(PropogateQueue, propogate_queue);
        zox_sys_o(LightNodeDirty, dirty);

        if (!propogate_queue->count) {
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
        PropogateQueue* nqueues[6];
        fetch_neightbor_propogation_queues(
            world,
            neighbors,
            nqueues);

        byte updated = 0;

        while (propogate_queue->count) {

            // if (propogate_queue->ptr[queue->count - 1].type != 0) continue;

            PropogateUpdate update = r_PropogateQueue(propogate_queue);

            if (depthl->value != update.depth) continue;   // for now

            /*byte current_light = get_value_LightNode(root_lnode, update.depth, update.pos, 0);

            // zox_log("[%s] Propogation accross chunks [%ix%ix%i] l[%i -> %i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, current_light, update.light);


            if (update.light <= current_light) {
                continue;
            }*/

            if (update.type == 0) {

                // zox_log("[%s] Begin Light Flooding [%ix%ix%i] l[%i -> %i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, current_light, update.light);

                set_LightNode(
                    root_lnode,
                    depthl->value,
                    update.pos,
                    update.light,
                    0
                );

                flood_light(
                    root_vnode,
                    root_lnode,
                    nnodesv,
                    nnodesl,
                    nqueues,
                    depthl->value,
                    update.pos,
                    update.light,
                    update.distance,
                    darklight,
                    light_air_decay
                );
            } /*else {

                // zox_log("[%s] Begin Dark Flooding [%ix%ix%i] l[%i] distance [%i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, update.light, update.distance);

                set_LightNode(
                    root_lnode,
                    depthl->value,
                    update.pos,
                    darklight,
                    0
                );
                dark_flood_light(
                    root_vnode,
                    root_lnode,
                    nnodesv,
                    nnodesl,
                    nqueues,
                    queue,
                    depthl->value,
                    update.pos,
                    update.light,
                    update.distance,
                    darklight,
                    light_air_decay
                );
            }*/

            updated = 1;
        }

        if (updated) {
            dirty->value = zox_dirty_trigger;
        }
    }
} zoxd_system2(LightFloodSystem);
