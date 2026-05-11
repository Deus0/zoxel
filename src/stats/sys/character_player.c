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
        if (state->value != zox_dirty_end) {
            continue;
        }
        zox_geter(realm->value, StatLinks, rstats);
        // add all attributes as 0
        for (int j = 0; j < rstats->length; j++) {
            entity rstat = rstats->value[j];
            if (!zox_valid(rstat)) {
                continue;
            }
            if (!zox_has(rstat, StatAttribute)) {
                continue;
            }
            // Spawn a regen
            entity stat = spawn_stat_attribute(world, rstat, e, NULL, 0, 0);
            // add_to_StatLinks(stats, stat);

            /*if (zox_has(stat, StatAttribute)) {
                entity character_stat = spawn_user_stat(world, stat, e);
                add_to_StatLinks(stats, character_stat);
            }*/
        }
    }
} zox_sys_end(PlayerCharacterStatsSystem);
