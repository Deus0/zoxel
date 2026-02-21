// Sides System will generate our chunk sides before rendering
// TODO: Let Colored use this too

// this function accounts for size of drawing voxels
static inline byte build_voxel_sides(
    const byte* solids,
    const VoxelNode* rvoctree,
    const VoxelNode** noctrees,
    const byte* ndepths,
    const VoxelNode* voctree,
    SidesOctree* sides,
    byte depth,
    byte3 position,
    byte direction
) {

    const VoxelNode* anode = get_adjacentn_VoxelNode(
        noctrees,
        rvoctree,
        byte3_to_int3(position),
        depth,
        direction
    );

    // Accounts for Dig vs Render Difference
    byte adepth = get_adjacent_depth(depth, ndepths, byte3_to_int3(position), direction);

    byte asolid;
    if (adepth > depth) {
        asolid = anode && get_node_sides_all_solid(
            solids,
            anode,
            reverse_direction(direction),
            adepth - depth
        );
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

static inline byte build_sides_dig(
    const byte* solids,
    const VoxelNode* rvoctree,
    const VoxelNode** noctrees,
    const byte* ndepths,
    const VoxelNode* voctree,
    SidesOctree* sides,
    byte rdepth,
    byte depth,
    byte3 position
) {

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

    if (depth < rdepth &&
        (zox_split_textured_quads ||
        (!zox_split_textured_quads && has_vkids))) {

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

        if (build_voxel_sides(
            solids,
            rvoctree,
            noctrees,
            ndepths,
            voctree,
            sides,
            depth,
            position,
            direction
        )) {
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

// DECIDE: Should I collapse sides octree nodes here?
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

        write_lock_SidesOctree(sides);

        // sides->value = 0;
        sides->value = build_sides_dig(
            solids,
            voctree,        // root_voctree
            noctrees,
            ndepths,
            voctree,
            sides,
            rdepth->value,
            0,
            byte3_zero
        );

        write_unlock_SidesOctree(sides);

        sdirty->value = zox_dirty_trigger;
    }
    free(solids);
} zox_sys_end(Chunk3SidesSystem);
