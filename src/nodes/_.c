/*
 * +----------------------------------------------------------+
 * | Zox Module: Nodes                                        |
 * |                                                          |
 * |  Graphs - Links - Trees - Connections - Node State       |
 * |                                                          |
 * +----------------------------------------------------------+
 *
 *  Used By:
 *
 *      - Quests
 *      - Models
 *      - Neurals
 *
 */
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_nodes(ecs* world) {
    zox_module(nodes);
    zox_components_nodes(world);
    zox_systems_nodes(world);
    add_hook_spawn_prefabs(spawn_prefabs_nodes);
}
