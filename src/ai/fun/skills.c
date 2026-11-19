// where its attacker is (AttackerLink), 0 without one
static inline byte npc_threat_position(ecs* world, entity e, float3* position) {
    const entity attacker = zox_get_link(world, e, AttackerLink);
    if (!zox_valid(attacker) || !zox_has(attacker, Position3D)) {
        return 0;
    }
    *position = zox_getv(attacker, Position3D);
    return 1;
}

// an npc's skill for its threat at a distance, any skill with a reach (SkillReach), more skills just more choices
//  - in reach, affordable and ready: its average damage, its current a little more (it doesn't flick between equals)
//  - cooling down (a slime's tentacle, 5s): below any ready one, so a long cooldown's skill is a now and then
//  - none in reach: the nearest reach, it moves into it (attack_system)
#define npc_skills_max 16
#define npc_skill_keep 0.25f           // the current's score added, to keep it on a near tie
#define npc_pounce_reach 2.5f          // a boneless npc's melee (a slime's pounce, skills/sys/pounce.c), it leaps

// its skill's reach for it (a melee's when it has none), a boneless one's melee a pounce's
static inline float2 npc_skill_reach(ecs* world, entity e, entity skill) {
    if (!zox_valid(skill) || !zox_has(skill, SkillReach)) {
        return (float2) { 0, 1.75f };
    }
    if (zox_has(skill, Melee) && !zox_valid(get_skeleton(world, e))) {
        return (float2) { 0, npc_pounce_reach };
    }
    return zox_getv(skill, SkillReach);
}

static inline byte npc_can_afford(ecs* world, entity e, entity skill) {
    if (disable_skill_costs) {
        return 1;
    }
    const entity resource = get_user_stat_from_realm(world, e, zox_get_link(world, skill, SkillResource));
    return !zox_valid(resource) || zox_getv(resource, StatValue) >= zox_getv(skill, SkillCost);
}

// not cooling down (as action_activate_system's)
static inline byte npc_skill_ready(ecs* world, entity skill) {
    return !(zox_has(skill, CooldownState) && zox_getv(skill, CooldownState)) && !(zox_has(skill, CooldownAt) && zox_getv(skill, CooldownAt));
}

entity npc_choose_skill(ecs* world, entity e, entity current, float distance) {
    entity skills[npc_skills_max];
    const uint count = zox_get_children_by_id(world, e, skills, npc_skills_max, zox_id(Skill));
    entity best = 0;
    float best_score = -FLT_MAX;
    for (uint i = 0; i < count; i++) {
        const entity skill = skills[i];
        if (!zox_has(skill, SkillReach)) {
            continue;
        }
        const float2 reach = npc_skill_reach(world, e, skill);
        const float gap = distance < reach.x ? reach.x - distance : distance > reach.y ? distance - reach.y : 0;
        float score;
        if (!npc_can_afford(world, e, skill)) {
            score = -1000 - gap;
        } else if (gap > 0) {
            score = -100 - gap;
        } else if (!npc_skill_ready(world, skill)) {
            score = -50 + (skill == current ? npc_skill_keep : 0);
        } else {
            score = 0.5f * (zox_getv(skill, SkillDamage) + zox_getv(skill, SkillDamageMax)) + (skill == current ? npc_skill_keep : 0);
        }
        if (score > best_score) {
            best_score = score;
            best = skill;
        }
    }
    return best;
}

