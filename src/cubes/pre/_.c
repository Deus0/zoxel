#include "cube.c"
#include "textured.c"

entity prefab_cube;
entity prefab_cube_textured;

void spawn_prefabs_cubes(ecs *world) {
    prefab_cube = spawn_prefab_cube(world);
    prefab_cube_textured = spawn_prefab_cube_textured(world, prefab_cube);
}
