#include "text.c"
#include "input.c"
entity prefab_text;
entity prefab_text_textured;
entity prefab_text_input;

void spawn_prefabs_texts(ecs *world) {
    prefab_text = spawn_prefab_text(world, prefab_layout2, prefab_zigel);
    prefab_text_textured = spawn_prefab_text(world, prefab_element_frame, prefab_zigel);
    prefab_text_input = spawn_prefab_text_input(world, prefab_text_textured);
}
