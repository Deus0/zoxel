// tst: octree_debug, debug_octree (dbg/octree.c) draws a cube a node, for any octree type
//  - a VoxelNode at depth 2 with three voxels set, one of them linked (a block vox's node: its ptr is a link)
//  - drawn to depth 2, air ignored: three cubes (counted by its colour function), the linked one drawn as a leaf,
//    never walked into as children (the old debugger macros read its link as children)
//  - drawn to depth 0: one cube, the root, its value whatever it holds
//  - run: bash bsh/tst.sh octree_debug
int zox_tst_octree_debug_drawn = 0;

static inline color_rgb zox_tst_octree_debug_count(byte value) {
    zox_tst_octree_debug_drawn++;
    return octree_debug_grey(value);
}

byte zox_tst_octree_debug_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_octree_debug_check(ecs* world) {
    VoxelNode root;
    create_VoxelNode(&root);
    const byte3 set[3] = { { 0, 0, 0 }, { 3, 1, 2 }, { 1, 3, 3 } };
    for (byte i = 0; i < 3; i++) {
        set_VoxelNode(&root, 2, set[i], 1);
    }
    VoxelNode* linked = getm_VoxelNode(&root, 2, set[1]);
    link_node_VoxelNode(linked, 123);
    debug_octree_style style = { sizeof(VoxelNode), octree_type_offset_VoxelNode, 0, 0, 2, 0, 0, zox_tst_octree_debug_count };
    debug_octree(world, &style, &root, NULL, float3_zero, 1);
    zox_tst_check(zox_tst_octree_debug_drawn == 3, "drew [%i] cubes to depth 2, the voxels set are 3", zox_tst_octree_debug_drawn);
    zox_tst_octree_debug_drawn = 0;
    style.target_depth = 0;
    style.ignore = -1;
    debug_octree(world, &style, &root, NULL, float3_zero, 1);
    zox_tst_check(zox_tst_octree_debug_drawn == 1, "drew [%i] cubes to depth 0, expected the root alone", zox_tst_octree_debug_drawn);
    dispose_node_link_VoxelNode(linked);
    close_VoxelNode(&root);
}

void zox_tsts_chunks3_debug_octree() {
    zox_tst_add("octree_debug", zox_tst_octree_debug_ready, zox_tst_octree_debug_check, 30);
}
