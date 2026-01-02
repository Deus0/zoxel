entity prefab_pickup;
entity prefab_pickup_basic;
#include "pickup.c"

void spawn_prefabs_pickups(ecs *world) {
    prefab_pickup_basic = spawn_prefab_pickup(world, prefab_cube);
#ifdef zox_prefabs_non_textured
    prefab_pickup = spawn_prefab_pickup(world, prefab_cube);
#else
    prefab_pickup = spawn_prefab_pickup(world, prefab_cube_textured);
#endif
}
