void vox_outlines(VoxelNode *chunk, byte node_depth, byte black_voxel) {
    short length = octree_size(node_depth);
    byte3 size = byte3_single(length);
    byte3 voxel_position;
    for (voxel_position.x = 0; voxel_position.x < size.x; voxel_position.x++) {
        for (voxel_position.y = 0; voxel_position.y < size.y; voxel_position.y++) {
            for (voxel_position.z = 0; voxel_position.z < size.z; voxel_position.z++) {
                byte on_edges = byte3_on_edges(voxel_position, size);
                if (on_edges) {
                    // skip if air
                    // byte3 temp_position_2 = voxel_position;
                    /*if (get_sub_node_voxel(chunk, &temp_position_2, node_depth) == 0) {
                        continue;
                    }*/
                    if (getv_VoxelNode(chunk, node_depth, voxel_position) == 0) {
                        continue;
                    }
                    if (!set_VoxelNode(chunk, node_depth, voxel_position, black_voxel)) {
                        zox_logw("Set Voxel Failure [%ix%ix%i]", voxel_position.x, voxel_position.x, voxel_position.z);
                    }
                    //byte2 set_voxel = (byte2) { black_voxel, node_depth };
                    //byte3 temp_position = voxel_position;
                    //set_octree_voxel(chunk, &temp_position, &set_voxel, 0);
                }
            }
        }
    }
}
