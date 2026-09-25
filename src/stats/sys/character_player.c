// NOTE: Players are richer beings from the aether
// - This runs after normal Character Stats
zox_sys2(PlayerCharacterStatsSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity realm = zox_get_link(world, e, RealmLink);
        spawn_base_stats(
            world,
            e,
            realm,
            dbg_log);
        // add all attributes as 0
        iter it2 = zox_children(world, realm);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity stat = it2.entities[j];
                if (!zox_has(stat, Stat)) {
                    continue;
                }
                if (!zox_has(stat, StatAttribute)) {
                    continue;
                }
                spawn_stat_attribute(world, e, stat, 0);
            }
        }
    }
} zox_sys_end(PlayerCharacterStatsSystem);
