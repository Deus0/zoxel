#ifndef zoxm_chunks3
#define zoxm_chunks3
/**
 *  Zox Chunks3(D)
 *
 *      - core voxel code
 *      - octree node, goes down n levels
 *      - can be rendered as cubes and optimized triangles
 *
 * */

// TOTO: Finish migrating terrain textured code here
// TOTO: Move any Chunks3 Colored code here


#include "set/_.c"
#include "sta/_.c"
#include "mcr/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "dbg/_.c"
#include "tst/_.c"

#include "textured/_.c"
#include "colored/_.c"
#include "io/_.c"
#include "structures/_.c"
#include "animations/_.c"

void module_dispose_chunks3(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_hook_on_destroyed_VoxelNode();
}

zox_begin_module(Chunks3)
    define_components_chunks3(world);
    define_systems_chunks3(world);
    define_systems_chunksio(world);
    define_systems_chunks3_debug(world);

    // hooks
    add_hook_spawn_prefabs(spawn_prefabs_chunks);
    zox_module_dispose(module_dispose_chunks3);
    initialize_hook_on_destroyed_VoxelNode();
    add_hook_on_destroyed_VoxelNode(destroy_node_link_VoxelNode);
    test_chunks3(world);

    // sub modules
    zox_import_module(Chunks3Colored);
    zox_import_module(Chunks3Textured);
    zox_import_module(Structures);
    zox_import_module(VoxelsAnimations);

zox_end_module(Chunks3)

#endif
