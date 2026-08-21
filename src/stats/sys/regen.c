// todo: rework from regen stat, and impact stat value
// do I need a velocity for stats? then regen will add to it? or just add directly..? :O hmm
zox_sys2(StatRegenSystem) {
    init_delta_time();
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
        if (!zox_valid(user) || !zox_has(user, Dead)) {
            continue;
        }
        byte dead = zox_getv(user, Dead);
        if (dead) {
            continue;
        }
        if (value->value >= max->value) {
            continue;
        }
        value->value += delta_time * regen_rate;
        if (value->value > max->value) {
            value->value = max->value;
        }
        // Signals to UI
        if (dirty->value != zox_dirty_active) {
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(StatRegenSystem);
