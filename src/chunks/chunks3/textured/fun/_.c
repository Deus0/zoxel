#include "blocks.c"
#include "face.c"

void fetch_neightbor_chunk_data(ecs* world, const ChunkNeighbors* chunk_neighbors, const VoxelNode** neighbors, byte* ndepths) {
    for (int i = 0; i < 6; i++) {
        entity e = chunk_neighbors->value[i];

        if (!zox_valid(e) || !zox_has(e, RenderDepth) || !zox_has(e, VoxelNode)) {
            ndepths[i] = 0;
            neighbors[i] = 0;
            continue;
        }

        neighbors[i] = zox_get(e, VoxelNode);

        zox_geter_value(e, RenderDepth, byte, rdepth);
        ndepths[i] = rdepth;

        // Sometimes neighbor is still generating
        // zox_geter_value(e, NodeDepth, byte, vdepth);
        // ndepths[i] = rdepth > vdepth ? vdepth : rdepth;
    }
}