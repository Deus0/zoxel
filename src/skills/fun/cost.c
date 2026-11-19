// a skill's cost paid from its user's resource stat (SkillResource: punch energy, shoot mana), 0 if it can't afford
//  it (a sound, no resource); an npc's stats are copies of its kind's, a step further from the realm's (tst npc_attacks)
byte skill_pay_cost(ecs* world, entity skill, entity user) {
    if (disable_skill_costs) {
        return 1;
    }
    const entity resource = get_user_stat_from_realm(world, user, zox_get_link(world, skill, SkillResource));
    if (!zox_valid(resource) || !zox_has(resource, StatValue)) {
        return 0;
    }
    const float value = zox_getv(resource, StatValue);
    const float cost = zox_getv(skill, SkillCost);
    if (value < cost) {
        spawn_sound_effect(world, sound_effect_no_resource, user);
        return 0;
    }
    zox_setv(resource, StatValue, value - cost);
    return 1;
}
