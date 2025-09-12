#include "tree.c"
#include "leaf.c"

entity prefab_dialogue_tree;
entity prefab_dialogue_leaf;

void spawn_prefabs_dialogues(ecs* world) {
    prefab_dialogue_tree = spawn_prefab_dialogue_tree(world, prefab_node_tree);
    prefab_dialogue_leaf = spawn_prefab_dialogue_leaf(world, prefab_node_leaf);

    // TODO: test spawn a tree here and display it on screen ui
    entity test = spawn_dialogue_tree(world, prefab_dialogue_tree, "Hello World.");
}