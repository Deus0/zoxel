entity spawn_prefab_melee(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("melee");
    zox_add(e, Melee);
    zox_set(e, SkillDamage, { 0 });
    zox_set(e, SkillDamageMax, { 0 });
    zox_set(e, SkillRange, { 0 });
    zox_set(e, SkillCost, { 0 });
    return e;
}
