/*
 * +--------------------------------------------------------------+
 * | Zox Module: Doors                                            |
 * |                                                              |
 * |  Open - Close - Swing - Pass Through                         |
 * |                                                              |
 * |  Uses: Block Datas - Vodes - Raycasts - Sounds - Collisions  |
 * |                                                              |
 * +--------------------------------------------------------------+
 *
 *  - a door is a 1 x 2 x 1 block (BlockDoor, items/fun/multi_block.c): its base and a filler above
 *  - its open state is block_data_open in its parts' block data (chunks3/fun/block_data.c): saved with its chunk,
 *    and terrain collisions skip a voxel with it set (terrain/collisions/sys/intersect2.c)
 *  - a right click on it toggles it (activations, door_toggle), with a creak or a thump
 *  - its block vox swings about its hinge to the state (sys/swing.c), spawned already swung (fun/_.c's hook)
 */
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_doors(ecs* world) {
    zox_module(doors);
    zox_components_doors(world);
    zox_systems_doors(world);
    add_hook_spawned_block(&door_on_spawned);
    zox_on_add(on_door_sound, [none] doors.DoorSound);
#ifdef zox_tests
    zox_tsts_doors(world);
#endif
}
