#ifndef zox_timers
#define zox_timers

byte zox_log_activations = 0;

#include "com/_.c"
#include "sys/_.c"

void zox_prefab_addc_user_timings(ecs* world, entity e) {
    zox_prefab_set(e, WarmupAt, { 0 });
    zox_prefab_set(e, WarmupState, { 0 });
    zox_prefab_set(e, CooldownAt, { 0 });
    zox_prefab_set(e, CooldownState, { 0 });
    zox_prefab_set(e, WarmupTime, { 0.5 });
    zox_prefab_set(e, CooldownTime, { 0.5 });
}

zox_begin_module(Timers) {
    zox_define_components_timers(world);
    zox_define_systems_timers(world);
} zox_end_module(Timers)

#endif
