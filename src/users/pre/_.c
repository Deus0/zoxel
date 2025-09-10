#include "data.c"

entity prefab_userd;

void spawn_prefabs_users(ecs* world) {
    prefab_userd = spawn_prefab_userd(world);
}