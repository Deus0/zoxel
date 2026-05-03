#include "hierarchy.c"
#include "inspector.c"
#include "profiler.c"
#include "label.c"

entity prefab_inspector;
entity prefab_hierarchy;
entity prefab_button_hierarchy;
entity prefab_game_debug_label;

entity spawn_prefab_button_hierarchy(ecs* world, entity p) {

    zox_prefab_child(p);
    zox_prefab_name("button_hierarchy");

    prefab_add_active_state(world, e, color_purple);

    return e;
}

void spawn_prefabs_editor(ecs* world) {
    prefab_hierarchy = spawn_prefab_hierarchy(world, prefab_window_textured);
    prefab_inspector = spawn_prefab_inspector(world, prefab_window_textured);
    prefab_button_hierarchy = spawn_prefab_button_hierarchy(world, prefab_button);
    prefab_game_debug_label = spawn_prefab_game_debug_label(world, prefab_label_background);
}
