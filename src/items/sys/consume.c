// Consumes a consumable item, adding its ConsumeValue to the users linked stat
// NOTE: ConsumeStat links to a realm stat, we find the users instance of it
zox_sys2(ItemConsumeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    zox_sys_in(ConsumeValue);
    zox_sys_out(Quantity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Activate, activate);
        zox_sys_i(ConsumeValue, consume_value);
        zox_sys_o(Quantity, quantity);
        if (activate->value != zox_dirty_active ||
            !quantity->value)
        {
            continue;
        }
        entity user = zox_get_parent(world, e);
        if (!zox_valid(user) ||
            zox_has(user, Dead))
        {
            continue;
        }
        entity realm_stat = zox_get_link(world, e, ConsumeStat);
        entity stat = get_user_stat_from_realm(world, user, realm_stat);
        if (!zox_valid(stat) ||
            !zox_has(stat, StatValue))
        {
            if (dbg_log) {
                zox_log("[%s] can't consume [%s], no stat [%s]",
                    zox_getn(user),
                    zox_sys_e_name,
                    zox_getn(realm_stat));
            }
            continue;
        }
        StatValue* value = zox_mut(stat, StatValue);
        // don't waste it when full
        byte has_max = zox_has(stat, StatValueMax);
        float max = has_max ? zox_getv(stat, StatValueMax) : 0;
        if (has_max && value->value >= max) {
            continue;
        }
        value->value += consume_value->value;
        if (has_max && value->value > max) {
            value->value = max;
        }
        zox_add(stat, Dirty);
        zox_add(stat, DataDirty);
        // use up item
        quantity->value--;
        zox_add(e, QuantityDirty);
        zox_add(e, Dirty);
        zox_add(e, DataDirty);
        // consume sound
        spawn_sound_generated(
            world,
            prefab_sound_generated,
            instrument_violin,
            note_frequencies[40 + rand() % 6],
            0.4,
            1.2f * get_volume_sfx());
        if (zox_has(user, SwingStart)) {
            float swing_time =
                zox_getv(e, WarmupTime) +
                zox_getv(e, CooldownTime);
            zox_setv(user, SwingStart, zox_current_time);
            zox_setv(user, SwingSpeed, swing_time);
        }
        if (dbg_log) {
            zox_log("[%s] consumed [%s] +[%f] [%s] -> [%f]",
                zox_getn(user),
                zox_sys_e_name,
                consume_value->value,
                zox_getn(stat),
                value->value);
        }
    }
} zox_sys_end(ItemConsumeSystem);
