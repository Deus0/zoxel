byte zox_log_activations = 0;

#include "com/_.c"
#include "sys/_.c"

void zox_prefab_addc_user_timings(ecs* world, entity e) {
    zox_set(e, WarmupAt, { 0 });
    zox_set(e, WarmupState, { 0 });
    zox_set(e, CooldownAt, { 0 });
    zox_set(e, CooldownState, { 0 });
    zox_set(e, WarmupTime, { 0.5 });
    zox_set(e, CooldownTime, { 0.5 });
}

void import_timers(ecs* world) {
    zox_module(timers);
    zox_components_timers(world);
    zox_systems_timers(world);
}
