entity spawn_prefab_poison(ecs *world) {
    zox_prefab();
    zox_prefab_name("poison");
    zox_add_tag(e, Dot);
    zox_add_tag(e, Poison);
    // zox_prefab_set(e, UserLink, { 0 });
    zox_prefab_set(e, SpawnerLink, { 0 });
    zox_prefab_set(e, SkillLink, { 0 });
    zox_prefab_set(e, SkillDamage, { 0 });
    zox_prefab_set(e, ParticlesEmitterLink, { 0 });
    return e;
}

entity spawn_poison(ecs *world, entity parent, entity prefab, entity spawner, entity skill, float damage) {
    zox_instance(prefab);
    zox_name("poison");
    if (zox_valid(parent)) {
        zox_set_parent(world, e, parent);
    }
    // zox_set(e, UserLink, { parent });          // user that owns poison debuff
    zox_set(e, SpawnerLink, { spawner });    // user that used skill
    zox_set(e, SkillLink, { skill });        // skill that created poison
    zox_set(e, SkillDamage, { damage });        // skill that created poison
    return e;
}
