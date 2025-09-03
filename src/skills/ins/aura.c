entity spawn_skill_aura(
    ecs *world,
    const entity prefab,
    char* name,
    const float value,
    const float range,
    const color particle_color
) {
    const entity e = spawn_meta_skill(world, prefab, name);
    // zox_instance(prefab)
    zox_prefab_set(e, SkillDamage, { value });
    zox_prefab_set(e, SkillRange, { range });
    zox_prefab_set(e, Color, { particle_color });
    return e;
}

entity spawn_skill_aura_t(
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
    zox_set(e, WarmupTime, { warmup });
    zox_set(e, CooldownTime, { cooldown });
    return e;
}
