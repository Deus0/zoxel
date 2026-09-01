#include "node.c"
#include "link.c"
#include "process.c"

entity prefab_node;
entity prefab_node_link;
entity prefab_process;
// damn work this out
extern entity prefab_realm;

void spawn_prefabs_nodes(ecs* world) {
    prefab_node = spawn_prefab_node(world);
    prefab_node_link = spawn_prefab_node_link(world);
    prefab_process = spawn_prefab_process(world);
}
