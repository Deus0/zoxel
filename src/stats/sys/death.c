// When health goes to 0, kill UserLink->value
// Set Dead to 1
zox_sys2(DeathSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StatValue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(StatValue, stat);
        entity user = zox_get_parent(world, e);
        if (!zox_valid(user) || !zox_has(user, Dead)) {
            continue;
        }
        if (stat->value > 0 || zox_getv(user, Dead)) {
            continue;
        }
        // we should just set a dead state here
        zox_muter(user, Dead, dead);
        dead->value = zox_dirty_trigger;
    }
} zox_sys_end(DeathSystem);
