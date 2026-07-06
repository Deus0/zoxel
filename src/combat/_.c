#if !defined(zoxm_combat) && defined(zoxm_users)
#define zoxm_combat

zoxc_byte(CombatState);
zoxc_entity(LastDamager);
zoxc_double(LastCombatTime);
zoxc_byte(Dead);
zoxc_double(DiedTime);
zoxc_float(Damage);

void zox_define_components_combat(ecs* world) {
    zoxd_byte(Dead);
    zoxd_double(DiedTime);
    zoxd_double(LastCombatTime);
    zoxd_entity(CombatState);
    zoxd_entity(LastDamager);
    zoxd_float(Damage);
}

#include "sta/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"
#include "dbg/_.c"

zox_begin_module(Combat) {
    zox_define_components_combat(world);
    define_systems_combat(world);
    add_hook_spawn_prefabs(spawn_prefabs_combat);
} zox_end_module(Combat);

#endif
