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
    // npcs start full, so they don't regenerate (dirtying their stats) until they're hurt
    if (!is_player_character) {
        health.x = health.y;
        energy.x = energy.y;
        mana.x = mana.y;
    }
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
    // Add Regen and Base Stats
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
                    is_player_character ?
                        player_base_regen :
                        base_regen);
            } else if (zox_has(stat, StatBase)) {
                // ie physical damage, starts at the realms base value
                spawn_stat_base(
                    world,
                    e,
                    stat,
                    zox_getv(stat, StatValue));
            }
        }
    }
}

// The base stats a realm gives characters (soul, health, energy, mana, regens, bases), any number,
//  the four kinds that set others' values remembered (what spawn_base_stats picks)
#define zox_realm_base_stats_max 32
typedef struct {
    entity soul;
    entity health;
    entity energy;
    entity mana;
    entity all[zox_realm_base_stats_max];
    byte count;
} RealmBaseStats;

static inline byte get_realm_base_stats(ecs* world, entity realm, RealmBaseStats* stats) {
    *stats = (RealmBaseStats) { 0 };
    iter it2 = zox_children(world, realm);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity stat = it2.entities[j];
            if (!zox_has(stat, Stat)) {
                continue;
            }
            byte base = 0;
            if (!stats->soul && zox_has(stat, StatLevel)) {
                stats->soul = stat;
                base = 1;
            }
            if (!stats->health && zox_has(stat, StatHealth)) {
                stats->health = stat;
                base = 1;
            } else if (!stats->energy && zox_has(stat, StatState)) {
                stats->energy = stat;
                base = 1;
            } else if (!stats->mana && zox_has(stat, StatState)) {
                stats->mana = stat;
                base = 1;
            }
            base |= zox_has(stat, StatRegen) || zox_has(stat, StatBase);
            if (base && stats->count < zox_realm_base_stats_max) {
                stats->all[stats->count++] = stat;
            }
        }
    }
    return stats->soul && stats->health && stats->energy && stats->mana;
}

// is a stat made from this realm stat: its prefab, or its prefab's prefab (a character prefab's stats are
//  realm stats' instances, an npc's copies of them are those stats' instances)
static inline byte is_realm_stat_of(ecs* world, entity stat, entity realm_stat) {
    entity prefab = zox_get_prefab(world, stat);
    for (int depth = 0; prefab && depth < 4; depth++) {
        if (prefab == realm_stat) {
            return 1;
        }
        prefab = zox_get_prefab(world, prefab);
    }
    return 0;
}

// does a character have a stat from this realm stat (its prefab may give it some)
static inline byte has_realm_stat(ecs* world, entity character, entity realm_stat) {
    iter it2 = zox_children(world, character);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            if (is_realm_stat_of(world, it2.entities[j], realm_stat)) {
                zox_children_end(it2);
                return 1;
            }
        }
    }
    return 0;
}

// a new npc's starting value of a stat (and its max, for states), npcs start full
static inline float npc_stat_start(ecs* world, const RealmBaseStats* stats, entity stat, float soul, float* max) {
    *max = 0;
    if (stat == stats->soul) {
        return soul;
    } else if (stat == stats->health) {
        return *max = health_base + soul * health_level_increase;
    } else if (stat == stats->energy) {
        return *max = energy_base + soul * energy_level_increase;
    } else if (stat == stats->mana) {
        return *max = mana_base + soul * mana_level_increase;
    } else if (zox_has(stat, StatRegen)) {
        return base_regen;
    }
    return zox_getv(stat, StatValue);  // a base, i.e. physical damage, at the realm's base value
}

// Gives a character (or a character prefab) the realm's base stats it doesn't have: what every character
//  shares, a unique one's prefab can add its own extras, the player also gets attributes and currencies
//  - soul and health given (each character kind its own), the rest follow from the soul
//  - checks the stats the character has, not ones spawned in the same deferred batch (call it once)
//  - npcs start full, the player starts its energy and mana at their base (it regenerates)
void fill_character_base_stats(ecs* world, entity character, entity realm, float soul, float health, byte is_player) {
    RealmBaseStats stats;
    if (!get_realm_base_stats(world, realm, &stats)) {
        zox_loge("the realm has invalid stats");
        return;
    }
    for (int s = 0; s < stats.count; s++) {
        const entity stat = stats.all[s];
        if (has_realm_stat(world, character, stat)) {
            continue;
        }
        if (stat == stats.health) {
            spawn_stat_state(world, character, stat, health, health);
            continue;
        }
        float max;
        const float value = npc_stat_start(world, &stats, stat, soul, &max);
        if (max > 0) {
            spawn_stat_state(world, character, stat, is_player ? (stat == stats.energy ? energy_base : mana_base) : value, max);
        } else if (stat == stats.soul) {
            spawn_stat_level(world, character, stat, value);
        } else {
            spawn_stat_base(world, character, stat, value);
        }
    }
    if (!is_player) {
        return;
    }
    // attributes and currencies (stat points, skill points) at 0
    iter it2 = zox_children(world, realm);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity stat = it2.entities[j];
            if (!zox_has(stat, Stat) || has_realm_stat(world, character, stat)) {
                continue;
            }
            if (zox_has(stat, StatAttribute)) {
                spawn_stat_attribute(world, character, stat, 0);
            } else if (zox_has(stat, StatCurrency)) {
                spawn_stat_currency(world, character, stat, 0);
            }
        }
    }
}


// NOTE: Gives npcs any realm base stats their prefab didn't (realm generation gives character prefabs all
//  of theirs, instances copy them, so this normally adds nothing)
zox_sys2(CharacterStatsSystem) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity realm = zox_get_link(world, e, RealmLink);
        const float soul = randf_range(1, 3);
        fill_character_base_stats(world, e, realm, soul, health_base + soul * health_level_increase, 0);
    }
} zox_sys_end(CharacterStatsSystem);
