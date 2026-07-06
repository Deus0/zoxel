// Sides System will generate our chunk sides before rendering
// TODO: Let Colored use this too

static inline byte is_node_solid(const byte* solidity, const VoxelNode* node) {
    if (!node || !node->value) {
        return 0;
    } else {
        return solidity ? solidity[node->value - 1] : 1;
    }
}

// delves down a voxel node, but only one one side
// Returns 0 if any Air
byte get_node_sides_all_solid(const byte* solidity, const VoxelNode* node, byte direction, byte distance) {
    if (!node) {
        // zox_log_error("get_node_sides_all_solid has invalid node.");
        return 0;
    }
    // at end of node tree, return if solid
    if (!has_children_VoxelNode(node) || !distance) {
        return is_node_solid(solidity, node);
    }
    VoxelNode* kids = get_children_VoxelNode(node);
    if (!kids) {
        zox_loge("get_node_sides_all_solid: null children.");
        return 0;
    }
    distance--;
    for (byte i = 0; i < octree_length; i++) {
        const VoxelNode* child = &kids[i];
        byte3 np = octree_positions_b[i];
        if ((direction == direction_left   && np.x != 0) ||
            (direction == direction_right  && np.x != 1) ||
            (direction == direction_down   && np.y != 0) ||
            (direction == direction_up     && np.y != 1) ||
            (direction == direction_back   && np.z != 0) ||
            (direction == direction_front  && np.z != 1)
        ) {
            continue;
        }
        // check underneath nodes
        if (!get_node_sides_all_solid(solidity, child, direction, distance)) {
            return 0;
        }
    }
    return 1;   // if all children pass, they are all solid
}

// this function accounts for size of drawing voxels
// NOTE: Returns 1 to build the side
static inline byte build_voxel_sides(const byte* solids, const VoxelNode* rvoctree, const VoxelNode** noctrees, const byte* ndepths, const VoxelNode* voctree, SidesOctree* sides, byte depth, byte3 position, byte direction) {
    int3 positioni = byte3_to_int3(position);
    const VoxelNode* adjacent_node = get_adjacentn_VoxelNode(noctrees, rvoctree,  positioni, depth, direction);
    if (!adjacent_node) {
        return 0;
    }
    // Accounts for Dig vs Render Difference
    byte adjacent_solid;
    byte adjacent_depth = get_adjacent_depth(depth, ndepths, positioni, direction);
    if (adjacent_depth > depth) {
        adjacent_solid = adjacent_node && get_node_sides_all_solid(solids, adjacent_node, reverse_direction(direction), adjacent_depth - depth);
    } else {
        // Accounts for null solids
        adjacent_solid = adjacent_node && adjacent_node->value && (!solids || (solids && solids[adjacent_node->value - 1]));
    }
    // Debug These
    return !adjacent_solid || zox_dbg_render_all_sides;
}

static inline byte build_sides_dig(const byte* solids, const VoxelNode* rvoctree, const VoxelNode** noctrees, const byte* ndepths, const VoxelNode* voctree, SidesOctree* sides, byte render_depth, byte depth, byte3 position) {
    /*if (!sides) {
        zox_loge("Sides null at [%ix%ix%i:%i]", position.x, position.y, position.z, depth);
        return 0;
    }*/
    // if air we stop here at any branch node
    if (!voctree->value) {
        // collapse sub node and set to 0
        // SidesOctree* child_sides = sides;
        sides->value = 0;
        if (sides->ptr) {
            collapse_SidesOctree(sides);
        }
        return 0;
    }
    // We should keep digging even when it's closed
    // keep digging
    byte has_vkids = !is_closed_VoxelNode(voctree);
    if (depth < render_depth && (zox_split_textured_quads || (!zox_split_textured_quads && has_vkids))) {
        byte3 cposition = position;
        byte3_multiply_byte(&cposition, 2);
        if (!sides->ptr) {
            open_SidesOctree(sides);
            // NOTE: If fails malloc
            if (!sides->ptr) {
                return 0;
            }
        }
        SidesOctree* sides_kids = (SidesOctree*) sides->ptr;
        const VoxelNode* kids = has_vkids ? get_children_VoxelNode(voctree) : NULL;
        byte did_build = 0;
        for (byte i = 0; i < 8; i++) {
            SidesOctree* sides_kid = &sides_kids[i];
            const VoxelNode* child_voxel = has_vkids ? &kids[i] : voctree;
            byte3 nposition = byte3_add(cposition, octree_positions_b[i]);
            if (build_sides_dig(solids, rvoctree, noctrees, ndepths, child_voxel, sides_kid, render_depth, depth + 1, nposition)) {
                did_build = 1;
            }
        }
        // set 1 if built for Branch Nodes
        if (did_build) {
            sides->value = did_build;
        }
        return did_build;
    }
    byte sides_value = 0;
    // NOTE: if solid, check all sides, Set side 1 if Air
    if (!solids || solids[voctree->value - 1]) {
        for (byte direction = 0; direction < 6; direction++) {
            if (build_voxel_sides(solids, rvoctree, noctrees, ndepths, voctree, sides, depth, position, direction)) {
                sides_value |= (1 << (direction + 1));
            }
        }
    }
    sides->value = sides_value;
    if (!sides_value) {
        collapse_SidesOctree(sides);
    }
    return sides_value;
}

// Make sure BlockManagerLink is first one
byte* blocks_fetch_solids(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    entity manager = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(BlockManagerLink, blocker);
        if (!zox_valid(blocker->value)) {
            continue;
        }
        manager = blocker->value;
        break;
    }
    if (!manager) {
        return NULL;
    }
    zox_geter(manager, BlockLinks, blocks);
    if (!blocks->length) {
        return NULL;
    }
    byte* solids = malloc(blocks->length * sizeof(byte));
    for (int i = 0; i < blocks->length; i++) {
        entity block = blocks->value[i];
        if (!zox_valid(block) || !zox_has(block, BlockModel)) {
            solids[i] = 1;
            continue;
        }
        solids[i] = zox_getv(block, BlockModel) == zox_block_solid;
    }
    return solids;
}

zox_sys2(Chunk3SidesSystem) {
    byte* solids = blocks_fetch_solids(it);
    if (!solids) {
        return;
    }
    zox_sys_world();
    zox_sys_begin_at(1);
    zox_sys_in(ChunkMeshDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(SidesOctree);
    zox_sys_out(SidesOctreeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ChunkMeshDirty, cdirty);
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, voctree);
        zox_sys_o(SidesOctree, sides);
        zox_sys_o(SidesOctreeDirty, sdirty);
        if (cdirty->value != zox_dirty_active) {
            continue;
        }
        if (render_depth->value == render_depth_uninitialized) {
            sides->value = 0;
            continue;
        }
        const VoxelNode *noctrees[6];
        byte ndepths[6];
        fetch_neightbor_chunk_data(world, neighbors, noctrees, ndepths);
        sides->value = build_sides_dig(solids, voctree, noctrees, ndepths, voctree, sides, render_depth->value, 0, byte3_zero);
        sdirty->value = zox_dirty_trigger;
        zox_sys_increment();
    }
    free(solids);
} zox_sys_end(Chunk3SidesSystem);
