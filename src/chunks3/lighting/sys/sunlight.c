// Progress ray in a sunlight direction, stops when solid, decreases when liquid
// TODO: Optimize LightNode System - group same values
// Triggers: VoxelNodeGenerated
void SunlightSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();

    zox_sys_in(RenderDepth);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNodeGenerated);

    zox_sys_out(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    zox_sys_out(MeshColorsGenerate);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNodeGenerated, dirtyv);
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

        // we skip if already at right depth
        if (depthl->value >= depthr->value) {
            zox_logw("Skip Updating lights, light depth already updated");
            continue;
        }
        depthl->value = depthr->value;

        byte queued_dirty = 0;
        entity chunkd = neighbors->value[direction_down];
        SunlightQueue* queued = zox_valid(chunkd) ? zox_gett_mut(chunkd, SunlightQueue) : NULL;

        // now for all XZ places we go through
        byte length = powers_of_two[depthl->value];
        byte3 pos;
        pos.y = length;
        for (pos.x = 0; pos.x < length; pos.x++) {
            for (pos.z = 0; pos.z < length; pos.z++) {
                if (sunbeam(
                    queued,
                    lnode,
                    vnode,
                    depthl->value,
                    pos,
                    sunlight
                )) {
                    queued_dirty = 1;
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
