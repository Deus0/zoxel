#include "character.c"

entity prefab_character2;

void spawn_prefabs_characters2(ecs *world) {
    prefab_character2 = spawn_prefab_character2(world);
}