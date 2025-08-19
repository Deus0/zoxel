// Progress ray in a sunlight direction, stops when solid, decreases when liquid
// TODO: Optimize LightNode System - group same values
// Triggers: VoxelNodeGenerated
void SunlightSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();

    zox_sys_in(VoxelNodeGenerated);
    zox_sys_in(NodeDepth);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNodeGenerated, dirtyv);
        zox_sys_i(NodeDepth, depthr);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightNode, lnode);
        zox_sys_o(LightNodeDepth, depthl);
        zox_sys_o(LightNodeDirty, dirty);

        if (dirtyv->value != zox_dirty_active) {
            continue;
        }

        // we skip if already at right depth
        if (depthl->value >= depthr->value) {
            // zox_logw("Skip Updating lights, light depth already updated");
            continue;
        }

        depthl->value = depthr->value;
        byte length = powers_of_two[depthl->value];

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
        LightQueue* n_queues[6];
        fetch_neightbor_propogation_queues(
            world,
            neighbors,
            n_queues);


        byte queued_dirty = 0;
        entity chunkd = neighbors->value[direction_down];

        // For now we skip unless bottom chunk - due to loading timing
        if (!zox_valid(chunkd)) continue;

        LightQueue* queued = zox_valid(chunkd) ? zox_gett_mut(chunkd, LightQueue) : NULL;

        zox_log_lighting_light("[%s] Topmost Sunbeams l[%i] d[%i]", zox_get_name(it->entities[i]), sunlight, depthl->value);

        // now for all XZ places we go through
        byte3 pos;

        pos.y = length - 1;
        for (pos.x = 0; pos.x < length; pos.x++) {
            for (pos.z = 0; pos.z < length; pos.z++) {

                zox_log_lighting_light("[%s] Begin Topmost Sunbeam [%ix%ix%i] l[%i]", zox_get_name(it->entities[i]), pos.x, pos.y, pos.z, sunlight);

                if (sunbeam(
                    queued,
                    lnode,
                    vnode,
                    depthl->value,
                    pos,
                    sunlight,
                    n_root_vnodes,
                    n_root_lnodes,
                    n_queues,
                    darklight,
                    light_air_decay
                )) {
                    queued_dirty = 1;
                }
            }
        }

        if (queued_dirty) {
            zox_mut_end(chunkd, LightQueue);
        }
        dirty->value = zox_dirty_trigger;

    }
} zoxd_system2(SunlightSystem);
