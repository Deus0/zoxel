entity prefab_line3D;
entity prefab_temporary_line3D;
entity prefab_cube_lines;
#include "line.c"
#include "temporary_line3D.c"
#include "cube.c"
#include "arrow.c"
#include "quad.c"
entity prefab_quad_lines;

void spawn_prefabs_lines3D(ecs *world) {
    prefab_line3D = spawn_prefab_line3D(world);
    prefab_temporary_line3D = spawn_prefab_temporary_line3D(world);
    prefab_cube_lines = spawn_prefab_cube_lines(world);
    prefab_quad_lines = spawn_prefab_quad_lines(world);
}
