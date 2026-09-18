/*
 * +------------------------------------------------------+
 * | Zox Module: Chunks3                                  |
 * |                                                      |
 * |  Generation - Octrees - Meshing - Spatial - Chunk    |
 * |                                                      |
 * +------------------------------------------------------+
 *
 *      - core voxel code
 *      - octree node, goes down n levels
 *      - can be rendered as cubes and optimized triangles
 *
 * */
#include "set/_.c"
#include "mcr/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"
#include "tst/_.c"
#include "io/_.c"

void import_chunks3(ecs* world) {
    zox_module(chunks3);
    zox_components_chunks3(world);
    zox_systems_chunks3(world);
    zox_systems_chunksio(world);
    zox_systems_chunks3_debug(world);
    add_hook_spawn_prefabs(spawn_prefabs_chunks);
    add_hook_files_load(zox_tests_chunks3);
}


// TODO: f2 + f3 = see sometimes  random quads at bottom of chunks, weird asf - now its just occasional mostly fixed
// TODO: Check per split quad if voxel exists
// TODO: Fetch all Block Managers found, not just single
// TODO: Build up adjacent faces in another system

/*
 *  Chunks3 Colored
 *
 *      - Specifically for our game models
 *      - Npcs
 *      - Grass
 *      - Clouds
 *      - Even Textured Blocks, they get baked down
 *
 * */
// TODO: Use Sides Data for building
// TODO: Proper AO using all cube neighbors

// todo: make basic building 12x6x6, spawn randomly in chunks
// todo: remember dont spawn chunk if surrounding chunk hasn't spawn their structures yet
//          - since structures will update voxels accross chunks
// todo: debug system will render bounds of structures around map (based on LODs)
// todo: spawn tree structures out of grass voxels, the structure bounding boxes exist to limit their growths

