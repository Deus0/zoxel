// Queued side updates for propogation
void SunlightQueueSystem(ecs_iter_t *it) {

    zox_sys_world();
    zox_sys_begin();

    zox_sys_in(RenderDepth);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);

    zox_sys_out(SunlightQueue);
    zox_sys_out(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(SunlightDirty);
    zox_sys_out(PropogateQueue);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(RenderDepth, depthr);
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(ChunkNeighbors, neighbors);

        zox_sys_o(SunlightQueue, queue);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(LightNodeDepth, depthl);
        zox_sys_o(SunlightDirty, sunlight_dirty);
        zox_sys_o(PropogateQueue, propogation_queue);

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

        // we skip if already at right depth
        if (depthl->value < depthr->value) {
            depthl->value = depthr->value;
        }

        byte queued_dirty = 0;
        entity chunkd = neighbors->value[direction_down];

        // For now we skip unless bottom chunk - due to loading timing
        if (!zox_valid(chunkd)) continue;

        SunlightQueue* sun_queued = zox_valid(chunkd) ? zox_gett_mut(chunkd, SunlightQueue) : NULL;

        byte length = powers_of_two[depthl->value];

        while (queue->count) {

            SunlightUpdate update = r_SunlightQueue(queue);
            byte3 pos = update.pos;

            if (pos.x >= length || pos.z >= length || pos.y > length) {
               //  zox_log_error("[r_SunlightQueue] position oob [%ix%ix%i]", pos.x, pos.y, pos.z);
                continue;
            }

            // TODO: Add type for light/dark beams
            // TODO: we should probably make this byte3, with y, since we are gonna be used that now
            // zox_log("Processed SunBeam [%ix%i]", pos.x, pos.y);
            if (update.type == 0 || update.type == 2) {

                if (sunbeam(
                    sun_queued,
                    root_lnode,
                    root_vnode,
                    depthl->value,
                    update.pos,
                    update.light,
                    update.type
                )) {
                    queued_dirty = 1;
                }

            } else {

                // zox_log("[%s] Dark Beam [%ix%ix%i] l[%i]", zox_get_name(it->entities[i]), update.pos.x, update.pos.y, update.pos.z, sunlight);

                if (dark_sunbeam(
                    sun_queued,
                    root_vnode,
                    root_lnode,
                    nnodesv,
                    nnodesl,
                    nqueues,
                    propogation_queue,
                    depthl->value,
                    update.pos,
                    sunlight,
                    darklight,
                    light_air_decay
                )) {
                    queued_dirty = 1;
                }
            }
        }

        if (queued_dirty) {
            zox_mut_end(chunkd, SunlightQueue);
        }

        // propogation: we can just add to propogation queue here,no need to run
        sunlight_dirty->value = zox_dirty_trigger;

    }
} zoxd_system2(SunlightQueueSystem);