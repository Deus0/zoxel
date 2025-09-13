#include "tree.c"
#include "leaf.c"
#include "run.c"
#include "ui.c"

entity prefab_dialogue_tree;
entity prefab_dialogue_leaf;
entity prefab_dialogue_run;
entity prefab_dialogue_ui;

void spawn_prefabs_dialogues(ecs* world) {
    prefab_dialogue_tree = spawn_prefab_dialogue_tree(world, prefab_node_tree);
    prefab_dialogue_leaf = spawn_prefab_dialogue_leaf(world, prefab_node_leaf);
    prefab_dialogue_run = spawn_prefab_dialogue_run(world, prefab_node_run);
    prefab_dialogue_ui = spawn_prefab_dialogue_ui(world, prefab_window);
}