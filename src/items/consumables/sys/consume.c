// NOTE: A consumable used (Activate, its press): its user's eat swing starts, eaten at its first bite (item_consume,
//  eve/eat.c); not when it can't be (dead, no stat to restore, or it's full: don't waste it)
// NOTE: ConsumeStat links to a realm stat, we find the users instance of it
static inline entity item_consume_stat(ecs* world, entity e, entity user) {
    if (!zox_valid(user) || zox_has(user, Dead)) {
        return 0;
    }
    const entity stat = get_user_stat_from_realm(world, user, zox_get_link(world, e, ConsumeStat));
    if (!zox_valid(stat) || !zox_has(stat, StatValue)) {
        return 0;
    }
    if (zox_has(stat, StatValueMax) && zox_getv(stat, StatValue) >= zox_getv(stat, StatValueMax)) {
        return 0;
    }
    return stat;
}

void item_consume_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    zox_sys_in(Quantity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Activate, activate);
        zox_sys_i(Quantity, quantity);
        if (activate->value != zox_dirty_active || !quantity->value) {
            continue;
        }
        const entity user = zox_get_parent(world, e);
        if (!item_consume_stat(world, e, user)) {
            continue;
        }
        skeleton_swing_begin(world, user, e, zox_current_time, zox_getv(e, SwingTime), zox_swing_eat, float3_zero);
    }
    zox_sys_on_end();
} zoxd_system(item_consume_system);

// eaten, at the eat swing's first bite: its ConsumeValue added to its user's stat, one used up, its sound
void item_consume(ecs* world, entity e, entity user) {
    const entity stat = zox_valid(e) && zox_getv(e, Quantity) ? item_consume_stat(world, e, user) : 0;
    if (!stat) {
        return;
    }
    StatValue* value = zox_mut(stat, StatValue);
    value->value += zox_getv(e, ConsumeValue);
    if (zox_has(stat, StatValueMax) && value->value > zox_getv(stat, StatValueMax)) {
        value->value = zox_getv(stat, StatValueMax);
    }
    zox_add(stat, Dirty);
    zox_add(stat, DataDirty);
    zox_getm(e, Quantity)->value--;
    zox_add(e, QuantityDirty);
    zox_add(e, Dirty);
    zox_add(e, DataDirty);
    spawn_sound_effect(world, sound_effect_consume, user);
}
