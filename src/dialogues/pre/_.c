#include "node.c"
#include "process.c"
#include "ui.c"

entity prefab_dialogue_node;
entity prefab_process_dialogue;
entity prefab_dialogue_ui;

void spawn_prefabs_dialogues(ecs* world) {
    // prefab_dialogue_tree = spawn_prefab_dialogue_tree(world, prefab_node_tree);
    prefab_dialogue_node = spawn_prefab_dialogue_node(world, prefab_node);
    prefab_process_dialogue = spawn_prefab_process_dialogue(world, prefab_process);
    prefab_dialogue_ui = spawn_prefab_dialogue_ui(world, prefab_window); // prefab_window_textured

    if (prefab_realm) {
        zox_prefab_add(prefab_realm, DialoguetreeLinks);
    }

    if (prefab_character3) {
        zox_prefab_character_add(DialogueProcessLink);
        zox_prefab_character_add(DialoguetreeLink);
    }
}
