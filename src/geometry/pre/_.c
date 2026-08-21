#include "cube.c"
#include "textured.c"

entity prefab_cube;
entity prefab_cube_textured;

void zox_prefabs_geometry(ecs* world) {
    prefab_cube = spawn_prefab_cube(world);
    prefab_cube_textured = spawn_prefab_cube_textured(world, prefab_cube);
}
