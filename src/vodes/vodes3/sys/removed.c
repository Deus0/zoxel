void remove_vodes(ecs *world, VoxelNode *node) {
    if (is_closed_VoxelNode(node)) {
        return;
    } else if (is_linked_VoxelNode(node)) {
        if (!node->value) { // if air
            if (is_linked_VoxelNode(node)) {
                destroy_node_link_VoxelNode(world, node);
            }
        }
    } else if (has_children_VoxelNode(node)) {
        VoxelNode* kids = (VoxelNode*) node->ptr;
        for (int i = 0; i < octree_length; i++) {
            remove_vodes(world, &kids[i]);
        }
    }
}

// cleans up vodes attached to air
zox_sys2(VodesRemoveSystem) {
    zox_sys_world()
    zox_sys_begin()
    // zox_sys_in(VoxelNodeDirty)
    zox_sys_in(BlocksSpawned)
    zox_sys_out(VoxelNode)
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // zox_sys_i(VoxelNodeDirty, voxelNodeDirty)
        zox_sys_i(BlocksSpawned, blocksSpawned)
        zox_sys_o(VoxelNode, node)
        if (zox_has(e, VoxelNodeDirty) &&
            blocksSpawned->value) {
            // write_lock_VoxelNode(node);
            remove_vodes(world, node);
            // write_unlock_VoxelNode(node);
        }
    }
} zox_sys_end(VodesRemoveSystem);