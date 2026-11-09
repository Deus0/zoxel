// Regen stats add their value (per second) to their linked StatState
void stat_regen_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StatValue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(StatValue, regen_value);
        entity user = zox_get_parent(world, e);
        if (!zox_valid(user) ||
            zox_has(user, Dead))
        {
            continue;
        }
        // linked stat must be the users own stat
        entity stat = zox_get_link(world, e, StatLink);
        if (!zox_valid(stat) ||
            zox_get_parent(world, stat) != user)
        {
            // link from realm regens target, applies next frame
            // NOTE: instances copy the realm regens StatLink, so remove it
            //  - unlink_all + link while deferred removes both, so unlink the old target only
            entity old_stat = stat;
            stat = get_user_stat_link(world, user, e);
            if (zox_valid(stat)) {
                if (zox_valid(old_stat) && old_stat != stat) {
                    zox_unlink(world, e, StatLink, old_stat);
                }
                zox_link(world, e, StatLink, stat);
            }
            continue;
        }
        if (!zox_has(stat, StatValue) ||
            !zox_has(stat, StatValueMax))
        {
            continue;
        }
        const float max = zox_getv(stat, StatValueMax);
        StatValue* value = zox_mut(stat, StatValue);
        if (!value ||
            value->value >= max)
        {
            continue;
        }
        if (value->value < 0) {
            value->value = 0;
        }
        // NOTE: Health at 0 is dying, pre_death_system disables our regens
        //  - until then don't heal it, or regen may cancel the death
        if (value->value == 0 &&
            zox_has(stat, StatHealth))
        {
            continue;
        }
        float regen = regen_value->value * regen_rate;
        if (regen <= 0) {
            continue;
        }
        // in combat?
        if (zox_has(stat, StatHealth)) {
            byte in_combat = zox_getv(user, CombatState) != zox_combat_peace;
            if (in_combat) {
                regen *= combat_regen_mul;
            }
        }
        value->value += zox_delta_time * regen;
        if (value->value > max) {
            value->value = max;
        }
        zox_add(stat, Dirty);
        zox_add(stat, DataDirty);
    }
    zox_sys_on_end();
} zoxd_system(stat_regen_system);
