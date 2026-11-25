// tst: block_sides, the faces showing at a voxel (get_chunk_sides, chunks3/fun/materials.c) as the meshes draw them:
//  what the raycast gizmo outlines (vrays/sys/raycast_gizmo.c), with any mesher (it outlined nothing with gpu meshing,
//  no SidesOctree was built there)
//  - a chunk3 parent's top middle chunk (chunk3_parent.c): two solid blocks stacked in the air, one on the chunk's edge
//  - the lower shows every face but its top, the upper every face but its bottom; the edge block's face into the next
//    chunk shows (air there); air shows none
//  - run: bash bsh/tst.sh block_sides
byte zox_tst_bsides_stage = 0;

byte zox_tst_block_sides_ready(ecs* world) {
    if (!zox_tst_chunk3_parent_ready(world)) {
        return 0;
    }
    if (!zox_tst_bsides_stage) {
        const byte solid = zox_tst_chunks_block(world, zox_block_material_solid, 0);
        VoxelNodeQueue* queue = zox_getm(zox_tst_chunk_at(world, (int3) { 0, 1, 0 }), VoxelNodeQueue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = solid, .position = (byte3) { 5, 6, 5 } });
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = solid, .position = (byte3) { 5, 7, 5 } });
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = solid, .position = (byte3) { 15, 6, 5 } });
        zox_tst_bsides_stage = 1;
        return 0;
    }
    return 1;
}

void zox_tst_block_sides_check(ecs* world) {
    const entity chunk = zox_tst_chunk_at(world, (int3) { 0, 1, 0 });
    const byte depth = zox_getv(chunk, NodeDepth);
    const byte all = 0x7e;  // a bit a direction, 1 << (direction + 1)
    const byte lower = get_chunk_sides(world, chunk, depth, (byte3) { 5, 6, 5 });
    const byte upper = get_chunk_sides(world, chunk, depth, (byte3) { 5, 7, 5 });
    const byte edge = get_chunk_sides(world, chunk, depth, (byte3) { 15, 6, 5 });
    const byte air = get_chunk_sides(world, chunk, depth, (byte3) { 2, 2, 2 });
    zox_log("[tst] [metric] block_sides: lower [%i] upper [%i] edge [%i] air [%i]", lower, upper, edge, air);
    zox_tst_check(lower == (all & ~(1 << (direction_up + 1))), "the lower block shows [%i], not all but its top", lower);
    zox_tst_check(upper == (all & ~(1 << (direction_down + 1))), "the upper block shows [%i], not all but its bottom", upper);
    zox_tst_check(edge == all, "the block on the chunk's edge shows [%i], not every face (the next chunk is air)", edge);
    zox_tst_check(!air, "air shows faces [%i]", air);
}
