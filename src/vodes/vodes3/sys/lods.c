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
        VoxelNode* kids = get_children_VoxelNode(node);
        for (byte i = 0; i < octree_length; i++) {
            set_vode_lods(world, &kids[i], render_depth);
        }
    }
}

// NOTE: Sets the World Blocks Depths
void VodesLodSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(RenderDistance);
    zox_sys_in(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDistanceDirty, dirty);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(VoxelNode, node);
        if (dirty->value == zox_dirty_active) {
            byte render_depth = camera_distance_to_block_depth(distance->value);
            // its octree locked while walked, writers can run alongside (thread_safety.md)
            spinlock* lock = zox_octree_lock(world, e);
            zox_octree_lock_begin(lock);
            set_vode_lods(
                world,
                node,
                render_depth);
            zox_octree_lock_end(lock);
        }
    }
    zox_sys_on_end();
} zoxd_system(VodesLodSystem);
