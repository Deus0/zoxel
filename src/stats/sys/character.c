// When health goes to 0, kill UserLink->value
// Set Dead to 1
// NOTE: Generates base stats for Characters
// TODO: Just add these onto the prefab
zox_sys2(CharacterStatsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    // zox_sys_out(StatLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        // zox_sys_o(StatLinks, stats);
        if (state->value != zox_dirty_active) {
            continue;
        }
        // Collect Realm Stats
        zox_geter(realm->value, StatLinks, rstats);
        entity realm_soul = 0;
        entity realm_health = 0;
        entity realm_energy = 0;
        entity realm_mana = 0;
        for (int j = 0; j < rstats->length; j++) {
            entity stat = rstats->value[j];
            if (!realm_soul && zox_has(stat, StatLevel)) {
                realm_soul = stat;
            }
            if (!realm_health && zox_has(stat, StatState)) {
                realm_health = stat;
            } else if (!realm_energy && zox_has(stat, StatState)) {
                realm_energy = stat;
            } else if (!realm_mana && zox_has(stat, StatState)) {
                realm_mana = stat;
            }
        }
        if (!zox_valid(realm_soul)) {
            zox_log_error("the realm has invalid stats")
            return;
        }
        // generate numbers here
        float soul_value = zox_has(e, PlayerLink) ? 1 : randf_range(1, 3);
        float2 health = (float2) { health_base, health_base + soul_value * health_level_increase };
        health.x = randf_range(health_base, health.y);
        float2 energy = (float2) { energy_base, energy_base + soul_value * energy_level_increase };
        float2 mana = (float2) { mana_base, mana_base + soul_value * mana_level_increase };
        // Soul
        entity ssoul = spawn_stat_level(world, realm_soul, e, NULL, soul_value, 0);
        // Health
        entity shealth = spawn_stat_state(world, realm_health, e, NULL, health.x, health.y, 0);
        entity senergy = spawn_stat_state(world, realm_energy, e, NULL, energy.x, energy.y, 0);
        entity smana = spawn_stat_state(world, realm_mana, e, NULL, mana.x, mana.y, 0);
        /*add_to_StatLinks(stats, ssoul);
        add_to_StatLinks(stats, shealth);
        add_to_StatLinks(stats, senergy);
        add_to_StatLinks(stats, smana);*/
        // Add Regen Stats
        for (int j = 0; j < rstats->length; j++) {
            entity rstat = rstats->value[j];
            if (!zox_valid(rstat)) {
                continue;
            }
            if (!zox_has(rstat, StatRegen)) {
                continue;
            }
            // Spawn a regen
            entity stat = spawn_stat_regen(world, rstat, e, NULL, 10, 0);
            // entity character_stat = spawn_user_stat(world, rstat, e);
            // add_to_StatLinks(stats, stat);
        }

        //entity stat_soul = spawn_user_stat(world, realm_soul, e);
        //zox_set(stat_soul, StatValue, { soul_value });
        /*entity stat_health = spawn_user_stat(world, realm_health, e);
        zox_set(stat_health, StatValue, { health.x })
        zox_set(stat_health, StatValueMax, { health.y })*/
        // Energy
        /*entity stat_energy = spawn_user_stat(world, realm_energy, e);
        zox_set(stat_energy, StatValue, { energy_base });
        zox_set(stat_energy, StatValueMax, { energy_base + soul_value * energy_level_increase })
        add_to_StatLinks(stats, stat_energy);*/
        // Mana
        /*entity stat_mana = spawn_user_stat(world, realm_mana, e);
        zox_set(stat_mana, StatValue, { mana_base })
        zox_set(stat_mana, StatValueMax, { mana_base + soul_value * mana_level_increase })
        add_to_StatLinks(stats, stat_mana);*/
    }
} zox_sys_end(CharacterStatsSystem);
