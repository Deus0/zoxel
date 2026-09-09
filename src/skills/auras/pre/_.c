entity spawn_prefab_aura(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("aura");
    zox_add(e, Aura);
    zox_add(e, SkillToggleable);
    zox_setv(e, SkillDamage, 0);
    zox_setv(e, SkillRange, 0);
    return e;
}

entity prefab_skill_aura;

void spawn_prefabs_auras(ecs* world) {
    prefab_skill_aura = spawn_prefab_aura(
        world,
        prefab_skill);
}
