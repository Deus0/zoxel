#include "node.c"
#include "process.c"
#include "model_group.c"
entity prefab_node_model;
entity prefab_process_model;
entity prefab_model_group;

void spawn_prefabs_models(ecs* world) {
    prefab_model_group = spawn_prefab_model_group(world);
    prefab_node_model = spawn_prefab_node_model(world, prefab_node);
    prefab_process_model = spawn_prefab_process_model(world, prefab_process);
}