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
byte get_node_sides_all_solid(const byte* solidity, const VoxelNode* node, byte axis, byte side, byte distance) {
    if (!node) {
        // zox_log_error("get_node_sides_all_solid has invalid node.");
        return 0;
    }
    // at end of node tree, return if solid
    if (!distance || !has_children_VoxelNode(node)) {
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
        // NOTE: Our bytes arrays reads z, y, x for some reason
        byte child_side = (i >> (2 - axis)) & 1;
        if (child_side != side) {
            continue;
        }
        // check underneath nodes
        if (!get_node_sides_all_solid(solidity, child, axis, side, distance)) {
            return 0;
        }
    }
    // NOTE: if all children pass, they are all solid
    return 1;
}

// this function accounts for size of drawing voxels
// NOTE: Returns 1 to build the side
static inline byte build_voxel_sides(const byte* solids, const VoxelNode* root, const VoxelNode** noctrees, const byte* ndepths, const VoxelNode* voxels, SidesOctree* sides, byte depth, byte3 position, byte direction) {
    int3 positioni = byte3_to_int3(position);
    const VoxelNode* adjacent_node = get_adjacentn_VoxelNode(noctrees, root,  positioni, depth, direction);
    if (!adjacent_node) {
        return 0;
    }
    // Accounts for Dig vs Render Difference
    byte adjacent_solid;
    byte adjacent_depth = get_adjacent_depth(depth, ndepths, positioni, direction);
    if (adjacent_depth > depth) {
        byte dist = adjacent_depth - depth;
        byte reversed = reverse_direction(direction);
        byte axis = reversed >> 1;  // 0=x, 1=y, 2=z
        byte side = reversed & 1;   // 0=negative side, 1=positive side
        adjacent_solid = get_node_sides_all_solid(solids, adjacent_node, axis, side, dist);
    } else {
        // Accounts for null solids
        adjacent_solid = adjacent_node && adjacent_node->value && (!solids || (solids && solids[adjacent_node->value - 1]));
    }
    // Debug These
    return !adjacent_solid || zox_dbg_render_all_sides;
}

static inline byte build_sides_dig(const byte* solids, const VoxelNode* root, const VoxelNode** noctrees, const byte* ndepths, const VoxelNode* voxels, SidesOctree* sides, byte render_depth, byte depth, byte3 position) {
    // if air we stop here at any branch node
    if (!voxels->value) {
        // collapse sub node and set to 0
        sides->value = 0;
        if (sides->ptr) {
            collapse_SidesOctree(sides);
        }
        return 0;
    }
    // We should keep digging even when it's closed
    byte has_vkids = !is_closed_VoxelNode(voxels);
    if (depth < render_depth && (zox_split_textured_quads || (!zox_split_textured_quads && has_vkids))) {
        byte3 cposition = position;
        byte3_multiply_byte(&cposition, 2);
        if (!sides->ptr) {
            open_one_SidesOctree(sides);
            // NOTE: If fails malloc
            if (!sides->ptr) {
                zox_loge("Sides Malloc Failed");
                return 0;
            }
        }
        SidesOctree* sides_kids = (SidesOctree*) sides->ptr;
        const VoxelNode* kids = has_vkids ? get_children_VoxelNode(voxels) : NULL;
        byte did_build = 0;
        for (byte i = 0; i < 8; i++) {
            SidesOctree* sides_kid = &sides_kids[i];
            const VoxelNode* child_voxel = has_vkids ? &kids[i] : voxels;
            byte3 nposition = byte3_add(cposition, octree_positions_b[i]);
            if (build_sides_dig(solids, root, noctrees, ndepths, child_voxel, sides_kid, render_depth, depth + 1, nposition)) {
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
    if (!solids || solids[voxels->value - 1]) {
        for (byte direction = 0; direction < 6; direction++) {
            if (build_voxel_sides(solids, root, noctrees, ndepths, voxels, sides, depth, position, direction)) {
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

// For each ChunkMaterial we generate sides data for meshes
zox_sys2(ChunkSidesSystem) {
    byte dbg_log = 0;
    byte max_process = 16;
    byte* solids = NULL;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(BuildChunkSides);
    zox_sys_out(SidesOctree);
    //zox_sys_out(SidesOctreeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, voxels);
        zox_sys_o(BuildChunkSides, build);
        zox_sys_o(SidesOctree, sides);
        //zox_sys_o(SidesOctreeDirty, sides_dirty);
        // NOTE: Process when Active state
        if (!build->value) {
            continue;
        }
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            continue;
        }
        if (zox_getv(e, GenerateChunk) || zox_getv(e, VoxelNodeDirty) || zox_getv(e, ChunkLodDirty)) {
            if (dbg_log) {
                zox_log("Waiting on Self to Build [%s]", zox_getn(e));
            }
            continue;
        }
        // fetch here instead
        if (!solids) {
            // entity chunk = zox_get_parent(world, e);
            entity manager = zox_getv(e, BlockManagerLink);
            zox_geter(manager, BlockLinks, blocks);
            solids = malloc(blocks->length * sizeof(byte));
            for (int i = 0; i < blocks->length; i++) {
                entity block = blocks->value[i];
                if (!zox_valid(block) || !zox_has(block, BlockModel)) {
                    solids[i] = 1;
                    continue;
                }
                solids[i] = zox_getv(block, BlockModel) == zox_block_solid;
            }
        }
        if (!solids) {
            zox_loge("No block solids");
            return;
        }
        const VoxelNode* noctrees[6];
        byte ndepths[6];
        fetch_neightbor_chunk_data(world, neighbors, noctrees, ndepths);
        sides->value = build_sides_dig(solids, voxels, noctrees, ndepths, voxels, sides, depth->value, 0, byte3_zero);
        // sides_dirty->value = zox_dirty_trigger;
        build->value = 0;
        if (dbg_log) {
            zox_log("Chunk Built Sides [%s]", zox_getn(e));
        }
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (zox_has(e2, ChunkMesh)) {
                    // zox_setm(e2, BuildChunkMesh, 1);
                    zox_set(e2, BuildChunkMesh, { 1 });
                    if (dbg_log) {
                        zox_log("Chunk Triggered Build [%s]:[%s]", zox_getn(e), zox_getn(e2));
                    }
                }
            }
        }
        // Should we set chunks to build here?
        zox_sys_increment();
    }
    if (solids) {
        free(solids);
    }
} zox_sys_end(ChunkSidesSystem);
