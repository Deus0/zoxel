entity local_realm;
entity prefab_realm;

#include "realm.c"

entity get_linked_realm(ecs* world, entity e) {
    return zox_valid(e) && zox_has(e, RealmLink) ? zox_getv(e, RealmLink) : 0;
}

void spawn_prefabs_realms(ecs* world) {
    prefab_realm = spawn_prefab_realm(world);
}
