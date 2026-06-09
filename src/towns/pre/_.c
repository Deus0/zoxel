#include "town.c"
entity prefab_town;

void zox_define_prefabs_towns(ecs* world) {
    if (prefab_tunk2) {
        zox_prefab_set(prefab_tunk2, TownMap, { 0 });
    }
    prefab_town = spawn_prefab_town(world);
}
