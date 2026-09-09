entity spawn_prefab_poison(ecs *world) {
    zox_prefab();
    zox_prefab_name("poison");
    zox_add(e, Dot);
    zox_add(e, Poison);
    zox_setv(e, SkillDamage, 0);
    return e;
}

entity spawn_poison(
    ecs *world,
    entity parent,
    entity prefab,
    entity spawner,
    entity skill,
    float damage)
{
    zox_instance(prefab);
    zox_name("poison");
    zox_setv(e, SkillDamage, damage);
    if (zox_valid(parent)) {
        zox_set_parent(world, e, parent);
    }
    zox_link(world, e, Spawner, spawner);
    zox_link(world, e, Skill, skill);
    return e;
}
