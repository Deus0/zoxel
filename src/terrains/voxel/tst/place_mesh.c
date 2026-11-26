// tst: place_mesh, placing a block in a chunk with no mesh (all air) gives it one (todo.md Bugs: "When place above
//  chunk - no mesh spawns")
//  - in an all air chunk (air_chunk.c: a world of its own, no player), places a bricks block in its middle
//      and one in its corner (a chunk edge, its neighbours have to show the faces they share)
//  - within a few seconds the chunk has a shown mesh (not Disabled) with faces, and its material's mesh is at the
//      chunk's depth
//  - run: bash bsh/tst.sh place_mesh
byte zox_tst_pmesh_stage = 0;
double zox_tst_pmesh_time = 0;
entity zox_tst_pmesh_chunk = 0;

// the chunk's shown meshes' vertices, 0 if none shown
static inline int zox_tst_pmesh_vertices(ecs* world, entity chunk, int* shown) {
    entity meshes[zox_chunk_meshes_max];
    const uint count = get_chunk_meshes(world, chunk, meshes);
    int vertices = 0;
    *shown = 0;
    for (uint i = 0; i < count; i++) {
        if (zox_has(meshes[i], Disabled)) {
            continue;
        }
        (*shown)++;
        vertices += zox_has(meshes[i], MeshVertices) ? zox_get(meshes[i], MeshVertices)->length : 0;
    }
    return vertices;
}

byte zox_tst_place_mesh_ready(ecs* world) {
    const entity air = zox_tst_streamed_air_chunk(world);
    if (!air) {
        return 0;
    }
    const entity terrain = zox_tst_world_terrain;
    zox_tst_progress(zox_tst_pmesh_stage * 1000 + (int) (zox_current_time - zox_tst_pmesh_time));
    if (zox_tst_pmesh_stage == 0) {
        const int length = octree_size(zox_getv(terrain, NodeDepth));
        zox_tst_pmesh_chunk = air;
        const entity bricks = zox_tst_air_block(world, zox_id(BlockBricks));
        zox_tst_check(zox_valid(bricks), "the realm has no bricks");
        if (!zox_valid(bricks)) {
            return 1;
        }
        VoxelNodeQueue* queue = zox_getm(zox_tst_pmesh_chunk, VoxelNodeQueue);
        const byte index = zox_getv(bricks, BlockIndex);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = index, .position = (byte3) { length / 2, length / 2, length / 2 } });
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = index, .position = byte3_zero });
        zox_tst_pmesh_stage = 1;
        zox_tst_pmesh_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "a mesh in the chunk the blocks were placed in";
    int shown;
    if (zox_tst_pmesh_vertices(world, zox_tst_pmesh_chunk, &shown) && !zox_has(zox_tst_pmesh_chunk, ChunkUpdate)) {
        return zox_current_time - zox_tst_pmesh_time > 1;
    }
    return zox_current_time - zox_tst_pmesh_time > 15;
}

void zox_tst_place_mesh_check(ecs* world) {
    const entity chunk = zox_tst_pmesh_chunk;
    zox_tst_check(zox_valid(chunk), "no all air chunk streamed in");
    if (!zox_valid(chunk)) {
        return;
    }
    entity meshes[zox_chunk_meshes_max];
    const uint count = get_chunk_meshes(world, chunk, meshes);
    int shown;
    const int vertices = zox_tst_pmesh_vertices(world, chunk, &shown);
    const byte depth = zox_getv(chunk, RenderDepth);
    int at_depth = 0;
    for (uint i = 0; i < count; i++) {
        at_depth += !zox_has(meshes[i], Disabled) && zox_getv(meshes[i], RenderDepth) == depth;
    }
    zox_log("[tst] [metric] place_mesh [%s]: [%i] meshes, [%i] shown ([%i] at its depth [%i]), [%i] vertices, after [%.1fs]",
        zox_getn(chunk), count, shown, at_depth, depth, vertices, zox_current_time - zox_tst_pmesh_time);
    zox_tst_check(shown, "the chunk shows no mesh after a block was placed in it");
    zox_tst_check(vertices >= 2 * 6 * 4, "the chunk's mesh has [%i] vertices, not the two blocks' faces", vertices);
    zox_tst_check(at_depth, "no shown mesh is at the chunk's depth [%i]", depth);
}
