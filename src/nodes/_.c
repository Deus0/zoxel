// generic nodes, used by blueprints and neurals and quests etc
#ifndef zoxm_nodes
#define zoxm_nodes

#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Nodes)
    add_hook_spawn_prefabs(spawn_prefabs_nodes);
    define_components_nodes(world);
    define_systems_nodes(world);
zox_end_module(Nodes)

#endif
