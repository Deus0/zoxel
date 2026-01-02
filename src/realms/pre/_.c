entity local_realm;
#include "realm.c"
entity prefab_realm;

void spawn_prefabs_realms(ecs* world) {
    prefab_realm = spawn_prefab_realm(world);
}