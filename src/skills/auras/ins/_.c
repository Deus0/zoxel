entity spawn_realm_skill_aura(ecs *world, entity parent, entity prefab, const char* name, const char* tname, color ecolor, float value, float range, float warmup, float cooldown) {
    entity e = spawn_realm_skill(world, parent, prefab, name, tname, ecolor);
    zox_set(e, SkillDamage, { value });
    zox_set(e, SkillRange, { range });
    zox_set(e, WarmupTime, { warmup });
    zox_set(e, CooldownTime, { cooldown });
    zox_make_prefab(e);
    return e;
}
