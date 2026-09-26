// When health goes to 0, kill UserLink->value
// Set Dead to 1
void spawn_base_stats(
    ecs* world,
    entity e,
    entity realm,
    byte dbg_log)
{
    entity realm_soul = 0;
    entity realm_health = 0;
    entity realm_energy = 0;
    entity realm_mana = 0;
    iter it2 = zox_children(world, realm);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity stat = it2.entities[j];
            if (!zox_has(stat, Stat)) {
                continue;
            }
            if (!realm_soul && zox_has(stat, StatLevel)) {
                realm_soul = stat;
            }
            if (!realm_health && zox_has(stat, StatHealth)) {
                realm_health = stat;
            } else if (!realm_energy && zox_has(stat, StatState)) {
                realm_energy = stat;
            } else if (!realm_mana && zox_has(stat, StatState)) {
                realm_mana = stat;
            }
        }
    }
    if (!zox_valid(realm_soul)) {
        zox_loge("the realm has invalid stats")
        return;
    }
    // generate numbers here
    byte is_player_character = zox_has(e, PlayerCharacter);
    float soul_value =
        is_player_character ?
            1 :
            randf_range(1, 3);
    float2 health;
    // = (float2) {
    //    health_base,
    //    health_base + soul_value * health_level_increase
    //};
    if (is_player_character) {
        health = float2_single(player_health_base);
    } else {
        health = float2_single(health_base + soul_value * health_level_increase);
    }
    health.x = randf_range(health_base, health.y);
    float2 energy = (float2) {
        energy_base,
        energy_base + soul_value * energy_level_increase
    };
    float2 mana = (float2) {
        mana_base,
        mana_base + soul_value * mana_level_increase
    };
    // Soul
    entity soule = zox_get_child_by_id(world, e, zox_id(StatSoul));
    if (!zox_valid(soule)) {
        spawn_stat_level(world, e, realm_soul, soul_value);
    }
    // Health
    entity healthe = zox_get_child_by_id(
        world,
        e,
        zox_id(StatHealth));
    if (!zox_valid(healthe)) {
        spawn_stat_state(
            world,
            e,
            realm_health,
            health.x,
            health.y);
    }
    spawn_stat_state(
        world,
        e,
        realm_energy,
        energy.x,
        energy.y);
    spawn_stat_state(
        world,
        e,
        realm_mana,
        mana.x,
        mana.y);
    if (dbg_log) {
        zox_log("[%s] spawned stat [%s] x[%f]",
            zox_getn(e),
            zox_getn(realm_energy),
            energy.y);
        zox_log("[%s] spawned stat [%s] x[%f]",
            zox_getn(e),
            zox_getn(realm_mana),
            mana.y);
    }
    // Add Regen Stats
    it2 = zox_children(world, realm);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity stat = it2.entities[j];
            if (!zox_has(stat, Stat)) {
                continue;
            }
            if (zox_has(stat, StatRegen)) {
                // Spawn a regen
                spawn_stat_regen(
                    world,
                    e,
                    stat,
                    base_regen);
            }
        }
    }
}

// NOTE: Generates base stats for Characters
zox_sys2(CharacterStatsSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // Collect Realm Stats
        entity realm = zox_get_link(world, e, RealmLink);
        spawn_base_stats(
            world,
            e,
            realm,
            dbg_log);
    }
} zox_sys_end(CharacterStatsSystem);
