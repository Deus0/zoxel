// When health goes to 0, kill UserLink->value
// Set Dead to 1
zox_sys2(DeathSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StatValue);
    zox_sys_in(UserLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(StatValue, stat);
        zox_sys_i(UserLink, user);

        if (stat->value > 0 || !zox_valid(user->value) ||
            !zox_has(user->value, Dead) || zox_gett_value(user->value, Dead)) {
            continue;
        }
        // we should just set a dead state here
        zox_muter(user->value, Dead, dead);
        dead->value = zox_dirty_trigger;
    }
} zox_sys_end(DeathSystem);