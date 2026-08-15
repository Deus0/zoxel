void destroy_vodes(ecs *world, VoxelNode *node) {
    if (is_closed_VoxelNode(node)) {
        return;
    } else if (is_linked_VoxelNode(node)) {
        if (!destroy_node_link_VoxelNode(world, node)) {
            zox_loge("failed to destroy voxel instance")
        }
    } else if (has_children_VoxelNode(node)) {
        VoxelNode* kids = (VoxelNode*) node->ptr;
        for (int i = 0; i < octree_length; i++) {
            destroy_vodes(world, &kids[i]);
        }
    }
}

// NOTE: Destroys World Blocks when they are out of range
zox_sys2(VodesDespawnSystem) {
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(RenderDistanceDirty)
    zox_sys_in(RenderDepth)
    zox_sys_out(VoxelNode)
    zox_sys_out(BlocksSpawned)
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDistanceDirty, distance_dirty);
        zox_sys_i(RenderDepth, depth);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(BlocksSpawned, spawned);
        if (!spawned->value) {
            continue;
        }
        /*if (voxels_dirty->value) {
            continue;
        }*/
        if (distance_dirty->value != zox_dirty_active) {
            continue;
        }
        // only destroy if outside range
        entity terrain = zox_get_parent(world, e);
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        byte in_spawn_zone = depth->value == terrain_depth; // depth->value <= block_vox_render_at_lod
        if (in_spawn_zone) {
            continue;
        }
        // write_lock_VoxelNode(node);
        destroy_vodes(world, node);
        // write_unlock_VoxelNode(node);
        spawned->value = 0;
    }
} zox_sys_end(VodesDespawnSystem);
