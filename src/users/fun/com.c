void zox_prefab_addc_user_timings(
    ecs* world,
    entity e
) {
    zox_prefab_set(e, WarmupAt, { 0 });
    zox_prefab_set(e, WarmupState, { 0 });
    zox_prefab_set(e, CooldownAt, { 0 });
    zox_prefab_set(e, CooldownState, { 0 });
    zox_prefab_set(e, WarmupTime, { 0.5 });
    zox_prefab_set(e, CooldownTime, { 0.5 });
}