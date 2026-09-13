#include "mesh.c"

entity prefab_mesh3;

void zox_prefabs_rendering3(ecs* world) {
    prefab_mesh3 = spawn_prefab_mesh3(world);
}
