/*
 * +--------------------------------------------------------------+
 * | Zox Module: Activations                                      |
 * |                                                              |
 * |  Right Click - Blocks By Kind                                |
 * |                                                              |
 * |  Uses: Raycasts - Doors - Beds                               |
 * |                                                              |
 * +--------------------------------------------------------------+
 *
 *  - a right click (TriggerActionB) on a block in reach uses it, by its kind (its tag): a door (BlockDoor) opens or
 *    closes (doors, door_toggle), a bed (BlockBed) is slept in (beds, character_sleep)
 *  - any entity with TriggerActionB, RaycastVoxelData and RaycastRange: a player's character today, npcs later
 */
#include "sys.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_activations(ecs* world) {
    zox_module(activations);
    zox_system(
        block_activate_system,
        zoxp_update,
        [in] core.TriggerActionB,
        [in] vrays.RaycastVoxelData,
        [in] vrays.RaycastRange,
    );
#ifdef zox_tests
    zox_tsts_activations(world);
#endif
}
