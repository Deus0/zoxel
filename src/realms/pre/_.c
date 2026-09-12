entity local_realm;
entity prefab_realm;

#include "realm.c"

entity get_linked_realm(ecs* world, entity e) {
    return zox_get_link(world, e, RealmLink);
}

void spawn_prefabs_realms(ecs* world) {
    prefab_realm = spawn_prefab_realm(world);
}
