#include "text.c"
#include "input.c"
entity prefab_zext;
entity prefab_zext_background;
entity prefab_zext_input;

void spawn_prefabs_texts(ecs *world) {
    prefab_zext = spawn_prefab_zext(world, prefab_layout2);
    prefab_zext_background = spawn_prefab_zext(world, prefab_element_textured);
    prefab_zext_input = spawn_prefab_zext_input(world, prefab_zext_background);
}