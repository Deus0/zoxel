void set_vode_lods(ecs *world, const VoxelNode *node, byte render_depth) {
    if (is_closed_VoxelNode(node)) {
        return;
    } else if (is_linked_VoxelNode(node)) {
        entity e = get_entity_VoxelNode(node);
        if (zox_valid(e) && zox_has(e, RenderDepth)) {
            byte current_render_depth = zox_getv(e, RenderDepth);
            if (current_render_depth != render_depth) {
                zox_set(e, RenderDepth, { render_depth });
                zox_set(e, RenderDepthDirty, { zox_dirty_trigger });
            }
        }
    } else if (has_children_VoxelNode(node)) {
        VoxelNode* kids = (VoxelNode*) node->ptr;
        for (byte i = 0; i < octree_length; i++) {
            set_vode_lods(world, &kids[i], render_depth);
        }
    }
}

// NOTE: Sets the World Blocks Depths
zox_sys2(VodesLodSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(RenderDistance);
    zox_sys_in(VoxelNode);
    zox_sys_in(BlocksSpawned);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDistanceDirty, dirty);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(BlocksSpawned, spawned);
        zox_sys_i(VoxelNode, node);
        if (dirty->value == zox_dirty_active && spawned->value) {
            byte render_depth = camera_distance_to_block_vox_depth(distance->value);
            set_vode_lods(world, node, render_depth);
        }
    }
} zox_sys_end(VodesLodSystem);
