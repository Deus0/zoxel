entity prefab_event;

#include "event.c"

void spawn_prefabs_generic(ecs* world) {
    prefab_event = spawn_prefab_generic_event(world);
}