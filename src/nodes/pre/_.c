#include "leaf.c"
#include "link.c"
#include "tree.c"
#include "run.c"

entity prefab_node_leaf;
entity prefab_node_link;
entity prefab_node_tree;
entity prefab_node_run;

void spawn_prefabs_nodes(ecs* world) {
    prefab_node_leaf = spawn_prefab_node_leaf(world);
    prefab_node_link = spawn_prefab_node_link(world);
    prefab_node_tree = spawn_prefab_node_tree(world, prefab_node_leaf);
    prefab_node_run = spawn_prefab_node_run(world);
}
