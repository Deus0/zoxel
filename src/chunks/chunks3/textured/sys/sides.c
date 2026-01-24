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

    byte adepth = get_adjacent_depth_VoxelNode(
        rdepth,
        ndepths,
        position,
        direction
    );

    // Accounts for Dig vs Render Difference
    if (adepth > depth) { // asolid &&
        asolid = get_node_sides_all_solid(
            solids,
            anode,
            reverse_direction(direction),
            adepth - depth                  // depth distance to check
        );
    }

    if (asolid) {
        return 0;
    }

    voctree->sides |= (1 << direction + 1);

    // Scale position first
    if (!zox_chunk3t_split) {

        // zox_log("Adding Side at [%ix%ix%i] - [%i]", position.x, position.y, position.z, direction);


        return 1;
    }

    return 0;   // for now


    // XZ for now
    /*float dividor = (float) powers_of_two[(data.rdepth - dig.depth)];
    float qsize = dig.scale / dividor;
    float3 positionf2 = dig.positionf;

    if (dig.direction == direction_down || dig.direction == direction_up) {

        float3 qscale = (float3) { qsize, dig.scale, qsize };

        for (float x = 0; x < dig.scale; x += qsize) {
            positionf2.x = dig.positionf.x + x;

            for (float z = 0; z < dig.scale; z += qsize) {
                positionf2.z = dig.positionf.z + z;

                zox_build_voxel_face(
                    data.mesh_data,
                    face.indicies,
                    face.vertices,
                    face.uvs,
                    positionf2,
                    qscale
                );

            }
        }

    } else if (dig.direction == direction_left || dig.direction == direction_right) {

        float3 qscale = (float3) { dig.scale, qsize, qsize };

        for (float y = 0; y < dig.scale; y += qsize) {
            positionf2.y = dig.positionf.y + y;

            for (float z = 0; z < dig.scale; z += qsize) {
                positionf2.z = dig.positionf.z + z;

                zox_build_voxel_face(
                    data.mesh_data,
                    face.indicies,
                    face.vertices,
                    face.uvs,
                    positionf2,
                    qscale
                );

            }
        }

    } else if (dig.direction == direction_back || dig.direction == direction_front) {

        float3 qscale = (float3) { qsize, qsize, dig.scale };

        for (float x = 0; x < dig.scale; x += qsize) {
            positionf2.x = dig.positionf.x + x;

            for (float y = 0; y < dig.scale; y += qsize) {
                positionf2.y = dig.positionf.y + y;

                zox_build_voxel_face(
                    data.mesh_data,
                    face.indicies,
                    face.vertices,
                    face.uvs,
                    positionf2,
                    qscale
                );

            }
        }

    }*/

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

            if (!kids[i].value) {
                continue;
            }

            VoxelNode* child_voctree = &kids[i];

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

    // only continue if voxels are solid
    if (!voctree->value || !solids[voctree->value - 1]) {
        return did_build;
    }

    // before we set sides, we need to empty the value
    voctree->sides = 0;

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