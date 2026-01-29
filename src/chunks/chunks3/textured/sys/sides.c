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
    // byte rdepth,
    byte depth,
    int3 position,
    byte direction
) {

    const VoxelNode* anode = get_adjacentn_VoxelNode(
        noctrees,
        rvoctree,
        position,
        depth,
        direction
    );

    // Accounts for Dig vs Render Difference
    byte adepth = get_adjacent_depth_VoxelNode(depth, ndepths, position, direction); // rdepth

    byte asolid;
    if (adepth > depth) {

        // The depth for neighbor is not detected straight away
        // asolid = 0;

        asolid = anode && get_node_sides_all_solid(
            solids,
            anode,
            reverse_direction(direction),
            adepth - depth
        );

    } else {
        asolid = anode && anode->value && solids[anode->value - 1];
    }

    // This doesnt even work
    // if (adepth != depth) asolid = 0;
    /*if (asolid && is_on_edge_VoxelNode(depth, position, direction)) {
        asolid = 0;
        // zox_log("depth [%i] adepth [%i]", depth, adepth);
    }*/

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
    int3 position
) {
    byte did_build = 0;

    // We should keep digging even when it's closed
    // keep digging
    byte has_vkids = !is_closed_VoxelNode(voctree);

    if (depth < rdepth &&
        (zox_split_textured_quads ||
        (!zox_split_textured_quads && has_vkids))) {

        int3 cposition = position;
        int3_multiply_int_p(&cposition, 2);

        const VoxelNode* kids = has_vkids ? get_children_VoxelNode(voctree) : NULL;

        for (byte i = 0; i < 8; i++) {

            const VoxelNode* cvoctree = has_vkids ? &kids[i] : voctree;

            // TODO: Make sure it sets parent node to non air
            /*if (!cvoctree->value) {
                continue;
            }*/

            int3 nposition = int3_add(cposition, octree_positions[i]);

            if (build_sides_dig(
                solids,
                rvoctree,
                noctrees,
                ndepths,
                cvoctree,
                sides,
                rdepth,
                depth + 1,
                nposition
            )) {
                did_build = 1;
            }
        }

        // set 1 if built for Branch Nodes
        set_SidesOctree(sides, depth, int3_to_byte3(position),  did_build, 0);

        return did_build;
    }

    // If air or non block, we return and set drawn to 0
    if (!voctree->value || !solids[voctree->value - 1]) {

        // TODO: Collapse any sub nodes here?
        set_SidesOctree(sides, depth, int3_to_byte3(position), 0, 0);

        return did_build;
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
            did_build = 1;
            ssides |= (1 << direction + 1);
        }
    }

    set_SidesOctree(sides, depth, int3_to_byte3(position), ssides, 0);

    return did_build;
}

// DECIDE: Should I collapse sides octree nodes here?
zox_sys2(Chunk3SidesSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    zox_sys_in(ChunkMeshDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(SidesOctree);
    zox_sys_out(SidesOctreeDirty);

    chunk3_textured_builder_data build_data;
    if (!cache_blocks_data(it, &build_data)) {
        return;
    }

    // Our Loop
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

        write_lock_VoxelNode(voctree);

            sides->value = build_sides_dig(
                build_data.solidity,
                voctree,        // root_voctree
                noctrees,
                ndepths,
                voctree,
                sides,
                rdepth->value,
                0,
                int3_zero
            );

        write_unlock_VoxelNode(voctree);

        sdirty->value = zox_dirty_trigger;
    }
} zox_sys_end(Chunk3SidesSystem);