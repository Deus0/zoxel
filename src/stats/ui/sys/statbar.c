zox_sys2(StatbarSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(BarLevel);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(BarLevel, bar);
        float new_value = bar->value;
        entity stat = zox_get_link(world, e, StatLink);
        if (!zox_valid(stat)) {
            new_value = 0;
            if (dbg_log) {
                zox_logw("Statbar Unlinked [%s]",
                    zox_sys_e_name);
            }
        } else if (zox_has(stat, StatState)) {
            zox_geter(stat, StatValue, value);
            zox_geter(stat, StatValueMax, max);
            new_value = value->value / max->value;
        } else if (zox_has(stat, StatLevel)) {
            zox_geter(stat, ExperienceValue, value);
            zox_geter(stat, ExperienceMax, max);
            new_value = value->value / max->value;
        }
        if (bar->value != new_value) {
            bar->value = new_value;
            if (dbg_log) {
                zox_log("[%s] Statbar [%s]:%f",
                    zox_sys_e_name,
                    zox_getn(stat),
                    new_value);
            }
        }
    }
} zox_sys_end(StatbarSystem);
