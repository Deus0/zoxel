// Progress ray in a sunlight direction, stops when solid, decreases when liquid
// TODO: Optimize LightNode System - group same values

// Triggers: VoxelNodeGenerated
void SunlightSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();

    zox_sys_in(VoxelNodeGenerated);
    zox_sys_in(VoxelNodeDirty);
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);

    zox_sys_out(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    zox_sys_out(MeshColorsGenerate);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNodeGenerated, dirtyv);
        zox_sys_i(VoxelNodeDirty, dirtyv2);
        zox_sys_i(RenderDepthDirty, dirtyr);
        zox_sys_i(RenderDepth, depthr);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_i(ChunkNeighbors, neighbors);

        zox_sys_o(LightNode, lnode);
        zox_sys_o(LightNodeDepth, depthl);
        zox_sys_o(LightNodeDirty, updated);
        zox_sys_o(MeshColorsGenerate, updated2);

        if (dirtyv->value != zox_dirty_active) {
            continue;
        }

        // sunlight uses RenderDepthDirty
        if (dirtyr->value != zox_dirty_active) {
            // continue;
        }

        // we skip if already at right depth
        if (depthl->value >= depthr->value) {
            // zox_log("Skip Updating lights");
            // continue;
        }
        depthl->value = depthr->value;

        byte queued_dirty = 0;
        entity chunkd = neighbors->value[direction_down];
        SunlightQueue* queued = zox_valid(chunkd) ? zox_gett_mut(chunkd, SunlightQueue) : NULL;

        // now for all XZ places we go through
        byte length = powers_of_two[depthl->value];
        for (byte x = 0; x < length; x++) {
            for (byte z = 0; z < length; z++) {

                // now we progress down
                byte light = sunlight; // full sunlight
                for (byte y = 0; y < length; y++) {
                    byte3 positionl = (byte3) { x, length - 1 - y, z };

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
                            light = darklight; // sunlight blocked
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
                    queued_dirty = 1;
                    a_SunlightQueue(queued, (SunlightUpdate) {
                        .positionl = { .x = x, .y = z }
                    });
                }
            }
        }

        if (queued_dirty) {
            zox_mut_end(chunkd, SunlightQueue);
        }

        // zox_log("Updated sunlight.");
        updated->value = zox_dirty_trigger;
        updated2->value = zox_dirty_trigger; // for now just put here

    }
} zoxd_system2(SunlightSystem);
