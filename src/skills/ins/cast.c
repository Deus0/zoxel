// a skill's timings: its cast (its warmup, 0 an instant skill: SkillInstant) and its cooldown
void skill_set_cast(ecs* world, entity e, float cast, float cooldown) {
    zox_setv(e, WarmupTime, cast);
    zox_setv(e, CooldownTime, cooldown);
    if (cast <= 0) {
        zox_add(e, SkillInstant);
    }
}
