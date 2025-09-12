#include "tree.c"
#include "leaf.c"
#include "run.c"

entity prefab_dialogue_tree;
entity prefab_dialogue_leaf;
entity prefab_dialogue_run;

void spawn_prefabs_dialogues(ecs* world) {
    prefab_dialogue_tree = spawn_prefab_dialogue_tree(world, prefab_node_tree);
    prefab_dialogue_leaf = spawn_prefab_dialogue_leaf(world, prefab_node_leaf);
    prefab_dialogue_run = spawn_prefab_dialogue_run(world, prefab_node_run);

    // TODO: test spawn a tree here and display it on screen ui
    entity test = spawn_dialogue_tree(world, prefab_dialogue_tree, "Hello World.");

    entity test_run = spawn_dialogue_run(world, prefab_dialogue_run, test);

    // Now Run it
    // Go Through Nodes - key press
}