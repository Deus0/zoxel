entity prefab_inspector;
entity prefab_hierarchy;

#include "hierarchy.c"
#include "inspector.c"
#include "profiler.c"

void spawn_prefabs_editor(ecs* world) {
    prefab_hierarchy = spawn_prefab_hierarchy(world, prefab_window);
    prefab_inspector = spawn_prefab_inspector(world, prefab_window);
}
