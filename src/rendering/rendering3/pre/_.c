#include "mesh.c"

entity prefab_mesh3;
entity prefab_lit_mesh;

void zox_prefabs_rendering3(ecs* world) {
    prefab_mesh3 = spawn_prefab_mesh3(world);
    prefab_lit_mesh = spawn_prefab_lit_mesh(world);
}

