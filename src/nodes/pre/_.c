#include "leaf.c"
#include "link.c"
#include "tree.c"

entity prefab_node_leaf;
entity prefab_node_link;
entity prefab_node_tree;

void spawn_prefabs_nodes(ecs* world) {
    prefab_node_leaf = spawn_prefab_node_leaf(world);
    prefab_node_link = spawn_prefab_node_link(world);
    prefab_node_tree = spawn_prefab_node_tree(world, prefab_node_leaf);
}
