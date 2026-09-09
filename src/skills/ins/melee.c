entity spawn_realm_skill_melee(
    ecs* world,
    entity parent,
    entity prefab,
    const char* name,
    const char* tname,
    color ecolor,
    float value,
    float vmax,
    float range,
    entity resource,
    float cost,
    float warmup,
    float cooldown)
{
    entity e = spawn_realm_skill(world, parent, prefab, name, tname, ecolor);
    zox_link(world, e, SkillResource, resource);
    zox_set(e, SkillCost, { cost });
    zox_set(e, SkillDamage, { value });
    zox_set(e, SkillDamageMax, { vmax });
    zox_set(e, SkillRange, { range });
    zox_set(e, WarmupTime, { warmup });
    zox_set(e, CooldownTime, { cooldown });
    zox_set(e, RaycastRange, { range });
    return e;
}
