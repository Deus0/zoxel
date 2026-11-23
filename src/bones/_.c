/*
 * +------------------------------------------------------+
 * | Zox Module: Bones                                    |
 * |                                                      |
 * |  Skeletons - Joints - Poses - Animation - Hierarchy  |
 |  ik: arms reaching for a point (ik/_.c)              |
 * |                                                      |
 * +------------------------------------------------------+
 */
#include "sha/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "ik/com/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "fun/_.c"
#include "eve/_.c"
#include "ik/_.c"

void import_bones(ecs* world) {
    zox_module(bones);
    zox_components_bones(world);
    zox_systems_bones(world);
    zox_add_module(ik);
    add_hook_spawn_prefabs(spawn_prefabs_bones);
    add_hook_spawn_prefabs(zox_events_bones);
    add_hook_terminal_command(process_arguments_bones);
}