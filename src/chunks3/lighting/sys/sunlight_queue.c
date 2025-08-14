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
    zox_sys_out(LightNodeDirty);
    zox_sys_out(MeshColorsGenerate);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(RenderDepth, depthr);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_i(ChunkNeighbors, neighbors);

        zox_sys_o(SunlightQueue, queue);
        zox_sys_o(LightNode, lnode);
        zox_sys_o(LightNodeDepth, depthl);
        zox_sys_o(LightNodeDirty, updated);
        zox_sys_o(MeshColorsGenerate, updated2);

        if (!queue->count) {
            continue;
        }

        // we skip if already at right depth
        if (depthl->value < depthr->value) {
            depthl->value = depthr->value;
        }

        byte queued_dirty = 0;
        entity chunkd = neighbors->value[direction_down];
        SunlightQueue* queued = zox_valid(chunkd) ? zox_gett_mut(chunkd, SunlightQueue) : NULL;

        byte length = powers_of_two[depthl->value];

        while (queue->count) {

            SunlightUpdate update = r_SunlightQueue(queue);
            byte2 pos = update.positionl;

            if (pos.x >= length || pos.y >= length) {
                zox_log_error("[r_SunlightQueue] position oob [%ix%i]", pos.x, pos.y);
                continue;
            }

            // beam down with positionl
            // now we progress down
            byte light = sunlight; // full sunlight

            for (byte y = 0; y < length; y++) {
                byte3 positionl = (byte3) {
                    pos.x,
                    length - 1 - y,
                    pos.y
                };

                if (light != darklight) {

                    const VoxelNode* check_node = get_VoxelNode_ex(
                        vnode,
                        depthl->value,
                        positionl,
                        0);

                    if (check_node) {
                        byte voxel = check_node->value;
                        if (voxel) {
                            light = darklight;
                        }
                    } else {
                        light = darklight;
                    }
                }

                // set light in LightNode
                set_LightNode_ex(
                    lnode,
                    depthl->value,
                    positionl,
                    light,
                    0);
            }
            if (light == sunlight && queued) {
                // add to queue of under chunk
                a_SunlightQueue(queued, (SunlightUpdate) {
                    .positionl = { .x = pos.x, .y = pos.y }
                });
                queued_dirty = 1;
            }
        }

        if (queued_dirty) {
            zox_mut_end(chunkd, SunlightQueue);
        }

        updated->value = zox_dirty_trigger;
        updated2->value = zox_dirty_trigger;
    }
} zoxd_system2(SunlightQueueSystem);