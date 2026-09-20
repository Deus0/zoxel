#include "com/_.c"
#include "dat/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

void import_ui_containers(ecs* world) {
    zox_module(ui_containers);
    zox_components_ui_containers(world);
    zox_systems_ui_containers(world);
    add_hook_spawn_prefabs(zox_spawn_prefabs_ui_containers);
}
