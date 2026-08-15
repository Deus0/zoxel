// TODO: Calculate only sides needed

byte zox_sides_only_target_depth = 0;

#define SIDE_X_NEG      (1 << 1)
#define SIDE_X_POS      (1 << 2)
#define SIDE_Y_NEG      (1 << 3)
#define SIDE_Y_POS      (1 << 4)
#define SIDE_Z_NEG      (1 << 5)
#define SIDE_Z_POS      (1 << 6)
#define SIDE_CALCULATED (1 << 7)

// Sides System will generate our chunk sides before rendering
// TODO: Let Colored use this too
static inline byte is_node_solid(
    const byte* solidity,
    const VoxelNode* node)
{
    byte value = node->value;
    if (!value) {
        return 0;
    }
    return solidity ? solidity[value - 1] : 1;
}

// delves down a voxel node, but only one one side
// Returns 0 if any Air
byte get_node_sides_all_solid(
    const byte* solidity,
    const VoxelNode* node,
    byte axis,
    byte side,
    byte distance)
{
    if (!node) {
        // zox_log_error("get_node_sides_all_solid has invalid node.");
        return 0;
    }
    // at end of node tree, return if solid
    if (!distance || !has_children_VoxelNode(node)) {
        return is_node_solid(solidity, node);
    }
    VoxelNode* kids = (VoxelNode*) node->ptr ;
    if (!kids) {
        zox_loge("get_node_sides_all_solid: null children.");
        return 0;
    }
    distance--;
    const byte* indices = side_child_indices[axis][side];
    for (byte i = 0; i < 4; i++) {
        if (!get_node_sides_all_solid(
            solidity,
            &kids[indices[i]],
            axis,
            side,
            distance
        )) {
            return 0;
        }
    }
    // NOTE: if all children pass, they are all solid
    return 1;
}

byte get_adjacent_depth(
    byte depth,
    const byte* neighbor_depths,
    int3 position,
    byte direction)
{
    if (is_on_edge_octree(depth, position, direction)) {
        return neighbor_depths[direction];
    } else {
        return depth;
    }
}

// this function accounts for size of drawing voxels
// NOTE: Returns 1 to build the side
static inline byte build_voxel_sides(
    const byte* solids,
    const VoxelNode* root,
    const VoxelNode** neighbor_voxels,
    const byte* neighbor_depths,
    const VoxelNode* voxels,
    SidesOctree* sides,
    byte depth,
    byte3 position,
    byte direction)
{
    int3 positioni = byte3_to_int3(position);
    const VoxelNode* adjacent_node = get_adjacentn_VoxelNode(neighbor_voxels, root, positioni, depth, direction);
    if (!adjacent_node) {
        // NOTE: Adds rendered facefor top of world
        if (direction == direction_up) {
            return 1;
        }
        return 0;
    }
    if (zox_dbg_render_all_sides) {
        return 1;
    }
    // Accounts for Dig vs Render Difference
    byte adjacent_solid;
    byte adjacent_depth = get_adjacent_depth(
        depth,
        neighbor_depths,
        positioni,
        direction);
    if (adjacent_depth > depth) {
        byte dist = adjacent_depth - depth;
        SideInfo side = side_infos[direction];
        adjacent_solid =
            get_node_sides_all_solid(
                solids,
                adjacent_node,
                side.axis,
                side.side,
                dist);
    } else {
        // Accounts for null solids
        byte value = adjacent_node->value;
        adjacent_solid = value &&
            (!solids || (solids && solids[value - 1]));
    }
    // Debug These
    return !adjacent_solid;
}


