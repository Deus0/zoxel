// DotsSystem - skill applies damage to a users health
zox_sys2(DotsSystem) {
    // const float damage_rate = 1.0f; // add this property to dot entity
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(UserLink);
    zox_sys_in(SpawnerLink);
    zox_sys_in(SkillDamage);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(UserLink, user);
        zox_sys_i(SkillDamage, skillDamage);
        zox_sys_i(SpawnerLink, spawnerLink);
        if (!zox_valid(user->value) || zox_gett_value(user->value, Dead) || !skillDamage->value) {
            continue;
        }
        float damage = skillDamage->value;
        if (zox_valid(spawnerLink->value) && !zox_gett_value(spawnerLink->value, Dead)) {
            // todo: influence stat link for auras -> to determine strengthing stats
            // zox_geter(spawnerLink->value, StatLinks, enemy_stats);
            entity enemy_stats[stats_children_capacity];
            uint enemy_stats_length = zox_get_children(world, spawnerLink->value, enemy_stats, stats_children_capacity);
            // Debuff Stat needs a StatLink here
            int k = 0; // assuming intelligence is like 4th stat attribute for now
            for (uint j = 0; j < enemy_stats_length; j++) {
                entity stat = enemy_stats[j];
                if (zox_has(stat, StatAttribute)) {
                    k++;
                    if (k == 3) {
                        float value = zox_get_value(stat, StatValue);
                        damage += value;
                        break;
                    }
                }
            }
        }
        // the character being damaged by debuff
        entity health_stat = zox_get_child_by_id(world, user->value, zox_id(HealthStat));
        // entity user_stats[stats_children_capacity];
        // uint user_stats_length = zox_get_children(world, user->value, user_stats, stats_children_capacity);
        // zox_geter(user->value, StatLinks, statLinks);
        // find health stat, maybe poison should target a stat type directly??
        // find_array_element_with_tag(statLinks, HealthStat, health_stat)
        if (!health_stat) {
            zox_log(" ! user had no health\n")
            continue;
        }
        // const entity health_stat = statLinks->value[0];
        float stat_value_max = zox_get_value(health_stat, StatValueMax)
        zox_muter(health_stat, StatValue, statValue)
        statValue->value += delta_time * skillDamage->value;
        if (statValue->value < 0) {
            statValue->value = 0;
        } else if (statValue->value > stat_value_max) {
            statValue->value = stat_value_max;
        }
        // rememer last to give xp - wait this tick rate - warlocks will always get xp lmao
        // zox_set(user->value, LastDamager, { spawnerLink->value })
        combat_on_hit(world, user->value,spawnerLink->value);
    }
} zox_sys_end(DotsSystem);
