#include "node.c"
#include "arrayd.c"
#include "safety.c"
#include "positions.c"
#include "io.c"
#include "sides.c"
#include "vox_mesh_grid.c"
#include "icon_bake.c"
#include "gpu_mesh.c"

void zox_tests_chunks3(ecs* world) {
    // test_nodes(world, 1000, 5);
    // test_arrayds(world, 1000, 50);
    // test_arrayds(world, 2, 2);
    // TODO: Make test crash it without safety locks
    // TODO: Detect corruption properly
    // A/B Testing for VoxelNodeLocks
#ifdef zox_tests_chunks3_safety  // a stress test, off (zox_tests is every test build)
    zox_tst_chunks3_safety(world);
#endif
}
