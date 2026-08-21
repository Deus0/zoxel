
// TODO: Make player just alter a MovementInput float2, then in Character, it grabs that, this seperates the control scheme from the movement itself

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "game/_.c"
#include "dbg/_.c"

void import_controllers3(ecs* world) {
    zox_module(controllers3);
    define_components_controllers3D(world);
    define_systems_controllers3(world);
    zox_define_systems_controllers3_game(world);
    add_hook_spawn_prefabs(spawn_prefabs_controllers3);
}