void LightPropogateBatchSystem(iter *it) {
    zox_ts_begin(light_propogate);

    // TODO: Make this do inside sunbeam instead, no need here unless voxel a light source

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SunlightDirty);
    zox_sys_in(LightNodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(SunlightDirty, sunlight_dirty);
        zox_sys_i(LightNodeDepth, depthl);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(LightNodeDirty, light_node_dirty);

        // sunlight uses RenderDepthDirty
        if (sunlight_dirty->value != zox_dirty_active) {
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

        // now for all XZ places we go through
        byte length = powers_of_two[depthl->value];
        for (byte x = 0; x < length; x++) {
            for (byte y = 0; y < length; y++) {
                for (byte z = 0; z < length; z++) {

                    byte3 positionl = (byte3) { x, y, z };

                    // flood with air
                    // TODO: this should be checked in flood light function - flood_light_start - checks voxel before spreading?

                    const VoxelNode* check_node = get_VoxelNode(
                        root_vnode,
                        depthl->value,
                        positionl,
                        0
                    );
                    byte check_voxel = check_node ? check_node->value : 0;

                    if (check_voxel) {
                        continue;
                    }

                    const LightNode* lnode = get_LightNode(
                        root_lnode,
                        depthl->value,
                        positionl,
                        0
                    );
                    byte check_light = lnode ? lnode->value : darklight;
                    if (check_light != sunlight) {
                        continue;
                    }
                    flood_light(
                        root_vnode,
                        root_lnode,
                        nnodesv,
                        nnodesl,
                        nqueues,
                        depthl->value,
                        positionl,
                        check_light,
                        light_propogation_distance,
                        darklight,
                        light_air_decay
                    );

                }
            }
        }
        light_node_dirty->value = 1;
    }
    zox_ts_end(light_propogate, 3, zox_profile_light_propogate);
} zoxd_system2(LightPropogateBatchSystem);