#include "node.c"
#include "process.c"

entity prefab_node_model;
entity prefab_process_model;

void spawn_prefabs_models(ecs* world) {
    prefab_node_model = spawn_prefab_node_model(world, prefab_node);
    prefab_process_model = spawn_prefab_process_model(world, prefab_process);
}