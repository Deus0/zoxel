// tst: vox_import, vox files scaled up to the finest Block Res by the depth they were made at (a .txt beside them)
//  - the top hat (16 voxels, made at depth 4: top_hat.txt) imports at depth 5, 32 voxels, so on a Block Res 5 body
//      it's the size hatty (32, made at depth 5) is (it was half)
//  - hatty, no .txt, as it was
//  - run: bash bsh/tst.sh vox_import
// the filled voxels of a vox at its depth
static inline int zox_tst_vox_import_filled(ecs* world, entity vox) {
    const byte depth = zox_getv(vox, NodeDepth);
    const int3 size = zox_getv(vox, ChunkSize);
    const VoxelNode* node = zox_get(vox, VoxelNode);
    int filled = 0;
    byte3 p;
    for (p.x = 0; p.x < size.x; p.x++) {
        for (p.y = 0; p.y < size.y; p.y++) {
            for (p.z = 0; p.z < size.z; p.z++) {
                byte3 q = p;
                filled += get_sub_node_voxel(node, &q, depth) != 0;
            }
        }
    }
    return filled;
}

byte zox_tst_vox_import_ready(ecs* world) {
    (void) world;
    return files_hashmap_voxes != NULL;
}

void zox_tst_vox_import_check(ecs* world) {
    const char* names[] = { "top_hat", "hatty", "cavalier_hat" };
    const int3 expected[] = { { 32, 32, 32 }, { 32, 10, 32 }, { 32, 18, 32 } };
    for (int i = 0; i < 3; i++) {
        const entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(names[i]));
        const entity vox = zox_valid(model) ? get_model_vox(world, model) : 0;
        zox_tst_check(zox_valid(vox), "no vox [%s]", names[i]);
        if (!zox_valid(vox)) {
            continue;
        }
        const int3 size = zox_getv(vox, ChunkSize);
        const byte depth = zox_getv(vox, NodeDepth);
        zox_log("[tst] [%s] depth [%i] size [%ix%ix%i] max render depth [%i] filled [%i]", names[i], depth,
            size.x, size.y, size.z, zox_getv(model, MaxRenderDepth), zox_tst_vox_import_filled(world, vox));
        zox_tst_check(depth == 5, "[%s] imported at depth [%i], not 5", names[i], depth);
        zox_tst_check(int3_equals(size, expected[i]), "[%s] is [%ix%ix%i], not [%ix%ix%i]", names[i],
            size.x, size.y, size.z, expected[i].x, expected[i].y, expected[i].z);
        zox_tst_check(zox_getv(model, MaxRenderDepth) >= 5, "[%s] renders at most depth [%i]", names[i], zox_getv(model, MaxRenderDepth));
    }
}
