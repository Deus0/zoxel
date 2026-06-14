// Sides System will generate our chunk sides before rendering
// TODO: Let Colored use this too

byte is_node_solid(const byte* solidity, const VoxelNode* node) {
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
    const VoxelNode* anode = get_adjacentn_VoxelNode(noctrees, rvoctree,  byte3_to_int3(position), depth, direction);
    if (!anode) {
        return 0;
    }
    // Accounts for Dig vs Render Difference
    byte adepth = get_adjacent_depth(depth, ndepths, byte3_to_int3(position), direction);
    byte asolid;
    if (adepth > depth) {
        asolid = anode && get_node_sides_all_solid(solids, anode, reverse_direction(direction), adepth - depth);
    } else {
        asolid = anode && anode->value &&
        // Accounts for null solids
        (!solids || (solids && solids[anode->value - 1]));
    }
    // Debug These
    // if (asolid && adepth != depth) asolid = 0;
    // if (asolid && is_on_edge_VoxelNode(depth, position, direction)) asolid = 0;
    return !asolid || zox_dbg_render_all_sides;
}

static inline byte build_sides_dig(const byte* solids, const VoxelNode* rvoctree, const VoxelNode** noctrees, const byte* ndepths, const VoxelNode* voctree, SidesOctree* sides, byte rdepth, byte depth, byte3 position) {
    // if air we stop here at any branch node
    if (!voctree->value) {
        // collapse sub node and set to 0
        SidesOctree* csides = getm_SidesOctree(sides, depth, position, 0);
        if (csides) {
            csides->value = 0;
            collapse_SidesOctree(csides);
        }
        return 0;
    }
    // We should keep digging even when it's closed
    // keep digging
    byte has_vkids = !is_closed_VoxelNode(voctree);
    if (depth < rdepth && (zox_split_textured_quads || (!zox_split_textured_quads && has_vkids))) {
        byte3 cposition = position;
        byte3_multiply_byte(&cposition, 2);
        const VoxelNode* kids = has_vkids ? get_children_VoxelNode(voctree) : NULL;
        byte did_build = 0;
        for (byte i = 0; i < 8; i++) {
            const VoxelNode* cvoctree = has_vkids ? &kids[i] : voctree;
            byte3 nposition = byte3_add(cposition, octree_positions_b[i]);
            if (build_sides_dig(solids, rvoctree, noctrees, ndepths, cvoctree, sides, rdepth, depth + 1, nposition)) {
                did_build = 1;
            }
        }
        // set 1 if built for Branch Nodes
        if (did_build) {
            set_SidesOctree(sides, depth, position, did_build, 0);
        }
        return did_build;
    }
    // If a non block, we stop here at leaf node
    if (solids && !solids[voctree->value - 1]) {
        SidesOctree* csides = getm_SidesOctree(sides, depth, position, 0);
        if (csides) {
            csides->value = 0;
            collapse_SidesOctree(csides);
        }
        return 0;
    }
    byte ssides = 0;
    for (byte direction = 0; direction < 6; direction++) {
        if (build_voxel_sides(solids, rvoctree, noctrees, ndepths, voctree, sides, depth, position, direction)) {
            ssides |= (1 << (direction + 1));
        }
    }
    if (ssides) {
        set_SidesOctree(sides, depth, position, ssides, 0);
    } else {
        // collapse sub node and set to 0
        SidesOctree* csides = getm_SidesOctree(sides, depth, position, 0);
        if (csides) {
            csides->value = 0;
            collapse_SidesOctree(csides);
        }
    }
    return ssides;
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
    // zox_sys_in(BlockManagerLink);
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
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, voctree);
        zox_sys_o(SidesOctree, sides);
        zox_sys_o(SidesOctreeDirty, sdirty);
        if (cdirty->value != zox_dirty_active) {
            continue;
        }
        if (rdepth->value == render_depth_invisible || rdepth->value == render_depth_spawning) {
            sides->value = 0;
            continue;
        }
        const VoxelNode *noctrees[6];
        byte ndepths[6];
        fetch_neightbor_chunk_data(world, neighbors, noctrees, ndepths);
        // write_lock_SidesOctree(sides);
        // sides->value = 0;
        sides->value = build_sides_dig(solids, voctree, noctrees, ndepths, voctree, sides, rdepth->value, 0, byte3_zero);
        // write_unlock_SidesOctree(sides);
        sdirty->value = zox_dirty_trigger;
    }
    free(solids);
} zox_sys_end(Chunk3SidesSystem);
