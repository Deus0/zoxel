/*
 * +------------------------------------------------------------------+
 * | Zox Module: Nodes                                                   |
 * |                                                                  |
 * |  Graphs - Links - Trees - Connections - Node State                 |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *  Used By:
 *
 *      - Quests
 *      - Models
 *      - Neurals
 *
 */
#ifndef zoxm_nodes
#define zoxm_nodes

#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Nodes) {
    define_components_nodes(world);
    define_systems_nodes(world);
    add_hook_spawn_prefabs(spawn_prefabs_nodes);
} zox_end_module(Nodes);

#endif
