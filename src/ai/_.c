/*
 * +--------------------------------------------------------+
 * | Zox Module: AI                                         |
 * |                                                        |
 * |  Decisions - Goals - Awareness - Behavior - Navigation |
 * |                                                        |
 * |  Uses: Characters                                      |
 * |                                                        |
 * +--------------------------------------------------------+
 */
// TODO: Rename back to AI and rename TerrainNpcs to npcs
float default_npc_follow_distance = 0.45f;
// TODO: a threat level - if you are higher level than npc they should have higher chance to flee!
#include "com/_.c"
#include "sta/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "fun/_.c"
#include "eve/_.c"

void import_ai(ecs* world) {
    zox_module(ai);
    zox_components_npcs(world);
    zox_systems_npcs(world);
    add_hook_spawn_prefabs(spawn_prefabs_npcs);
    add_hook_spawn_prefabs(zox_events_ai);
}
