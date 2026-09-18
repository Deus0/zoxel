/*
 * +------------------------------------------------------+
 * | Zox Module: Bones                                    |
 * |                                                      |
 * |  Skeletons - Joints - Poses - Animation - Hierarchy  |
 * |                                                      |
 * +------------------------------------------------------+
 */
#include "sha/_.c"
#include "com/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"
#include "eve/_.c"

void import_bones(ecs* world) {
    zox_module(bones);
    zox_components_bones(world);
    zox_systems_bones(world);
    add_hook_spawn_prefabs(spawn_prefabs_bones);
    add_hook_spawn_prefabs(zox_events_bones);
}