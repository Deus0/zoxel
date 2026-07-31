zox_sys2(StatbarSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StatLink);
    zox_sys_out(BarLevel);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(StatLink, stat);
        zox_sys_o(BarLevel, bar);
        float new_value = bar->value;
        if (!zox_valid(stat->value)) {
            new_value = 0;
        } else if (zox_has(stat->value, StatState)) {
            zox_geter(stat->value, StatValue, value)
            zox_geter(stat->value, StatValueMax, max)
            new_value = value->value / max->value;
        } else if (zox_has(stat->value, StatLevel)) {
            zox_geter(stat->value, ExperienceValue, value)
            zox_geter(stat->value, ExperienceMax, max)
            new_value = value->value / max->value;
        }
        if (bar->value != new_value) {
            bar->value = new_value;
            if (dbg_log) {
                zox_log("Statbar [%s]:%f", zox_getn(stat->value), new_value);
            }
        }
    }
} zox_sys_end(StatbarSystem);
