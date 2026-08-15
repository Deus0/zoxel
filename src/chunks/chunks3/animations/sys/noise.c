void random_fill_octree(VoxelNode* node, byte voxel, byte depth) {
    node->value = voxel;
    if (depth > 0) {
        depth--;
        open_one_VoxelNode(node);
        VoxelNode* kids = (VoxelNode*) node->ptr;
        for (int i = 0; i < octree_length; i++) {
            random_fill_octree(&kids[i], voxel, depth);
        }
    } else {
        if (rand() % 101 >= fill_octree_random_rate) {
            node->value = 0;
        } else if (rand() % 101 >= fill_octree_random_rate2) {
            node->value = 1;
        } else if (rand() % 101 >= fill_octree_random_rate3) {
            node->value = 2;
        }
    }
}

zox_sys2(NoiseVoxelNodeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_out(GenerateModel);
    zox_sys_out(VoxelNode);
    zox_sys_out(ColorRGBs);
    // zox_sys_out(BuildMesh);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(GenerateModel, generate);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(ColorRGBs, colors);
        // zox_sys_o(BuildMesh, build_mesh);
        if (generate->value != 1) {
            continue;
        }
        resize_ColorRGBs(colors, 2);
        for (int j = 0; j < 2; j++) {
            colors->value[j] = (color_rgb) { rand_range(0, 255), rand_range(0, 255), rand_range(0, 255) };
        }
        // write_lock_VoxelNode(voctree);
        random_fill_octree(voctree, 1, depth->value);
        // write_unlock_VoxelNode(voctree);
        // build_mesh->value = zox_build_chunk_mesh_run;
        zox_setv(e, BuildMesh, zox_build_chunk_mesh_run);
        generate->value = 0;
    }
} zox_sys_end(NoiseVoxelNodeSystem);
