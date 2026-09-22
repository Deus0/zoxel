entity prefab_quest;

#include "quest.c"

void zox_prefabs_quests(ecs *world) {
    prefab_quest = spawn_prefab_quest(world);
}
