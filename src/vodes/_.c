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
#include "doors/_.c"
#include "activations/_.c"

#ifdef zox_tests
// the test level (_/tst zox_tst_add_level): no block voxes (grass, flowers)
void zox_tst_level_vodes() {
    zox_disable_vodes = 1;
}
#endif

void import_vodes(ecs* world) {
    zox_module(vodes);
    zoxd_components_vodes(world);
    zox_add_module(vodes3);
    zox_add_module(doors);
    zox_add_module(activations);
#ifdef zox_tests
    zox_tst_add_level(zox_tst_level_vodes);
#endif
}
