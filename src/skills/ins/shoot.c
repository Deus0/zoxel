entity spawn_realm_skill_shoot(
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
    entity e = spawn_realm_skill(
        world,
        parent,
        prefab,
        name,
        tname,
        ecolor);
    zox_link(world, e, SkillResource, resource);
    zox_setv(e, SkillCost, cost);
    zox_setv(e, SkillDamage, value);
    zox_setv(e, SkillDamageMax, vmax);
    zox_setv(e, SkillRange, range);
    zox_setv(e, WarmupTime, warmup);
    zox_setv(e, CooldownTime, cooldown);
    zox_setv(e, RaycastRange, range);
    return e;
}
