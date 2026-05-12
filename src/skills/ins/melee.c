entity spawn_realm_skill_melee(ecs* world, entity parent, entity prefab, const char* name, const char* tname, color ecolor, float value, float vmax, float range, entity resource, float cost, float warmup, float cooldown) {
    entity e = spawn_realm_skill(world, parent, prefab, name, tname, ecolor);
    // const entity e = spawn_meta_skill(world, prefab, name);
    zox_set(e, SkillDamage, { value });
    zox_set(e, SkillDamageMax, { vmax });
    zox_set(e, SkillRange, { range });
    zox_set(e, SkillCost, { cost });
    zox_set(e, SkillResourceLink, { resource });
    zox_set(e, WarmupTime, { warmup });
    zox_set(e, CooldownTime, { cooldown });
    return e;
}

/*entity spawn_skill_melee_t(ecs *world,
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
    return e;
}
*/