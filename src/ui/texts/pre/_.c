#include "text.c"
entity prefab_zext;
entity prefab_zext_background;

void spawn_prefabs_texts(ecs *world) {
    prefab_zext = spawn_prefab_zext(world, prefab_layout2);
    prefab_zext_background = spawn_prefab_zext(world, prefab_element_textured);
}