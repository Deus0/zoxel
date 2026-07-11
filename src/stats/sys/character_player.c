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
        zox_geter(realm->value, StatLinks, stats);
        spawn_base_stats(world, e, stats);
        // add all attributes as 0
        for (int j = 0; j < stats->length; j++) {
            entity rstat = stats->value[j];
            if (!zox_valid(rstat)) {
                continue;
            }
            if (!zox_has(rstat, StatAttribute)) {
                continue;
            }
            spawn_stat_attribute(world, e, rstat, 0);
        }
    }
} zox_sys_end(PlayerCharacterStatsSystem);
