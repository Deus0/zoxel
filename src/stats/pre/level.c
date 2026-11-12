/*entity create_stat_level(
    ecs *world,
    const char *name,
    const entity texture,
    const color_rgb c
) {
    entity e = spawn_meta_stat(world, prefab_stat, name);
    zox_add(e, StatLevel);
    zox_set(e, StatValue, { 1 });
    zox_set(e, ExperienceValue, { 0 });
    zox_set(e, ExperienceMax, { zox_experience_max_start });
    zox_set(e, TextureLink, { texture });
    zox_set(e, ColorRGB, { c });
    return e;
}*/
