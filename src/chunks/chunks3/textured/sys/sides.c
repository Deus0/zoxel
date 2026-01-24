// this function accounts for size of drawing voxels
static inline byte build_voxel_sides(
    const byte* solids,
    const VoxelNode* root_voctree,
    const VoxelNode** noctrees,
    const byte* ndepths,
    VoxelNode* voctree,
    SidesOctree* sides,
    byte rdepth,
    byte depth,
    int3 position,
    byte direction
) {

    const VoxelNode* anode = get_adjacentn_VoxelNode(
        noctrees,
        root_voctree,
        position,
        depth,
        direction
    );

    byte asolid = anode && anode->value && solids[anode->value - 1];

    // Accounts for Dig vs Render Difference
    byte adepth = get_adjacent_depth_VoxelNode(rdepth, ndepths, position, direction);
    if (adepth > depth) { // asolid &&
        asolid = get_node_sides_all_solid(
            solids,
            anode,
            reverse_direction(direction),
            adepth - depth
        );
    }

    // Commenting thii out for all sides doesn't fix
    if (asolid && !zox_dbg_render_all_sides) {
        return 0;
    }

    voctree->sides |= (1 << direction + 1);

    // Scale position first
    if (!zox_chunk3t_split) {
        // zox_log("Adding Side at [%ix%ix%i] - [%i]", position.x, position.y, position.z, direction);

        return 1;
    }

    return 1;

}

static inline byte build_voxel_sides_dig(
    const byte* solids,
    const VoxelNode* root_voctree,
    const VoxelNode** noctrees,
    const byte* ndepths,
    VoxelNode* voctree,
    SidesOctree* sides,
    byte rdepth,
    byte depth,
    int3 position
) {
    byte did_build = 0;

    // keep digging
    if (depth < rdepth && !is_closed_VoxelNode(voctree)) {

        int3_multiply_int_p(&position, 2);
        depth++;

        // only dig for solid child nodes
        VoxelNode* kids = get_children_VoxelNode(voctree);

        for (byte i = 0; i < 8; i++) {

            VoxelNode* child_voctree = &kids[i];

            // TODO: Make sure it sets parent node to non air
            /*if (!child_voctree->value) {
                continue;
            }*/

            int3 nposition = int3_add(position, octree_positions[i]);

            if (build_voxel_sides_dig(
                solids,
                root_voctree,
                noctrees,
                ndepths,
                child_voctree,
                sides,
                rdepth,
                depth,
                nposition
            )) {
                did_build = 1;
            }
        }

        // set 1 if built for Branch Nodes
        voctree->sides = did_build;

        return did_build;
    }

    // Sides Empty first!
    voctree->sides = 0;

    // only continue if voxels are solid
    if (!voctree->value || !solids[voctree->value - 1]) {
        return did_build;
    }


    for (byte direction = 0; direction < 6; direction++) {

        if (build_voxel_sides(
            solids,
            root_voctree,
            noctrees,
            ndepths,
            voctree,
            sides,
            rdepth,
            depth,
            position,
            direction
        )) {
            did_build = 1;
        }
    }

    return did_build;
}

zox_sys2(Chunk3SidesSystem) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    zox_sys_in(ChunkMeshDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(VoxelNode);
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
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(SidesOctree, sides);
        zox_sys_o(SidesOctreeDirty, sdirty);

        if (cdirty->value != zox_dirty_active) {
            continue;
        }

        const VoxelNode *noctrees[6];
        byte ndepths[6];
        fetch_neightbor_chunk_data(world, neighbors, noctrees, ndepths);

        write_lock_VoxelNode(voctree);

            voctree->sides = build_voxel_sides_dig(
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