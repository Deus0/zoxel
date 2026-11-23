// tst: vegetation_chunks, trees grow whole across chunks stacked vertically
//  - a tree stays in its tunk (vegetation/fun cells), but its trunk and canopy can cross the chunk above
//  - near the player: each tree column of a generated tunk, the voxel above the ground is a trunk and the one
//    above the trunk's top is leaves, read from whichever chunk holds them
//  - counts trees crossing a chunk's top (often none: tst vegetation_across draws that case)
//  - waits for a drawn tree, up to 60s; none (a new player starts in a town, it clears its trees, or water): the
//    player moved on every 10s, held in the air, its streaming following
//  - run: bash bsh/tst.sh vegetation_chunks
int zox_tst_veg_chunks_frames = 0;
int zox_tst_veg_columns = 0, zox_tst_veg_unloaded = 0, zox_tst_veg_cleared = 0;    // tree columns seen, not loaded, cleared

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
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    return getv_VoxelNode(voxels, terrain_depth, local);
}

// the drawn trees near the player, those crossing a chunk's top and those without leaves
static inline int zox_tst_veg_count(ecs* world, int* crossing_out, int* no_leaves_out) {
    zox_tst_veg_columns = zox_tst_veg_unloaded = zox_tst_veg_cleared = 0;
    const entity terrain = local_terrain;
    const ChunkLinks* chunks = zox_get(terrain, ChunkLinks);
    const byte terrain_depth = zox_getv(terrain, NodeDepth);
    const lint seed = zox_getv(terrain, Seed);
    const int chunk_length = octree_size(terrain_depth);
    int trees = 0, crossing = 0, no_trunk = 0, no_leaves = 0;
    // under the sea the voxel over the ground is water, not a trunk
    const entity water = zox_get_child_by_id(world, zox_get_parent(world, terrain), zox_id(BlockWater));
    const int water_id = zox_valid(water) ? zox_getv(water, BlockIndex) : -1;
    iter it2 = zox_children(world, terrain);
    while (zox_children_next(it2)) {
        for (int i = 0; i < it2.count; i++) {
            const entity tunk = it2.entities[i];
            if (!zox_has(tunk, VegetationMap) || !zox_has(tunk, HeightMap) || !zox_has(tunk, TunkLod) ||
                zox_getv(tunk, TunkLod) != terrain_depth || zox_getv(tunk, GenerateTunk))
            {
                continue;
            }
            const VegetationMap* vegetation = zox_get(tunk, VegetationMap);
            const HeightMap* heights = zox_get(tunk, HeightMap);
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
                    zox_tst_veg_columns++;
                    if (trunk < 0 || leaf < 0 || ground <= 0) {
                        zox_tst_veg_unloaded++;
                        continue;
                    }
                    if (!trunk || trunk == water_id) {
                        zox_tst_veg_cleared++;
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
    *crossing_out = crossing;
    *no_leaves_out = no_leaves;
    (void) no_trunk;
    return trees;
}

double zox_tst_veg_chunks_start = 0;
byte zox_tst_veg_moved = 0;

byte zox_tst_vegetation_chunks_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "a drawn tree in the chunks near the player";
    if (!zox_tst_veg_chunks_start) {
        zox_tst_veg_chunks_start = zox_current_time;
    }
    zox_tst_progress(zox_tst_veg_chunks_frames);    // waiting on the streaming isn't a stall
    if (++zox_tst_veg_chunks_frames % 10) {
        return 0;
    }
    int crossing, no_leaves;
    if (zox_tst_veg_count(world, &crossing, &no_leaves) > 0) {
        return 1;
    }
    // in a town, or by water: moved on every 10s, 64 blocks a new way (one 48 block move ended by water or still in
    //  the town), held in the air
    const entity character = zox_tst_player_character(world);
    const int moves = (int) ((zox_current_time - zox_tst_veg_chunks_start) / 10);
    if (zox_tst_veg_moved < moves && moves <= 4 && zox_valid(character)) {
        const float scale = zox_getv(local_terrain, BlockScale);
        const float3 ways[4] = { { 64, 0, 0 }, { 0, 0, 64 }, { -64, 0, 64 }, { -64, 0, -64 } };
        float3 p = float3_add(zox_getv(character, Position3D), float3_scale(ways[moves - 1], scale));
        if (!zox_tst_veg_moved) {
            p.y += 24 * scale;
        }
        zox_add(character, DisableGravity);
        zox_add(character, DisableMovement);
        zox_setv(character, Velocity3D, float3_zero);
        zox_setv(character, Position3D, p);
        zox_tst_veg_moved = moves;
        zox_log("[tst] vegetation_chunks no trees yet (a town, water?), moved the player on [%i]", moves);
    }
    return zox_current_time - zox_tst_veg_chunks_start > 60;    // streaming there is slow under load (35s, a full run)
}

void zox_tst_vegetation_chunks_check(ecs* world) {
    int crossing, no_leaves;
    const int trees = zox_tst_veg_count(world, &crossing, &no_leaves);
    zox_log("[tst] [metric] vegetation_chunks trees [%i] crossing a chunk's top [%i], without leaves [%i] after [%.1fs]",
        trees, crossing, no_leaves, zox_current_time - zox_tst_veg_chunks_start);
    zox_log("[tst] vegetation_chunks tree columns [%i]: not loaded [%i], cleared (towns, water) [%i]", zox_tst_veg_columns,
        zox_tst_veg_unloaded, zox_tst_veg_cleared);
    zox_tst_check(trees > 0, "no drawn trees found near the player");
    // crossing trees are rare near the player (hills at a chunk's top): tst vegetation_across checks them every run
    zox_tst_check(!no_leaves, "[%i] of [%i] trees have no leaves above their trunk (a chunk didn't draw its part?)",
        no_leaves, trees);
}

// one world every run (zox_tst_seed, else 666): a random one sometimes had no tree for a minute around its spawn (a
//  town, a lake)
void zox_tst_vegetation_chunks_setup() {
    setenv("zox_tst_seed", "666", 0);
}

void zox_tsts_vegetation_chunks(ecs* world) {
    zox_tst_add("vegetation_chunks", zox_tst_vegetation_chunks_ready, zox_tst_vegetation_chunks_check, 120);
    zox_tst_set_setup("vegetation_chunks", zox_tst_vegetation_chunks_setup, 0);
}
