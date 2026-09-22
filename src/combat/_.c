zoxc_byte(CombatState);
zoxc_float(Damage);
zoxc_double(LastCombatTime);
zox_tag(AttackerLink);

void zox_components_combat(ecs* world) {
    zoxd_byte(CombatState);
    zoxd_float(Damage);
    zoxd_double(LastCombatTime);
    zoxd_nf_tag(AttackerLink);
}

#include "sta/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"
#include "dbg/_.c"

void import_combat(ecs* world) {
    zox_module(combat);
    zox_components_combat(world);
    zox_systems_combat(world);
    add_hook_spawn_prefabs(spawn_prefabs_combat);
}
