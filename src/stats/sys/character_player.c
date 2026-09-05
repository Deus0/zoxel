// NOTE: Players are richer beings from the aether
// - This runs after normal Character Stats
zox_sys2(PlayerCharacterStatsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        if (state->value != zox_dirty_active) {
            continue;
        }
        spawn_base_stats(world, e, realm->value);
        // add all attributes as 0
        iter it2 = zox_children(world, realm->value);
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
        /*for (int j = 0; j < stats->length; j++) {
            entity rstat = stats->value[j];
            if (!zox_valid(rstat)) {
                continue;
            }*/
            }
        }
    }
} zox_sys_end(PlayerCharacterStatsSystem);
