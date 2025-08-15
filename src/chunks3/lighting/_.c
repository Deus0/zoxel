#ifndef zoxm_lighting3
#define zoxm_lighting3

// TODO: Player to place with VoxelNodeQueue
// TODO: Clear VoxelNodeQueue end of Frame
// TODO: use VoxelNodeQueue to add to light queues
//      - Remove Block = check if above is sunlight - Propogate sides
//      - Add Block - if above is sun, shadow down


// -> for each neighbor, we have 6
//      -> If out of bounds:
//          ->wrap position, add to queue, we can get the light and voxel though to check if meets conditions
//      -> otherwise
//          -> check if air
//              -> if air we check light
//                  -> if light less than current (decayed light) we set it and propogate there
//          -> if not air, i.e. solid, of course light doesnt go through solid you twat
// NOTE: Stop setting neighbor data, thats const, you are literally corrupting memory

#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "dbg/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Lighting3)
    define_components_lighting3(world);
    define_systems_lighting3(world);
zox_end_module(Lighting3)

#endif