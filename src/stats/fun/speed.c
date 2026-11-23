// a character's movement speed (StatSpeed, 1 a player's) and intelligence (StatIntelligence), from its stats
//  - a slime (level 1) moves and pounces at about 0.6 a player's speed, and watches longer between attacks
//  - read on changes (a battle starting, an attack, a pounce), not every frame: it walks its children
static inline float character_stat_tagged(ecs* world, entity e, entity tag, float none) {
    const entity stat = zox_valid(e) ? zox_get_child_by_id(world, e, tag) : 0;
    return zox_valid(stat) && zox_has(stat, StatValue) ? zox_getv(stat, StatValue) : none;
}

static inline float character_speed(ecs* world, entity e) {
    return float_max(0.1f, character_stat_tagged(world, e, zox_id(StatSpeed), 1));
}

static inline float character_intelligence(ecs* world, entity e) {
    return character_stat_tagged(world, e, zox_id(StatIntelligence), 0);
}

// an npc's attribute (agility, intelligence) at a level
static inline float npc_attribute_at(float soul) {
    return soul * npc_attribute_level_increase;
}

// an npc's movement speed at a level: its base raised by its agility, as its realm stat scales (StatScaleAmount)
static inline float npc_speed_at(ecs* world, entity realm_speed, float soul) {
    const float scale = zox_has(realm_speed, StatScaleAmount) ? zox_getv(realm_speed, StatScaleAmount) : 0;
    return float_min(npc_speed_max, npc_speed_base + npc_attribute_at(soul) * scale);
}
