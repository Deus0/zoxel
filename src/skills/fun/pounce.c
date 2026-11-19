// a skill used over time (SkillUsing, SkillUsedAt, SkillTarget on its user, SkillReleased and SkillHit as it goes):
//  one at a time
static inline byte skill_using_begin(ecs* world, entity user, entity skill, float3 target) {
    if (zox_has(user, SkillUsing)) {
        return 0;
    }
    zox_set(user, SkillUsing, { skill });
    zox_set(user, SkillUsedAt, { zox_current_time });
    zox_set(user, SkillTarget, { target });
    return 1;
}

static inline void skill_using_end(ecs* world, entity user) {
    zox_remove(user, SkillUsing);
    zox_remove(user, SkillUsedAt);
    zox_remove(user, SkillReleased);
    zox_remove(user, SkillHit);
    zox_remove(user, SkillTarget);
}

// a melee user without a skeleton (a slime) pounces, not swings: it squashes, springs at its aim and hits what it
//  lands on (sys/pounce.c); its shape a Scale3 while it does
void pounce_begin(ecs* world, entity user, entity skill) {
    if (skill_using_begin(world, user, skill, float3_zero)) {
        zox_set(user, Scale3, { float3_one });
    }
}
