/*
 * Module: Lights3
 *
 *      - Used to lightup voxel worlds
 *
 * */
#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "dbg/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_lights3(ecs* world) {
    zox_module(lights3);
    define_components_lights3(world);
    define_systems_lights3(world);
    zoxd_systems_light3_debug(world);
    // initialize_settings_lights3(world);
}

// ATM: Refactoring queues, so i can make sure not to spread light if darkbeam or darklight exists
// TODO: Player to place with VoxelNodeQueue
// TODO: Clear VoxelNodeQueue end of Frame
// TODO: use VoxelNodeQueue to add to light queues
//      - Remove Block = check if above is sunlight - Propogate sides
//      - Add Block - if above is sun, shadow down
// TODO: Oursunbeams and propogation batch, should be stopping at solids, and propogatingfrom that
// TODO: Propogate out from voxels with light emmision property
