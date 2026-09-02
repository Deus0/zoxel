// todo: rework from regen stat, and impact stat value
// do I need a velocity for stats? then regen will add to it? or just add directly..? :O hmm
void stat_regen_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StatValueMax);
    zox_sys_out(StatValue);
    zox_sys_out(StatDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(StatValueMax, max);
        zox_sys_o(StatValue, value);
        zox_sys_o(StatDirty, dirty);
        entity user = zox_get_parent(world, e);
        if (!zox_valid(user) ||
            zox_has(user, Dead) ||
            value->value >= max->value)
        {
            continue;
        }
        if (value->value < 0) {
            value->value = 0;
        }
        // PreDeath will be added in other system
        if (value->value == 0) {
            continue;
        }
        value->value += zox_delta_time * regen_rate;
        if (value->value > max->value) {
            value->value = max->value;
        }
        // Signals to UI
        if (dirty->value != zox_dirty_active) {
            dirty->value = zox_dirty_trigger;
        }
    }
    zox_sys_on_end();
} zoxd_system(stat_regen_system);
