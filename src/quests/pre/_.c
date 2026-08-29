#include "quest.c"
entity prefab_quest;

void spawn_prefabs_quests(ecs *world) {
    prefab_quest = spawn_prefab_quest(world);
}
