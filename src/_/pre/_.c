entity prefab_profile;

#include "profile.c"

void zox_prefabs_core(ecs* world) {
    prefab_profile = spawn_prefab_profile(world);
}