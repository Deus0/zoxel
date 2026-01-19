zox_sys2(StatsRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(StatLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(StatLinks, stats);

        if (state->value != zox_generate_realm_stats) {
            continue;
        }

        const float sat = 1.4f;
        const float dark = 0.4f;
        color_rgb soul_c = color_rgb_purple;
        color_rgb health_c = color_rgb_red;
        color_rgb energy_c = color_rgb_yellow;
        color_rgb mana_c = color_rgb_cyan;
        soul_c = color_rgb_darken(color_rgb_saturate(soul_c, sat), dark);
        health_c = color_rgb_darken(color_rgb_saturate(health_c, sat), dark);
        energy_c = color_rgb_darken(color_rgb_saturate(energy_c, sat), dark);
        mana_c = color_rgb_darken(color_rgb_saturate(mana_c, sat), dark);

        byte i = 0;
        // levels
        entity soul = spawn_stat_level_t(world, "soul", "soul", soul_c);
        zox_add_tag(soul, StatSoul);
        add_to_StatLinks(stats, soul);

        // states
        entity health = spawn_stat_state_t(world, "health", "health", health_c);
        zox_add_tag(health, HealthStat);
        entity energy = spawn_stat_state_t(world, "energy", "energy", energy_c);
        entity mana = spawn_stat_state_t(world, "mana", "mana", mana_c);
        add_to_StatLinks(stats, health);
        add_to_StatLinks(stats, energy);
        add_to_StatLinks(stats, mana);

        // Regens
        entity health_regen = spawn_stat_regen_t(world, "health regen", "healthregen");
        entity energy_regen = spawn_stat_regen_t(world, "energy regen", "energyregen");
        entity mana_regen = spawn_stat_regen_t(world, "mana regen", "manaregen");
        add_to_StatLinks(stats, health_regen);
        add_to_StatLinks(stats, energy_regen);
        add_to_StatLinks(stats, mana_regen);

        // Attributes
        entity strength = spawn_stat_attribute_t(world, "strength", "strength");
        entity vitality = spawn_stat_attribute_t(world, "vitality", "vitality");
        entity endurance = spawn_stat_attribute_t(world, "endurance", "endurance");
        entity intelligence = spawn_stat_attribute_t(world, "intelligence", "intelligence");
        entity wisdom = spawn_stat_attribute_t(world, "wisdom", "wisdom");
        entity agility = spawn_stat_attribute_t(world, "agility", "agility");
        entity dexterity = spawn_stat_attribute_t(world, "dexterity", "dexterity");
        entity charm = spawn_stat_attribute_t(world, "charm", "charm");
        entity luck = spawn_stat_attribute_t(world, "luck", "luck");
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