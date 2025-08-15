void LightPropogateSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SunlightDirty);
    zox_sys_in(LightNodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(LightNode);
    zox_sys_out(PropogateQueue);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(SunlightDirty, dirty);
        zox_sys_i(LightNodeDepth, depthl);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_o(LightNode, root_lnode);
        zox_sys_o(PropogateQueue, queue);

        // sunlight uses RenderDepthDirty
        if (dirty->value != zox_dirty_active) {
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

        // now for all XZ places we go through
        byte length = powers_of_two[depthl->value];
        for (byte x = 0; x < length; x++) {
            for (byte y = 0; y < length; y++) {
                for (byte z = 0; z < length; z++) {

                    byte3 positionl = (byte3) { x, y, z };

                    const VoxelNode* check_node = get_VoxelNode_ex(
                        root_vnode,
                        depthl->value,
                        positionl,
                        0);
                    byte check_voxel = check_node ? check_node->value : 0;

                    // flood with air
                    // TODO: this should be checked in flood light function
                    if (!check_voxel) {

                        const LightNode* lnode = get_LightNode_ex(
                            root_lnode,
                            depthl->value,
                            positionl,
                            0);
                        byte check_light = lnode ? lnode->value : darklight;
                        if (check_light == sunlight) {
                            // zox_log("propogating sunlight.");
                            flood_light_recursive(
                                root_vnode,
                                root_lnode,
                                nnodesv,
                                nnodesl,
                                depthl->value,
                                positionl,
                                check_light,
                                queue,
                                8);
                            // if side voxel, add to other chunks queue
                        }
                    }
                }
            }
        }
    }
} zoxd_system2(LightPropogateSystem);