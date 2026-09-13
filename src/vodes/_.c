/*
 *  Module Vodes
 *
 *      - Basically World Blocks
 *      - Chests, Doors, Grass, etc
 *      - Block Health too
 *      - Anything that uses entities on top of chunks for Voxels
 *
 * */
byte zox_disable_vodes = 0;

// TODO: Use better states for spawning / updating Vodes
// TODO: Use the placee queue for Vode Updates
//          - just spawn intially when load increases to max

#include "com/_.c"
#include "vodes3/_.c"

void import_vodes(ecs* world) {
    zox_module(vodes);
    zoxd_components_vodes(world);
    zox_add_module(vodes3);
}
