// When health goes to 0, kill UserLink->value
// Set Dead to 1
// NOTE: Generates base stats for Characters
zox_sys2(CharacterStatsSystem) {
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
        entity soule = zox_get_child_by_id(world, e, zox_id(StatSoul));
        if (!zox_valid(soule)) {
            spawn_stat_level(world, e, realm_soul, soul_value);
        }
        // Health
        entity healthe = zox_get_child_by_id(world, e, zox_id(StatHealth));
        if (!zox_valid(healthe)) {
            spawn_stat_state(world, e, realm_health, health.x, health.y);
        }
        spawn_stat_state(world, e, realm_energy, energy.x, energy.y);
        spawn_stat_state(world, e, realm_mana, mana.x, mana.y);
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
            spawn_stat_regen(world, e, rstat, 10);
        }
    }
} zox_sys_end(CharacterStatsSystem);
