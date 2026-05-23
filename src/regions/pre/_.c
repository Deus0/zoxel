#include "region.c"
entity prefab_region;

void spawn_prefabs_regions(ecs* world) {
    prefab_region = spawn_prefab_region(world);
    /*if (prefab_terrain) {
        zox_prefab_add(prefab_terrain, RegionLinks);
    }*/
}
