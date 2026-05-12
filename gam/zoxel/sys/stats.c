zox_sys2(StatsRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(StatLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(StatLinks, stats);
        if (state->value != zox_generate_realm_stats) {
            continue;
        }
        float sat = 1.4f;
        float dark = 0.4f;
        color_rgb soul_c = color_rgb_purple;
        color_rgb health_c = color_rgb_red;
        color_rgb energy_c = color_rgb_yellow;
        color_rgb mana_c = color_rgb_cyan;
        soul_c = color_rgb_darken(color_rgb_saturate(soul_c, sat), dark);
        health_c = color_rgb_darken(color_rgb_saturate(health_c, sat), dark);
        energy_c = color_rgb_darken(color_rgb_saturate(energy_c, sat), dark);
        mana_c = color_rgb_darken(color_rgb_saturate(mana_c, sat), dark);
        // levels
        // entity soul = spawn_stat_level_t(world, "soul", "soul", soul_c);
        entity soul = spawn_realm_stat(world, e, prefab_stat_level, "soul", "soul", soul_c);
        zox_add_tag(soul, StatSoul);
        // states
        entity health = spawn_realm_stat(world, e, prefab_stat_state, "health", "health", health_c);
        zox_add_tag(health, HealthStat);
        entity energy = spawn_realm_stat(world, e, prefab_stat_state, "energy", "energy", energy_c);
        entity mana = spawn_realm_stat(world, e, prefab_stat_state, "mana", "mana", mana_c);
        // Regens
        entity health_regen = spawn_realm_stat(world, e, prefab_stat_regen, "health regen", "healthregen", health_c);
        entity energy_regen = spawn_realm_stat(world, e, prefab_stat_regen, "energy regen", "energyregen", energy_c);
        entity mana_regen = spawn_realm_stat(world, e, prefab_stat_regen, "mana regen", "manaregen", mana_c);
        // Attributes
        entity strength = spawn_realm_stat(world, e, prefab_stat_attribute, "strength", "strength", color_rgb_white);
        entity vitality = spawn_realm_stat(world, e, prefab_stat_attribute, "vitality", "vitality", color_rgb_white);
        entity endurance = spawn_realm_stat(world, e, prefab_stat_attribute, "endurance", "magic_defence", color_rgb_white);
        entity intelligence = spawn_realm_stat(world, e, prefab_stat_attribute, "intelligence", "intelligence", color_rgb_white);
        entity wisdom = spawn_realm_stat(world, e, prefab_stat_attribute, "wisdom", "wisdom", color_rgb_white);
        entity agility = spawn_realm_stat(world, e, prefab_stat_attribute, "agility", "agility", color_rgb_white);
        entity dexterity = spawn_realm_stat(world, e, prefab_stat_attribute, "dexterity", "dexterity", color_rgb_white);
        entity charm = spawn_realm_stat(world, e, prefab_stat_attribute, "charm", "charm", color_rgb_white);
        entity luck = spawn_realm_stat(world, e, prefab_stat_attribute, "luck", "luck", color_rgb_white);
        add_to_StatLinks(stats, soul);
        add_to_StatLinks(stats, health);
        add_to_StatLinks(stats, energy);
        add_to_StatLinks(stats, mana);
        add_to_StatLinks(stats, health_regen);
        add_to_StatLinks(stats, energy_regen);
        add_to_StatLinks(stats, mana_regen);
        add_to_StatLinks(stats, strength);
        add_to_StatLinks(stats, vitality);
        add_to_StatLinks(stats, endurance);
        add_to_StatLinks(stats, intelligence);
        add_to_StatLinks(stats, wisdom);
        add_to_StatLinks(stats, agility);
        add_to_StatLinks(stats, dexterity);
        add_to_StatLinks(stats, charm);
        add_to_StatLinks(stats, luck);

        // Jobs
        // create regen stat
        // stats.value[i++] = spawn_stat_level_t(world, "farming", "farming", color_rgb_gray);

        zox_logv("At [%f] Realm [stats] [%i] spawned.", zox_current_time, stats->length);
    }
} zox_sys_end(StatsRealmSpawnSystem);
