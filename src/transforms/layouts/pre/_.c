// #include "canvas.c"
#include "layout2.c"
#include "layout3.c"
// entity prefab_canvas;
entity prefab_layout2_root;
entity prefab_layout2;
entity prefab_layout3;
entity prefab_layout3_child;

void spawn_prefabs_layouts2(ecs *world) {
    prefab_layout2_root = spawn_prefab_layout2_root(world);
    prefab_layout2 = spawn_prefab_layout2(world);
    // Layouts
    prefab_layout3 = spawn_prefab_layout3(world);
    prefab_layout3_child = spawn_prefab_layout3_child(world, prefab_layout3);
}
