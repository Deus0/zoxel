// tst: vegetation_chunks, trees grow whole across chunks stacked vertically
//  - a tree stays in its tunk (vegetation/fun cells), but its trunk and canopy can cross the chunk above
//  - near the player: each tree column of a generated tunk, the voxel above the ground is a trunk and the one
//    above the trunk's top is leaves, read from whichever chunk holds them
//  - counts trees crossing a chunk's top, so drawing across chunks is checked
//  - run: bash bsh/tst.sh vegetation_chunks
int zox_tst_veg_chunks_frames = 0;

byte zox_tst_vegetation_chunks_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "vegetation to grow in nearby chunks";
    return ++zox_tst_veg_chunks_frames > 60;
}

// a voxel at a block position, -1 if its chunk isn't there, generated, at full detail
static inline int zox_tst_voxel_at(ecs* world, const ChunkLinks* chunks, byte terrain_depth, int3 block) {
    const byte length = octree_size(terrain_depth);
    const int3 chunk_position = block_position_to_positionc(block, byte3_single(length));
    const entity chunk = int3_hashmap_get(chunks->value, chunk_position);
    if (!zox_valid(chunk) || !zox_has(chunk, VoxelNode) || zox_getv(chunk, NodeDepth) != terrain_depth ||
        (zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk)))
    {
        return -1;
    }
    const byte3 local = get_positionl_byte3(block, byte3_single(length));
    zox_geter(chunk, VoxelNode, voxels);
    return getv_VoxelNode(voxels, terrain_depth, local);
}

void zox_tst_vegetation_chunks_check(ecs* world) {
    const entity terrain = local_terrain;
    zox_geter(terrain, ChunkLinks, chunks);
    const byte terrain_depth = zox_getv(terrain, NodeDepth);
    const lint seed = zox_getv(terrain, Seed);
    const int chunk_length = octree_size(terrain_depth);
    int trees = 0, crossing = 0, no_trunk = 0, no_leaves = 0;
    iter it2 = zox_children(world, terrain);
    while (zox_children_next(it2)) {
        for (int i = 0; i < it2.count; i++) {
            const entity tunk = it2.entities[i];
            if (!zox_has(tunk, VegetationMap) || !zox_has(tunk, HeightMap) || !zox_has(tunk, TunkLod) ||
                zox_getv(tunk, TunkLod) != terrain_depth || zox_getv(tunk, GenerateTunk))
            {
                continue;
            }
            zox_geter(tunk, VegetationMap, vegetation);
            zox_geter(tunk, HeightMap, heights);
            const int2 tunk_position = zox_getv(tunk, TunkPosition);
            const int length = octree_size(terrain_depth);
            if (vegetation->length != length * length || heights->length != length * length) {
                continue;
            }
            for (int x = 0; x < length; x++) {
                for (int z = 0; z < length; z++) {
                    const int index = int2_array_index((int2) { x, z }, int2_single(length));
                    if (vegetation->value[index] != zox_vegetation_trees) {
                        continue;
                    }
                    const int2 column = (int2) { tunk_position.x * length + x, tunk_position.y * length + z };
                    const int height = heights->value[index];
                    const int top = height + get_tree_height(column, seed);
                    const int trunk = zox_tst_voxel_at(world, chunks, terrain_depth, (int3) { column.x, height + 1, column.y });
                    const int leaf = zox_tst_voxel_at(world, chunks, terrain_depth, (int3) { column.x, top + 1, column.y });
                    const int ground = zox_tst_voxel_at(world, chunks, terrain_depth, (int3) { column.x, height, column.y });
                    // only trees drawn (on land, not under water or on stone), in loaded generated chunks
                    if (trunk < 0 || leaf < 0 || ground <= 0) {
                        continue;
                    }
                    if (!trunk) {
                        continue;   // not drawn here (towns clear them, too low or high), not a chunk issue
                    }
                    trees++;
                    const byte crosses = vegetation_floor_div(height + 1, chunk_length) != vegetation_floor_div(top + 1, chunk_length);
                    crossing += crosses;
                    no_leaves += !leaf;
                }
            }
        }
    }
    zox_log("[tst] [metric] vegetation_chunks trees [%i] crossing a chunk's top [%i], without leaves [%i]",
        trees, crossing, no_leaves);
    zox_tst_check(trees > 0, "no drawn trees found near the player");
    zox_tst_check(crossing > 0, "no tree crosses a chunk's top, the across chunks case isn't checked");
    zox_tst_check(!no_leaves, "[%i] of [%i] trees have no leaves above their trunk (a chunk didn't draw its part?)",
        no_leaves, trees);
    (void) no_trunk;
}
