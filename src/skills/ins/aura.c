entity spawn_realm_skill_aura(ecs *world, entity parent, entity prefab, const char* name, const char* tname, color ecolor, float value, float range, float warmup, float cooldown) {
    entity e = spawn_realm_skill(world, parent, prefab, name, tname, ecolor);
    zox_set(e, SkillDamage, { value });
    zox_set(e, SkillRange, { range });
    zox_set(e, WarmupTime, { warmup });
    zox_set(e, CooldownTime, { cooldown });
    zox_make_prefab(e);
    return e;
}

/*entity spawn_skill_aura_t(
    ecs *world,
    char *name,
    const float value,
    const float range,
    const color particle_color,
    char *texture_name,
    const float warmup,
    const float cooldown
) {
    const entity e = spawn_skill_aura(
        world,
        prefab_skill_aura,
        name,
        value,
        range,
        particle_color);
    zox_set(e, TextureLink, { string_hashmap_get(files_hashmap_textures, new_string_data(texture_name)) });
    return e;
}
*/