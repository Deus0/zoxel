// When health goes to 0, kill UserLink->value
// Set Dead to 1
void pre_death_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StatValue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(StatValue, stat);
        entity user = zox_get_parent(world, e);
        if (!zox_valid(user) ||
            zox_has(user, Dead) ||
            stat->value > 0)
        {
            if (dbg_log >= 2) {
                zox_log("[%s] healthy at [%f]",
                    zox_getn(user),
                    stat->value);
            }
            continue;
        }
        zox_add(user, PreDeath);
        if (dbg_log) {
            zox_log("[%s] has Pre Died.",
                zox_getn(user));
        }
    }
    zox_sys_on_end();
} zoxd_system(pre_death_system);

void death_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, PreDeath);
        zox_add(e, Dead);
        zox_add(e, DeathDirty);
        zox_add(e, DisableMovement);
        zox_setv(e, DiedTime, zox_current_time);
        if (dbg_log) {
            zox_log("[%s] has Died.",
                zox_sys_e_name);
        }
    }
    zox_sys_on_end();
} zoxd_system(death_system);

void post_death_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, DeathDirty);
    }
    zox_sys_on_end();
} zoxd_system(post_death_system);

