// generic nodes, used by blueprints and neurals and quests etc
#ifndef zoxm_nodes
#define zoxm_nodes

zox_tag(Node);
zox_tag(Nodelink);
zox_tag(Nodegraph);
zoxc_entity(NodeParent);
zoxc_entity(NodeNext);
#include "prefabs/prefabs.c"

zox_begin_module(Nodes)
    zoxd_tag(Node);
    zoxd_tag(Nodelink);
    zoxd_tag(Nodegraph);
    zoxd_entity(NodeParent);
    zoxd_entity(NodeNext);
    spawn_prefabs_nodes(world);
    // test_nodes(world);
zox_end_module(Nodes)

#endif
