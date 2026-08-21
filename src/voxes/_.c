/*
 * +------------------------------------------------------------------+
 * | Zox Module: Voxes                                                |
 * |                                                                  |
 * |  Chunk Containers | Colored Voxes | Models                       |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *  TODO:
 *
 *      - Just integrate into Chunks & Models
 *      - For grouping Chunks together!
 *      - colored vox models
 *
 * */
#include "dat/_.c"
#include "com/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "io/_.c"
#include "dbg/_.c"
#include "tst/_.c"

void dispose_voxes(ecs *world, void *ctx) {
    dispose_files_voxes(world);
}

void import_voxes(ecs* world) {
    zox_module(voxes);
    zox_module_dispose(dispose_voxes);
    define_components_voxes(world);
    define_systems_voxes(world);
    add_hook_spawn_prefabs(zox_define_prefabs_voxes);
    add_hook_terminal_command(process_arguments_voxes);
    add_hook_files_load(load_files_voxes);
}
