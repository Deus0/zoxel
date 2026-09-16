void random_fill_octree(
    VoxelNode* node,
    byte voxel,
    byte depth)
{
    node->value = voxel;
    if (depth > 0) {
        depth--;
        open_one_VoxelNode(node);
        VoxelNode* kids = (VoxelNode*) node->ptr;
        for (int i = 0; i < octree_length; i++) {
            random_fill_octree(
                &kids[i],
                voxel,
                depth);
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

void noise_model_system(iter* it) {
    byte dbg_log = 1;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_out(GenerateModel);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeLock);
    zox_sys_out(ColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(GenerateModel, generate);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(VoxelNodeLock, lock);
        zox_sys_o(ColorRGBs, colors);
        if (generate->value != 1) {
            continue;
        }
        resize_ColorRGBs(colors, 2);
        for (int j = 0; j < 2; j++) {
            colors->value[j] = (color_rgb) {
                rand_range(0, 255),
                rand_range(0, 255),
                rand_range(0, 255)
            };
        }
        spin_lock(&lock->value);
        random_fill_octree(
            voctree,
            1,
            depth->value);
        spin_unlock(&lock->value);
        generate->value = 0;
        zox_remove(e, GenerateModel);
        zox_add(e, VoxelNodeDirty);
        if (dbg_log) {
            zox_log("Noise Chunk Generated [%s]",
                zox_sys_e_name);
        }
    }
    zox_sys_on_end();
} zoxd_system(noise_model_system);