static inline byte build_sides_dig(
    const byte* solids,
    const VoxelNode* root,
    const VoxelNode** neighbor_voxels,
    const byte* neighbor_depths,
    const VoxelNode* voxels,
    SidesOctree* sides,
    byte target_depth,
    byte depth,
    byte3 position,
    byte dbg_log)
{
    // NOTE: if air we stop here at any branch node
    // NOTE: Assumes voxel octree has optimized for solid non air values first
    byte value = voxels->value;
    if (!value) {
        // collapse sub node and set to 0
        sides->value = 0;
        if (sides->ptr) {
            collapse_SidesOctree(sides);
        }
        return 0;
    }
    // We should keep digging even when it's closed
    // NOTE: Dig down voxel + sides octree
    byte has_vkids = !is_closed_VoxelNode(voxels);
    if (depth < target_depth && (zox_split_textured_quads || (!zox_split_textured_quads && has_vkids))) {
        if (!sides->ptr) {
            open_one_SidesOctree(sides);
            // NOTE: If fails malloc
            if (!sides->ptr) {
                zox_loge("Sides Malloc Failed");
                return 0;
            }
        }
        byte3 child_position = {
            position.x << 1,
            position.y << 1,
            position.z << 1
        };
        SidesOctree* sides_kids = (SidesOctree*) sides->ptr;
        const VoxelNode* kids = has_vkids ? (const VoxelNode*) voxels->ptr : NULL;
        // byte did_build = 0;
        byte child_depth = depth + 1;
        for (byte i = 0; i < 8; i++) {
            SidesOctree* sides_kid = &sides_kids[i];
            const VoxelNode* child_voxel = has_vkids ? &kids[i] : voxels;
            byte3 nposition = byte3_add(child_position, octree_positions[i]);
            build_sides_dig(solids, root, neighbor_voxels, neighbor_depths, child_voxel, sides_kid, target_depth, child_depth, nposition, dbg_log);
        }
        if (zox_sides_only_target_depth && depth < target_depth) {
            return 0;
        }
    }
    // Set our sides here at depth
    sides->value = 0;
    // NOTE: if solid, check all sides, Set side 1 if Air
    if (value && (!solids || solids[value - 1])) {
        for (byte direction = 0; direction < 6; direction++) {
            if (build_voxel_sides(solids, root, neighbor_voxels, neighbor_depths, voxels, sides, depth, position, direction)) {
                sides->value |= (1 << (direction + 1));
            }
        }
    }
    if (dbg_log && sides->value) {
        zox_log("Sides was added at Depth [%i] Pos [%ix%ix%i] -> [%i]", depth, position.x, position.y, position.z, sides->value);
    }
    return sides->value;
}

// TODO: The issue is now we generate sides per level instead of just whats needed! Perhaps a bitmask of which sides are generated is better! .... but when swapping its good to have ready for build! - we can just keep a depth of sides and when updating render depth, it can build higher if needed, this way its not building high on lower depth chunks

// TODO: Create a Sides entity so each BlockMaterial can have its own meshes underneath (Water would need seperate sides)

// TODO: Add a BlockSolidityCache to BlockManager

// TODO: Only generate sides at target depth

// NOTE: Keeps updated when voxels octree changes
// NOTE: Calculates the solid sides of a voxel octree per material
zox_sys2(ChunkSidesSystem) {
    init_side_child_indices();
    byte dbg_log = 0;
    byte max_process = !zox_disable_process_skips ? 1 : 0;
    byte* solids = NULL;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(BuildChunkSides);
    zox_sys_out(SidesOctree);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, voxels);
        zox_sys_o(BuildChunkSides, build);
        zox_sys_o(SidesOctree, sides);
        // NOTE: Process when Active state
        if (!build->value) {
            continue;
        }
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            continue;
        }
        // HMmm
        if (zox_getv(e, VoxelNodeDirty)) {
            if (dbg_log) {
                zox_log("[%s] [%s]: Delayed due to [VoxelNodeDirty]", zox_getn(it->system), zox_getn(e));
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
        const VoxelNode* neighbor_voxels[6];
        byte neighbor_depths[6];
        memset(neighbor_voxels, 0, 6 * sizeof(const VoxelNode*));
        memset(neighbor_depths, 0, 6 * sizeof(byte));
        for (byte j = 0; j < 6; j++) {
            entity e = neighbors->value[j];
            if (!zox_valid(e)) {
                continue;
            }
#ifdef zox_safety_checks
            if (!zox_has(e, NodeDepth) || !zox_has(e, VoxelNode)) {
                zox_loge("Neighbor [%s] has invalid components", zox_getn(e));
                continue;
            }
#endif
            neighbor_voxels[j] = zox_get(e, VoxelNode);
            neighbor_depths[j] = zox_getv(e, NodeDepth);
        }
        build_sides_dig(solids, voxels, neighbor_voxels, neighbor_depths, voxels, sides, depth->value, 0, byte3_zero, dbg_log);
        build->value = 0;
        if (dbg_log) {
            zox_log("Built Sides [%s]", zox_getn(e));
        }
        // Triggers meshes to build now
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (zox_has(e2, ChunkMesh)) {
                    zox_setv(e2, BuildMesh, zox_build_chunk_mesh_run);
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
