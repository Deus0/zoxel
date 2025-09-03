entity spawn_skill_melee(
    ecs *world,
    const entity prefab,
    char* name,
    const float value,
    const float value_max,
    const float range,
    const entity resource,
    const float cost
) {
    const entity e = spawn_meta_skill(world, prefab, name);
    zox_set(e, SkillDamage, { value });
    zox_set(e, SkillDamageMax, { value_max });
    zox_set(e, SkillRange, { range });
    zox_set(e, SkillCost, { cost });
    zox_set(e, SkillResourceLink, { resource });
    return e;
}

entity spawn_skill_melee_t(
    ecs *world,
    char* name,
    const float value,
    const float value_max,
    const float range,
    const entity resource,
    const float cost,
    char *texture_name,
    const float warmup,
    const float cooldown
) {
    const entity e = spawn_skill_melee(
        world,
        prefab_skill_melee,
        name,
        value,
        value_max,
        range,
        resource,
        cost);
    zox_set(e, TextureLink, { string_hashmap_get(files_hashmap_textures, new_string_data(texture_name)) });
    zox_set(e, WarmupTime, { warmup });
    zox_set(e, CooldownTime, { cooldown });
    return e;
}
