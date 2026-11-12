// Bonus per attribute point
float attribute_health_scale = 2;
float attribute_mana_scale = 2;
float attribute_energy_scale = 2;
float attribute_regen_scale = 0.5f;     // x regen_rate
float attribute_damage_scale = 0.7f;
float attribute_defence_scale = 0.5f;

void StatsRealmSpawnSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
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
        entity soul = spawn_realm_stat(
            world,
            e,
            prefab_stat_level,
            "soul",
            "soul",
            soul_c);
        zox_add(soul, StatSoul);
        // states
        entity health = spawn_realm_stat(
            world,
            e,
            prefab_stat_state,
            "health",
            "health",
            health_c);
        zox_add(health, StatHealth);
        entity energy = spawn_realm_stat(
            world,
            e,
            prefab_stat_state,
            "energy",
            "energy",
            energy_c);
        entity mana = spawn_realm_stat(
            world,
            e,
            prefab_stat_state,
            "mana",
            "mana",
            mana_c);
        // Regens
        entity health_regen = spawn_realm_stat_regen(
            world,
            e,
            prefab_stat_regen,
            "health regen",
            "healthregen",
            health_c,
            health);
        entity energy_regen = spawn_realm_stat_regen(
            world,
            e,
            prefab_stat_regen,
            "energy regen",
            "energyregen",
            energy_c,
            energy);
        entity mana_regen = spawn_realm_stat_regen(
            world,
            e,
            prefab_stat_regen,
            "mana regen",
            "manaregen",
            mana_c,
            mana);
        // Base
        entity physical_damage = spawn_realm_stat(
            world,
            e,
            prefab_stat_base,
            "physical damage",
            "physical_damage",
            color_rgb_white);
        zox_add(physical_damage, StatDamage);
        zox_set(physical_damage, StatValue, { 0 });
        entity physical_defence = spawn_realm_stat(
            world,
            e,
            prefab_stat_base,
            "physical defence",
            "physical_defence",
            color_rgb_white);
        zox_add(physical_defence, StatDefence);
        zox_set(physical_defence, StatValue, { 0 });
        zox_link(world, physical_damage, StatDefenceLink, physical_defence);
        // Currencies
        entity stat_point = spawn_realm_stat(
            world,
            e,
            prefab_stat_currency,
            "stat point",
            "stat_point",
            color_rgb_white);
        zox_add(stat_point, StatPoint);
        zox_set(stat_point, StatLevelReward, { 3 });
        entity skill_point = spawn_realm_stat(
            world,
            e,
            prefab_stat_currency,
            "skill point",
            "skill_point",
            color_rgb_white);
        zox_add(skill_point, SkillPoint);
        zox_set(skill_point, StatLevelReward, { 1 });
        // Attributes
        entity strength = spawn_realm_stat(
            world,
            e,
            prefab_stat_attribute,
            "strength",
            "strength",
            color_rgb_white);
        entity vitality = spawn_realm_stat(
            world,
            e,
            prefab_stat_attribute,
            "vitality",
            "vitality",
            color_rgb_white);
        entity endurance = spawn_realm_stat(
            world,
            e,
            prefab_stat_attribute,
            "endurance",
            "endurance",
            color_rgb_white);
        entity intelligence = spawn_realm_stat(
            world,
            e,
            prefab_stat_attribute,
            "intelligence",
            "intelligence",
            color_rgb_white);
        entity wisdom = spawn_realm_stat(
            world,
            e,
            prefab_stat_attribute,
            "wisdom",
            "wisdom",
            color_rgb_white);
        entity agility = spawn_realm_stat(
            world,
            e,
            prefab_stat_attribute,
            "agility",
            "agility",
            color_rgb_white);
        entity dexterity = spawn_realm_stat(
            world,
            e,
            prefab_stat_attribute,
            "dexterity",
            "dexterity",
            color_rgb_white);
        spawn_realm_stat(
            world,
            e,
            prefab_stat_attribute,
            "charm",
            "charm",
            color_rgb_white);
        spawn_realm_stat(
            world,
            e,
            prefab_stat_attribute,
            "luck",
            "luck",
            color_rgb_white);
        // Attribute Scaling - stat gains amount per attribute point
        set_realm_stat_scale(world, health, strength, attribute_health_scale);
        set_realm_stat_scale(world, health_regen, vitality, attribute_regen_scale);
        set_realm_stat_scale(world, mana, intelligence, attribute_mana_scale);
        set_realm_stat_scale(world, mana_regen, wisdom, attribute_regen_scale);
        set_realm_stat_scale(world, energy, agility, attribute_energy_scale);
        set_realm_stat_scale(world, energy_regen, dexterity, attribute_regen_scale);
        set_realm_stat_scale(world, physical_damage, strength, attribute_damage_scale);
        set_realm_stat_scale(world, physical_defence, endurance, attribute_defence_scale);
        // Jobs
        // create regen stat
        // stats.value[i++] = spawn_stat_level_t(world, "farming", "farming", color_rgb_gray);
        zox_logv("Realm [stats] spawned");
    }
    zox_sys_on_end();
} zoxd_system(StatsRealmSpawnSystem);
