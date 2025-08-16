// Queued side updates for propogation
// NOTE: We need to account for depth differences

void PropogateQueueSystem(ecs_iter_t *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LightNodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(LightNode);
    zox_sys_out(SunlightDirty);
    zox_sys_out(PropogateQueue);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(LightNodeDepth, depthl);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(SunlightDirty, dirty);
        zox_sys_o(PropogateQueue, queue);

        if (!queue->count) {
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

        while (queue->count) {

            PropogateUpdate update = r_PropogateQueue(queue);

            if (depthl->value != update.depth) continue;   // for now

            // zox_log("Propogation accross chunks [%ix%ix%i] l[%i]", update.pos.x, update.pos.y, update.pos.z, update.light);

            if (update.type == 0) {
                set_LightNode_ex(root_lnode, depthl->value, update.pos, update.light, 0);

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
            } else {
                zox_log("Reverse Flooding Lights [%ix%ix%i] l[%i]",
                        update.pos.x, update.pos.y, update.pos.z, update.light);
                set_LightNode_ex(root_lnode, depthl->value, update.pos, darklight, 0);
                flood_light_remove(
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
            }


        }

        if (updated) {
            dirty->value = zox_dirty_trigger;
        }
    }
} zoxd_system2(PropogateQueueSystem);
