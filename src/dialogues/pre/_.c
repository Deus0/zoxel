entity prefab_node_speech;
entity prefab_process_dialogue;
entity prefab_dialogue_text;

#include "node.c"
#include "process.c"
#include "ui.c"

void zox_prefabs_dialogues(ecs* world) {
    prefab_node_speech = spawn_prefab_node_speech(
        world,
        prefab_node);
    prefab_process_dialogue = spawn_prefab_process_dialogue(
        world,
        prefab_process);
    prefab_dialogue_text = spawn_prefab_dialogue_text(
        world,
        prefab_text);
}
