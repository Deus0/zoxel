// DotsSystem - skill applies damage to a users health
zox_sys2(DotsSystem) {
    // const float damage_rate = 1.0f; // add this property to dot entity
    // byte dbg_log = 1;
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SpawnerLink);
    zox_sys_in(SkillDamage);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SkillDamage, skillDamage);
        zox_sys_i(SpawnerLink, spawner);
        entity defender = zox_get_parent(world, e);
        if (!zox_valid(defender) || zox_gett_value(defender, Dead) || !skillDamage->value) {
            continue;
        }
        float damage = skillDamage->value;
        // Modify Damage by Attackers Buffs
        // TODO: We should apply this when adding Debuffs
        entity attacker = spawner->value;
        if (zox_valid(attacker) && !zox_gett_value(attacker, Dead)) {
            // todo: influence stat link for auras -> to determine strengthing stats
            entity attacker_stats[stats_children_capacity];
            uint attacker_stats_length = zox_get_children(world, attacker, attacker_stats, stats_children_capacity);
            // Debuff Stat needs a StatLink here
            int k = 0; // assuming intelligence is like 4th stat attribute for now
            for (uint j = 0; j < attacker_stats_length; j++) {
                entity stat = attacker_stats[j];
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
        entity health_stat = zox_get_child_by_id(world, defender, zox_id(StatHealth));
        if (!health_stat) {
            zox_logw("Uuser had no health");
            continue;
        }
        // const entity health_stat = statLinks->value[0];
        float stat_value_max = zox_get_value(health_stat, StatValueMax);
        zox_muter(health_stat, StatValue, statValue);
        statValue->value += delta_time * skillDamage->value;
        if (statValue->value < 0) {
            statValue->value = 0;
        } else if (statValue->value > stat_value_max) {
            statValue->value = stat_value_max;
        }
        // rememer last to give xp - wait this tick rate - warlocks will always get xp lmao
        combat_on_hit(world, defender, attacker);
    }
} zox_sys_end(DotsSystem);
