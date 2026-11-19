entity spawn_prefab_shoot(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("shoot");
    zox_add(e, Shoot);
    zox_set(e, SkillDamage, { 0 });
    zox_set(e, SkillDamageMax, { 0 });
    zox_set(e, SkillRange, { 0 });
    zox_set(e, SkillCost, { 0 });
    zox_set(e, SkillReach, { { 3, 10 } });      // a step back, its shot falls off past it
    // a slow burn on its hit: 0.5 a second for 5s
    zox_add(e, Burn);
    zox_set(e, Damage, { 0.5f });
    zox_add(e, DamagePerSecond);
    zox_set(e, DamageTime, { 5 });
    return e;
}
