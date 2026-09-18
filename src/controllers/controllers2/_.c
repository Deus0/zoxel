#include "dat/settings.c"
zox_tag(PlayerCharacter2);
zoxc_float2(Movement2);
#include "pre/_.c"
#include "sys/_.c"
#include "fun/label.c"

void import_controllers2(ecs* world) {
    zox_module(controllers2);
    zoxd_tag(PlayerCharacter2);
    zoxd_float2(Movement2);
    zox_systems_controllers2(world);
    add_hook_spawn_prefabs(spawn_prefabs_controllers2);
}
// TODO: movement should be done in one system, and we can use a Move float2 in our character - Movement2 and Movement3 - should be a float -1 to 1, and determine intention to move

