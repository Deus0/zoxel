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
        VoxelNode* kids = get_children_VoxelNode(node);
        for (int i = 0; i < octree_length; i++) {
            remove_vodes(world, &kids[i]);
        }
    }
}

// cleans up vodes attached to air
void VodesRemoveSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(VoxelNode, node);
        // its octree locked while written, readers can run alongside (thread_safety.md)
        spinlock* write_lock = zox_octree_lock(world, e);
        zox_octree_lock_begin(write_lock);
        remove_vodes(world, node);
        zox_octree_lock_end(write_lock);
    }
    zox_sys_on_end();
} zoxd_system(VodesRemoveSystem);