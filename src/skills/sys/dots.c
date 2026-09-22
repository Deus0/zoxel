// DotsSystem - skill applies damage to a users health
void dots_system(iter* it) {
    // const float damage_rate = 1.0f; // add this property to dot entity
    // byte dbg_log = 0;
    init_delta_time();
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkillDamage);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SkillDamage, damage);
        entity defender = zox_get_parent(world, e);
        if (!zox_valid(defender) ||
            zox_has(defender, Dead) ||
            !damage->value) {
            continue;
        }
        float apply_damage = damage->value;
        // Modify Damage by Attackers Buffs
        // TODO: We should apply this when adding Debuffs
        entity attacker = zox_get_link(world, e, Spawner);
        if (zox_valid(attacker) &&
            !zox_has(attacker, Dead))
        {
            // todo: influence stat link for auras -> to determine strengthing stats
            // entity attacker_stats[stats_children_capacity];
            // uint attacker_stats_length = zox_get_children(world, attacker, attacker_stats, stats_children_capacity);
            // TODO: Link Skills to Stats that boost them
            entity boost_stat = 0;
            int k = 0;
            iter it2 = zox_children(world, attacker);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count; j++) {
                    entity stat = it2.entities[j];
                    if (!zox_has(stat, StatAttribute)) {
                        continue;
                    }
                    k++;
                    if (k == 3) {
                        boost_stat = stat;
                        break;
                    }
                }
            }
            // Debuff Stat needs a StatLink here
            // assuming intelligence is like 4th stat attribute for now
            if (boost_stat) {
                apply_damage += zox_getv(boost_stat, StatValue);
            }
        }
        // the character being damaged by debuff
        entity health_stat = zox_get_child_by_id(world, defender, zox_id(StatHealth));
        if (!health_stat) {
            zox_logw("Uuser had no health");
            continue;
        }
        // const entity health_stat = statLinks->value[0];
        float stat_value_max = zox_getv(health_stat, StatValueMax);
        zox_muter(health_stat, StatValue, stat);
        stat->value += delta_time * apply_damage;
        if (stat->value < 0) {
            stat->value = 0;
        } else if (stat->value > stat_value_max) {
            stat->value = stat_value_max;
        }
        // rememer last to give xp - wait this tick rate - warlocks will always get xp lmao
        combat_on_hit(world, defender, attacker);
    }
    zox_sys_on_end();
} zoxd_system(dots_system);
