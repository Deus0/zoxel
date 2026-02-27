void destroy_vodes(ecs *world, VoxelNode *node) {
    if (is_closed_VoxelNode(node)) {
        return;
    } else if (is_linked_VoxelNode(node)) {
        if (!destroy_node_link_VoxelNode(world, node)) {
            zox_log_error("failed to destroy voxel instance")
        }
    } else if (has_children_VoxelNode(node)) {
        VoxelNode* kids = get_children_VoxelNode(node);
        for (int i = 0; i < octree_length; i++) {
            destroy_vodes(world, &kids[i]);
        }
    }
}

zox_sys2(VodesDespawnSystem) {
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(VoxelNodeDirty)
    zox_sys_in(RenderDistanceDirty)
    zox_sys_in(RenderDepth)
    zox_sys_in(VoxLink)
    zox_sys_out(VoxelNode)
    zox_sys_out(BlocksSpawned)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxelNodeDirty, voxelNodeDirty)
        zox_sys_i(RenderDistanceDirty, renderDistanceDirty)
        zox_sys_i(RenderDepth, renderDepth)
        zox_sys_i(VoxLink, vox_link)
        zox_sys_o(VoxelNode, node)
        zox_sys_o(BlocksSpawned, blocksSpawned)
        if (voxelNodeDirty->value != zox_dirty_active && !(blocksSpawned->value && renderDistanceDirty->value == zox_dirty_active)) {
            continue;
        }
        // only destroy if outside range
        zox_geter_value(vox_link->value, NodeDepth, byte, terrain_depth);
        byte can_spawn_vodes = renderDepth->value == terrain_depth;
        if (can_spawn_vodes) { // renderDepth->value <= block_vox_render_at_lod) {
            continue;
        }
        write_lock_VoxelNode(node);
        destroy_vodes(world, node);
        write_unlock_VoxelNode(node);
        blocksSpawned->value = 0;
    }
} zox_sys_end(VodesDespawnSystem);