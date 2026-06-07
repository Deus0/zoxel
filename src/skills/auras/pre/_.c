entity spawn_prefab_aura(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("aura");
    zox_add_tag(e, Aura);
    zox_add_tag(e, SkillToggleable);
    zox_prefab_set(e, SkillDamage, { 0 });
    zox_prefab_set(e, SkillRange, { 0 });
    zox_prefab_set(e, SkillResourceLink, { 0 });
    zox_prefab_set(e, ParticlesEmitterLink, { 0 });
    return e;
}

entity prefab_skill_aura;

void spawn_prefabs_auras(ecs* world) {
    prefab_skill_aura = spawn_prefab_aura(world, prefab_skill);
}
