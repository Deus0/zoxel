entity prefab_statbar2D;
entity prefab_statbar3D;
#include "statbar2.c"
#include "statbar3.c"
#include "frame.c"
#include "icon.c"
#include "menu.c"

void spawn_prefabs_ui_stats(ecs *world) {
    prefab_statbar2D = spawn_prefab_statbar2(world, prefab_elementbar2);
    prefab_statbar3D = spawn_prefab_statbar3D(world, prefab_elementbar3D);
}
