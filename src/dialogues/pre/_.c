entity prefab_dialogue_node;
entity prefab_process_dialogue;
entity prefab_dialogue_text;

#include "node.c"
#include "process.c"
#include "ui.c"

void spawn_prefabs_dialogues(ecs* world) {
    prefab_dialogue_node = spawn_prefab_dialogue_node(
        world,
        prefab_node);
    prefab_process_dialogue = spawn_prefab_process_dialogue(
        world,
        prefab_process);
    prefab_dialogue_text = spawn_prefab_dialogue_text(
        world,
        prefab_text);
}
