// #include "canvas.c"
#include "layout2.c"
// entity prefab_canvas;
entity prefab_layout2;

void spawn_prefabs_layouts2(ecs *world) {
    prefab_layout2 = spawn_prefab_layout2(world);
}