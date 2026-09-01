entity prefab_quest;
entity prefab_node_give_quest;

#include "quest.c"
#include "nodes.c"

void zox_prefabs_quests(ecs *world) {
    prefab_quest = spawn_prefab_quest(world);
    prefab_node_give_quest = spawn_prefab_node_give_quest(
        world,
        prefab_node);
}
