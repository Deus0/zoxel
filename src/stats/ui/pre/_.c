entity prefab_statbar2;
#include "statbar2.c"
#include "statbar3.c"
#include "frame.c"
#include "icon.c"
#include "menu.c"

void spawn_prefabs_ui_stats(ecs *world) {
    prefab_statbar2 = spawn_prefab_statbar2(world, prefab_bar2);
}
