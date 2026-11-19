entity spawn_prefab_tentacle(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("tentacle");
    zox_add(e, Tentacle);
    zox_set(e, SkillDamage, { 0 });
    zox_set(e, SkillDamageMax, { 0 });
    zox_set(e, SkillRange, { 0 });
    zox_set(e, SkillCost, { 0 });
    zox_set(e, SkillReach, { { 1.2f, 4 } });    // past a pounce's, its whip's length
    return e;
}
