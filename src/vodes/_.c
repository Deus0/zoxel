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

#include "vodes3/_.c"

void import_vodes(ecs* world) {
    zox_module(vodes);
    zox_add_module(vodes3);
}
