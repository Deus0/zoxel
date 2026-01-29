// todo: make sure we only close blocks that can be grouped together here (we shouldn't group grass etc)
// doesn't close any block voxes

// if air but any children are not air, set to the first solid voxel
void optimize_voxel_solids(VoxelNode* voctree) {

    if (!voctree || !has_children_VoxelNode(voctree)) {
        return;
    }

    // dig first
    VoxelNode* kids = get_children_VoxelNode(voctree);
    for (byte i = 0; i < octree_length; i++) {
        optimize_voxel_solids(&kids[i]);
    }

    if (voctree->value) {
        return;
    }

    byte any_solid = 1;

    for (byte i = 0; i < octree_length; i++) {
        VoxelNode* child = &kids[i];

        if (child->value) {
            any_solid = child->value;
            break;
        }
    }

    if (any_solid) {
        voctree->value = any_solid;
    }
}


void reduce_voxel_octrees(ecs* world, VoxelNode* voctree) {

    if (!voctree || !has_children_VoxelNode(voctree)) {
        return;
    }

    // dig first
    VoxelNode* kids = get_children_VoxelNode(voctree);
    for (byte i = 0; i < octree_length; i++) {
        reduce_voxel_octrees(world, &kids[i]);
    }

    byte same_type = 255;

    for (byte i = 0; i < octree_length; i++) {
        VoxelNode* child = &kids[i];

        if (is_opened_VoxelNode(child)) {
            return; // if a child voctree is open, then don't close this voctree
        }

        if (same_type == 255) {
            same_type = child->value;
        } else if (same_type != child->value) {
            same_type = 255;
            break;
        }
    }

    if (same_type != 255) {
        close_VoxelNode(world, voctree);
        voctree->value = same_type;
    }
}

// Break down our voctrees, if they are the same type
// When VoxelNodeDirty is zox_dirty_active
zox_sys2(VoxelNodeCleanupSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_out(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxelNodeDirty, dirty);
        zox_sys_o(VoxelNode, voctree);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        write_lock_VoxelNode(voctree);

            reduce_voxel_octrees(world, voctree);

            optimize_voxel_solids(voctree);

        write_unlock_VoxelNode(voctree);
    }
} zox_sys_end(VoxelNodeCleanupSystem);